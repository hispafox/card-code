#include "pin.h"

#include "memoria.h"
#include "utilidades.h"

#define PIN_DIRECCION_LONGITUD  0x0010u
#define PIN_DIRECCION_DIGITOS   0x0011u

static uint8_t pin_intentos = PIN_INTENTOS_MAXIMOS;
static uint8_t pin_comprobado;
static uint8_t modo_personalizacion;
static uint8_t pin_rechazado;

static void pin_rechazar(void)
{
    pin_rechazado = 1u;
}

void pin_personalizar(const uint8_t *pin, uint8_t longitud)
{
    memoria_escribir(PIN_DIRECCION_LONGITUD, longitud);
    for (uint8_t i = 0u; i < longitud; i++) {
        memoria_escribir((uint16_t)(PIN_DIRECCION_DIGITOS + i), pin[i]);
    }
    pin_intentos = PIN_INTENTOS_MAXIMOS;
    pin_comprobado = 0u;
}

uint8_t pin_verificar(const uint8_t *pin, uint8_t longitud)
{
    pin_comprobado = 0u;
    if (pin_intentos == 0u) {
        return 0u;
    }
    pin_intentos--;    /* el intento se gasta antes de comparar */
    if (longitud != memoria_leer(PIN_DIRECCION_LONGITUD)) {
        return 0u;
    }
    for (uint8_t i = 0u; i < longitud; i++) {
        if (pin[i] != memoria_leer((uint16_t)(PIN_DIRECCION_DIGITOS + i))) {
            return 0u;
        }
    }
    pin_intentos = PIN_INTENTOS_MAXIMOS;
    pin_comprobado = 1u;
    return 1u;
}

uint8_t pin_intentos_restantes(void)
{
    return pin_intentos;
}

uint8_t pin_verificado(void)
{
    return pin_comprobado;
}

void pin_modo_personalizacion(uint8_t activo)
{
    modo_personalizacion = activo;
}

uint8_t pin_cambiar(const uint8_t *pin, uint8_t longitud)
{
    pin_rechazado = 0u;
    if (modo_personalizacion)
        COMPROBAR(longitud == PIN_LONGITUD_MAXIMA);
    else
        pin_rechazar();
    if (pin_rechazado || longitud != PIN_LONGITUD_MAXIMA) {
        return 0u;
    }
    pin_personalizar(pin, longitud);
    return 1u;
}
