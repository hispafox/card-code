#include "comandos.h"

#include <string.h>

#include "memoria.h"

uint8_t buf2[16];
uint8_t resp[2];
int flag2;
int n2;
uint8_t tmp;
uint8_t tmp2;
uint8_t cnt;
uint8_t ok;
uint16_t saldo;
uint16_t imp;
uint16_t sw;
uint8_t ultimo_ins;
uint8_t ultimo_lc;
uint16_t num_cmd;
uint16_t num_err;
uint8_t flag3;
uint8_t buf3[4];
int res;
uint16_t aux;
uint8_t modo;

int chk(uint8_t *b, int n)
{
    int i;

    if (n != 4) {
        return 0;
    }
    for (i = 0; i < 4; i++) {
        if (b[i] != mem_leer((uint16_t)(0x10 + i))) {
            return 0;
        }
    }
    return 1;
}

void proc(uint8_t *b, int n)
{
    num_cmd++;
    sw = 0x9000;
    ultimo_ins = b[1];
    ultimo_lc = b[4];
    n2 = n;

    if (b[1] == 0x20) {
        /* verificar el PIN */
        if (n == 0 || n > 16) {
            sw = 0x6700;
        } else {
            memcpy(buf2, &b[5], n);
            cnt = mem_leer(0x20);
            if (cnt == 0) {
                sw = 0x6983;
            } else {
                mem_escribir(0x20, cnt - 1);
                ok = chk(buf2, n);
                if (ok) {
                    flag2 = 1;
                    mem_escribir(0x20, 3);
                } else {
                    flag2 = 0;
                    sw = 0x6300;
                }
            }
        }
    } else if (b[1] == 0x30) {
        /* pagar */
        if (n == 0 || n > 16) {
            sw = 0x6700;
        } else {
            memcpy(buf2, &b[5], n);
            if (!flag2) {
                sw = 0x6982;
            } else if (n != 2) {
                sw = 0x6A80;
            } else {
                imp = (uint16_t)((buf2[0] << 8) | buf2[1]);
                saldo = ((uint16_t)mem_leer(0x0040) << 8) | mem_leer(0x0041);
                if (imp > saldo) {
                    sw = 0x6985;
                } else {
                    saldo = saldo - imp;
                    tmp = (uint8_t)(saldo >> 8);
                    tmp2 = (uint8_t)(saldo & 0xFF);
                    mem_escribir(0x0040, tmp);
                    mem_escribir(0x0041, tmp2);
                }
            }
        }
    } else if (b[1] == 0x32) {
        /* recargar */
        if (n == 0 || n > 16) {
            sw = 0x6700;
        } else {
            memcpy(buf2, &b[5], n);
            if (n != 2) {
                sw = 0x6A80;
            } else {
                imp = (uint16_t)((buf2[0] << 8) | buf2[1]);
                saldo = ((uint16_t)mem_leer(0x0040) << 8) | mem_leer(0x0041);
                if (imp > 0xFFFF - saldo) {
                    sw = 0x6A80;
                } else {
                    saldo = saldo + imp;
                    tmp = (uint8_t)(saldo >> 8);
                    tmp2 = (uint8_t)(saldo & 0xFF);
                    mem_escribir(0x0040, tmp);
                    mem_escribir(0x0041, tmp2);
                }
            }
        }
    } else {
        sw = 0x6D00;
    }

    /* preparar la respuesta */
    if (sw != 0x9000) {
        num_err++;
    }
    resp[0] = (uint8_t)(sw >> 8);
    resp[1] = (uint8_t)(sw & 0xFF);
}

uint16_t comando_procesar(const uint8_t *apdu, uint8_t longitud)
{
    proc((uint8_t *)apdu, longitud);
    return (uint16_t)((resp[0] << 8) | resp[1]);
}
