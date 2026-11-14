#include "crc.h"

#if defined(CRC_CON_ENSAMBLADOR)
uint16_t crc_calcular_x64(const uint8_t *datos, size_t longitud);   /* en crc_x64.S */
#else
static uint16_t crc_calcular_en_c(const uint8_t *datos, size_t longitud)
{
    uint16_t crc = 0xFFFFu;

    for (size_t i = 0; i < longitud; i++) {
        crc ^= (uint16_t)((uint16_t)datos[i] << 8);
        for (uint8_t bit = 0; bit < 8; bit++) {
            if ((crc & 0x8000u) != 0) {
                crc = (uint16_t)((crc << 1) ^ 0x1021u);
            } else {
                crc = (uint16_t)(crc << 1);
            }
        }
    }
    return crc;
}
#endif

uint16_t crc_calcular(const uint8_t *datos, size_t longitud)
{
#if defined(CRC_CON_ENSAMBLADOR)
    return crc_calcular_x64(datos, longitud);
#else
    return crc_calcular_en_c(datos, longitud);
#endif
}
