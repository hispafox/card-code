#include "consultas.h"

#include "estados.h"
#include "saldo.h"

uint16_t saldo_consultar(uint8_t *respuesta, uint8_t *longitud, uint8_t con_historial)
{
    uint16_t saldo = saldo_leer();

    respuesta[0] = 0;
    respuesta[1] = (uint8_t)(saldo >> 8);
    respuesta[2] = (uint8_t)(saldo & 0xFFu);
    *longitud = LONGITUD_CABECERA;
    if (con_historial) {
        uint8_t movimientos = historial_leer(&respuesta[LONGITUD_CABECERA]);
        respuesta[0] = movimientos;
        *longitud = (uint8_t)(LONGITUD_CABECERA + movimientos * TAM_MOVIMIENTO);
    }
    return SW_CORRECTO;
}
