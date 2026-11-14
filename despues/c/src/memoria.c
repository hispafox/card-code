#include "memoria.h"

#include <string.h>

static uint8_t celdas[MEMORIA_TAMANO];
static uint8_t fallo_simulado;

void memoria_inicializar(void)
{
    memset(celdas, 0, sizeof celdas);
    fallo_simulado = 0u;
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
    if (direccion < MEMORIA_TAMANO && fallo_simulado == 0u) {
        celdas[direccion] = valor;
    }
}

void memoria_simular_fallo(uint8_t activo)
{
    fallo_simulado = activo;
}
