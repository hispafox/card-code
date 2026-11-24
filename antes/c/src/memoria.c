#include "memoria.h"

#include <string.h>

#include "crc.h"

static uint8_t celdas[MEMORIA_TAMANO];
static uint8_t fallar_siguiente;

void memoria_inicializar(void)
{
    memset(celdas, 0, sizeof celdas);
    fallar_siguiente = 0u;
}

uint8_t memoria_leer(uint16_t direccion)
{
    if (direccion >= MEMORIA_TAMANO) {
        return 0u;
    }
    return celdas[direccion];
}

memoria_estado memoria_escribir(uint16_t direccion, uint8_t valor)
{
    if (fallar_siguiente) {
        fallar_siguiente = 0u;
        return MEMORIA_FALLO_ESCRITURA;
    }
    if (direccion >= MEMORIA_TAMANO) {
        return MEMORIA_FALLO_ESCRITURA;
    }
    celdas[direccion] = valor;
    return MEMORIA_SIN_ERROR;
}

void memoria_fallar_siguiente_escritura(void)
{
    fallar_siguiente = 1u;
}

uint16_t memoria_huella(void)
{
    const uint8_t *bufer = celdas;
    size_t longitud = MEMORIA_TAMANO;
    uint16_t crc;

    crc = crc_calcular(bufer, longitud);
    return crc;
}
