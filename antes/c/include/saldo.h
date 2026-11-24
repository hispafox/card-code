#ifndef SALDO_H
#define SALDO_H

#include <stdint.h>

#include "memoria.h"

/* El saldo ocupa dos bytes de la memoria persistente, el alto primero, y detrás va su CRC. */
#define SALDO_LONGITUD           2u
#define SALDO_GUARDADO_LONGITUD  (SALDO_LONGITUD + 2u)

/* Devuelve el saldo, o 0 si no cuadra con su CRC. */
uint16_t       saldo_leer(void);
memoria_estado saldo_escribir(uint16_t saldo);

#endif /* SALDO_H */
