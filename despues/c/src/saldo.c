#include "saldo.h"

#include "memoria.h"
#include "utilidades.h"

#define SALDO_DIRECCION  0x0040u

uint16_t saldo_leer(void)
{
    uint8_t bytes[SALDO_LONGITUD];

    bytes[0] = memoria_leer(SALDO_DIRECCION);
    bytes[1] = memoria_leer(SALDO_DIRECCION + 1u);
    return util_leer_u16(bytes);
}

void saldo_escribir(uint16_t saldo)
{
    uint8_t bytes[SALDO_LONGITUD];

    util_escribir_u16(bytes, saldo);
    memoria_escribir(SALDO_DIRECCION, bytes[0]);
    memoria_escribir(SALDO_DIRECCION + 1u, bytes[1]);
}
