#include "memoria.h"

#include <string.h>


extern uint16_t crc16_x64(const uint8_t *datos, uint16_t longitud);

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

uint16_t memoria_huella(void)
{
    const uint8_t *bufer = celdas;
    uint16_t longitud = MEMORIA_TAMANO;
    uint16_t crc;

    crc = crc16_x64(bufer, longitud);
    return crc;
}
