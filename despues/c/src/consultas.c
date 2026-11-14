#include "consultas.h"

#include "estados.h"
#include "saldo.h"

static uint8_t cabecera_preparar(uint8_t *respuesta, uint8_t movimientos)
{
    uint16_t saldo = saldo_leer();

    respuesta[0] = movimientos;
    respuesta[1] = (uint8_t)(saldo >> 8);
    respuesta[2] = (uint8_t)(saldo & 0xFFu);
    return LONGITUD_CABECERA;
}

uint16_t saldo_consultar(uint8_t *respuesta, uint8_t *longitud)
{
    *longitud = cabecera_preparar(respuesta, 0);
    return SW_CORRECTO;
}

uint16_t historial_consultar(uint8_t *respuesta, uint8_t *longitud)
{
    uint8_t movimientos = historial_leer(&respuesta[LONGITUD_CABECERA]);

    *longitud = (uint8_t)(cabecera_preparar(respuesta, movimientos) + movimientos * TAM_MOVIMIENTO);
    return SW_CORRECTO;
}
