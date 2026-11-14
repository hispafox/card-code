#include <stdint.h>
#include "registro.h"

#define REGISTRO_DIRECCION_OPERACIONES  0x0080u
#define REGISTRO_DIRECCION_LONGITUD     0x0081u
#define REGISTRO_DIRECCION_LLENA        0x0082u
#define REGISTRO_SEPARADOR              0xFFu

static void registro_marcar_llena(void)
{
    memoria_escribir(REGISTRO_DIRECCION_LLENA, 1u);
}

void registro_guardar_respuesta(const comando_respuesta *respuesta)
{
    memoria_escribir(REGISTRO_DIRECCION_OPERACIONES, (uint8_t)(registro_operaciones() + 1u));
    memoria_escribir(REGISTRO_DIRECCION_LONGITUD, respuesta->longitud);
}

void registro_anadir_resumen(comando_respuesta *respuesta)
{
    uint8_t operaciones = registro_operaciones();

    if (respuesta->longitud >= 255u) {
        registro_marcar_llena();
    }
    respuesta->bufer[respuesta->longitud++] = REGISTRO_SEPARADOR;
    respuesta->bufer[respuesta->longitud++] = operaciones;
}

uint8_t registro_operaciones(void)
{
    return memoria_leer(REGISTRO_DIRECCION_OPERACIONES);
}

uint8_t registro_ultima_longitud(void)
{
    return memoria_leer(REGISTRO_DIRECCION_LONGITUD);
}

uint8_t registro_llena(void)
{
    return memoria_leer(REGISTRO_DIRECCION_LLENA);
}
