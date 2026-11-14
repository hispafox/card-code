#ifndef COMANDO_H
#define COMANDO_H

#include <stdint.h>

#define COMANDO_LONGITUD_MAXIMA  64u

typedef struct comando_respuesta comando_respuesta;

comando_respuesta *comando_respuesta_iniciar(uint8_t *bufer, uint8_t capacidad);

uint8_t comando_respuesta_longitud(const comando_respuesta *respuesta);
uint8_t comando_respuesta_llena(const comando_respuesta *respuesta);
void    comando_respuesta_anadir(comando_respuesta *respuesta,
                                 const uint8_t *datos, uint8_t n);

/* Procesa un comando y deja su respuesta en 'respuesta'.
   Devuelve el código de estado. 'apdu' no se modifica. */
uint16_t comando_procesar(const uint8_t *apdu, uint8_t longitud, comando_respuesta *respuesta);

/* Añade 'datos' a la respuesta y termina con el código de estado.
   Si no caben, deja la respuesta vacía y devuelve sin escribir. */
void     comando_responder(comando_respuesta *respuesta, uint16_t sw,
                           const uint8_t *datos, uint8_t longitud_datos);

#endif /* COMANDO_H */
