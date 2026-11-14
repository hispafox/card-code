#include "unity.h"

#include <string.h>

#include "comandos.h"
#include "memoria.h"
#include "pin.h"

/* Las pruebas solo usan los comandos: no saben cómo está hecho el módulo por dentro. */
#define INS_VERIFICAR_PIN  0x20u
#define INS_PAGAR          0x30u
#define INS_RECARGAR       0x32u

static const uint8_t PIN_TITULAR[LONGITUD_PIN] = {1, 2, 3, 4};
static const uint8_t PIN_EQUIVOCADO[LONGITUD_PIN] = {9, 9, 9, 9};

static uint16_t enviar(uint8_t ins, const uint8_t *datos, uint8_t longitud)
{
    uint8_t apdu[5 + 32] = {0x80, 0, 0, 0, 0};
    apdu[1] = ins;
    apdu[4] = longitud;
    if (longitud > 0) {
        memcpy(&apdu[5], datos, longitud);
    }
    return comando_procesar(apdu, longitud);
}

static uint16_t enviar_importe(uint8_t ins, uint16_t importe)
{
    uint8_t datos[2] = {(uint8_t)(importe >> 8), (uint8_t)(importe & 0xFFu)};
    return enviar(ins, datos, 2);
}

static void verificar_pin(void)
{
    enviar(INS_VERIFICAR_PIN, PIN_TITULAR, LONGITUD_PIN);
}

void setUp(void)
{
    memoria_formatear();
    pin_personalizar(PIN_TITULAR);
}

void tearDown(void) {}

static void test_el_pin_correcto_se_acepta(void)
{
    TEST_ASSERT_EQUAL_HEX16(0x9000, enviar(INS_VERIFICAR_PIN, PIN_TITULAR, LONGITUD_PIN));
}

static void test_el_pin_equivocado_se_rechaza(void)
{
    TEST_ASSERT_EQUAL_HEX16(0x6300, enviar(INS_VERIFICAR_PIN, PIN_EQUIVOCADO, LONGITUD_PIN));
}

static void test_tres_fallos_bloquean_el_pin(void)
{
    for (int i = 0; i < 3; i++) {
        enviar(INS_VERIFICAR_PIN, PIN_EQUIVOCADO, LONGITUD_PIN);
    }
    TEST_ASSERT_EQUAL_HEX16(0x6983, enviar(INS_VERIFICAR_PIN, PIN_TITULAR, LONGITUD_PIN));
}

static void test_un_acierto_devuelve_los_intentos(void)
{
    enviar(INS_VERIFICAR_PIN, PIN_EQUIVOCADO, LONGITUD_PIN);
    enviar(INS_VERIFICAR_PIN, PIN_EQUIVOCADO, LONGITUD_PIN);
    verificar_pin();
    enviar(INS_VERIFICAR_PIN, PIN_EQUIVOCADO, LONGITUD_PIN);
    enviar(INS_VERIFICAR_PIN, PIN_EQUIVOCADO, LONGITUD_PIN);
    TEST_ASSERT_EQUAL_HEX16(0x9000, enviar(INS_VERIFICAR_PIN, PIN_TITULAR, LONGITUD_PIN));
}

static void test_verificar_sin_datos_o_con_datos_de_mas_se_rechaza(void)
{
    uint8_t datos[17] = {0};
    TEST_ASSERT_EQUAL_HEX16(0x6700, enviar(INS_VERIFICAR_PIN, PIN_TITULAR, 0));
    TEST_ASSERT_EQUAL_HEX16(0x6700, enviar(INS_VERIFICAR_PIN, datos, sizeof datos));
}

static void test_se_paga_hasta_el_saldo_exacto_y_nada_mas(void)
{
    TEST_ASSERT_EQUAL_HEX16(0x9000, enviar_importe(INS_RECARGAR, 500));
    verificar_pin();
    TEST_ASSERT_EQUAL_HEX16(0x9000, enviar_importe(INS_PAGAR, 120));
    TEST_ASSERT_EQUAL_HEX16(0x9000, enviar_importe(INS_PAGAR, 380));
    TEST_ASSERT_EQUAL_HEX16(0x6985, enviar_importe(INS_PAGAR, 1));
}

static void test_recargar_sin_datos_o_con_datos_de_mas_se_rechaza(void)
{
    uint8_t datos[17] = {0};
    TEST_ASSERT_EQUAL_HEX16(0x6700, enviar(INS_RECARGAR, datos, 0));
    TEST_ASSERT_EQUAL_HEX16(0x6700, enviar(INS_RECARGAR, datos, sizeof datos));
}

static void test_una_recarga_que_desborda_el_saldo_se_rechaza(void)
{
    enviar_importe(INS_RECARGAR, 0xFFFF);
    TEST_ASSERT_EQUAL_HEX16(0x6A80, enviar_importe(INS_RECARGAR, 1));
}

static void test_pagar_sin_pin_se_rechaza(void)
{
    enviar_importe(INS_RECARGAR, 500);
    TEST_ASSERT_EQUAL_HEX16(0x6982, enviar_importe(INS_PAGAR, 120));
}

static void test_pagar_mas_del_saldo_se_rechaza(void)
{
    enviar_importe(INS_RECARGAR, 100);
    verificar_pin();
    TEST_ASSERT_EQUAL_HEX16(0x6985, enviar_importe(INS_PAGAR, 101));
}

static void test_pagar_sin_datos_o_con_datos_de_mas_se_rechaza(void)
{
    uint8_t datos[17] = {0};
    verificar_pin();
    TEST_ASSERT_EQUAL_HEX16(0x6700, enviar(INS_PAGAR, datos, 0));
    TEST_ASSERT_EQUAL_HEX16(0x6700, enviar(INS_PAGAR, datos, sizeof datos));
}

static void test_una_instruccion_desconocida_se_rechaza(void)
{
    TEST_ASSERT_EQUAL_HEX16(0x6D00, enviar(0x99, PIN_TITULAR, LONGITUD_PIN));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_el_pin_correcto_se_acepta);
    RUN_TEST(test_el_pin_equivocado_se_rechaza);
    RUN_TEST(test_tres_fallos_bloquean_el_pin);
    RUN_TEST(test_un_acierto_devuelve_los_intentos);
    RUN_TEST(test_verificar_sin_datos_o_con_datos_de_mas_se_rechaza);
    RUN_TEST(test_se_paga_hasta_el_saldo_exacto_y_nada_mas);
    RUN_TEST(test_recargar_sin_datos_o_con_datos_de_mas_se_rechaza);
    RUN_TEST(test_una_recarga_que_desborda_el_saldo_se_rechaza);
    RUN_TEST(test_pagar_sin_pin_se_rechaza);
    RUN_TEST(test_pagar_mas_del_saldo_se_rechaza);
    RUN_TEST(test_pagar_sin_datos_o_con_datos_de_mas_se_rechaza);
    RUN_TEST(test_una_instruccion_desconocida_se_rechaza);
    return UNITY_END();
}
