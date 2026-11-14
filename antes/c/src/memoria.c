#include <stdint.h>
#include "memoria.h"

#include <string.h>

static uint8_t celdas[MEMORIA_TAMANO];

void memoria_inicializar(void)
{
    memset(celdas, 0, sizeof celdas);
}

uint8_t memoria_leer(uint16_t direccion)
{
    if (direccion >= MEMORIA_TAMANO) {
        return 0u;
    }
    return celdas[direccion];
}

void memoria_escribir(uint16_t direccion, uint8_t valor)
{
    if (direccion < MEMORIA_TAMANO) {
        celdas[direccion] = valor;
    }
}
