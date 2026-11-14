#ifndef CRC16_H
#define CRC16_H

#include <stddef.h>
#include <stdint.h>

/* CRC de 16 bits (polinomio 0x1021, valor inicial 0xFFFF) de un bloque de datos. */
uint16_t crc16_calcular(const uint8_t *datos, size_t longitud);

#endif
