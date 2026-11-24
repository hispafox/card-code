#include "unity.h"

#include <string.h>

#include "comando.h"
#include "memoria.h"
#include "pin.h"
#include "registro.h"
#include "saldo.h"

/*
 * Las pruebas mandan comandos completos y miran el código de estado, con
 * los valores de la norma escritos tal cual. Son las mismas en el antes y
 * en el después.
 */
static const uint8_t PIN_TITULAR[] = {1, 2, 3, 4};
static const uint8_t PIN_EQUIVOCADO[] = {9, 9, 9, 9};
static const uint8_t PIN_LARGO[8] = {1, 2, 3, 4, 5, 6, 7, 8};
static const uint8_t PIN_NUEVO[8] = {8, 7, 6, 5, 4, 3, 2, 1};

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

static uint16_t verificar(const uint8_t *pin)
{
    return enviar(0x20, pin, 4);
}

/* El cambio de PIN trae el actual y el nuevo, ocho bytes cada uno. */
static uint16_t cambiar_pin(const uint8_t *actual, const uint8_t *nuevo)
{
    uint8_t datos[2 * PIN_LONGITUD_MAXIMA];
    memcpy(datos, actual, PIN_LONGITUD_MAXIMA);
    memcpy(&datos[PIN_LONGITUD_MAXIMA], nuevo, PIN_LONGITUD_MAXIMA);
    return enviar(0x24, datos, sizeof datos);
}

