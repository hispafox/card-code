#include "liquidacion.h"

#define LIQUIDACION_COMISION_DIVISOR  100   /* el 1 por ciento */
#define LIQUIDACION_COMISION_MINIMA   5
#define LIQUIDACION_REDONDEO          10    /* a la decena de arriba */

unsigned short ult_comision;

static unsigned short comision(unsigned short imp)
{
    unsigned short x = imp / LIQUIDACION_COMISION_DIVISOR;

    if (x < LIQUIDACION_COMISION_MINIMA)
        x = LIQUIDACION_COMISION_MINIMA;
    return x;
}

int liquidar(struct mov *m, int n, long *neto, unsigned short *com, int modo)
{
    int i;
    long tot = 0;
    unsigned short c = 0;

    if (n < 0 || n > MAXMOV)
        return LIQUIDACION_N_NO_VALE;

    for (i = 0; i < n; i++) {
        switch (m[i].t) {
        case LIQUIDACION_PAGO:
            tot = tot - m[i].imp;
            c = c + comision(m[i].imp);
            break;
        case LIQUIDACION_RECARGA:
            tot = tot + m[i].imp;
            break;
        case LIQUIDACION_DEVOLUCION:
            tot = tot + m[i].imp;
            c = c + comision(m[i].imp);
            break;
        default:
            return LIQUIDACION_TIPO_DESCONOCIDO;
        }
    }

    if (modo == 1) {
        if (c % LIQUIDACION_REDONDEO != 0)
            c = c + (LIQUIDACION_REDONDEO - c % LIQUIDACION_REDONDEO);
    }

    *neto = tot;
    *com = c;
    ult_comision = c;
    return 0;
}
