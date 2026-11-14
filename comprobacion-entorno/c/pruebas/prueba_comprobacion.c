#include "unity.h"
#include "comprobacion.h"

void setUp(void) {}
void tearDown(void) {}

static void test_suma_dos_valores(void)
{
    TEST_ASSERT_EQUAL_HEX16(0x0005u, comprobacion_sumar(2u, 3u));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_suma_dos_valores);
    return UNITY_END();
}
