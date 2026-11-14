#include "unity.h"

#include <string.h>

#include "comando.h"
#include "memoria.h"
#include "pin.h"
#include "registro.h"

/*
 * Las pruebas mandan comandos completos y miran el código de estado, con
 * los valores de la norma escritos tal cual.
 */
static const uint8_t PIN_TITULAR[] = {1, 2, 3, 4};
static const uint8_t PIN_EQUIVOCADO[] = {9, 9, 9, 9};

static uint16_t enviar(uint8_t ins, uint8_t p1, const uint8_t *datos, uint8_t lc)
{
    uint8_t apdu[80] = {0x80, 0, 0, 0, 0};
    apdu[1] = ins;
    apdu[2] = p1;
    apdu[4] = lc;
    if (lc > 0) {
        memcpy(&apdu[5], datos, lc);
    }
    return comando_procesar(apdu, (uint8_t)(5 + lc));
}

static uint16_t enviar_importe(uint8_t ins, uint16_t importe)
{
    uint8_t datos[2] = {(uint8_t)(importe >> 8), (uint8_t)(importe & 0xFF)};
    return enviar(ins, 0, datos, 2);
}

static uint16_t verificar(const uint8_t *pin)
{
    return enviar(0x20, 0, pin, 4);
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
    TEST_ASSERT_EQUAL_HEX16(0x9000, verificar(PIN_TITULAR));
}

static void test_el_pin_equivocado_no_satisface_la_seguridad(void)
{
    TEST_ASSERT_EQUAL_HEX16(0x6982, verificar(PIN_EQUIVOCADO));
}

static void test_tres_fallos_bloquean_el_pin(void)
{
    verificar(PIN_EQUIVOCADO);
    verificar(PIN_EQUIVOCADO);
    verificar(PIN_EQUIVOCADO);
    TEST_ASSERT_EQUAL_HEX16(0x6983, verificar(PIN_TITULAR));
}

