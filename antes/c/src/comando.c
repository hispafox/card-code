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
#define COMANDO_INS_PERSONALIZAR  0x50u

#define COMANDO_SW_SIN_ERROR                0x9000u
#define COMANDO_SW_FALLO_MEMORIA            0x6581u
#define COMANDO_SW_LONGITUD_INCORRECTA      0x6700u
#define COMANDO_SW_SEGURIDAD_NO_SATISFECHA  0x6982u
#define COMANDO_SW_METODO_BLOQUEADO         0x6983u
#define COMANDO_SW_SALDO_INSUFICIENTE       0x6985u
#define COMANDO_SW_DATOS_INCORRECTOS        0x6A80u
#define COMANDO_SW_INS_NO_ADMITIDA          0x6D00u
#define COMANDO_SW_SIN_DIAGNOSTICO          0x6F00u

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

/* La única traducción de los errores de la memoria a códigos de estado de la respuesta. */
static uint16_t comando_sw_de(memoria_estado estado)
{
    switch (estado) {
    case MEMORIA_SIN_ERROR:         return COMANDO_SW_SIN_ERROR;
    case MEMORIA_FALLO_ESCRITURA:   return COMANDO_SW_FALLO_MEMORIA;
    case MEMORIA_DATOS_CORROMPIDOS: return COMANDO_SW_FALLO_MEMORIA;
    }
    return COMANDO_SW_SIN_DIAGNOSTICO;
}

static uint16_t comando_verificar_pin(const uint8_t *apdu)
{
    uint8_t  lc = apdu[COMANDO_POSICION_LC];
    uint8_t  pin_recibido[lc];                  /* tantos bytes como diga el comando */
    uint16_t resultado = COMANDO_SW_SEGURIDAD_NO_SATISFECHA;

    if (pin_intentos_restantes() == 0u) {
        return COMANDO_SW_METODO_BLOQUEADO;
    }
    memcpy(pin_recibido, &apdu[COMANDO_POSICION_DATOS], lc);
    if (pin_verificar(pin_recibido, lc)) {
        resultado = COMANDO_SW_SIN_ERROR;
    }
    memset(pin_recibido, 0, lc);
    return resultado;
}

static uint16_t comando_cambiar_pin(const uint8_t *apdu, uint8_t longitud)
{
    uint8_t  pin_actual[PIN_LONGITUD_MAXIMA];
    uint8_t  pin_nuevo[PIN_LONGITUD_MAXIMA];
    uint16_t resultado = COMANDO_SW_SIN_DIAGNOSTICO;   /* hasta que todo haya salido bien */

    if (longitud != COMANDO_LONGITUD_CAMBIO_PIN) {
        return COMANDO_SW_LONGITUD_INCORRECTA;      /* aún no hay nada que limpiar */
    }
    if (pin_intentos_restantes() == 0u) {
        return COMANDO_SW_METODO_BLOQUEADO;          /* ni aquí */
    }
    memcpy(pin_actual, &apdu[COMANDO_POSICION_DATOS], PIN_LONGITUD_MAXIMA);
    memcpy(pin_nuevo, &apdu[COMANDO_POSICION_DATOS + PIN_LONGITUD_MAXIMA], PIN_LONGITUD_MAXIMA);

    if (!pin_verificar(pin_actual, PIN_LONGITUD_MAXIMA)) {
        resultado = COMANDO_SW_SEGURIDAD_NO_SATISFECHA;
        goto limpiar;
    }
    pin_modo_personalizacion(1u);
    if (!pin_cambiar(pin_nuevo, PIN_LONGITUD_MAXIMA)) {
        resultado = COMANDO_SW_FALLO_MEMORIA;
        goto limpiar;
    }
    resultado = COMANDO_SW_SIN_ERROR;                  /* solo se llega aquí por el camino normal */

limpiar:
    pin_modo_personalizacion(0u);
    memset(pin_actual, 0, sizeof pin_actual);
    memset(pin_nuevo, 0, sizeof pin_nuevo);
    return resultado;
}

