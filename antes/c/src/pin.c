#include "pin.h"

#include "memoria.h"

extern int flag2;    /* está en comandos.c */

void pin_personalizar(const uint8_t pin[LONGITUD_PIN])
{
    for (uint8_t i = 0; i < LONGITUD_PIN; i++) {
        mem_escribir((uint16_t)(0x0010u + i), pin[i]);
    }
    mem_escribir(0x0020u, INTENTOS_MAX);
    flag2 = 0;
}