static void test_un_pin_mas_largo_que_el_maximo_se_rechaza(void)
{
    const uint8_t largo[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    TEST_ASSERT_EQUAL_HEX16(0x6700, enviar(0x20, 0, largo, sizeof largo));
}

static void test_si_no_se_puede_anotar_el_pin_es_un_fallo_de_memoria(void)
{
    memoria_simular_fallo(1);
    TEST_ASSERT_EQUAL_HEX16(0x6581, verificar(PIN_TITULAR));
}

static void test_se_paga_hasta_el_saldo_exacto_y_nada_mas(void)
{
    TEST_ASSERT_EQUAL_HEX16(0x9000, enviar_importe(0x32, 500));
    verificar(PIN_TITULAR);
    TEST_ASSERT_EQUAL_HEX16(0x9000, enviar_importe(0x30, 120));
    TEST_ASSERT_EQUAL_HEX16(0x9000, enviar_importe(0x30, 380));
    TEST_ASSERT_EQUAL_HEX16(0x6985, enviar_importe(0x30, 1));
}

static void test_pagar_sin_pin_no_satisface_la_seguridad(void)
{
    enviar_importe(0x32, 500);
    TEST_ASSERT_EQUAL_HEX16(0x6982, enviar_importe(0x30, 120));
}

static void test_un_pago_de_cero_se_rechaza(void)
{
    enviar_importe(0x32, 500);
    verificar(PIN_TITULAR);
    TEST_ASSERT_EQUAL_HEX16(0x6A80, enviar_importe(0x30, 0));
}

static void test_un_pago_con_la_longitud_equivocada_se_rechaza(void)
{
    const uint8_t tres[3] = {0, 1, 0};
    verificar(PIN_TITULAR);
    TEST_ASSERT_EQUAL_HEX16(0x6700, enviar(0x30, 0, tres, sizeof tres));
}

static void test_una_recarga_por_encima_del_saldo_maximo_se_rechaza(void)
{
    TEST_ASSERT_EQUAL_HEX16(0x9000, enviar_importe(0x32, 50000));
    TEST_ASSERT_EQUAL_HEX16(0x6A80, enviar_importe(0x32, 1));
}

static void test_la_consulta_devuelve_el_saldo_y_el_codigo_de_estado(void)
{
    uint8_t longitud;
    const uint8_t *respuesta;

    enviar_importe(0x32, 0x0123);
    TEST_ASSERT_EQUAL_HEX16(0x9000, enviar(0x34, 0, NULL, 0));
    respuesta = comando_ultima_respuesta(&longitud);
    TEST_ASSERT_EQUAL_UINT8(4, longitud);
    TEST_ASSERT_EQUAL_HEX8(0x01, respuesta[0]);
    TEST_ASSERT_EQUAL_HEX8(0x23, respuesta[1]);
    TEST_ASSERT_EQUAL_HEX8(0x90, respuesta[2]);
    TEST_ASSERT_EQUAL_HEX8(0x00, respuesta[3]);
}

static void test_el_pago_queda_registrado_con_su_terminal(void)
{
    const uint8_t importe[2] = {0, 120};
    registro_pago pago;

    enviar_importe(0x32, 500);
    verificar(PIN_TITULAR);
    enviar(0x30, 7, importe, 2);
    registro_leer_pago(&pago);
    TEST_ASSERT_EQUAL_UINT16(120, pago.importe);
    TEST_ASSERT_EQUAL_UINT16(500, pago.saldo_anterior);
    TEST_ASSERT_EQUAL_UINT16(380, pago.saldo_nuevo);
    TEST_ASSERT_EQUAL_UINT8(7, pago.terminal);
}

static void test_un_pago_rechazado_no_se_registra(void)
{
    registro_pago pago;

    enviar_importe(0x32, 500);
    enviar_importe(0x30, 120);
    registro_leer_pago(&pago);
    TEST_ASSERT_EQUAL_UINT16(0, pago.importe);
}

static void test_una_instruccion_desconocida_se_rechaza(void)
{
    TEST_ASSERT_EQUAL_HEX16(0x6D00, enviar(0x99, 0, PIN_TITULAR, 4));
}

static void test_la_respuesta_sin_cifrar_lleva_los_datos_tal_cual(void)
{
    const uint8_t datos[2] = {0x01, 0x23};
    uint8_t bufer[4];
    uint8_t longitud;

    comando_responder(bufer, &longitud, 0x9000, datos, 2, 0);
    TEST_ASSERT_EQUAL_UINT8(4, longitud);
    TEST_ASSERT_EQUAL_HEX8(0x01, bufer[0]);
    TEST_ASSERT_EQUAL_HEX8(0x23, bufer[1]);
    TEST_ASSERT_EQUAL_HEX8(0x90, bufer[2]);
    TEST_ASSERT_EQUAL_HEX8(0x00, bufer[3]);
}

static void test_la_respuesta_cifrada_no_lleva_los_datos_en_claro(void)
{
    const uint8_t datos[2] = {0x01, 0x23};
    uint8_t bufer[4];
    uint8_t longitud;

    comando_responder(bufer, &longitud, 0x9000, datos, 2, 1);
    TEST_ASSERT_EQUAL_UINT8(4, longitud);
    TEST_ASSERT_EQUAL_HEX8(0x01 ^ 0x5A, bufer[0]);
    TEST_ASSERT_EQUAL_HEX8(0x23 ^ 0x5A, bufer[1]);
    TEST_ASSERT_EQUAL_HEX8(0x90, bufer[2]);
    TEST_ASSERT_EQUAL_HEX8(0x00, bufer[3]);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_el_pin_correcto_se_acepta);
    RUN_TEST(test_el_pin_equivocado_no_satisface_la_seguridad);
    RUN_TEST(test_tres_fallos_bloquean_el_pin);
    RUN_TEST(test_un_pin_mas_largo_que_el_maximo_se_rechaza);
    RUN_TEST(test_si_no_se_puede_anotar_el_pin_es_un_fallo_de_memoria);
    RUN_TEST(test_se_paga_hasta_el_saldo_exacto_y_nada_mas);
    RUN_TEST(test_pagar_sin_pin_no_satisface_la_seguridad);
    RUN_TEST(test_un_pago_de_cero_se_rechaza);
    RUN_TEST(test_un_pago_con_la_longitud_equivocada_se_rechaza);
    RUN_TEST(test_una_recarga_por_encima_del_saldo_maximo_se_rechaza);
    RUN_TEST(test_la_consulta_devuelve_el_saldo_y_el_codigo_de_estado);
    RUN_TEST(test_el_pago_queda_registrado_con_su_terminal);
    RUN_TEST(test_un_pago_rechazado_no_se_registra);
    RUN_TEST(test_una_instruccion_desconocida_se_rechaza);
    RUN_TEST(test_la_respuesta_sin_cifrar_lleva_los_datos_tal_cual);
    RUN_TEST(test_la_respuesta_cifrada_no_lleva_los_datos_en_claro);
    return UNITY_END();
}
