#include "unity.h"

#include "memoria.h"
#include "pin.h"

/*
 * Las pruebas del PIN, sin el procesado de comandos: con pin.h y comando_sw.h
 * basta. En el antes, pin.h incluía comando.h, y el PIN no se podía usar solo.
 */
static const uint8_t PIN_TITULAR[] = {1, 2, 3, 4};
static const uint8_t PIN_EQUIVOCADO[] = {9, 9, 9, 9};

void setUp(void)
{
    memoria_inicializar();
    pin_personalizar(PIN_TITULAR, sizeof PIN_TITULAR);
}

void tearDown(void) {}

static void test_el_pin_correcto_se_acepta(void)
{
    TEST_ASSERT_EQUAL_HEX16(COMANDO_SW_SIN_ERROR, pin_comprobar(PIN_TITULAR, sizeof PIN_TITULAR));
    TEST_ASSERT_EQUAL_UINT8(1, pin_verificado());
}

static void test_cada_fallo_gasta_un_intento(void)
{
    pin_comprobar(PIN_EQUIVOCADO, sizeof PIN_EQUIVOCADO);
    TEST_ASSERT_EQUAL_UINT8(PIN_INTENTOS_MAXIMOS - 1u, pin_intentos_restantes());
    TEST_ASSERT_EQUAL_UINT8(0, pin_verificado());
}

static void test_un_acierto_devuelve_los_intentos(void)
{
    pin_comprobar(PIN_EQUIVOCADO, sizeof PIN_EQUIVOCADO);
    pin_comprobar(PIN_TITULAR, sizeof PIN_TITULAR);
    TEST_ASSERT_EQUAL_UINT8(PIN_INTENTOS_MAXIMOS, pin_intentos_restantes());
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_el_pin_correcto_se_acepta);
    RUN_TEST(test_cada_fallo_gasta_un_intento);
    RUN_TEST(test_un_acierto_devuelve_los_intentos);
    return UNITY_END();
}
