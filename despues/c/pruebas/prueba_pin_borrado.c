#include "unity.h"

#include <stddef.h>

#include "comando.h"
#include "memoria.h"
#include "pin.h"

/*
 * La prueba nueva del después: la copia del PIN que hace pin_comprobar_comando
 * queda borrada salga por donde salga. Como la copia es una variable local, la
 * prueba no la puede leer; lo que hace es espiar memcpy y memset. El enlazador
 * redirige las llamadas a estas dos funciones (opciones en prueba_pin_borrado.opciones),
 * que apuntan qué búfer se copió y cuál se borró, y después llaman a las de verdad.
 */
void *__real_memcpy(void *destino, const void *origen, size_t n);
void *__real_memset(void *destino, int valor, size_t n);

static void *bufer_copiado;
static void *bufer_borrado;
static size_t bytes_borrados;

void *__wrap_memcpy(void *destino, const void *origen, size_t n)
{
    bufer_copiado = destino;
    return __real_memcpy(destino, origen, n);
}

void *__wrap_memset(void *destino, int valor, size_t n)
{
    if (valor == 0) {
        bufer_borrado = destino;
        bytes_borrados = n;
    }
    return __real_memset(destino, valor, n);
}

static const uint8_t PIN_TITULAR[] = {1, 2, 3, 4};
static const uint8_t PIN_EQUIVOCADO[] = {9, 9, 9, 9};

void setUp(void)
{
    memoria_inicializar();
    pin_personalizar(PIN_TITULAR, sizeof PIN_TITULAR);
    bufer_copiado = NULL;
    bufer_borrado = NULL;
    bytes_borrados = 0;
}

void tearDown(void) {}

static void comprobar_que_la_copia_se_ha_borrado(void)
{
    TEST_ASSERT_NOT_NULL(bufer_copiado);
    TEST_ASSERT_EQUAL_PTR(bufer_copiado, bufer_borrado);
    TEST_ASSERT_EQUAL_UINT32(PIN_LONGITUD_MAXIMA, bytes_borrados);
}

static void test_con_el_pin_correcto_la_copia_se_borra(void)
{
    TEST_ASSERT_EQUAL_HEX16(0x9000, pin_comprobar_comando(PIN_TITULAR, sizeof PIN_TITULAR));
    comprobar_que_la_copia_se_ha_borrado();
}

static void test_con_el_pin_equivocado_la_copia_se_borra(void)
{
    TEST_ASSERT_EQUAL_HEX16(0x6982, pin_comprobar_comando(PIN_EQUIVOCADO, sizeof PIN_EQUIVOCADO));
    comprobar_que_la_copia_se_ha_borrado();
}

static void test_si_falla_la_memoria_la_copia_se_borra(void)
{
    memoria_simular_fallo(1);
    TEST_ASSERT_EQUAL_HEX16(0x6581, pin_comprobar_comando(PIN_TITULAR, sizeof PIN_TITULAR));
    comprobar_que_la_copia_se_ha_borrado();
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_con_el_pin_correcto_la_copia_se_borra);
    RUN_TEST(test_con_el_pin_equivocado_la_copia_se_borra);
    RUN_TEST(test_si_falla_la_memoria_la_copia_se_borra);
    return UNITY_END();
}
