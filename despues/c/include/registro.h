#ifndef REGISTRO_H
#define REGISTRO_H

#include <stdint.h>

/* El último pago, tal como queda anotado en la memoria persistente. */
typedef struct {
    uint16_t importe;
    uint16_t saldo_anterior;
    uint16_t saldo_nuevo;
    uint8_t  terminal;
} registro_pago;

void registro_guardar_pago(const registro_pago *pago);
void registro_leer_pago(registro_pago *pago);

#endif
