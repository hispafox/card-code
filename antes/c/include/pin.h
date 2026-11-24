#ifndef PIN_H
#define PIN_H

#include <stdint.h>

#include "memoria.h"

#define PIN_LONGITUD_MAXIMA 8u
#define PIN_INTENTOS_MAXIMOS 3u

/* Graba el PIN del titular en la memoria persistente y deja los intentos al máximo. */
memoria_estado pin_personalizar(const uint8_t *pin, uint8_t longitud);

uint8_t pin_verificar(const uint8_t *pin, uint8_t longitud);
uint8_t pin_intentos_restantes(void);
uint8_t pin_verificado(void);

/*
 * Cambia el PIN. Solo se admite en el modo de personalización, y entonces el
 * PIN nuevo tiene que tener la longitud máxima. Devuelve 1 si lo ha cambiado,
 * y 0 si no se admite o si ha fallado la escritura.
 */
void    pin_modo_personalizacion(uint8_t activo);
uint8_t pin_cambiar(const uint8_t *pin, uint8_t longitud);

#endif /* PIN_H */
