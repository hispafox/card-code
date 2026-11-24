#ifndef COMANDO_H
#define COMANDO_H

#include <stdint.h>

#define COMANDO_LONGITUD_MAXIMA  64u

/* Procesa un comando completo («longitud» cuenta la cabecera y los datos) y devuelve su código de estado. */
uint16_t comando_procesar(const uint8_t *apdu, uint8_t longitud);

/* La respuesta del último comando: los datos, si los hay, y el código de estado. */
const uint8_t *comando_ultima_respuesta(uint8_t *longitud);

#endif /* COMANDO_H */
