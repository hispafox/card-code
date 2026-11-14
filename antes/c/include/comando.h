#ifndef COMANDO_H
#define COMANDO_H

#include <string.h>
#include "memoria.h"
#include "pin.h"

#define COMANDO_LONGITUD_MAXIMA  64u
#define COMANDO_REINTENTOS_LECTURA 3u

typedef struct {
    uint8_t *bufer;
    uint8_t  capacidad;
    uint8_t  longitud;
} comando_respuesta;

extern uint8_t comando_ultimo_ins;

uint16_t comando_procesar(const uint8_t *apdu, uint8_t longitud, comando_respuesta *respuesta);
void     comando_responder(comando_respuesta *respuesta, uint16_t sw,
                           const uint8_t *datos, uint8_t longitud_datos);
uint8_t  comando_copiar_datos(uint8_t *destino, const uint8_t *origen, uint8_t n);

#define COMANDO_SW_SIN_ERROR                0x9000u
#define COMANDO_SW_LONGITUD_INCORRECTA      0x6700u
#define COMANDO_SW_METODO_BLOQUEADO         0x6983u
#define COMANDO_SW_SEGURIDAD_NO_SATISFECHA  0x6982u
#define COMANDO_SW_SALDO_INSUFICIENTE       0x6985u
#define COMANDO_SW_INS_NO_ADMITIDA          0x6D00u

#endif
