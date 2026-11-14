#include "registro.h"

#include "memoria.h"

#define REGISTRO_DIRECCION  0x0080u

static void registro_escribir_16(uint16_t direccion, uint16_t valor)
{
    memoria_escribir(direccion, (uint8_t)(valor >> 8));
    memoria_escribir((uint16_t)(direccion + 1u), (uint8_t)valor);
}

static uint16_t registro_leer_16(uint16_t direccion)
{
    return (uint16_t)(((uint16_t)memoria_leer(direccion) << 8) | memoria_leer((uint16_t)(direccion + 1u)));
}

void registro_guardar_pago(const registro_pago *pago)
{
    registro_escribir_16(REGISTRO_DIRECCION, pago->importe);
    registro_escribir_16(REGISTRO_DIRECCION + 2u, pago->saldo_anterior);
    registro_escribir_16(REGISTRO_DIRECCION + 4u, pago->saldo_nuevo);
    memoria_escribir(REGISTRO_DIRECCION + 6u, pago->terminal);
}

void registro_leer_pago(registro_pago *pago)
{
    pago->importe = registro_leer_16(REGISTRO_DIRECCION);
    pago->saldo_anterior = registro_leer_16(REGISTRO_DIRECCION + 2u);
    pago->saldo_nuevo = registro_leer_16(REGISTRO_DIRECCION + 4u);
    pago->terminal = memoria_leer(REGISTRO_DIRECCION + 6u);
}
