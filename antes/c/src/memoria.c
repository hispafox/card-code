#include "memoria.h"

#include <string.h>

#define DIR_REGISTROS  0x0100u

static uint8_t memoria[TAM_MEMORIA];
static int fallo_simulado = 0;

void memoria_formatear(void)
{
    memset(memoria, 0, sizeof memoria);
    fallo_simulado = 0;
}

uint8_t mem_leer(uint16_t direccion)
{
    if (direccion >= TAM_MEMORIA) {
        return 0;
    }
    return memoria[direccion];
}

void mem_escribir(uint16_t direccion, uint8_t valor)
{
    if (direccion < TAM_MEMORIA) {
        memoria[direccion] = valor;
    }
}

static uint16_t direccion_registro(uint8_t numero, uint8_t posicion)
{
    return (uint16_t)(DIR_REGISTROS + numero * TAM_REGISTRO + posicion);
}

int memoria_escribir_byte(uint8_t numero, uint8_t posicion, uint8_t valor)
{
    if (fallo_simulado || numero >= NUM_REGISTROS || posicion >= TAM_REGISTRO) {
        return -1;
    }
    memoria[direccion_registro(numero, posicion)] = valor;
    return 0;
}

uint8_t memoria_leer_byte(uint8_t numero, uint8_t posicion)
{
    if (numero >= NUM_REGISTROS || posicion >= TAM_REGISTRO) {
        return 0;
    }
    return memoria[direccion_registro(numero, posicion)];
}

void memoria_simular_fallo(int activo)
{
    fallo_simulado = activo;
}
