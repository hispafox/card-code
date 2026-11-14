#ifndef UTILIDADES_H
#define UTILIDADES_H

#include <stdint.h>

#define MINIMO(a, b)        ((a) < (b) ? (a) : (b))
#define LEER_U16(p)         ((uint16_t)(((uint16_t)(p)[0] << 8) | (p)[1]))
#define ESCRIBIR_U16(p, v)  (p)[0] = (uint8_t)((v) >> 8); (p)[1] = (uint8_t)(v)
#define COMPROBAR(c)        if (!(c)) fallo(__LINE__)

/* Avisa de que una comprobación no se ha cumplido, con la línea. */
void fallo(uint16_t linea);

#endif /* UTILIDADES_H */
