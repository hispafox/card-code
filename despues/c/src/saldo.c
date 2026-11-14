#include "saldo.h"

#include "memoria.h"

#define SALDO_DIRECCION  0x0040u

uint16_t saldo_leer(void)
{
    return (uint16_t)(((uint16_t)memoria_leer(SALDO_DIRECCION) << 8) | memoria_leer(SALDO_DIRECCION + 1u));
}

void saldo_escribir(uint16_t saldo)
{
    memoria_escribir(SALDO_DIRECCION, (uint8_t)(saldo >> 8));
    memoria_escribir(SALDO_DIRECCION + 1u, (uint8_t)(saldo & 0xFFu));
}
