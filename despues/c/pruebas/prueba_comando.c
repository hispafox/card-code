#include "unity.h"

#include <string.h>

#include "comando.h"
#include "memoria.h"
#include "pin.h"

/*
 * Las pruebas mandan comandos completos y miran el código de estado, con
 * los valores de la norma escritos tal cual: no dependen de cómo se llaman
 * las constantes por dentro.
 */
static const uint8_t PIN_TITULAR[] = {1, 2, 3, 4};
static const uint8_t PIN_EQUIVOCADO[] = {9, 9, 9, 9};

static uint16_t enviar(uint8_t ins, const uint8_t *datos, uint8_t lc)
{
    uint8_t apdu[80] = {0x80, 0, 0, 0, 0};
    apdu[1] = ins;
    apdu[4] = lc;
    if (lc > 0) {
        memcpy(&apdu[5], datos, lc);
    }
    return comando_procesar(apdu, (uint8_t)(5 + lc));
}

static uint16_t enviar_importe(uint8_t ins, uint16_t importe)
{
    uint8_t datos[2] = {(uint8_t)(importe >> 8), (uint8_t)(importe & 0xFF)};
    return enviar(ins, datos, 2);
}

static void verificar_pin(void)
{
    enviar(0x20, PIN_TITULAR, sizeof PIN_TITULAR);
}

void setUp(void)
{
    memoria_inicializar();
    pin_personalizar(PIN_TITULAR, sizeof PIN_TITULAR);
    comando_inicializar();
}

void tearDown(void) {}

static void test_el_pin_correcto_se_acepta(void)
{
    TEST_ASSERT_EQUAL_HEX16(0x9000, enviar(0x20, PIN_TITULAR, sizeof PIN_TITULAR));
}

static void test_el_pin_equivocado_se_rechaza(void)
{
    TEST_ASSERT_EQUAL_HEX16(0x6300, enviar(0x20, PIN_EQUIVOCADO, sizeof PIN_EQUIVOCADO));
}

static void test_un_pin_mas_largo_que_el_maximo_se_rechaza(void)
{
    const uint8_t largo[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    TEST_ASSERT_EQUAL_HEX16(0x6700, enviar(0x20, largo, sizeof largo));
}

static void test_un_comando_mas_corto_que_la_cabecera_se_rechaza(void)
{
    const uint8_t apdu[4] = {0x80, 0x20, 0, 0};
    TEST_ASSERT_EQUAL_HEX16(0x6700, comando_procesar(apdu, sizeof apdu));
}

static void test_un_comando_mas_largo_que_el_maximo_se_rechaza(void)
{
    uint8_t apdu[65] = {0x80, 0x20, 0, 0, 4};
    TEST_ASSERT_EQUAL_HEX16(0x6700, comando_procesar(apdu, sizeof apdu));
}

static void test_con_el_pin_bloqueado_la_seguridad_no_se_satisface(void)
{
    for (int i = 0; i < 3; i++) {
        enviar(0x20, PIN_EQUIVOCADO, sizeof PIN_EQUIVOCADO);
    }
    TEST_ASSERT_EQUAL_HEX16(0x6982, enviar(0x20, PIN_TITULAR, sizeof PIN_TITULAR));
}

static void test_se_paga_hasta_el_saldo_exacto_y_nada_mas(void)
{
    TEST_ASSERT_EQUAL_HEX16(0x9000, enviar_importe(0x32, 500));
    verificar_pin();
    TEST_ASSERT_EQUAL_HEX16(0x9000, enviar_importe(0x30, 120));
    TEST_ASSERT_EQUAL_HEX16(0x9000, enviar_importe(0x30, 380));
    TEST_ASSERT_EQUAL_HEX16(0x6985, enviar_importe(0x30, 1));
}

static void test_pagar_sin_pin_se_rechaza(void)
{
    enviar_importe(0x32, 500);
    TEST_ASSERT_EQUAL_HEX16(0x6982, enviar_importe(0x30, 120));
}

static void test_una_recarga_por_encima_del_saldo_maximo_se_rechaza(void)
{
    TEST_ASSERT_EQUAL_HEX16(0x9000, enviar_importe(0x32, 50000));
    TEST_ASSERT_EQUAL_HEX16(0x6A80, enviar_importe(0x32, 1));
}

static void test_un_importe_con_la_longitud_equivocada_se_rechaza(void)
{
    const uint8_t tres[3] = {0, 1, 0};
    TEST_ASSERT_EQUAL_HEX16(0x6700, enviar(0x32, tres, sizeof tres));
}

static void test_una_instruccion_desconocida_se_rechaza(void)
{
    TEST_ASSERT_EQUAL_HEX16(0x6D00, enviar(0x99, PIN_TITULAR, sizeof PIN_TITULAR));
}

static void test_la_respuesta_lleva_el_codigo_de_estado(void)
{
    uint8_t longitud;
    const uint8_t *respuesta;

    enviar(0x99, PIN_TITULAR, sizeof PIN_TITULAR);
    respuesta = comando_respuesta(&longitud);
    TEST_ASSERT_EQUAL_UINT8(2, longitud);
    TEST_ASSERT_EQUAL_HEX8(0x6D, respuesta[0]);
    TEST_ASSERT_EQUAL_HEX8(0x00, respuesta[1]);
}

/* En el antes, la consulta está en la cabecera pero el despacho no tiene su «case». */
static void test_la_consulta_devuelve_el_saldo_y_el_codigo_de_estado(void)
{
    uint8_t longitud;
    const uint8_t *respuesta;

    enviar_importe(0x32, 0x0123);
    TEST_ASSERT_EQUAL_HEX16(0x9000, enviar(0x34, NULL, 0));
    respuesta = comando_respuesta(&longitud);
    TEST_ASSERT_EQUAL_UINT8(4, longitud);
    TEST_ASSERT_EQUAL_HEX8(0x01, respuesta[0]);
    TEST_ASSERT_EQUAL_HEX8(0x23, respuesta[1]);
    TEST_ASSERT_EQUAL_HEX8(0x90, respuesta[2]);
    TEST_ASSERT_EQUAL_HEX8(0x00, respuesta[3]);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_el_pin_correcto_se_acepta);
    RUN_TEST(test_el_pin_equivocado_se_rechaza);
    RUN_TEST(test_un_pin_mas_largo_que_el_maximo_se_rechaza);
    RUN_TEST(test_un_comando_mas_corto_que_la_cabecera_se_rechaza);
    RUN_TEST(test_un_comando_mas_largo_que_el_maximo_se_rechaza);
    RUN_TEST(test_con_el_pin_bloqueado_la_seguridad_no_se_satisface);
    RUN_TEST(test_se_paga_hasta_el_saldo_exacto_y_nada_mas);
    RUN_TEST(test_pagar_sin_pin_se_rechaza);
    RUN_TEST(test_una_recarga_por_encima_del_saldo_maximo_se_rechaza);
    RUN_TEST(test_un_importe_con_la_longitud_equivocada_se_rechaza);
    RUN_TEST(test_una_instruccion_desconocida_se_rechaza);
    RUN_TEST(test_la_respuesta_lleva_el_codigo_de_estado);
    RUN_TEST(test_la_consulta_devuelve_el_saldo_y_el_codigo_de_estado);
    return UNITY_END();
}
