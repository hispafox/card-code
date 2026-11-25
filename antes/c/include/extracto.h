#ifndef EXTRACTO_H
#define EXTRACTO_H

#include <stdint.h>

/* Un movimiento del registro: el tipo (1 pago, 2 recarga, 3 devolucion) y el importe en centimos. */
typedef struct {
    uint8_t  tipo;
    uint16_t importe;
} extracto_movimiento;

/* Escribe en 'resp' el extracto de los 'n' movimientos, y los totales si 'con_totales' no es 0.
   Devuelve cuantos bytes ha escrito, o 0 si no caben o hay un tipo desconocido. */
uint16_t extracto_generar(const extracto_movimiento *movs, uint8_t n,
                          uint8_t *resp, uint16_t tam, int con_totales);

#endif /* EXTRACTO_H */
