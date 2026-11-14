#ifndef PIN_H
#define PIN_H

#include <stdint.h>

#define LONGITUD_PIN  4u
#define INTENTOS_MAX  3u

/* Graba el PIN del titular y deja los intentos al máximo. */
void pin_personalizar(const uint8_t pin[LONGITUD_PIN]);

#endif
