#ifndef MEMORIA_H
#define MEMORIA_H

#include <stdint.h>

/*
 * Memoria persistente de la tarjeta. En el equipo de desarrollo es un
 * array en RAM; en la tarjeta sería la EEPROM o la flash.
 */
#define TAM_MEMORIA    512u
#define NUM_REGISTROS  4u
#define TAM_REGISTRO   16u

/* Deja toda la memoria a cero, como una tarjeta recién fabricada. */
void memoria_formatear(void);

uint8_t mem_leer(uint16_t direccion);
void    mem_escribir(uint16_t direccion, uint8_t valor);

/* Escribe un byte de un registro. Devuelve 0 si ha ido bien. */
int     memoria_escribir_byte(uint8_t numero, uint8_t posicion, uint8_t valor);
uint8_t memoria_leer_byte(uint8_t numero, uint8_t posicion);

/* Solo en el equipo de desarrollo: hace que las escrituras de registros fallen. */
void memoria_simular_fallo(int activo);

#endif
