#include "crc16.h"

uint16_t crc16_calcular(const uint8_t *datos, size_t longitud)
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
