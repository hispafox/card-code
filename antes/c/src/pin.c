#include "pin.h"

#include "memoria.h"

uint8_t contador;

void personalizar(const uint8_t *pin, uint8_t longitud)
{
    escribir(0x10, longitud);
    for (uint8_t i = 0; i < longitud; i++) {
        escribir((uint16_t)(0x11 + i), pin[i]);
    }
    inicializar();
}

void inicializar(void)
{
    contador = 3;
}

uint8_t verificar(const uint8_t *pin, uint8_t longitud)
{
    if (contador == 0) {
        return 0;
    }
    contador--;    /* el intento se gasta antes de comparar */
    if (longitud != leer(0x10)) {
        return 0;
    }
    for (uint8_t i = 0; i < longitud; i++) {
        if (pin[i] != leer((uint16_t)(0x11 + i))) {
            return 0;
        }
    }
    contador = 3;
    return 1;
}