/* La tarjeta sale de fábrica con el PIN del titular y el saldo a cero, con su CRC. */
void setUp(void)
{
    memoria_inicializar();
    TEST_ASSERT_EQUAL_INT(MEMORIA_SIN_ERROR, pin_personalizar(PIN_TITULAR, sizeof PIN_TITULAR));
    TEST_ASSERT_EQUAL_INT(MEMORIA_SIN_ERROR, saldo_escribir(0u));
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

static void test_un_pago_de_cero_se_rechaza(void)
{
    enviar_importe(0x32, 500);
    verificar(PIN_TITULAR);
    TEST_ASSERT_EQUAL_HEX16(0x6A80, enviar_importe(0x30, 0));
}

static void test_una_recarga_por_encima_del_saldo_maximo_se_rechaza(void)
{
    TEST_ASSERT_EQUAL_HEX16(0x9000, enviar_importe(0x32, 50000));
    TEST_ASSERT_EQUAL_HEX16(0x6A80, enviar_importe(0x32, 1));
}

static void test_la_consulta_devuelve_el_saldo_con_su_crc(void)
{
    uint8_t longitud;
    const uint8_t *respuesta;

    enviar_importe(0x32, 0x0123);
    TEST_ASSERT_EQUAL_HEX16(0x9000, enviar(0x34, NULL, 0));
    respuesta = comando_ultima_respuesta(&longitud);
    TEST_ASSERT_EQUAL_UINT8(6, longitud);
    TEST_ASSERT_EQUAL_HEX8(0x01, respuesta[0]);
    TEST_ASSERT_EQUAL_HEX8(0x23, respuesta[1]);
    TEST_ASSERT_EQUAL_HEX8(0x3A, respuesta[2]);    /* CRC de 01 23 */
    TEST_ASSERT_EQUAL_HEX8(0x3F, respuesta[3]);
    TEST_ASSERT_EQUAL_HEX8(0x90, respuesta[4]);
    TEST_ASSERT_EQUAL_HEX8(0x00, respuesta[5]);
}

static void test_el_pago_queda_registrado_con_su_crc(void)
{
    registro_pago pago;

    enviar_importe(0x32, 500);
    verificar(PIN_TITULAR);
    enviar_importe(0x30, 120);
    TEST_ASSERT_EQUAL_INT(MEMORIA_SIN_ERROR, registro_leer_pago(&pago));
    TEST_ASSERT_EQUAL_UINT16(120, pago.importe);
    TEST_ASSERT_EQUAL_UINT16(500, pago.saldo_anterior);
    TEST_ASSERT_EQUAL_UINT16(380, pago.saldo_nuevo);
}

static void test_un_registro_corrompido_se_detecta(void)
{
    registro_pago pago;

    enviar_importe(0x32, 500);
    verificar(PIN_TITULAR);
    enviar_importe(0x30, 120);
    memoria_escribir(0x0081, 0x77);    /* un byte del importe registrado */
    TEST_ASSERT_EQUAL_INT(MEMORIA_DATOS_CORROMPIDOS, registro_leer_pago(&pago));
}

/* Pruebas nuevas del después: en el antes, las dos responderían 90 00. */
static void test_si_falla_la_escritura_del_saldo_el_pago_responde_fallo_de_memoria(void)
{
    enviar_importe(0x32, 500);
    verificar(PIN_TITULAR);
    memoria_fallar_siguiente_escritura();
    TEST_ASSERT_EQUAL_HEX16(0x6581, enviar_importe(0x30, 120));
}

static void copiar_memoria(uint8_t *copia)
{
    for (uint16_t i = 0u; i < MEMORIA_TAMANO; i++) {
        copia[i] = memoria_leer(i);
    }
}

static void test_un_saldo_corrompido_no_se_convierte_en_un_saldo(void)
{
    uint8_t antes[MEMORIA_TAMANO];
    uint8_t despues[MEMORIA_TAMANO];

    enviar_importe(0x32, 500);
    verificar(PIN_TITULAR);
    memoria_escribir(0x0041, 0x77);    /* un byte del saldo guardado */
    copiar_memoria(antes);
    TEST_ASSERT_EQUAL_HEX16(0x6581, enviar_importe(0x30, 120));
    TEST_ASSERT_EQUAL_HEX16(0x6581, enviar_importe(0x32, 100));
    copiar_memoria(despues);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(antes, despues, MEMORIA_TAMANO);    /* ninguno de los dos ha escrito */
}

static void test_la_huella_de_la_memoria_cambia_al_escribir(void)
{
    uint16_t antes = memoria_huella();

    /* Un byte suelto: el saldo y el registro llevan su CRC detrás, y un bloque
       que termina en su propio CRC no cambia el CRC de toda la memoria. */
    memoria_escribir(0x0100, 0x55);
    TEST_ASSERT_NOT_EQUAL(antes, memoria_huella());
}

static void test_el_cambio_de_pin_con_el_pin_actual_cambia_el_pin(void)
{
    TEST_ASSERT_EQUAL_INT(MEMORIA_SIN_ERROR, pin_personalizar(PIN_LARGO, sizeof PIN_LARGO));
    TEST_ASSERT_EQUAL_HEX16(0x9000, cambiar_pin(PIN_LARGO, PIN_NUEVO));
    TEST_ASSERT_EQUAL_HEX16(0x9000, enviar(0x20, PIN_NUEVO, sizeof PIN_NUEVO));
}

static void test_el_cambio_de_pin_con_otro_pin_actual_no_satisface_la_seguridad(void)
{
    TEST_ASSERT_EQUAL_INT(MEMORIA_SIN_ERROR, pin_personalizar(PIN_LARGO, sizeof PIN_LARGO));
    TEST_ASSERT_EQUAL_HEX16(0x6982, cambiar_pin(PIN_NUEVO, PIN_NUEVO));
    TEST_ASSERT_EQUAL_HEX16(0x9000, enviar(0x20, PIN_LARGO, sizeof PIN_LARGO));
}

static void test_una_instruccion_desconocida_se_rechaza(void)
{
    TEST_ASSERT_EQUAL_HEX16(0x6D00, enviar(0x99, PIN_TITULAR, 4));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_el_pin_correcto_se_acepta);
    RUN_TEST(test_el_pin_equivocado_no_satisface_la_seguridad);
    RUN_TEST(test_tres_fallos_bloquean_el_pin);
    RUN_TEST(test_un_pin_mas_largo_que_el_maximo_se_rechaza);
    RUN_TEST(test_se_paga_hasta_el_saldo_exacto_y_nada_mas);
    RUN_TEST(test_pagar_sin_pin_no_satisface_la_seguridad);
    RUN_TEST(test_un_pago_de_cero_se_rechaza);
    RUN_TEST(test_una_recarga_por_encima_del_saldo_maximo_se_rechaza);
    RUN_TEST(test_la_consulta_devuelve_el_saldo_con_su_crc);
    RUN_TEST(test_el_pago_queda_registrado_con_su_crc);
    RUN_TEST(test_un_registro_corrompido_se_detecta);
    RUN_TEST(test_si_falla_la_escritura_del_saldo_el_pago_responde_fallo_de_memoria);
    RUN_TEST(test_un_saldo_corrompido_no_se_convierte_en_un_saldo);
    RUN_TEST(test_la_huella_de_la_memoria_cambia_al_escribir);
    RUN_TEST(test_el_cambio_de_pin_con_el_pin_actual_cambia_el_pin);
    RUN_TEST(test_el_cambio_de_pin_con_otro_pin_actual_no_satisface_la_seguridad);
    RUN_TEST(test_una_instruccion_desconocida_se_rechaza);
    return UNITY_END();
}
