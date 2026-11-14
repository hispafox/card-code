#include "pin.h"

#include "estados.h"
#include "memoria.h"

#define DIR_PIN       0x0010u
#define DIR_INTENTOS  0x0020u

static int pin_verificado = 0;

static uint8_t intentos_leer(void)
{
    return mem_leer(DIR_INTENTOS);
}

static void intentos_escribir(uint8_t intentos)
{
    mem_escribir(DIR_INTENTOS, intentos);
}

/* Compara el PIN presentado con el guardado y apunta el resultado,
   que es lo que consulta pin_esta_verificado. */
static int pin_comparar(const uint8_t *pin, uint8_t longitud)
{
    pin_verificado = 0;
    if (longitud != LONGITUD_PIN) {
        return 0;
    }
    for (uint8_t i = 0; i < LONGITUD_PIN; i++) {
        if (pin[i] != mem_leer((uint16_t)(DIR_PIN + i))) {
            return 0;
        }
    }
    pin_verificado = 1;
    return 1;
}

void pin_personalizar(const uint8_t pin[LONGITUD_PIN])
{
    for (uint8_t i = 0; i < LONGITUD_PIN; i++) {
        mem_escribir((uint16_t)(DIR_PIN + i), pin[i]);
    }
    intentos_escribir(INTENTOS_MAX);
    pin_verificado = 0;
}

uint16_t pin_verificar(const uint8_t *pin, uint8_t longitud)
{
    uint8_t intentos = intentos_leer();

    if (intentos == 0) {
        return SW_PIN_BLOQUEADO;
    }
    intentos_escribir(intentos - 1);    /* el intento se gasta antes de comparar */
    if (!pin_comparar(pin, longitud)) {
        return SW_PIN_INCORRECTO;
    }
    intentos_escribir(INTENTOS_MAX);
    return SW_CORRECTO;
}

int pin_esta_verificado(void)
{
    return pin_verificado;
}
