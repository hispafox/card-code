#include "comando.h"

#include "pin.h"
#include "saldo.h"

#define IMPORTE_LONGITUD          2u
#define RESPUESTA_SALDO_LONGITUD (SALDO_LONGITUD + COMANDO_SW_LONGITUD)

static const uint16_t SALDO_MAXIMO = 50000u;

static uint8_t respuesta[RESPUESTA_SALDO_LONGITUD];
static uint8_t respuesta_longitud;
static uint8_t pin_comprobado;

static uint16_t importe_leer(const uint8_t *apdu)
{
    return (uint16_t)(((uint16_t)apdu[COMANDO_POSICION_DATOS] << 8) | apdu[COMANDO_POSICION_DATOS + 1u]);
}

static uint16_t comando_validar(const uint8_t *apdu, uint8_t longitud)
{
    if (longitud > COMANDO_LONGITUD_MAXIMA) {
        return COMANDO_SW_LONGITUD_INCORRECTA;
    }
    if (apdu[COMANDO_POSICION_INS] == COMANDO_INS_VERIFICAR_PIN) {
        if (longitud < COMANDO_LONGITUD_CABECERA
            || apdu[COMANDO_POSICION_LC] > PIN_LONGITUD_MAXIMA) {
            return COMANDO_SW_LONGITUD_INCORRECTA;
        }
        if (pin_intentos_restantes() == 0u) {
            return COMANDO_SW_METODO_BLOQUEADO;
        }
    }
    if (apdu[COMANDO_POSICION_INS] == COMANDO_INS_PAGAR
        || apdu[COMANDO_POSICION_INS] == COMANDO_INS_RECARGAR) {
        if (longitud != COMANDO_LONGITUD_CABECERA + IMPORTE_LONGITUD
            || apdu[COMANDO_POSICION_LC] != IMPORTE_LONGITUD) {
            return COMANDO_SW_LONGITUD_INCORRECTA;
        }
    }
    return COMANDO_SW_SIN_ERROR;
}

static uint16_t comando_verificar_pin(const uint8_t *apdu, uint8_t longitud)
{
    (void)longitud;
    pin_comprobado = pin_verificar(&apdu[COMANDO_POSICION_DATOS], apdu[COMANDO_POSICION_LC]);
    if (pin_comprobado == 0u) {
        return COMANDO_SW_PIN_INCORRECTO;
    }
    return COMANDO_SW_SIN_ERROR;
}

static uint16_t comando_pagar(const uint8_t *apdu, uint8_t longitud)
{
    (void)longitud;
    if (pin_comprobado == 0u) {
        return COMANDO_SW_SEGURIDAD_NO_SATISFECHA;
    }
    uint16_t importe = importe_leer(apdu);
    uint16_t saldo = saldo_leer();
    if (importe > saldo) {
        return COMANDO_SW_SALDO_INSUFICIENTE;
    }
    saldo_escribir((uint16_t)(saldo - importe));
    return COMANDO_SW_SIN_ERROR;
}

static uint16_t comando_recargar(const uint8_t *apdu, uint8_t longitud)
{
    (void)longitud;
    uint16_t importe = importe_leer(apdu);
    uint16_t saldo = saldo_leer();
    if (saldo + importe > SALDO_MAXIMO) {
        return COMANDO_SW_IMPORTE_EXCESIVO;
    }
    saldo_escribir((uint16_t)(saldo + importe));
    return COMANDO_SW_SIN_ERROR;
}

static uint16_t comando_consultar(const uint8_t *apdu, uint8_t longitud)
{
    (void)apdu;
    (void)longitud;
    uint16_t saldo = saldo_leer();
    respuesta[0] = (uint8_t)(saldo >> 8);
    respuesta[1] = (uint8_t)(saldo & 0xFFu);
    respuesta_longitud = SALDO_LONGITUD;
    return COMANDO_SW_SIN_ERROR;
}

static uint16_t comando_despachar(comando_ins ins, const uint8_t *apdu, uint8_t longitud)
{
    switch (ins) {
    case COMANDO_INS_VERIFICAR_PIN: return comando_verificar_pin(apdu, longitud);
    case COMANDO_INS_PAGAR:         return comando_pagar(apdu, longitud);
    case COMANDO_INS_RECARGAR:      return comando_recargar(apdu, longitud);
    case COMANDO_INS_CONSULTAR:     return comando_consultar(apdu, longitud);
    }
    return COMANDO_SW_INS_NO_ADMITIDA;
}

void comando_inicializar(void)
{
    pin_comprobado = 0u;
    respuesta_longitud = 0u;
}

uint16_t comando_procesar(const uint8_t *apdu, uint8_t longitud)
{
    respuesta_longitud = 0u;
    uint16_t sw = comando_validar(apdu, longitud);
    if (sw == COMANDO_SW_SIN_ERROR) {
        sw = comando_despachar((comando_ins)apdu[COMANDO_POSICION_INS], apdu, longitud);
    }
    respuesta[respuesta_longitud] = (uint8_t)(sw >> 8);
    respuesta[respuesta_longitud + 1u] = (uint8_t)(sw & 0xFFu);
    respuesta_longitud += COMANDO_SW_LONGITUD;
    return sw;
}

const uint8_t *comando_respuesta(uint8_t *longitud)
{
    *longitud = respuesta_longitud;
    return respuesta;
}
