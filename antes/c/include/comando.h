#ifndef COMANDO_H
#define COMANDO_H

#include <stdint.h>

#define LONGITUD_MAX 64
#define EXITO        0x9000
typedef struct {
    uint8_t ins;
    uint8_t longitud;
} cabecera_t;

#define COMANDO_POSICION_INS        1u
#define COMANDO_SW_INS_NO_ADMITIDA  0x6D00u

#define COMANDO_INS_VERIFICAR_PIN 0x20u
#define COMANDO_INS_PAGAR         0x30u
#define COMANDO_INS_RECARGAR      0x32u
#define COMANDO_INS_CONSULTAR     0x34u

/* Deja el procesado de comandos como al encender la tarjeta. */
void comando_inicializar(void);

/*
 * Procesa un comando completo («longitud» cuenta la cabecera y los datos)
 * y devuelve su código de estado.
 */
uint16_t comando_procesar(const uint8_t *apdu, uint8_t longitud);

/* La respuesta del último comando: los datos, si los hay, y el código de estado. */
const uint8_t *comando_respuesta(uint8_t *longitud);

#endif
