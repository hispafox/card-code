#include "unity.h"

#include <string.h>

#include "comando.h"
#include "memoria.h"
#include "pin.h"
#include "registro.h"

/*
 * Las pruebas mandan comandos completos y miran el código de estado y la
 * respuesta. Son las mismas en las dos versiones; solo cambia cómo se prepara
 * la respuesta y cómo se lee su longitud, porque en el después la estructura
 * está escondida.
 */
static const uint8_t PIN_TITULAR[] = {1, 2, 3, 4};
static const uint8_t PIN_EQUIVOCADO[] = {9, 9, 9, 9};

static uint8_t bufer[COMANDO_LONGITUD_MAXIMA];
static comando_respuesta *respuesta;

static uint16_t enviar(uint8_t ins, const uint8_t *datos, uint8_t lc)
{
    uint8_t apdu[80] = {0x80, 0, 0, 0, 0};
    apdu[1] = ins;
    apdu[4] = lc;
    if (lc > 0) {
        memcpy(&apdu[5], datos, lc);
    }
    respuesta = comando_respuesta_iniciar(bufer, sizeof bufer);
    return comando_procesar(apdu, (uint8_t)(5 + lc), respuesta);
}

static uint16_t enviar_importe(uint8_t ins, uint16_t importe)
{
    uint8_t datos[2] = {(uint8_t)(importe >> 8), (uint8_t)(importe & 0xFF)};
    return enviar(ins, datos, 2);
}

static uint16_t verificar(const uint8_t *pin)
{
    return enviar(0x20, pin, 4);
}

void setUp(void)
{
    memoria_inicializar();
    pin_personalizar(PIN_TITULAR, sizeof PIN_TITULAR);
}

void tearDown(void) {}

static void test_el_pin_correcto_se_acepta_y_responde_9000(void)
{
    TEST_ASSERT_EQUAL_HEX16(0x9000, verificar(PIN_TITULAR));
    TEST_ASSERT_EQUAL_UINT8(2, comando_respuesta_longitud(respuesta));
    TEST_ASSERT_EQUAL_HEX8(0x90, bufer[0]);
    TEST_ASSERT_EQUAL_HEX8(0x00, bufer[1]);
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
    TEST_ASSERT_EQUAL_HEX16(0x6700, enviar(0x20, largo, sizeof largo));
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

static void test_la_consulta_devuelve_el_saldo_y_el_resumen_del_registro(void)
{
    enviar_importe(0x32, 0x0123);    /* primera operación */
    TEST_ASSERT_EQUAL_HEX16(0x9000, enviar(0x34, NULL, 0));
    TEST_ASSERT_EQUAL_UINT8(6, comando_respuesta_longitud(respuesta));
    TEST_ASSERT_EQUAL_HEX8(0x01, bufer[0]);
    TEST_ASSERT_EQUAL_HEX8(0x23, bufer[1]);
    TEST_ASSERT_EQUAL_HEX8(0xFF, bufer[2]);    /* el separador */
    TEST_ASSERT_EQUAL_HEX8(0x01, bufer[3]);    /* una operación registrada */
    TEST_ASSERT_EQUAL_HEX8(0x90, bufer[4]);
    TEST_ASSERT_EQUAL_HEX8(0x00, bufer[5]);
}

static void test_el_registro_cuenta_las_operaciones_y_su_longitud(void)
{
    verificar(PIN_EQUIVOCADO);
    enviar(0x34, NULL, 0);
    TEST_ASSERT_EQUAL_UINT8(2, registro_operaciones());
    TEST_ASSERT_EQUAL_UINT8(6, registro_ultima_longitud());
}

static void test_una_instruccion_desconocida_se_rechaza(void)
{
    TEST_ASSERT_EQUAL_HEX16(0x6D00, enviar(0x99, PIN_TITULAR, 4));
}

static void test_un_comando_mas_corto_que_la_cabecera_se_rechaza(void)
{
    const uint8_t apdu[4] = {0x80, 0x20, 0, 0};
    respuesta = comando_respuesta_iniciar(bufer, sizeof bufer);
    TEST_ASSERT_EQUAL_HEX16(0x6700, comando_procesar(apdu, sizeof apdu, respuesta));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_el_pin_correcto_se_acepta_y_responde_9000);
    RUN_TEST(test_el_pin_equivocado_no_satisface_la_seguridad);
    RUN_TEST(test_tres_fallos_bloquean_el_pin);
    RUN_TEST(test_un_pin_mas_largo_que_el_maximo_se_rechaza);
    RUN_TEST(test_se_paga_hasta_el_saldo_exacto_y_nada_mas);
    RUN_TEST(test_pagar_sin_pin_no_satisface_la_seguridad);
    RUN_TEST(test_la_consulta_devuelve_el_saldo_y_el_resumen_del_registro);
    RUN_TEST(test_el_registro_cuenta_las_operaciones_y_su_longitud);
    RUN_TEST(test_una_instruccion_desconocida_se_rechaza);
    RUN_TEST(test_un_comando_mas_corto_que_la_cabecera_se_rechaza);
    return UNITY_END();
}
