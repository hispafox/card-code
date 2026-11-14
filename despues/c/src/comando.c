#include "comando.h"

#include <string.h>

#include "pin.h"
#include "registro.h"
#include "saldo.h"

#define RESPUESTA_LONGITUD_MAXIMA (SALDO_LONGITUD + COMANDO_SW_LONGITUD)
/* Cifrado de juguete, solo para el ejemplo: en una tarjeta sería el de la sesión segura. */
#define COMANDO_CLAVE_CIFRADO     0x5Au

static const uint16_t SALDO_MAXIMO = 50000u;

static uint8_t bufer_respuesta[RESPUESTA_LONGITUD_MAXIMA];
static comando_respuesta respuesta = { bufer_respuesta, RESPUESTA_LONGITUD_MAXIMA, 0u };

static uint8_t comando_construir_respuesta(uint8_t *bufer, uint16_t sw)
{
    bufer[0] = (uint8_t)(sw >> 8);
    bufer[1] = (uint8_t)sw;
    return COMANDO_SW_LONGITUD;
}

static void comando_copiar_datos(comando_respuesta *respuesta, const uint8_t *datos,
                                 uint8_t longitud_datos, uint8_t clave)
{
    for (uint8_t i = 0u; i < longitud_datos; i++) {
        respuesta->bufer[respuesta->longitud + i] = (uint8_t)(datos[i] ^ clave);
    }
    respuesta->longitud = (uint8_t)(respuesta->longitud + longitud_datos);
}

void comando_responder(comando_respuesta *respuesta, uint16_t sw,
                       const uint8_t *datos, uint8_t longitud_datos)
{
    comando_copiar_datos(respuesta, datos, longitud_datos, 0u);
    respuesta->longitud = (uint8_t)(respuesta->longitud
                                    + comando_construir_respuesta(&respuesta->bufer[respuesta->longitud], sw));
}

void comando_responder_cifrado(comando_respuesta *respuesta, uint16_t sw,
                               const uint8_t *datos, uint8_t longitud_datos)
{
    comando_copiar_datos(respuesta, datos, longitud_datos, COMANDO_CLAVE_CIFRADO);
    respuesta->longitud = (uint8_t)(respuesta->longitud
                                    + comando_construir_respuesta(&respuesta->bufer[respuesta->longitud], sw));
}

static uint16_t comando_leer_importe(const uint8_t *apdu)
{
    return (uint16_t)(((uint16_t)apdu[COMANDO_POSICION_DATOS] << 8)
                      | apdu[COMANDO_POSICION_DATOS + 1u]);
}

uint16_t comando_pagar(const uint8_t *apdu, uint8_t longitud)
{
    uint16_t importe;
    uint16_t saldo;

    if (longitud != COMANDO_LONGITUD_PAGO) {
        return COMANDO_SW_LONGITUD_INCORRECTA;
    }
    if (!pin_verificado()) {
        return COMANDO_SW_SEGURIDAD_NO_SATISFECHA;
    }
    importe = comando_leer_importe(apdu);
    if (importe == 0u) {
        return COMANDO_SW_DATOS_INCORRECTOS;
    }
    saldo = saldo_leer();
    if (importe > saldo) {
        return COMANDO_SW_SALDO_INSUFICIENTE;
    }
    saldo_escribir((uint16_t)(saldo - importe));
    return COMANDO_SW_SIN_ERROR;
}

uint16_t comando_recargar(const uint8_t *apdu, uint8_t longitud)
{
    uint16_t importe;
    uint16_t saldo;

    if (longitud != COMANDO_LONGITUD_PAGO) {
        return COMANDO_SW_LONGITUD_INCORRECTA;
    }
    importe = comando_leer_importe(apdu);
    if (importe == 0u) {
        return COMANDO_SW_DATOS_INCORRECTOS;
    }
    saldo = saldo_leer();
    if (saldo + importe > SALDO_MAXIMO) {
        return COMANDO_SW_DATOS_INCORRECTOS;
    }
    saldo_escribir((uint16_t)(saldo + importe));
    return COMANDO_SW_SIN_ERROR;
}

/* Paga y, si ha ido bien, deja anotado el pago con el terminal que lo pidió (P1). */
static uint16_t comando_pagar_y_registrar(const uint8_t *apdu, uint8_t longitud)
{
    registro_pago pago;
    uint16_t sw;

    pago.saldo_anterior = saldo_leer();
    sw = comando_pagar(apdu, longitud);
    if (sw == COMANDO_SW_SIN_ERROR) {
        pago.importe = comando_leer_importe(apdu);
        pago.saldo_nuevo = saldo_leer();
        pago.terminal = apdu[COMANDO_POSICION_P1];
        registro_guardar_pago(&pago);
    }
    return sw;
}

static void comando_consultar(void)
{
    uint16_t saldo = saldo_leer();
    uint8_t saldo_bytes[SALDO_LONGITUD] = { (uint8_t)(saldo >> 8), (uint8_t)saldo };

    comando_responder(&respuesta, COMANDO_SW_SIN_ERROR, saldo_bytes, SALDO_LONGITUD);
}

void comando_inicializar(void)
{
    respuesta.longitud = 0u;
}

uint16_t comando_procesar(const uint8_t *apdu, uint8_t longitud)
{
    uint16_t sw;

    respuesta.longitud = 0u;
    if (longitud < COMANDO_LONGITUD_CABECERA) {
        sw = COMANDO_SW_LONGITUD_INCORRECTA;
    } else {
        switch ((comando_ins)apdu[COMANDO_POSICION_INS]) {
        case COMANDO_INS_VERIFICAR_PIN:
            sw = pin_comprobar_comando(&apdu[COMANDO_POSICION_DATOS], apdu[COMANDO_POSICION_LC]);
            break;
        case COMANDO_INS_PAGAR:
            sw = comando_pagar_y_registrar(apdu, longitud);
            break;
        case COMANDO_INS_RECARGAR:
            sw = comando_recargar(apdu, longitud);
            break;
        case COMANDO_INS_CONSULTAR:
            comando_consultar();
            return COMANDO_SW_SIN_ERROR;
        default:
            sw = COMANDO_SW_INS_NO_ADMITIDA;
            break;
        }
    }
    comando_responder(&respuesta, sw, NULL, 0u);
    return sw;
}

const uint8_t *comando_ultima_respuesta(uint8_t *longitud)
{
    *longitud = respuesta.longitud;
    return respuesta.bufer;
}
