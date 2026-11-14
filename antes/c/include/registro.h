#ifndef REGISTRO_H
#define REGISTRO_H

#include "comando.h"

void registro_guardar_respuesta(const comando_respuesta *respuesta);

/* Añade a la respuesta un separador y el número de operaciones registradas. */
void registro_anadir_resumen(comando_respuesta *respuesta);

uint8_t registro_operaciones(void);
uint8_t registro_ultima_longitud(void);
uint8_t registro_llena(void);

#endif
