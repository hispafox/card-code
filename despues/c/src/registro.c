#include "registro.h"

#include "crc.h"
#include "memoria.h"
#include "utilidades.h"

#define REGISTRO_DIRECCION  0x0080u
#define REGISTRO_LONGITUD   6u    /* tres valores de dos bytes */

static memoria_estado registro_volcar(const uint8_t *bytes, uint8_t n)
{
    memoria_estado estado = MEMORIA_SIN_ERROR;

    for (uint8_t i = 0u; i < n && estado == MEMORIA_SIN_ERROR; i++) {
        estado = memoria_escribir((uint16_t)(REGISTRO_DIRECCION + i), bytes[i]);
    }
    return estado;
}

memoria_estado registro_guardar_pago(const registro_pago *pago)
{
    uint8_t bufer[REGISTRO_LONGITUD + 2u];
    uint8_t longitud = REGISTRO_LONGITUD;
    uint16_t crc;

    util_escribir_u16(&bufer[0], pago->importe);
    util_escribir_u16(&bufer[2], pago->saldo_anterior);
    util_escribir_u16(&bufer[4], pago->saldo_nuevo);
    crc = crc_calcular(bufer, longitud);
    util_escribir_u16(&bufer[REGISTRO_LONGITUD], crc);
    return registro_volcar(bufer, sizeof bufer);
}

memoria_estado registro_leer_pago(registro_pago *pago)
{
    uint8_t bufer[REGISTRO_LONGITUD + 2u];

    for (uint8_t i = 0u; i < sizeof bufer; i++) {
        bufer[i] = memoria_leer((uint16_t)(REGISTRO_DIRECCION + i));
    }
    if (crc_calcular(bufer, REGISTRO_LONGITUD) != util_leer_u16(&bufer[REGISTRO_LONGITUD])) {
        return MEMORIA_DATOS_CORROMPIDOS;
    }
    pago->importe = util_leer_u16(&bufer[0]);
    pago->saldo_anterior = util_leer_u16(&bufer[2]);
    pago->saldo_nuevo = util_leer_u16(&bufer[4]);
    return MEMORIA_SIN_ERROR;
}
