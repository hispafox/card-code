#ifndef COMANDOS_H
#define COMANDOS_H

#include <stdint.h>

/*
 * Procesa un comando y devuelve su palabra de estado. «longitud» es el
 * número de bytes de datos que siguen a la cabecera (el Lc del comando).
 */
uint16_t comando_procesar(const uint8_t *apdu, uint8_t longitud);

#endif
