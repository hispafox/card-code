#ifndef PIN_H
#define PIN_H

#include <stdint.h>

#define PIN_LONGITUD_MAXIMA   8u
#define PIN_INTENTOS_MAXIMOS  3u

/* Graba el PIN del titular en la memoria persistente y deja los intentos al máximo. */
void    pin_personalizar(const uint8_t *pin, uint8_t longitud);

void    pin_inicializar(void);
uint8_t pin_verificar(const uint8_t *pin, uint8_t longitud);
uint8_t pin_intentos_restantes(void);

#endif
