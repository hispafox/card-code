#include "registro.h"

#include "estados.h"
#include "memoria.h"
#include "pin.h"

uint16_t registro_escribir(uint8_t numero, const uint8_t *datos, uint8_t longitud)
{
    uint16_t sw = SW_CORRECTO;

    if (pin_esta_verificado()) {
        if (numero < NUM_REGISTROS) {
            if (longitud <= TAM_REGISTRO) {
                for (uint8_t i = 0; i < longitud; i++) {
                    if (memoria_escribir_byte(numero, i, datos[i]) != 0) {
                        sw = SW_ERROR_MEMORIA;
                        break;
                    }
                }
            } else {
                sw = SW_LONGITUD_INCORRECTA;
            }
        } else {
            sw = SW_REGISTRO_NO_EXISTE;
        }
    } else {
        sw = SW_PIN_NO_VERIFICADO;
    }
    return sw;
}
