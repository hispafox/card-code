#include "comando.h"

#include "pin.h"
#include "saldo.h"

#define RESPUESTA_SALDO_LONGITUD 4u   /* dos del saldo y dos del código de estado */
#define SALDO_MAXIMO 50000

uint8_t respuesta[RESPUESTA_SALDO_LONGITUD];
uint8_t respuesta_longitud;
uint8_t estado;

static uint16_t validar(const uint8_t *b, uint8_t n)
{
    cabecera_t cabecera;
    uint8_t intentos = contador;

    cabecera.ins = b[1];
    cabecera.longitud = b[4];
    if (n > LONGITUD_MAX) {
        return 0x6700;
    }
    if (b[1] == 0x20) {
        if (n < 5 || b[4] > 8) {
            return 0x6700;
        }
        if (intentos == 0) {
            return 0x6982;
        }
    }
    if (cabecera.ins == 0x30 || cabecera.ins == 0x32) {
        if (n != 7 || cabecera.longitud != 2) {
            return 0x6700;
        }
    }
    return EXITO;
}

static uint16_t comando_verificar_pin(const uint8_t *apdu, uint8_t longitud)
{
    (void)longitud;
    estado = verificar(&apdu[5], apdu[4]);
    if (estado == 0) {
        return 0x6300;
    }
    return EXITO;
}

static uint16_t comando_pagar(const uint8_t *apdu, uint8_t longitud)
{
    (void)longitud;
    if (estado == 0) {
        return 0x6982;
    }
    uint16_t importe = (uint16_t)((apdu[5] << 8) | apdu[6]);
    uint16_t saldo = saldo_leer();
    if (importe > saldo) {
        return 0x6985;
    }
    saldo_escribir((uint16_t)(saldo - importe));
    return EXITO;
}

static uint16_t comando_recargar(const uint8_t *apdu, uint8_t longitud)
{
    (void)longitud;
    uint16_t importe = (uint16_t)((apdu[5] << 8) | apdu[6]);
    uint16_t saldo = saldo_leer();
    if (saldo + importe > SALDO_MAXIMO) {
        return 0x6A80;
    }
    saldo_escribir((uint16_t)(saldo + importe));
    return EXITO;
}

static uint16_t comando_despachar(const uint8_t *apdu, uint8_t longitud)
{
    switch (apdu[COMANDO_POSICION_INS]) {
    case COMANDO_INS_VERIFICAR_PIN: return comando_verificar_pin(apdu, longitud);
    case COMANDO_INS_PAGAR:         return comando_pagar(apdu, longitud);
    case COMANDO_INS_RECARGAR:      return comando_recargar(apdu, longitud);
    default:                        return COMANDO_SW_INS_NO_ADMITIDA;
    }
}

void comando_inicializar(void)
{
    estado = 0;
    respuesta_longitud = 0;
}

uint16_t comando_procesar(const uint8_t *apdu, uint8_t longitud)
{
    respuesta_longitud = 0;
    uint16_t sw = validar(apdu, longitud);
    if (sw == EXITO) {
        sw = comando_despachar(apdu, longitud);
    }
    respuesta[respuesta_longitud] = (uint8_t)(sw >> 8);
    respuesta[respuesta_longitud + 1] = (uint8_t)(sw & 0xFF);
    respuesta_longitud += 2;
    return sw;
}

const uint8_t *comando_respuesta(uint8_t *longitud)
{
    *longitud = respuesta_longitud;
    return respuesta;
}
