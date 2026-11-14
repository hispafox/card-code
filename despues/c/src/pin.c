#include "pin.h"

#include <string.h>

#include "comando.h"
#include "memoria.h"

#define PIN_DIRECCION_LONGITUD    0x0010u
#define PIN_DIRECCION_DIGITOS     0x0011u
#define PIN_DIRECCION_VERIFICADO  0x0020u

static uint8_t pin_intentos = PIN_INTENTOS_MAXIMOS;

uint8_t pin_intentos_restantes(void)
{
    return pin_intentos;
}

void pin_personalizar(const uint8_t *pin, uint8_t longitud)
{
    memoria_escribir(PIN_DIRECCION_LONGITUD, longitud);
    for (uint8_t i = 0u; i < longitud; i++) {
        memoria_escribir((uint16_t)(PIN_DIRECCION_DIGITOS + i), pin[i]);
    }
    memoria_escribir(PIN_DIRECCION_VERIFICADO, 0u);
    pin_inicializar();
}

void pin_inicializar(void)
{
    pin_intentos = PIN_INTENTOS_MAXIMOS;
}

uint8_t pin_verificar(const uint8_t *pin, uint8_t longitud)
{
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
    return 1u;
}

uint8_t pin_verificado(void)
{
    return (uint8_t)(memoria_leer(PIN_DIRECCION_VERIFICADO) == 1u);
}

uint8_t pin_marcar_verificado(void)
{
    memoria_escribir(PIN_DIRECCION_VERIFICADO, 1u);
    return pin_verificado();    /* si la escritura falló, no queda anotado */
}

uint16_t pin_comprobar_comando(const uint8_t *datos, uint8_t longitud)
{
    uint8_t  pin_recibido[PIN_LONGITUD_MAXIMA];
    uint16_t resultado = COMANDO_SW_SIN_ERROR;

    if (longitud > PIN_LONGITUD_MAXIMA) {
        return COMANDO_SW_LONGITUD_INCORRECTA;       /* aún no hay nada que borrar */
    }
    if (pin_intentos_restantes() == 0u) {
        return COMANDO_SW_METODO_BLOQUEADO;          /* ni aquí */
    }
    memcpy(pin_recibido, datos, longitud);
    if (!pin_verificar(pin_recibido, longitud)) {
        resultado = COMANDO_SW_SEGURIDAD_NO_SATISFECHA;
    } else if (!pin_marcar_verificado()) {
        resultado = COMANDO_SW_FALLO_MEMORIA;
    }
    memset(pin_recibido, 0, sizeof pin_recibido);
    return resultado;
}
