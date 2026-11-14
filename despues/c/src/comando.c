#include "comando.h"     /* la propia cabecera, la primera */
#include <string.h>
#include "memoria.h"
#include "pin.h"

#include "comando_sw.h"
#include "registro.h"

#define COMANDO_REINTENTOS_LECTURA 3u

#define COMANDO_POSICION_INS        1u
#define COMANDO_POSICION_LC         4u
#define COMANDO_POSICION_DATOS      5u
#define COMANDO_LONGITUD_CABECERA 5u
#define COMANDO_LONGITUD_IMPORTE  (COMANDO_LONGITUD_CABECERA + 2u)
#define COMANDO_SW_LONGITUD       2u

#define COMANDO_INS_VERIFICAR_PIN 0x20u
#define COMANDO_INS_PAGAR         0x30u
#define COMANDO_INS_RECARGAR      0x32u
#define COMANDO_INS_CONSULTAR     0x34u

#define SALDO_DIRECCION  0x0040u

struct comando_respuesta {
    uint8_t *bufer;
    uint8_t  capacidad;
    uint8_t  longitud;
};

static comando_respuesta comando_respuesta_actual;

static uint8_t comando_ultimo_ins;

comando_respuesta *comando_respuesta_iniciar(uint8_t *bufer, uint8_t capacidad)
{
    comando_respuesta_actual.bufer     = bufer;
    comando_respuesta_actual.capacidad = capacidad;
    comando_respuesta_actual.longitud  = 0u;
    return &comando_respuesta_actual;
}

uint8_t comando_respuesta_longitud(const comando_respuesta *respuesta)
{
    return respuesta->longitud;
}

uint8_t comando_respuesta_llena(const comando_respuesta *respuesta)
{
    return (uint8_t)(respuesta->longitud >= respuesta->capacidad);
}

static uint8_t comando_copiar_datos(uint8_t *destino, const uint8_t *origen, uint8_t n)
{
    memcpy(destino, origen, n);
    return n;
}

void comando_respuesta_anadir(comando_respuesta *respuesta,
                              const uint8_t *datos, uint8_t n)
{
    if (n > respuesta->capacidad - respuesta->longitud) {
        return;    /* no cabe: no se escribe ni un byte */
    }
    respuesta->longitud = (uint8_t)(respuesta->longitud
                                    + comando_copiar_datos(&respuesta->bufer[respuesta->longitud], datos, n));
}

void comando_responder(comando_respuesta *respuesta, uint16_t sw,
                       const uint8_t *datos, uint8_t longitud_datos)
{
    const uint8_t sw_bytes[COMANDO_SW_LONGITUD] = { (uint8_t)(sw >> 8), (uint8_t)sw };

    uint8_t libre = (uint8_t)(respuesta->capacidad - respuesta->longitud);

    if ((uint16_t)(longitud_datos + COMANDO_SW_LONGITUD) > libre) {
        respuesta->longitud = 0u;
        return;
    }
    comando_respuesta_anadir(respuesta, datos, longitud_datos);
    comando_respuesta_anadir(respuesta, sw_bytes, COMANDO_SW_LONGITUD);
}

/* El saldo se lee dos veces hasta que las dos lecturas coinciden, con un máximo de intentos. */
static uint16_t comando_leer_saldo(void)
{
    uint16_t saldo = 0u;

    for (uint8_t i = 0u; i < COMANDO_REINTENTOS_LECTURA; i++) {
        uint16_t primera = (uint16_t)((memoria_leer(SALDO_DIRECCION) << 8) | memoria_leer(SALDO_DIRECCION + 1u));
        uint16_t segunda = (uint16_t)((memoria_leer(SALDO_DIRECCION) << 8) | memoria_leer(SALDO_DIRECCION + 1u));
        saldo = segunda;
        if (primera == segunda) {
            break;
        }
    }
    return saldo;
}

static void comando_escribir_saldo(uint16_t saldo)
{
    memoria_escribir(SALDO_DIRECCION, (uint8_t)(saldo >> 8));
    memoria_escribir(SALDO_DIRECCION + 1u, (uint8_t)saldo);
}

static uint16_t comando_importe(const uint8_t *apdu)
{
    return (uint16_t)((apdu[COMANDO_POSICION_DATOS] << 8) | apdu[COMANDO_POSICION_DATOS + 1u]);
}

static uint16_t comando_pagar(const uint8_t *apdu, uint8_t longitud)
{
    uint16_t saldo;

    if (longitud != COMANDO_LONGITUD_IMPORTE) {
        return COMANDO_SW_LONGITUD_INCORRECTA;
    }
    if (!pin_verificado()) {
        return COMANDO_SW_SEGURIDAD_NO_SATISFECHA;
    }
    saldo = comando_leer_saldo();
    if (comando_importe(apdu) > saldo) {
        return COMANDO_SW_SALDO_INSUFICIENTE;
    }
    comando_escribir_saldo((uint16_t)(saldo - comando_importe(apdu)));
    return COMANDO_SW_SIN_ERROR;
}

static uint16_t comando_recargar(const uint8_t *apdu, uint8_t longitud)
{
    if (longitud != COMANDO_LONGITUD_IMPORTE) {
        return COMANDO_SW_LONGITUD_INCORRECTA;
    }
    comando_escribir_saldo((uint16_t)(comando_leer_saldo() + comando_importe(apdu)));
    return COMANDO_SW_SIN_ERROR;
}

uint16_t comando_procesar(const uint8_t *apdu, uint8_t longitud, comando_respuesta *respuesta)
{
    uint16_t sw;

    if (longitud < COMANDO_LONGITUD_CABECERA || longitud > COMANDO_LONGITUD_MAXIMA) {
        comando_responder(respuesta, COMANDO_SW_LONGITUD_INCORRECTA, NULL, 0u);
        return COMANDO_SW_LONGITUD_INCORRECTA;
    }
    comando_ultimo_ins = apdu[COMANDO_POSICION_INS];
    switch (comando_ultimo_ins) {
    case COMANDO_INS_VERIFICAR_PIN:
        sw = pin_comprobar(&apdu[COMANDO_POSICION_DATOS], apdu[COMANDO_POSICION_LC]);
        break;
    case COMANDO_INS_PAGAR:
        sw = comando_pagar(apdu, longitud);
        break;
    case COMANDO_INS_RECARGAR:
        sw = comando_recargar(apdu, longitud);
        break;
    case COMANDO_INS_CONSULTAR: {
        uint16_t saldo = comando_leer_saldo();
        const uint8_t saldo_bytes[2] = { (uint8_t)(saldo >> 8), (uint8_t)saldo };

        comando_respuesta_anadir(respuesta, saldo_bytes, sizeof saldo_bytes);
        registro_anadir_resumen(respuesta);
        sw = COMANDO_SW_SIN_ERROR;
        break;
    }
    default:
        sw = COMANDO_SW_INS_NO_ADMITIDA;
        break;
    }
    comando_responder(respuesta, sw, NULL, 0u);
    registro_guardar_respuesta(respuesta);
    return sw;
}
