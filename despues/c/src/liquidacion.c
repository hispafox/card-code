#include "liquidacion.h"

#define LIQUIDACION_COMISION_DIVISOR  100   /* el 1 por ciento */
#define LIQUIDACION_COMISION_MINIMA   5
#define LIQUIDACION_REDONDEO          10    /* a la decena de arriba */

unsigned short ult_comision;

static unsigned short liquidacion_comision(unsigned short importe)
{
    unsigned short comision = importe / LIQUIDACION_COMISION_DIVISOR;

    if (comision < LIQUIDACION_COMISION_MINIMA)
        comision = LIQUIDACION_COMISION_MINIMA;
    return comision;
}

int liquidar(struct mov *movimientos, int n, long *neto, unsigned short *comision, int modo)
{
    int i;
    long acumulado = 0;
    unsigned short comision_total = 0;

    if (n < 0 || n > MAXMOV)
        return LIQUIDACION_N_NO_VALE;

    for (i = 0; i < n; i++) {
        switch (movimientos[i].t) {
        case LIQUIDACION_PAGO:
            acumulado = acumulado - movimientos[i].imp;
            comision_total = comision_total + liquidacion_comision(movimientos[i].imp);
            break;
        case LIQUIDACION_RECARGA:
            acumulado = acumulado + movimientos[i].imp;
            break;
        case LIQUIDACION_DEVOLUCION:
            acumulado = acumulado + movimientos[i].imp;
            comision_total = comision_total + liquidacion_comision(movimientos[i].imp);
            break;
        default:
            return LIQUIDACION_TIPO_DESCONOCIDO;
        }
    }

    if (modo == 1) {
        if (comision_total % LIQUIDACION_REDONDEO != 0)
            comision_total = comision_total + (LIQUIDACION_REDONDEO - comision_total % LIQUIDACION_REDONDEO);
    }

    *neto = acumulado;
    *comision = comision_total;
    ult_comision = comision_total;
    return 0;
}
