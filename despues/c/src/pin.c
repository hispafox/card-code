#include "pin.h"

#include "memoria.h"

#define PIN_DIRECCION_LONGITUD  0x0010u
#define PIN_DIRECCION_DIGITOS   0x0011u

static uint8_t intentos;

void pin_personalizar(const uint8_t *pin, uint8_t longitud)
{
    memoria_escribir(PIN_DIRECCION_LONGITUD, longitud);
    for (uint8_t i = 0u; i < longitud; i++) {
        memoria_escribir((uint16_t)(PIN_DIRECCION_DIGITOS + i), pin[i]);
    }
    pin_inicializar();
}

void pin_inicializar(void)
{
    intentos = PIN_INTENTOS_MAXIMOS;
}

uint8_t pin_verificar(const uint8_t *pin, uint8_t longitud)
{
    if (intentos == 0u) {
        return 0u;
    }
    intentos--;    /* el intento se gasta antes de comparar */
    if (longitud != memoria_leer(PIN_DIRECCION_LONGITUD)) {
        return 0u;
    }
    for (uint8_t i = 0u; i < longitud; i++) {
        if (pin[i] != memoria_leer((uint16_t)(PIN_DIRECCION_DIGITOS + i))) {
            return 0u;
        }
    }
    intentos = PIN_INTENTOS_MAXIMOS;
    return 1u;
}

uint8_t pin_intentos_restantes(void)
{
    return intentos;
}
