#include "saldo.h"

#include "memoria.h"

uint16_t saldo_leer(void)
{
    return (uint16_t)(((uint16_t)leer(0x40) << 8) | leer(0x41));
}

void saldo_escribir(uint16_t saldo)
{
    escribir(0x40, (uint8_t)(saldo >> 8));
    escribir(0x41, (uint8_t)(saldo & 0xFF));
}
