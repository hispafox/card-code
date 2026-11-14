#ifndef REGISTRO_H
#define REGISTRO_H

#include <stdint.h>

/* Escribe los datos en uno de los registros de la memoria persistente. */
uint16_t registro_escribir(uint8_t numero, const uint8_t *datos, uint8_t longitud);

#endif
