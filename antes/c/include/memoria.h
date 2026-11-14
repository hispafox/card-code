#ifndef MEMORIA_H
#define MEMORIA_H

#include <stdint.h>

/*
 * Memoria persistente de la tarjeta. En el equipo de desarrollo es un
 * array en RAM; en la tarjeta sería la EEPROM o la flash.
 */
#define MEMORIA_TAMANO  512u

/* Deja toda la memoria a cero, como una tarjeta recién fabricada. */
void    memoria_inicializar(void);
uint8_t memoria_leer(uint16_t direccion);
void    memoria_escribir(uint16_t direccion, uint8_t valor);

/* Solo en el equipo de desarrollo: con 1, las escrituras fallan sin avisar. */
void    memoria_simular_fallo(uint8_t activo);

#endif
