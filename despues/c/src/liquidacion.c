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

static unsigned short liquidacion_redondear(unsigned short comision)
{
    unsigned short resto = comision % LIQUIDACION_REDONDEO;

    if (resto == 0)
        return comision;
    return comision + (LIQUIDACION_REDONDEO - resto);
}

/* Deja el neto y la comision, sin redondear. Con un error, no toca las salidas. */
static int liquidacion_acumular(struct mov *movimientos, int n, long *neto, unsigned short *comision)
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

    *neto = acumulado;
    *comision = comision_total;
    return 0;
}

int liquidar(struct mov *movimientos, int n, long *neto, unsigned short *comision, int modo)
{
    int resultado = liquidacion_acumular(movimientos, n, neto, comision);

    if (resultado != 0)
        return resultado;
    if (modo == 1)
        *comision = liquidacion_redondear(*comision);
    ult_comision = *comision;
    return 0;
}
