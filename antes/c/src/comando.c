#include "comando.h"

#include <string.h>

#include "crc.h"
#include "pin.h"
#include "registro.h"
#include "saldo.h"
#include "utilidades.h"

#define COMANDO_POSICION_INS        1u
#define COMANDO_POSICION_LC         4u
#define COMANDO_POSICION_DATOS      5u
#define COMANDO_LONGITUD_CABECERA   5u
#define COMANDO_LONGITUD_IMPORTE    (COMANDO_LONGITUD_CABECERA + 2u)
#define COMANDO_LONGITUD_CAMBIO_PIN (COMANDO_LONGITUD_CABECERA + 2u * PIN_LONGITUD_MAXIMA)

#define COMANDO_INS_VERIFICAR_PIN 0x20u
#define COMANDO_INS_CAMBIAR_PIN   0x24u
#define COMANDO_INS_PAGAR         0x30u
#define COMANDO_INS_RECARGAR      0x32u
#define COMANDO_INS_CONSULTAR     0x34u

#define COMANDO_SW_SIN_ERROR                0x9000u
#define COMANDO_SW_FALLO_MEMORIA            0x6581u
#define COMANDO_SW_LONGITUD_INCORRECTA      0x6700u
#define COMANDO_SW_SEGURIDAD_NO_SATISFECHA  0x6982u
#define COMANDO_SW_METODO_BLOQUEADO         0x6983u
#define COMANDO_SW_SALDO_INSUFICIENTE       0x6985u
#define COMANDO_SW_DATOS_INCORRECTOS        0x6A80u
#define COMANDO_SW_INS_NO_ADMITIDA          0x6D00u

#define COMANDO_CRC_LONGITUD      2u
#define COMANDO_SW_LONGITUD       2u
#define RESPUESTA_LONGITUD_MAXIMA (SALDO_LONGITUD + COMANDO_CRC_LONGITUD + COMANDO_SW_LONGITUD)

static const uint16_t SALDO_MAXIMO = 50000u;

static uint8_t respuesta[RESPUESTA_LONGITUD_MAXIMA];
static uint8_t respuesta_longitud;

/* Añade datos a la respuesta, los que quepan. */
static void comando_anadir(const uint8_t *datos, uint8_t n)
{
    uint8_t caben = util_minimo(n, (uint8_t)(RESPUESTA_LONGITUD_MAXIMA - respuesta_longitud));

    for (uint8_t i = 0u; i < caben; i++) {
        respuesta[respuesta_longitud++] = datos[i];
    }
}

static uint16_t comando_verificar_pin(const uint8_t *apdu)
{
    if (apdu[COMANDO_POSICION_LC] > PIN_LONGITUD_MAXIMA) {
        return COMANDO_SW_LONGITUD_INCORRECTA;
    }
    if (pin_intentos_restantes() == 0u) {
        return COMANDO_SW_METODO_BLOQUEADO;
    }
    if (!pin_verificar(&apdu[COMANDO_POSICION_DATOS], apdu[COMANDO_POSICION_LC])) {
        return COMANDO_SW_SEGURIDAD_NO_SATISFECHA;
    }
    return COMANDO_SW_SIN_ERROR;
}

static uint16_t comando_cambiar_pin(const uint8_t *apdu, uint8_t longitud)
{
    uint8_t pin_actual[PIN_LONGITUD_MAXIMA];
    uint8_t pin_nuevo[PIN_LONGITUD_MAXIMA];

    if (longitud != COMANDO_LONGITUD_CAMBIO_PIN) {
        return COMANDO_SW_LONGITUD_INCORRECTA;
    }
    if (pin_intentos_restantes() == 0u) {
        return COMANDO_SW_METODO_BLOQUEADO;
    }
    memcpy(pin_actual, &apdu[COMANDO_POSICION_DATOS], PIN_LONGITUD_MAXIMA);
    if (!pin_verificar(pin_actual, PIN_LONGITUD_MAXIMA)) {
        memset(pin_actual, 0, sizeof pin_actual);
        return COMANDO_SW_SEGURIDAD_NO_SATISFECHA;
    }
    memcpy(pin_nuevo, &apdu[COMANDO_POSICION_DATOS + PIN_LONGITUD_MAXIMA], PIN_LONGITUD_MAXIMA);
    pin_modo_personalizacion(1u);
    if (!pin_cambiar(pin_nuevo, PIN_LONGITUD_MAXIMA)) {
        memset(pin_actual, 0, sizeof pin_actual);
        memset(pin_nuevo, 0, sizeof pin_nuevo);
        return COMANDO_SW_FALLO_MEMORIA;          /* el modo se queda abierto */
    }
    pin_modo_personalizacion(0u);
    memset(pin_actual, 0, sizeof pin_actual);
    memset(pin_nuevo, 0, sizeof pin_nuevo);
    return COMANDO_SW_SIN_ERROR;
}