static uint16_t comando_pagar(const uint8_t *apdu, uint8_t longitud)
{
    registro_pago  pago;
    memoria_estado estado;

    if (longitud != COMANDO_LONGITUD_IMPORTE) {
        return COMANDO_SW_LONGITUD_INCORRECTA;
    }
    if (!pin_verificado()) {
        return COMANDO_SW_SEGURIDAD_NO_SATISFECHA;
    }
    pago.importe = util_leer_u16(&apdu[COMANDO_POSICION_DATOS]);
    if (pago.importe == 0u) {
        return COMANDO_SW_DATOS_INCORRECTOS;
    }
    estado = saldo_leer(&pago.saldo_anterior);
    if (estado != MEMORIA_SIN_ERROR) {
        return comando_sw_de(estado);
    }
    if (pago.importe > pago.saldo_anterior) {
        return COMANDO_SW_SALDO_INSUFICIENTE;
    }
    pago.saldo_nuevo = (uint16_t)(pago.saldo_anterior - pago.importe);
    estado = saldo_escribir(pago.saldo_nuevo);
    if (estado == MEMORIA_SIN_ERROR) {
        estado = registro_guardar_pago(&pago);
    }
    return comando_sw_de(estado);
}

static uint16_t comando_recargar(const uint8_t *apdu, uint8_t longitud)
{
    uint16_t       importe;
    uint16_t       saldo;
    memoria_estado estado;

    if (longitud != COMANDO_LONGITUD_IMPORTE) {
        return COMANDO_SW_LONGITUD_INCORRECTA;
    }
    importe = util_leer_u16(&apdu[COMANDO_POSICION_DATOS]);
    estado = saldo_leer(&saldo);
    if (estado != MEMORIA_SIN_ERROR) {
        return comando_sw_de(estado);
    }
    if (importe == 0u || saldo + importe > SALDO_MAXIMO) {
        return COMANDO_SW_DATOS_INCORRECTOS;
    }
    return comando_sw_de(saldo_escribir((uint16_t)(saldo + importe)));
}

/* El saldo, seguido de su CRC para que el terminal compruebe que llega entero. */
static uint16_t comando_consultar(void)
{
    uint8_t        bufer[SALDO_LONGITUD + COMANDO_CRC_LONGITUD];
    uint8_t        longitud = SALDO_LONGITUD;
    uint16_t       saldo;
    uint16_t       crc;
    memoria_estado estado;

    estado = saldo_leer(&saldo);
    if (estado != MEMORIA_SIN_ERROR) {
        return comando_sw_de(estado);
    }
    util_escribir_u16(bufer, saldo);
    crc = crc_calcular(bufer, longitud);
    util_escribir_u16(&bufer[SALDO_LONGITUD], crc);
    comando_anadir(bufer, sizeof bufer);
    return COMANDO_SW_SIN_ERROR;
}

/* Los datos de la personalización: un byte de longitud y el PIN, y otro y el saldo inicial. */
typedef struct {
    const uint8_t *pin;
    uint8_t        pin_longitud;
    uint16_t       saldo_inicial;
} comando_personalizacion;

static void comando_leer_personalizacion(const uint8_t *datos, comando_personalizacion *c)
{
    const uint8_t *p = datos;

    c->pin_longitud = *p++;
    c->pin = p;
    p += c->pin_longitud;
    p++;                                        /* la longitud del saldo, que es siempre 2 */
    c->saldo_inicial = util_leer_u16(p);
}

/* La personalización de fábrica: solo se admite en una tarjeta sin PIN. */
static uint16_t comando_personalizar(const uint8_t *apdu)
{
    comando_personalizacion c;
    memoria_estado          estado;

    if (pin_personalizado()) {
        return COMANDO_SW_SEGURIDAD_NO_SATISFECHA;
    }
    comando_leer_personalizacion(&apdu[COMANDO_POSICION_DATOS], &c);
    if (c.pin_longitud == 0u || c.pin_longitud > PIN_LONGITUD_MAXIMA) {
        return COMANDO_SW_DATOS_INCORRECTOS;
    }
    estado = pin_personalizar(c.pin, c.pin_longitud);
    if (estado == MEMORIA_SIN_ERROR) {
        estado = saldo_escribir(c.saldo_inicial);
    }
    return comando_sw_de(estado);
}

static uint16_t comando_despachar(const uint8_t *apdu, uint8_t longitud)
{
    switch (apdu[COMANDO_POSICION_INS]) {
    case COMANDO_INS_VERIFICAR_PIN: return comando_verificar_pin(apdu);
    case COMANDO_INS_CAMBIAR_PIN:   return comando_cambiar_pin(apdu, longitud);
    case COMANDO_INS_PAGAR:         return comando_pagar(apdu, longitud);
    case COMANDO_INS_RECARGAR:      return comando_recargar(apdu, longitud);
    case COMANDO_INS_CONSULTAR:     return comando_consultar();
    case COMANDO_INS_PERSONALIZAR:  return comando_personalizar(apdu);
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
