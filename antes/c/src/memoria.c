#include "memoria.h"

#include <string.h>

uint8_t datos[TAMANO];
uint16_t escrituras;    /* cuántas escrituras lleva la memoria */

void iniciar(void)
{
    memset(datos, 0, sizeof datos);
    escrituras = 0;
}

uint8_t leer(uint16_t direccion)
{
    if (direccion >= TAMANO) {
        return 0;
    }
    return datos[direccion];
}

void escribir(uint16_t direccion, uint8_t valor)
{
    if (direccion < TAMANO) {
        datos[direccion] = valor;
        escrituras++;
    }
}
