#include "unity.h"

#include "estados.h"
#include "memoria.h"
#include "pin.h"
#include "registro.h"

static const uint8_t PIN_TITULAR[LONGITUD_PIN] = {1, 2, 3, 4};
static const uint8_t DATOS[] = {0xCA, 0xFE, 0x01};

void setUp(void)
{
    memoria_formatear();
    pin_personalizar(PIN_TITULAR);
    pin_verificar(PIN_TITULAR, LONGITUD_PIN);
}

void tearDown(void) {}

static void test_sin_pin_verificado_no_escribe(void)
{
    pin_personalizar(PIN_TITULAR);
    TEST_ASSERT_EQUAL_HEX16(SW_PIN_NO_VERIFICADO, registro_escribir(0, DATOS, sizeof DATOS));
    TEST_ASSERT_EQUAL_HEX8(0x00, memoria_leer_byte(0, 0));
}

static void test_un_registro_que_no_existe(void)
{
    TEST_ASSERT_EQUAL_HEX16(SW_REGISTRO_NO_EXISTE, registro_escribir(NUM_REGISTROS, DATOS, sizeof DATOS));
}

static void test_datos_que_no_caben(void)
{
    uint8_t demasiados[TAM_REGISTRO + 1u] = {0};
    TEST_ASSERT_EQUAL_HEX16(SW_LONGITUD_INCORRECTA, registro_escribir(0, demasiados, sizeof demasiados));
}

static void test_longitud_cero_no_escribe_nada(void)
{
    TEST_ASSERT_EQUAL_HEX16(SW_CORRECTO, registro_escribir(1, DATOS, 0));
    TEST_ASSERT_EQUAL_HEX8(0x00, memoria_leer_byte(1, 0));
}

static void test_escritura_correcta(void)
{
    TEST_ASSERT_EQUAL_HEX16(SW_CORRECTO, registro_escribir(2, DATOS, sizeof DATOS));
    TEST_ASSERT_EQUAL_HEX8(0xCA, memoria_leer_byte(2, 0));
    TEST_ASSERT_EQUAL_HEX8(0xFE, memoria_leer_byte(2, 1));
    TEST_ASSERT_EQUAL_HEX8(0x01, memoria_leer_byte(2, 2));
}

static void test_escritura_que_falla(void)
{
    memoria_simular_fallo(1);
    TEST_ASSERT_EQUAL_HEX16(SW_ERROR_MEMORIA, registro_escribir(3, DATOS, sizeof DATOS));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_sin_pin_verificado_no_escribe);
    RUN_TEST(test_un_registro_que_no_existe);
    RUN_TEST(test_datos_que_no_caben);
    RUN_TEST(test_longitud_cero_no_escribe_nada);
    RUN_TEST(test_escritura_correcta);
    RUN_TEST(test_escritura_que_falla);
    return UNITY_END();
}
