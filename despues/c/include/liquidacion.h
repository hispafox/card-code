#ifndef LIQUIDACION_H
#define LIQUIDACION_H

#define MAXMOV 16

enum {
    LIQUIDACION_PAGO = 1,
    LIQUIDACION_RECARGA = 2,
    LIQUIDACION_DEVOLUCION = 3
};

#define LIQUIDACION_N_NO_VALE         (-1)
#define LIQUIDACION_TIPO_DESCONOCIDO  (-2)

struct mov {
    unsigned char t;      /* LIQUIDACION_PAGO, LIQUIDACION_RECARGA o LIQUIDACION_DEVOLUCION */
    unsigned short imp;   /* importe en centimos */
};

extern unsigned short ult_comision;

/* Liquida los movimientos del dia. modo 1 = redondear la comision.
   Devuelve 0 si va bien, LIQUIDACION_N_NO_VALE o LIQUIDACION_TIPO_DESCONOCIDO. */
int liquidar(struct mov *m, int n, long *neto, unsigned short *com, int modo);

#endif
