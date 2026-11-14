#ifndef MEMORIA_H
#define MEMORIA_H

#include <stdint.h>

/*
 * Memoria persistente de la tarjeta. En el equipo de desarrollo es un
 * array en RAM; en la tarjeta sería la EEPROM o la flash.
 */
#define TAMANO  512u

/* Deja toda la memoria a cero, como una tarjeta recién fabricada. */
void    iniciar(void);
uint8_t leer(uint16_t direccion);
void    escribir(uint16_t direccion, uint8_t valor);

#endif
