#include "comando.h"

#include "memoria.h"
#include "pin.h"
#include "registro.h"

#define SALDO_DIRECCION           0x0040u
#define RESPUESTA_LONGITUD_MAXIMA 4u
#define CLAVE_CIFRADO             0x5Au   /* cifrado de juguete, solo para el ejemplo */

static const uint16_t SALDO_MAXIMO = 50000u;

static uint8_t respuesta[RESPUESTA_LONGITUD_MAXIMA];
static uint8_t longitud_respuesta;

uint8_t  construir_respuesta(uint8_t *bufer, uint16_t sw) { bufer[0] = (uint8_t)(sw >> 8); bufer[1] = (uint8_t)sw; return 2u; }
uint8_t  comprobar_longitud(uint8_t longitud, uint8_t esperada) { return (uint8_t)(longitud == esperada); }

void copiar_datos(uint8_t *destino, const uint8_t *datos, uint8_t longitud_datos, uint8_t cifrar)
{
    for (uint8_t i = 0u; i < longitud_datos; i++) {
        destino[i] = cifrar ? (uint8_t)(datos[i] ^ CLAVE_CIFRADO) : datos[i];
    }
}

void comando_responder(uint8_t *respuesta, uint8_t *longitud_respuesta, uint16_t sw,
                       const uint8_t *datos, uint8_t longitud_datos, uint8_t cifrar)
{
    copiar_datos(respuesta, datos, longitud_datos, cifrar);
    *longitud_respuesta = (uint8_t)(longitud_datos + construir_respuesta(&respuesta[longitud_datos], sw));
}

uint16_t comando_pagar(const uint8_t *apdu, uint8_t longitud)
{
    uint16_t importe;
    uint16_t saldo;

    if (longitud == COMANDO_LONGITUD_PAGO) {
        if (pin_verificado()) {
            importe = (uint16_t)(((uint16_t)apdu[5] << 8) | apdu[6]);
            if (importe != 0u) {
                saldo = (uint16_t)(((uint16_t)memoria_leer(SALDO_DIRECCION) << 8)
                                   | memoria_leer(SALDO_DIRECCION + 1u));
                if (importe <= saldo) {
                    saldo = (uint16_t)(saldo - importe);
                    memoria_escribir(SALDO_DIRECCION, (uint8_t)(saldo >> 8));
                    memoria_escribir(SALDO_DIRECCION + 1u, (uint8_t)saldo);
                    return COMANDO_SW_SIN_ERROR;
                } else {
                    return COMANDO_SW_SALDO_INSUFICIENTE;
                }
            } else {
                return COMANDO_SW_DATOS_INCORRECTOS;
            }
        } else {
            return COMANDO_SW_SEGURIDAD_NO_SATISFECHA;
        }
    } else {
        return COMANDO_SW_LONGITUD_INCORRECTA;
    }
}

uint16_t comando_recargar(const uint8_t *apdu, uint8_t longitud)
{
    uint16_t importe;
    uint16_t saldo;

    if (longitud == COMANDO_LONGITUD_PAGO) {
        importe = (uint16_t)(((uint16_t)apdu[5] << 8) | apdu[6]);
        if (importe != 0u) {
            saldo = (uint16_t)(((uint16_t)memoria_leer(SALDO_DIRECCION) << 8)
                               | memoria_leer(SALDO_DIRECCION + 1u));
            if (saldo + importe <= SALDO_MAXIMO) {
                saldo = (uint16_t)(saldo + importe);
                memoria_escribir(SALDO_DIRECCION, (uint8_t)(saldo >> 8));
                memoria_escribir(SALDO_DIRECCION + 1u, (uint8_t)saldo);
                return COMANDO_SW_SIN_ERROR;
            } else {
                return COMANDO_SW_DATOS_INCORRECTOS;
            }
        } else {
            return COMANDO_SW_DATOS_INCORRECTOS;
        }
    } else {
        return COMANDO_SW_LONGITUD_INCORRECTA;
    }
}

/* Paga y, si ha ido bien, deja anotado el pago con el terminal que lo pidió (P1). */
uint16_t pagar_y_registrar(const uint8_t *apdu, uint8_t longitud)
{
    registro_pago pago;
    uint16_t sw;

    pago.saldo_anterior = (uint16_t)(((uint16_t)memoria_leer(SALDO_DIRECCION) << 8)
                                     | memoria_leer(SALDO_DIRECCION + 1u));
    sw = comando_pagar(apdu, longitud);
    if (sw == COMANDO_SW_SIN_ERROR) {
        pago.importe = (uint16_t)(((uint16_t)apdu[5] << 8) | apdu[6]);
        pago.saldo_nuevo = (uint16_t)(((uint16_t)memoria_leer(SALDO_DIRECCION) << 8)
                                      | memoria_leer(SALDO_DIRECCION + 1u));
        pago.terminal = apdu[2];
        registro_guardar_pago(pago.importe, pago.saldo_anterior,
                              pago.saldo_nuevo, pago.terminal);
    }
    return sw;
}

void consultar(void)
{
    uint8_t saldo_bytes[2];

    saldo_bytes[0] = memoria_leer(SALDO_DIRECCION);
    saldo_bytes[1] = memoria_leer(SALDO_DIRECCION + 1u);
    comando_responder(respuesta, &longitud_respuesta, COMANDO_SW_SIN_ERROR,
                      saldo_bytes, 2u, 0u);
}

void comando_inicializar(void)
{
    longitud_respuesta = 0u;
}

uint16_t comando_procesar(const uint8_t *apdu, uint8_t longitud)
{
    uint16_t sw;

    longitud_respuesta = 0u;
    if (longitud < COMANDO_LONGITUD_CABECERA) {
        sw = COMANDO_SW_LONGITUD_INCORRECTA;
    } else {
        switch ((comando_ins)apdu[COMANDO_POSICION_INS]) {
        case COMANDO_INS_VERIFICAR_PIN:
            sw = pin_comprobar_comando(&apdu[COMANDO_POSICION_DATOS], apdu[COMANDO_POSICION_LC]);
            break;
        case COMANDO_INS_PAGAR:
            sw = pagar_y_registrar(apdu, longitud);
            break;
        case COMANDO_INS_RECARGAR:
            sw = comando_recargar(apdu, longitud);
            break;
        case COMANDO_INS_CONSULTAR:
            consultar();
            return COMANDO_SW_SIN_ERROR;
        default:
            sw = COMANDO_SW_INS_NO_ADMITIDA;
            break;
        }
    }
    comando_responder(respuesta, &longitud_respuesta, sw, 0, 0u, 0u);
    return sw;
}

const uint8_t *comando_ultima_respuesta(uint8_t *longitud)
{
    *longitud = longitud_respuesta;
    return respuesta;
}
