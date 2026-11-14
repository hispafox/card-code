#ifndef PIN_H
#define PIN_H

#include "comando.h"

#define PIN_LONGITUD_MAXIMA 8u
#define PIN_INTENTOS_MAXIMOS 3u

/* Graba el PIN del titular en la memoria persistente y deja los intentos al máximo. */
void     pin_personalizar(const uint8_t *pin, uint8_t longitud);

/* El comando de verificar el PIN. Devuelve un código de estado COMANDO_SW_... */
uint16_t pin_comprobar(const uint8_t *pin, uint8_t longitud);

uint8_t  pin_intentos_restantes(void);
uint8_t  pin_verificado(void);

#endif
