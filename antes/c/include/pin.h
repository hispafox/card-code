#ifndef PIN_H
#define PIN_H

#include <stdint.h>

/* Graba el PIN del titular en la memoria persistente y deja los intentos al máximo. */
void    personalizar(const uint8_t *pin, uint8_t longitud);

void    inicializar(void);
uint8_t verificar(const uint8_t *pin, uint8_t longitud);
extern uint8_t contador;

/* Añadida después, ya con el prefijo del módulo. */
uint8_t pin_intentos_restantes(void);

#endif
