#include "historial.h"

#include "memoria.h"

#define DIR_HISTORIAL     0x0050u                 /* cuántos movimientos hay */
#define DIR_MOVIMIENTOS   (DIR_HISTORIAL + 1u)

static uint16_t direccion_movimiento(uint8_t indice)
{
    return (uint16_t)(DIR_MOVIMIENTOS + indice * TAM_MOVIMIENTO);
}

void historial_anotar(uint8_t tipo, uint16_t importe)
{
    uint8_t cuantos = mem_leer(DIR_HISTORIAL);

    /* Se desplazan los anteriores un hueco; el más antiguo se pierde. */
    for (uint8_t i = NUM_MOVIMIENTOS - 1u; i > 0; i--) {
        for (uint8_t b = 0; b < TAM_MOVIMIENTO; b++) {
            mem_escribir((uint16_t)(direccion_movimiento(i) + b),
                         mem_leer((uint16_t)(direccion_movimiento((uint8_t)(i - 1u)) + b)));
        }
    }
    mem_escribir(direccion_movimiento(0), tipo);
    mem_escribir((uint16_t)(direccion_movimiento(0) + 1u), (uint8_t)(importe >> 8));
    mem_escribir((uint16_t)(direccion_movimiento(0) + 2u), (uint8_t)(importe & 0xFFu));
    if (cuantos < NUM_MOVIMIENTOS) {
        mem_escribir(DIR_HISTORIAL, (uint8_t)(cuantos + 1u));
    }
}

uint8_t historial_leer(uint8_t *destino)
{
    uint8_t cuantos = mem_leer(DIR_HISTORIAL);

    for (uint8_t i = 0; i < cuantos * TAM_MOVIMIENTO; i++) {
        destino[i] = mem_leer((uint16_t)(DIR_MOVIMIENTOS + i));
    }
    return cuantos;
}
