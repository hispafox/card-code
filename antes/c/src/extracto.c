#include "extracto.h"

uint16_t extracto_generar(const extracto_movimiento *movs, uint8_t n,
                          uint8_t *resp, uint16_t tam, int con_totales)
{
    uint16_t pos = 0u;
    uint16_t i;
    uint16_t t1 = 0u; /* total de pagos */
    uint16_t t2 = 0u; /* total de recargas */
    uint16_t t3 = 0u; /* total de devoluciones */
    uint8_t  s = 0u;  /* suma de control */

    /* comprobar que cabe: 2 de cabecera, 3 por movimiento, 6 de totales y 1 de la suma */
    if (tam < 2 + 3 * n + (con_totales ? 6 : 0) + 1) {
        return 0u;
    }
    if (n > 8u) {
        return 0u;
    }

    /* cabecera: la version del formato y el numero de movimientos */
    resp[pos++] = 0x01u;
    resp[pos++] = n;

    for (i = 0u; i < n; i++) {
        /* la letra del tipo */
        switch (movs[i].tipo) {
        case 1: /* pago */
            resp[pos++] = 'P';
            break;
        case 2: /* recarga */
            resp[pos++] = 'R';
            break;
        case 3: /* devolucion */
            resp[pos++] = 'D';
            break;
        default:
            return 0u;
        }
        /* el importe, el byte alto primero */
        resp[pos++] = (uint8_t)(movs[i].importe >> 8);
        resp[pos++] = (uint8_t)(movs[i].importe & 0xFFu);
        if (con_totales) {
            switch (movs[i].tipo) {
            case 1:
                t1 = (uint16_t)(t1 + movs[i].importe);
                break;
            case 2:
                t2 = (uint16_t)(t2 + movs[i].importe);
                break;
            case 3:
                t3 = (uint16_t)(t3 + movs[i].importe);
                break;
            default:
                break;
            }
        }
    }

    if (con_totales) {
        /* los totales, el byte alto primero */
        resp[pos++] = (uint8_t)(t1 >> 8);
        resp[pos++] = (uint8_t)(t1 & 0xFFu);
        resp[pos++] = (uint8_t)(t2 >> 8);
        resp[pos++] = (uint8_t)(t2 & 0xFFu);
        resp[pos++] = (uint8_t)(t3 >> 8);
        resp[pos++] = (uint8_t)(t3 & 0xFFu);
    }

    /* suma de control: el o exclusivo de todo lo anterior */
    for (i = 0u; i < pos; i++) {
        s = (uint8_t)(s ^ resp[i]);
    }
    resp[pos++] = s;
    return pos;
}
