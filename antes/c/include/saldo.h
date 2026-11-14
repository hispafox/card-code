#ifndef SALDO_H
#define SALDO_H

#include <stdint.h>

/* Los importes llegan en dos bytes, el más significativo primero. */
#define LONGITUD_IMPORTE  2u

uint16_t saldo_leer(void);
void     saldo_escribir(uint16_t saldo);

/* Suman o restan el importe que llega en los datos del comando. */
uint16_t recarga_aplicar(const uint8_t *datos, uint8_t longitud);
uint16_t pago_aplicar(const uint8_t *datos, uint8_t longitud);

#endif
