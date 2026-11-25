#include "unity.h"

#include "liquidacion.h"

/*
 * Pruebas de caracterizacion de liquidar: fijan lo que hace hoy, no lo que
 * deberia hacer. Cada valor esperado salio de ejecutar el codigo. Lo que
 * parece un error lleva RARO delante: se fija tal como esta y, si hay que
 * corregirlo, se corrige despues, en un cambio propio.
 */

enum { PAGO = 1, RECARGA = 2, DEVOLUCION = 3 };
enum { SIN_REDONDEO = 0, CON_REDONDEO = 1 };

static long           neto;
static unsigned short comision;

void setUp(void)
{
    neto = 7;
    comision = 7;
    ult_comision = 7;
}

void tearDown(void) {}

static int liquidar_uno(unsigned char tipo, unsigned short importe)
{
    struct mov m[1];

    m[0].t = tipo;
    m[0].imp = importe;
    return liquidar(m, 1, &neto, &comision, SIN_REDONDEO);
}

static void test_un_pago_resta_el_importe_y_cobra_el_uno_por_ciento(void)
{
    TEST_ASSERT_EQUAL_INT(0, liquidar_uno(PAGO, 1000));
    TEST_ASSERT_EQUAL_INT32(-1000, neto);
    TEST_ASSERT_EQUAL_UINT16(10, comision);
}

static void test_la_comision_minima_es_cinco(void)
{
    liquidar_uno(PAGO, 300);
    TEST_ASSERT_EQUAL_UINT16(5, comision);
}

static void test_el_uno_por_ciento_se_trunca(void)
{
    liquidar_uno(PAGO, 1099);
    TEST_ASSERT_EQUAL_UINT16(10, comision);
}

/* RARO: un pago de cero cobra la comision minima. */
static void test_un_pago_de_cero_cobra_cinco(void)
{
    liquidar_uno(PAGO, 0);
    TEST_ASSERT_EQUAL_INT32(0, neto);
    TEST_ASSERT_EQUAL_UINT16(5, comision);
}

static void test_una_recarga_suma_y_no_cobra(void)
{
    liquidar_uno(RECARGA, 2000);
    TEST_ASSERT_EQUAL_INT32(2000, neto);
    TEST_ASSERT_EQUAL_UINT16(0, comision);
}

/* RARO: una devolucion cobra comision, igual que un pago. */
static void test_una_devolucion_suma_y_tambien_cobra(void)
{
    liquidar_uno(DEVOLUCION, 1000);
    TEST_ASSERT_EQUAL_INT32(1000, neto);
    TEST_ASSERT_EQUAL_UINT16(10, comision);
}

static void test_varios_movimientos_se_acumulan(void)
{
    struct mov m[] = {{PAGO, 1000}, {RECARGA, 2000}, {PAGO, 50}, {DEVOLUCION, 300}};

    TEST_ASSERT_EQUAL_INT(0, liquidar(m, 4, &neto, &comision, SIN_REDONDEO));
    TEST_ASSERT_EQUAL_INT32(1250, neto);
    TEST_ASSERT_EQUAL_UINT16(20, comision);
}

static void test_el_redondeo_sube_la_comision_a_la_decena(void)
{
    struct mov m[] = {{PAGO, 1000}, {PAGO, 300}};

    liquidar(m, 2, &neto, &comision, CON_REDONDEO);
    TEST_ASSERT_EQUAL_UINT16(20, comision);
}

static void test_el_redondeo_deja_igual_una_decena_exacta(void)
{
    struct mov m[] = {{PAGO, 1000}, {PAGO, 1000}};

    liquidar(m, 2, &neto, &comision, CON_REDONDEO);
    TEST_ASSERT_EQUAL_UINT16(20, comision);
}

/* RARO: solo el modo 1 redondea; cualquier otro valor se comporta como el 0. */
static void test_un_modo_distinto_de_uno_no_redondea(void)
{
    struct mov m[] = {{PAGO, 1000}, {PAGO, 300}};

    liquidar(m, 2, &neto, &comision, 2);
    TEST_ASSERT_EQUAL_UINT16(15, comision);
}

static void test_un_tipo_desconocido_devuelve_menos_dos_y_no_toca_nada(void)
{
    struct mov m[] = {{PAGO, 1000}, {4, 1000}};

    TEST_ASSERT_EQUAL_INT(-2, liquidar(m, 2, &neto, &comision, SIN_REDONDEO));
    TEST_ASSERT_EQUAL_INT32(7, neto);
    TEST_ASSERT_EQUAL_UINT16(7, comision);
    TEST_ASSERT_EQUAL_UINT16(7, ult_comision);
}

static void test_mas_de_dieciseis_o_menos_de_cero_devuelve_menos_uno(void)
{
    struct mov m[17] = {{0}};

    TEST_ASSERT_EQUAL_INT(-1, liquidar(m, 17, &neto, &comision, SIN_REDONDEO));
    TEST_ASSERT_EQUAL_INT(-1, liquidar(m, -1, &neto, &comision, SIN_REDONDEO));
    TEST_ASSERT_EQUAL_INT32(7, neto);
}

static void test_dieciseis_importes_maximos(void)
{
    struct mov m[16];
    int        i;

    for (i = 0; i < 16; i++) {
        m[i].t = PAGO;
        m[i].imp = 65535;
    }
    TEST_ASSERT_EQUAL_INT(0, liquidar(m, 16, &neto, &comision, SIN_REDONDEO));
    TEST_ASSERT_EQUAL_INT32(-1048560, neto);
    TEST_ASSERT_EQUAL_UINT16(10480, comision);
}

static void test_la_ultima_comision_se_guarda_con_el_redondeo(void)
{
    struct mov m[] = {{PAGO, 1000}, {PAGO, 300}};

    liquidar(m, 2, &neto, &comision, CON_REDONDEO);
    TEST_ASSERT_EQUAL_UINT16(20, ult_comision);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_un_pago_resta_el_importe_y_cobra_el_uno_por_ciento);
    RUN_TEST(test_la_comision_minima_es_cinco);
    RUN_TEST(test_el_uno_por_ciento_se_trunca);
    RUN_TEST(test_un_pago_de_cero_cobra_cinco);
    RUN_TEST(test_una_recarga_suma_y_no_cobra);
    RUN_TEST(test_una_devolucion_suma_y_tambien_cobra);
    RUN_TEST(test_varios_movimientos_se_acumulan);
    RUN_TEST(test_el_redondeo_sube_la_comision_a_la_decena);
    RUN_TEST(test_el_redondeo_deja_igual_una_decena_exacta);
    RUN_TEST(test_un_modo_distinto_de_uno_no_redondea);
    RUN_TEST(test_un_tipo_desconocido_devuelve_menos_dos_y_no_toca_nada);
    RUN_TEST(test_mas_de_dieciseis_o_menos_de_cero_devuelve_menos_uno);
    RUN_TEST(test_dieciseis_importes_maximos);
    RUN_TEST(test_la_ultima_comision_se_guarda_con_el_redondeo);
    return UNITY_END();
}
