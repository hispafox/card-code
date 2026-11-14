#include "saldo.h"

#include "estados.h"
#include "historial.h"
#include "memoria.h"
#include "pin.h"

#define DIR_SALDO   0x0040u
#define SALDO_MAX   0xFFFFu

uint16_t saldo_leer(void)
{
    return (uint16_t)(((uint16_t)mem_leer(DIR_SALDO) << 8) | mem_leer(DIR_SALDO + 1u));
}

void saldo_escribir(uint16_t saldo)
{
    mem_escribir(DIR_SALDO, (uint8_t)(saldo >> 8));
    mem_escribir(DIR_SALDO + 1u, (uint8_t)(saldo & 0xFFu));
}

static uint16_t importe_leer(const uint8_t *datos)
{
    return (uint16_t)(((uint16_t)datos[0] << 8) | datos[1]);
}

uint16_t recarga_aplicar(const uint8_t *datos, uint8_t longitud)
{
    if (longitud != LONGITUD_IMPORTE) {
        return SW_DATOS_INCORRECTOS;
    }
    uint16_t importe = importe_leer(datos);
    uint16_t saldo = saldo_leer();
    if (importe > SALDO_MAX - saldo) {
        return SW_DATOS_INCORRECTOS;
    }
    saldo_escribir((uint16_t)(saldo + importe));
    historial_anotar(MOVIMIENTO_RECARGA, importe);
    return SW_CORRECTO;
}

uint16_t pago_aplicar(const uint8_t *datos, uint8_t longitud)
{
    if (!pin_esta_verificado()) {
        return SW_PIN_NO_VERIFICADO;
    }
    if (longitud != LONGITUD_IMPORTE) {
        return SW_DATOS_INCORRECTOS;
    }
    uint16_t importe = importe_leer(datos);
    uint16_t saldo = saldo_leer();
    if (importe > saldo) {
        return SW_SALDO_INSUFICIENTE;
    }
    saldo_escribir((uint16_t)(saldo - importe));
    historial_anotar(MOVIMIENTO_PAGO, importe);
    return SW_CORRECTO;
}
