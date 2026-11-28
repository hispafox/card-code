#include "comandos.h"

#include <string.h>

#include "estados.h"
#include "pin.h"
#include "saldo.h"

static uint8_t bufer_datos[TAM_BUFER_DATOS];

static uint16_t comando_verificar_pin(const uint8_t *apdu, uint8_t longitud)
{
    if (longitud == 0) {
        return SW_LONGITUD_INCORRECTA;
    }
    if (longitud > TAM_BUFER_DATOS) {
        return SW_LONGITUD_INCORRECTA;
    }
    memcpy(bufer_datos, &apdu[OFFSET_DATOS], longitud);
    return pin_verificar(bufer_datos, longitud);
}

static uint16_t comando_pagar(const uint8_t *apdu, uint8_t longitud)
{
    if (longitud == 0) {
        return SW_LONGITUD_INCORRECTA;
    }
    memcpy(bufer_datos, &apdu[OFFSET_DATOS], longitud);
    return pago_aplicar(bufer_datos, longitud);
}

static uint16_t comando_recargar(const uint8_t *apdu, uint8_t longitud)
{
    if (longitud == 0 || longitud >= TAM_BUFER_DATOS + 1) {
        return SW_LONGITUD_INCORRECTA;
    }
    memcpy(bufer_datos, &apdu[OFFSET_DATOS], longitud);
    return recarga_aplicar(bufer_datos, longitud);
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
