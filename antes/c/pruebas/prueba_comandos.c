#include "unity.h"

#include <string.h>

#include "comandos.h"
#include "estados.h"
#include "memoria.h"
#include "pin.h"
#include "saldo.h"

static const uint8_t PIN_TITULAR[LONGITUD_PIN] = {1, 2, 3, 4};
static const uint8_t PIN_EQUIVOCADO[LONGITUD_PIN] = {9, 9, 9, 9};

/* Monta el comando con su cabecera y lo procesa. */
static uint16_t enviar(uint8_t ins, const uint8_t *datos, uint8_t longitud)
{
    uint8_t apdu[OFFSET_DATOS + 2u * TAM_BUFER_DATOS] = {0x80, 0, 0, 0, 0};
    apdu[OFFSET_INS] = ins;
    apdu[OFFSET_LC] = longitud;
    if (longitud > 0) {
        memcpy(&apdu[OFFSET_DATOS], datos, longitud);
    }
    return comando_procesar(apdu, longitud);
}

static uint16_t enviar_importe(uint8_t ins, uint16_t importe)
{
    uint8_t datos[LONGITUD_IMPORTE] = {(uint8_t)(importe >> 8), (uint8_t)(importe & 0xFFu)};
    return enviar(ins, datos, LONGITUD_IMPORTE);
}

void setUp(void)
{
    memoria_formatear();
    pin_personalizar(PIN_TITULAR);
}

void tearDown(void) {}

static void test_el_pin_correcto_se_acepta(void)
{
    TEST_ASSERT_EQUAL_HEX16(SW_CORRECTO, enviar(INS_VERIFICAR_PIN, PIN_TITULAR, LONGITUD_PIN));
}

static void test_el_pin_equivocado_se_rechaza(void)
{
    TEST_ASSERT_EQUAL_HEX16(SW_PIN_INCORRECTO, enviar(INS_VERIFICAR_PIN, PIN_EQUIVOCADO, LONGITUD_PIN));
}

static void test_tres_fallos_bloquean_el_pin(void)
{
    for (int i = 0; i < 3; i++) {
        enviar(INS_VERIFICAR_PIN, PIN_EQUIVOCADO, LONGITUD_PIN);
    }
    TEST_ASSERT_EQUAL_HEX16(SW_PIN_BLOQUEADO, enviar(INS_VERIFICAR_PIN, PIN_TITULAR, LONGITUD_PIN));
}

static void test_un_acierto_devuelve_los_intentos(void)
{
    enviar(INS_VERIFICAR_PIN, PIN_EQUIVOCADO, LONGITUD_PIN);
    enviar(INS_VERIFICAR_PIN, PIN_EQUIVOCADO, LONGITUD_PIN);
    enviar(INS_VERIFICAR_PIN, PIN_TITULAR, LONGITUD_PIN);
    enviar(INS_VERIFICAR_PIN, PIN_EQUIVOCADO, LONGITUD_PIN);
    enviar(INS_VERIFICAR_PIN, PIN_EQUIVOCADO, LONGITUD_PIN);
    TEST_ASSERT_EQUAL_HEX16(SW_CORRECTO, enviar(INS_VERIFICAR_PIN, PIN_TITULAR, LONGITUD_PIN));
}

static void test_verificar_sin_datos_se_rechaza(void)
{
    TEST_ASSERT_EQUAL_HEX16(SW_LONGITUD_INCORRECTA, enviar(INS_VERIFICAR_PIN, PIN_TITULAR, 0));
}

static void test_verificar_con_datos_de_mas_se_rechaza(void)
{
    uint8_t datos[TAM_BUFER_DATOS + 1u] = {0};
    TEST_ASSERT_EQUAL_HEX16(SW_LONGITUD_INCORRECTA, enviar(INS_VERIFICAR_PIN, datos, sizeof datos));
}

static void test_la_recarga_suma_al_saldo(void)
{
    TEST_ASSERT_EQUAL_HEX16(SW_CORRECTO, enviar_importe(INS_RECARGAR, 500));
    TEST_ASSERT_EQUAL_UINT16(500, saldo_leer());
}

static void test_recargar_sin_datos_se_rechaza(void)
{
    TEST_ASSERT_EQUAL_HEX16(SW_LONGITUD_INCORRECTA, enviar(INS_RECARGAR, PIN_TITULAR, 0));
}

static void test_recargar_con_datos_de_mas_se_rechaza(void)
{
    uint8_t datos[TAM_BUFER_DATOS + 1u] = {0};
    TEST_ASSERT_EQUAL_HEX16(SW_LONGITUD_INCORRECTA, enviar(INS_RECARGAR, datos, sizeof datos));
}

static void test_el_pago_resta_del_saldo(void)
{
    enviar_importe(INS_RECARGAR, 500);
    enviar(INS_VERIFICAR_PIN, PIN_TITULAR, LONGITUD_PIN);
    TEST_ASSERT_EQUAL_HEX16(SW_CORRECTO, enviar_importe(INS_PAGAR, 120));
    TEST_ASSERT_EQUAL_UINT16(380, saldo_leer());
}

static void test_pagar_sin_pin_se_rechaza(void)
{
    enviar_importe(INS_RECARGAR, 500);
    TEST_ASSERT_EQUAL_HEX16(SW_PIN_NO_VERIFICADO, enviar_importe(INS_PAGAR, 120));
    TEST_ASSERT_EQUAL_UINT16(500, saldo_leer());
}

static void test_pagar_mas_del_saldo_se_rechaza(void)
{
    enviar_importe(INS_RECARGAR, 100);
    enviar(INS_VERIFICAR_PIN, PIN_TITULAR, LONGITUD_PIN);
    TEST_ASSERT_EQUAL_HEX16(SW_SALDO_INSUFICIENTE, enviar_importe(INS_PAGAR, 101));
    TEST_ASSERT_EQUAL_UINT16(100, saldo_leer());
}

static void test_pagar_con_datos_de_mas_se_rechaza(void)
{
    uint8_t datos[TAM_BUFER_DATOS + 1u] = {0};
    enviar(INS_VERIFICAR_PIN, PIN_TITULAR, LONGITUD_PIN);
    TEST_ASSERT_EQUAL_HEX16(SW_LONGITUD_INCORRECTA, enviar(INS_PAGAR, datos, sizeof datos));
}

static void test_una_instruccion_desconocida_se_rechaza(void)
{
    TEST_ASSERT_EQUAL_HEX16(SW_INS_NO_SOPORTADA, enviar(0x99, PIN_TITULAR, LONGITUD_PIN));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_el_pin_correcto_se_acepta);
    RUN_TEST(test_el_pin_equivocado_se_rechaza);
    RUN_TEST(test_tres_fallos_bloquean_el_pin);
    RUN_TEST(test_un_acierto_devuelve_los_intentos);
    RUN_TEST(test_verificar_sin_datos_se_rechaza);
    RUN_TEST(test_verificar_con_datos_de_mas_se_rechaza);
    RUN_TEST(test_la_recarga_suma_al_saldo);
    RUN_TEST(test_recargar_sin_datos_se_rechaza);
    RUN_TEST(test_recargar_con_datos_de_mas_se_rechaza);
    RUN_TEST(test_el_pago_resta_del_saldo);
    RUN_TEST(test_pagar_sin_pin_se_rechaza);
    RUN_TEST(test_pagar_mas_del_saldo_se_rechaza);
    RUN_TEST(test_pagar_con_datos_de_mas_se_rechaza);
    RUN_TEST(test_una_instruccion_desconocida_se_rechaza);
    return UNITY_END();
}
