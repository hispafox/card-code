#ifndef HISTORIAL_H
#define HISTORIAL_H

#include <stdint.h>

/* Los últimos movimientos del saldo, el más reciente primero. */
#define NUM_MOVIMIENTOS     3u
#define TAM_MOVIMIENTO      3u    /* tipo y el importe en dos bytes */
#define MOVIMIENTO_RECARGA  0x01u
#define MOVIMIENTO_PAGO     0x02u

void historial_anotar(uint8_t tipo, uint16_t importe);

/* Copia los movimientos en «destino» y devuelve cuántos hay. */
uint8_t historial_leer(uint8_t *destino);

#endif
