#ifndef SALDO_H
#define SALDO_H

#include <stdint.h>

/* El saldo ocupa dos bytes de la memoria persistente, el alto primero. */
#define SALDO_LONGITUD  2u

uint16_t saldo_leer(void);
void     saldo_escribir(uint16_t saldo);

#endif /* SALDO_H */
