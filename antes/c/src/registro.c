#include "registro.h"

#include "memoria.h"
#include "utilidades.h"

#define REGISTRO_DIRECCION  0x0080u
#define REGISTRO_LONGITUD   6u    /* tres valores de dos bytes */

extern uint16_t crc16_x64(const uint8_t *datos, uint16_t longitud);

static void registro_volcar(const uint8_t *bytes, uint8_t n)
{
    for (uint8_t i = 0u; i < n; i++) {
        memoria_escribir((uint16_t)(REGISTRO_DIRECCION + i), bytes[i]);
    }
}

void registro_guardar_pago(const registro_pago *pago)
{
    uint8_t bufer[REGISTRO_LONGITUD + 2u];
    uint8_t longitud = REGISTRO_LONGITUD;
    uint16_t crc;

    ESCRIBIR_U16(&bufer[0], pago->importe);
    ESCRIBIR_U16(&bufer[2], pago->saldo_anterior);
    ESCRIBIR_U16(&bufer[4], pago->saldo_nuevo);
    crc = crc16_x64(bufer, longitud);
    ESCRIBIR_U16(&bufer[REGISTRO_LONGITUD], crc);
    registro_volcar(bufer, sizeof bufer);
}

uint8_t registro_leer_pago(registro_pago *pago)
{
    uint8_t bufer[REGISTRO_LONGITUD + 2u];

    for (uint8_t i = 0u; i < sizeof bufer; i++) {
        bufer[i] = memoria_leer((uint16_t)(REGISTRO_DIRECCION + i));
    }
    pago->importe = LEER_U16(&bufer[0]);
    pago->saldo_anterior = LEER_U16(&bufer[2]);
    pago->saldo_nuevo = LEER_U16(&bufer[4]);
    return (uint8_t)(crc16_x64(bufer, REGISTRO_LONGITUD) == LEER_U16(&bufer[REGISTRO_LONGITUD]));
}
