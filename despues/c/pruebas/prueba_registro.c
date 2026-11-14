#include "unity.h"

#include "comando.h"
#include "memoria.h"
#include "registro.h"

/*
 * La prueba nueva del registro: con la respuesta llena, el registro lo anota
 * y no escribe ni un byte. En el antes, la comprobación era contra 255, el
 * máximo de un uint8_t, y con un búfer de 64 bytes no saltaba nunca.
 */
void setUp(void)
{
    memoria_inicializar();
}

void tearDown(void) {}

static void test_con_la_respuesta_llena_el_registro_lo_anota_y_no_escribe(void)
{
    uint8_t bufer[2];
    const uint8_t dos[2] = {0x01, 0x23};
    comando_respuesta *respuesta = comando_respuesta_iniciar(bufer, sizeof bufer);

    comando_respuesta_anadir(respuesta, dos, sizeof dos);
    TEST_ASSERT_EQUAL_UINT8(1, comando_respuesta_llena(respuesta));

    registro_anadir_resumen(respuesta);
    TEST_ASSERT_EQUAL_UINT8(1, registro_llena());
    TEST_ASSERT_EQUAL_UINT8(2, comando_respuesta_longitud(respuesta));
    TEST_ASSERT_EQUAL_HEX8(0x01, bufer[0]);
    TEST_ASSERT_EQUAL_HEX8(0x23, bufer[1]);
}

static void test_con_sitio_el_registro_anade_el_separador_y_las_operaciones(void)
{
    uint8_t bufer[8];
    comando_respuesta *respuesta = comando_respuesta_iniciar(bufer, sizeof bufer);

    registro_anadir_resumen(respuesta);
    TEST_ASSERT_EQUAL_UINT8(0, registro_llena());
    TEST_ASSERT_EQUAL_UINT8(2, comando_respuesta_longitud(respuesta));
    TEST_ASSERT_EQUAL_HEX8(0xFF, bufer[0]);
    TEST_ASSERT_EQUAL_HEX8(0x00, bufer[1]);
}

static void test_si_los_datos_no_caben_la_respuesta_se_queda_como_estaba(void)
{
    uint8_t bufer[3];
    const uint8_t cuatro[4] = {1, 2, 3, 4};
    comando_respuesta *respuesta = comando_respuesta_iniciar(bufer, sizeof bufer);

    comando_respuesta_anadir(respuesta, cuatro, sizeof cuatro);
    TEST_ASSERT_EQUAL_UINT8(0, comando_respuesta_longitud(respuesta));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_con_la_respuesta_llena_el_registro_lo_anota_y_no_escribe);
    RUN_TEST(test_con_sitio_el_registro_anade_el_separador_y_las_operaciones);
    RUN_TEST(test_si_los_datos_no_caben_la_respuesta_se_queda_como_estaba);
    return UNITY_END();
}
