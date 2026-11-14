#include "pin.h"

#include "memoria.h"

#define PIN_DIRECCION_LONGITUD  0x0010u
#define PIN_DIRECCION_DIGITOS   0x0011u

static uint8_t pin_intentos = PIN_INTENTOS_MAXIMOS;
static uint8_t pin_comprobado;

void pin_personalizar(const uint8_t *pin, uint8_t longitud)
{
    memoria_escribir(PIN_DIRECCION_LONGITUD, longitud);
    for (uint8_t i = 0u; i < longitud; i++) {
        memoria_escribir((uint16_t)(PIN_DIRECCION_DIGITOS + i), pin[i]);
    }
    pin_intentos = PIN_INTENTOS_MAXIMOS;
    pin_comprobado = 0u;
}

static uint8_t pin_coincide(const uint8_t *pin, uint8_t longitud)
{
    if (longitud != memoria_leer(PIN_DIRECCION_LONGITUD)) {
        return 0u;
    }
    for (uint8_t i = 0u; i < longitud; i++) {
        if (pin[i] != memoria_leer((uint16_t)(PIN_DIRECCION_DIGITOS + i))) {
            return 0u;
        }
    }
    return 1u;
}

uint16_t pin_comprobar(const uint8_t *pin, uint8_t longitud)
{
    pin_comprobado = 0u;
    if (longitud > PIN_LONGITUD_MAXIMA) {
        return COMANDO_SW_LONGITUD_INCORRECTA;
    }
    if (pin_intentos == 0u) {
        return COMANDO_SW_METODO_BLOQUEADO;
    }
    pin_intentos--;    /* el intento se gasta antes de comparar */
    if (!pin_coincide(pin, longitud)) {
        return COMANDO_SW_SEGURIDAD_NO_SATISFECHA;
    }
    pin_intentos = PIN_INTENTOS_MAXIMOS;
    pin_comprobado = 1u;
    return COMANDO_SW_SIN_ERROR;
}

uint8_t pin_intentos_restantes(void)
{
    return pin_intentos;
}

uint8_t pin_verificado(void)
{
    return pin_comprobado;
}
