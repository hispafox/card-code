#ifndef REGISTRO_H
#define REGISTRO_H

#include <stdint.h>

struct comando_respuesta;

/* Anota en la memoria persistente cuánto ocupó la última respuesta y cuenta las operaciones. */
void registro_guardar_respuesta(const struct comando_respuesta *respuesta);

/* Añade a la respuesta un separador y el número de operaciones registradas. */
void registro_anadir_resumen(struct comando_respuesta *respuesta);

uint8_t registro_operaciones(void);
uint8_t registro_ultima_longitud(void);
/* Si alguna vez se ha intentado añadir a una respuesta ya llena. */
uint8_t registro_llena(void);

#endif /* REGISTRO_H */
