#ifndef CONSULTAS_H
#define CONSULTAS_H

#include <stdint.h>

#include "historial.h"

/*
 * Respuesta: cuántos movimientos siguen, el saldo en dos bytes y, en la
 * consulta del historial, los movimientos.
 */
#define LONGITUD_CABECERA   3u
#define TAM_RESPUESTA_MAX   (LONGITUD_CABECERA + NUM_MOVIMIENTOS * TAM_MOVIMIENTO)

uint16_t saldo_consultar(uint8_t *respuesta, uint8_t *longitud);
uint16_t historial_consultar(uint8_t *respuesta, uint8_t *longitud);

#endif
