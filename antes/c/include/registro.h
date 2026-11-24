#ifndef REGISTRO_H
#define REGISTRO_H

#include <stdint.h>

#include "memoria.h"

/* El último pago, tal como queda anotado en la memoria persistente, con su CRC. */
typedef struct {
    uint16_t importe;
    uint16_t saldo_anterior;
    uint16_t saldo_nuevo;
} registro_pago;

memoria_estado registro_guardar_pago(const registro_pago *pago);
/* Lee el último pago. Devuelve 0 si su CRC no cuadra (la memoria se ha corrompido). */
uint8_t        registro_leer_pago(registro_pago *pago);

#endif /* REGISTRO_H */
