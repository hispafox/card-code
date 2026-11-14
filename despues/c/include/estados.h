#ifndef ESTADOS_H
#define ESTADOS_H

/* Formato del comando: CLA, INS, P1, P2, Lc y los datos. */
#define OFFSET_CLA    0u
#define OFFSET_INS    1u
#define OFFSET_P1     2u
#define OFFSET_P2     3u
#define OFFSET_LC     4u
#define OFFSET_DATOS  5u

/* Instrucciones que entiende la tarjeta. */
#define INS_VERIFICAR_PIN  0x20u
#define INS_PAGAR          0x30u
#define INS_RECARGAR       0x32u

/* Palabras de estado de la respuesta, con los valores de ISO/IEC 7816-4. */
#define SW_CORRECTO             0x9000u
#define SW_PIN_INCORRECTO       0x6300u
#define SW_ERROR_MEMORIA        0x6581u
#define SW_LONGITUD_INCORRECTA  0x6700u
#define SW_PIN_NO_VERIFICADO    0x6982u
#define SW_PIN_BLOQUEADO        0x6983u
#define SW_SALDO_INSUFICIENTE   0x6985u
#define SW_DATOS_INCORRECTOS    0x6A80u
#define SW_REGISTRO_NO_EXISTE   0x6A83u
#define SW_INS_NO_SOPORTADA     0x6D00u

/* Tamaño máximo de los datos de un comando. */
#define TAM_BUFER_DATOS  16u

#endif