static uint16_t comando_pagar(const uint8_t *apdu, uint8_t longitud)
{
    registro_pago pago;

    if (longitud != COMANDO_LONGITUD_IMPORTE) {
        return COMANDO_SW_LONGITUD_INCORRECTA;
    }
    if (!pin_verificado()) {
        return COMANDO_SW_SEGURIDAD_NO_SATISFECHA;
    }
    pago.importe = util_leer_u16(&apdu[COMANDO_POSICION_DATOS]);
    pago.saldo_anterior = saldo_leer();
    if (pago.importe == 0u) {
        return COMANDO_SW_DATOS_INCORRECTOS;
    }
    if (pago.importe > pago.saldo_anterior) {
        return COMANDO_SW_SALDO_INSUFICIENTE;
    }
    pago.saldo_nuevo = (uint16_t)(pago.saldo_anterior - pago.importe);
    saldo_escribir(pago.saldo_nuevo);
    registro_guardar_pago(&pago);
    return COMANDO_SW_SIN_ERROR;
}

static uint16_t comando_recargar(const uint8_t *apdu, uint8_t longitud)
{
    uint16_t importe;
    uint16_t saldo;

    if (longitud != COMANDO_LONGITUD_IMPORTE) {
        return COMANDO_SW_LONGITUD_INCORRECTA;
    }
    importe = util_leer_u16(&apdu[COMANDO_POSICION_DATOS]);
    saldo = saldo_leer();
    if (importe == 0u || saldo + importe > SALDO_MAXIMO) {
        return COMANDO_SW_DATOS_INCORRECTOS;
    }
    saldo_escribir((uint16_t)(saldo + importe));
    return COMANDO_SW_SIN_ERROR;
}

/* El saldo, seguido de su CRC para que el terminal compruebe que llega entero. */
static uint16_t comando_consultar(void)
{
    uint8_t bufer[SALDO_LONGITUD + COMANDO_CRC_LONGITUD];
    uint8_t longitud = SALDO_LONGITUD;
    uint16_t crc;

    util_escribir_u16(bufer, saldo_leer());
    crc = crc_calcular(bufer, longitud);
    util_escribir_u16(&bufer[SALDO_LONGITUD], crc);
    comando_anadir(bufer, sizeof bufer);
    return COMANDO_SW_SIN_ERROR;
}

static uint16_t comando_despachar(const uint8_t *apdu, uint8_t longitud)
{
    switch (apdu[COMANDO_POSICION_INS]) {
    case COMANDO_INS_VERIFICAR_PIN: return comando_verificar_pin(apdu);
    case COMANDO_INS_CAMBIAR_PIN:   return comando_cambiar_pin(apdu, longitud);
    case COMANDO_INS_PAGAR:         return comando_pagar(apdu, longitud);
    case COMANDO_INS_RECARGAR:      return comando_recargar(apdu, longitud);
    case COMANDO_INS_CONSULTAR:     return comando_consultar();
    default:                        return COMANDO_SW_INS_NO_ADMITIDA;
    }
}

uint16_t comando_procesar(const uint8_t *apdu, uint8_t longitud)
{
    uint8_t sw_bytes[COMANDO_SW_LONGITUD];
    uint16_t sw;

    respuesta_longitud = 0u;
    if (longitud < COMANDO_LONGITUD_CABECERA || longitud > COMANDO_LONGITUD_MAXIMA) {
        sw = COMANDO_SW_LONGITUD_INCORRECTA;
    } else {
        sw = comando_despachar(apdu, longitud);
    }
    util_escribir_u16(sw_bytes, sw);
    comando_anadir(sw_bytes, COMANDO_SW_LONGITUD);
    return sw;
}

const uint8_t *comando_ultima_respuesta(uint8_t *longitud)
{
    *longitud = respuesta_longitud;
    return respuesta;
}
