#include "registro.h"

#include "comando.h"
#include "memoria.h"

#define REGISTRO_DIRECCION_OPERACIONES  0x0080u
#define REGISTRO_DIRECCION_LONGITUD     0x0081u
#define REGISTRO_DIRECCION_LLENA        0x0082u

static const uint8_t registro_separador = 0xFFu;

static void registro_marcar_llena(void)
{
    memoria_escribir(REGISTRO_DIRECCION_LLENA, 1u);
}

void registro_guardar_respuesta(const struct comando_respuesta *respuesta)
{
    memoria_escribir(REGISTRO_DIRECCION_OPERACIONES, (uint8_t)(registro_operaciones() + 1u));
    memoria_escribir(REGISTRO_DIRECCION_LONGITUD, comando_respuesta_longitud(respuesta));
}

void registro_anadir_resumen(struct comando_respuesta *respuesta)
{
    uint8_t operaciones = registro_operaciones();

    if (comando_respuesta_llena(respuesta)) {
        registro_marcar_llena();
    }
    comando_respuesta_anadir(respuesta, &registro_separador, 1u);
    comando_respuesta_anadir(respuesta, &operaciones, 1u);
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
