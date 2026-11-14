#include "saldo.h"

#include "memoria.h"

/* El saldo ocupa dos bytes de la memoria persistente, el alto primero. */
#define DIR_SALDO  0x0040u

uint16_t saldo_leer(void)
{
    return (uint16_t)(((uint16_t)mem_leer(DIR_SALDO) << 8) | mem_leer(DIR_SALDO + 1u));
}

void saldo_escribir(uint16_t saldo)
{
    mem_escribir(DIR_SALDO, (uint8_t)(saldo >> 8));
    mem_escribir(DIR_SALDO + 1u, (uint8_t)(saldo & 0xFFu));
}
