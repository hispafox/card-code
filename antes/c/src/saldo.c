#include "saldo.h"

#include "crc.h"
#include "memoria.h"
#include "utilidades.h"

#define SALDO_DIRECCION  0x0040u

uint16_t saldo_leer(void)
{
    uint8_t bytes[SALDO_GUARDADO_LONGITUD];   /* el saldo y su CRC */

    for (uint8_t i = 0u; i < SALDO_GUARDADO_LONGITUD; i++) {
        bytes[i] = memoria_leer((uint16_t)(SALDO_DIRECCION + i));
    }
    if (crc_calcular(bytes, SALDO_LONGITUD) != util_leer_u16(&bytes[SALDO_LONGITUD])) {
        return 0u;                            /* el saldo no cuadra con su CRC */
    }
    return util_leer_u16(bytes);
}

memoria_estado saldo_escribir(uint16_t saldo)
{
    uint8_t        bytes[SALDO_GUARDADO_LONGITUD];
    memoria_estado estado = MEMORIA_SIN_ERROR;

    util_escribir_u16(bytes, saldo);
    util_escribir_u16(&bytes[SALDO_LONGITUD], crc_calcular(bytes, SALDO_LONGITUD));
    for (uint8_t i = 0u; i < SALDO_GUARDADO_LONGITUD && estado == MEMORIA_SIN_ERROR; i++) {
        estado = memoria_escribir((uint16_t)(SALDO_DIRECCION + i), bytes[i]);
    }
    return estado;
}
