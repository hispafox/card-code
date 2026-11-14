#include "unity.h"

#include <setjmp.h>

#include "estados.h"
#include "memoria.h"
#include "pin.h"

static const uint8_t PIN_TITULAR[LONGITUD_PIN] = {1, 2, 3, 4};
static const uint8_t PIN_EQUIVOCADO[LONGITUD_PIN] = {9, 9, 9, 9};

static jmp_buf antes_del_corte;

static void cortar_alimentacion(void)
{
    longjmp(antes_del_corte, 1);
}

void setUp(void)
{
    memoria_formatear();
    pin_personalizar(PIN_TITULAR);
}

void tearDown(void) {}

/*
 * El corte llega en la segunda lectura de la memoria: la primera es la del
 * contador de intentos y la segunda, la del primer dígito del PIN guardado,
 * en mitad de la comparación. pin_verificar no llega a devolver nada.
 */
static void test_un_corte_a_mitad_de_la_comparacion_gasta_el_intento(void)
{
    memoria_simular_corte(2, cortar_alimentacion);
    if (setjmp(antes_del_corte) == 0) {
        pin_verificar(PIN_EQUIVOCADO, LONGITUD_PIN);
        TEST_FAIL_MESSAGE("La verificación tenía que cortarse a mitad");
    }
    memoria_simular_corte(0, 0);

    /* Si el intento cortado se gastó, con dos fallos más el PIN queda bloqueado. */
    pin_verificar(PIN_EQUIVOCADO, LONGITUD_PIN);
    pin_verificar(PIN_EQUIVOCADO, LONGITUD_PIN);
    TEST_ASSERT_EQUAL_HEX16(SW_PIN_BLOQUEADO, pin_verificar(PIN_TITULAR, LONGITUD_PIN));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_un_corte_a_mitad_de_la_comparacion_gasta_el_intento);
    return UNITY_END();
}
