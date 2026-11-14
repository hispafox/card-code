#include "registro.h"

#include "estados.h"
#include "memoria.h"
#include "pin.h"

static uint16_t datos_grabar(uint8_t numero, const uint8_t *datos, uint8_t longitud)
{
    for (uint8_t i = 0; i < longitud; i++) {
        if (memoria_escribir_byte(numero, i, datos[i]) != 0) {
            return SW_ERROR_MEMORIA;
        }
    }
    return SW_CORRECTO;
}

uint16_t registro_escribir(uint8_t numero, const uint8_t *datos, uint8_t longitud)
{
    if (!pin_esta_verificado()) {
        return SW_PIN_NO_VERIFICADO;
    }
    if (numero >= NUM_REGISTROS) {
        return SW_REGISTRO_NO_EXISTE;
    }
    if (longitud > TAM_REGISTRO) {
        return SW_LONGITUD_INCORRECTA;
    }
    return datos_grabar(numero, datos, longitud);
}
