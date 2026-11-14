#include "utilidades.h"

/* En la tarjeta, un fallo así la bloquearía; en el equipo de desarrollo se apunta. */
static uint16_t fallos;
static uint16_t ultimo_fallo_linea;

void fallo(uint16_t linea)
{
    fallos++;
    ultimo_fallo_linea = linea;
}
