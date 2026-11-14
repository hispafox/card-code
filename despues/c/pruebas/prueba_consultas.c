#include "unity.h"

#include "consultas.h"
#include "estados.h"
#include "memoria.h"
#include "pin.h"
#include "saldo.h"

static const uint8_t PIN_TITULAR[LONGITUD_PIN] = {1, 2, 3, 4};
static uint8_t respuesta[TAM_RESPUESTA_MAX];
static uint8_t longitud;

static void recargar(uint16_t importe)
{
    const uint8_t datos[LONGITUD_IMPORTE] = {(uint8_t)(importe >> 8), (uint8_t)(importe & 0xFFu)};
    recarga_aplicar(datos, LONGITUD_IMPORTE);
}

static void pagar(uint16_t importe)
{
    const uint8_t datos[LONGITUD_IMPORTE] = {(uint8_t)(importe >> 8), (uint8_t)(importe & 0xFFu)};
    pago_aplicar(datos, LONGITUD_IMPORTE);
}

void setUp(void)
{
    memoria_formatear();
    pin_personalizar(PIN_TITULAR);
    pin_verificar(PIN_TITULAR, LONGITUD_PIN);
}

void tearDown(void) {}

static void test_la_consulta_del_saldo_devuelve_solo_la_cabecera(void)
{
    recargar(0x0123);
    TEST_ASSERT_EQUAL_HEX16(SW_CORRECTO, saldo_consultar(respuesta, &longitud));
    TEST_ASSERT_EQUAL_UINT8(LONGITUD_CABECERA, longitud);
    TEST_ASSERT_EQUAL_HEX8(0x00, respuesta[0]);
    TEST_ASSERT_EQUAL_HEX8(0x01, respuesta[1]);
    TEST_ASSERT_EQUAL_HEX8(0x23, respuesta[2]);
}

static void test_el_historial_sin_movimientos(void)
{
    TEST_ASSERT_EQUAL_HEX16(SW_CORRECTO, historial_consultar(respuesta, &longitud));
    TEST_ASSERT_EQUAL_UINT8(LONGITUD_CABECERA, longitud);
    TEST_ASSERT_EQUAL_HEX8(0x00, respuesta[0]);
}

static void test_el_historial_trae_el_movimiento_mas_reciente_primero(void)
{
    recargar(500);
    pagar(120);
    TEST_ASSERT_EQUAL_HEX16(SW_CORRECTO, historial_consultar(respuesta, &longitud));
    TEST_ASSERT_EQUAL_UINT8(LONGITUD_CABECERA + 2u * TAM_MOVIMIENTO, longitud);
    TEST_ASSERT_EQUAL_HEX8(2, respuesta[0]);
    TEST_ASSERT_EQUAL_HEX8(MOVIMIENTO_PAGO, respuesta[3]);
    TEST_ASSERT_EQUAL_HEX8(120, respuesta[5]);
    TEST_ASSERT_EQUAL_HEX8(MOVIMIENTO_RECARGA, respuesta[6]);
}

static void test_el_historial_guarda_solo_los_ultimos(void)
{
    recargar(1);
    recargar(2);
    recargar(3);
    recargar(4);
    historial_consultar(respuesta, &longitud);
    TEST_ASSERT_EQUAL_HEX8(NUM_MOVIMIENTOS, respuesta[0]);
    TEST_ASSERT_EQUAL_HEX8(4, respuesta[5]);
    TEST_ASSERT_EQUAL_HEX8(2, respuesta[11]);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_la_consulta_del_saldo_devuelve_solo_la_cabecera);
    RUN_TEST(test_el_historial_sin_movimientos);
    RUN_TEST(test_el_historial_trae_el_movimiento_mas_reciente_primero);
    RUN_TEST(test_el_historial_guarda_solo_los_ultimos);
    return UNITY_END();
}
