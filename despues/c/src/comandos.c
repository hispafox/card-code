#include "comandos.h"

#include <string.h>

#include "estados.h"
#include "pin.h"
#include "saldo.h"

/* Los importes llegan en dos bytes, el más significativo primero. */
#define LONGITUD_IMPORTE  2u
#define SALDO_MAX         0xFFFFu

static uint8_t bufer_datos[TAM_BUFER_DATOS];

static uint16_t datos_copiar(const uint8_t *apdu, uint8_t longitud)
{
    if (longitud == 0 || longitud > TAM_BUFER_DATOS) {
        return SW_LONGITUD_INCORRECTA;
    }
    memcpy(bufer_datos, &apdu[OFFSET_DATOS], longitud);
    return SW_CORRECTO;
}

static uint16_t importe_leer(const uint8_t *datos)
{
    return (uint16_t)(((uint16_t)datos[0] << 8) | datos[1]);
}

static uint16_t comando_verificar_pin(const uint8_t *apdu, uint8_t longitud)
{
    uint16_t sw = datos_copiar(apdu, longitud);
    if (sw != SW_CORRECTO) {
        return sw;
    }
    return pin_verificar(bufer_datos, longitud);
}

static uint16_t comando_pagar(const uint8_t *apdu, uint8_t longitud)
{
    uint16_t sw = datos_copiar(apdu, longitud);
    if (sw != SW_CORRECTO) {
        return sw;
    }
    if (!pin_esta_verificado()) {
        return SW_PIN_NO_VERIFICADO;
    }
    if (longitud != LONGITUD_IMPORTE) {
        return SW_DATOS_INCORRECTOS;
    }
    uint16_t importe = importe_leer(bufer_datos);
    uint16_t saldo = saldo_leer();
    if (importe > saldo) {
        return SW_SALDO_INSUFICIENTE;
    }
    saldo_escribir((uint16_t)(saldo - importe));
    return SW_CORRECTO;
}

static uint16_t comando_recargar(const uint8_t *apdu, uint8_t longitud)
{
    uint16_t sw = datos_copiar(apdu, longitud);
    if (sw != SW_CORRECTO) {
        return sw;
    }
    if (longitud != LONGITUD_IMPORTE) {
        return SW_DATOS_INCORRECTOS;
    }
    uint16_t importe = importe_leer(bufer_datos);
    uint16_t saldo = saldo_leer();
    if (importe > SALDO_MAX - saldo) {
        return SW_DATOS_INCORRECTOS;
    }
    saldo_escribir((uint16_t)(saldo + importe));
    return SW_CORRECTO;
}

static uint16_t comando_despachar(const uint8_t *apdu, uint8_t longitud)
{
    switch (apdu[OFFSET_INS]) {
    case INS_VERIFICAR_PIN:
        return comando_verificar_pin(apdu, longitud);
    case INS_PAGAR:
        return comando_pagar(apdu, longitud);
    case INS_RECARGAR:
        return comando_recargar(apdu, longitud);
    default:
        return SW_INS_NO_SOPORTADA;
    }
}

uint16_t comando_procesar(const uint8_t *apdu, uint8_t longitud)
{
    return comando_despachar(apdu, longitud);
}
