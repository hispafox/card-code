#ifndef COMANDO_H
#define COMANDO_H

#include <stdint.h>

/* El comando: clase, instrucción, dos parámetros, Lc (la longitud de los datos) y los datos. */
#define COMANDO_POSICION_INS        1u
#define COMANDO_POSICION_LC         4u
#define COMANDO_POSICION_DATOS      5u
#define COMANDO_LONGITUD_CABECERA 5u
#define COMANDO_LONGITUD_MAXIMA  64u
#define COMANDO_SW_SIN_ERROR     0x9000u

/* Códigos de estado de la respuesta, con los valores de ISO/IEC 7816-4. */
#define COMANDO_SW_PIN_INCORRECTO           0x6300u
#define COMANDO_SW_LONGITUD_INCORRECTA      0x6700u
#define COMANDO_SW_SEGURIDAD_NO_SATISFECHA  0x6982u
#define COMANDO_SW_METODO_BLOQUEADO         0x6983u
#define COMANDO_SW_SALDO_INSUFICIENTE       0x6985u
#define COMANDO_SW_IMPORTE_EXCESIVO         0x6A80u
#define COMANDO_SW_INS_NO_ADMITIDA          0x6D00u
#define COMANDO_SW_LONGITUD      2u

typedef enum {
    COMANDO_INS_VERIFICAR_PIN = 0x20,
    COMANDO_INS_PAGAR         = 0x30,
    COMANDO_INS_RECARGAR      = 0x32,
    COMANDO_INS_CONSULTAR     = 0x34
} comando_ins;

typedef struct {
    uint8_t ins;
    uint8_t longitud;
} comando_cabecera;

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
