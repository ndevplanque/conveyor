#include <Arduino.h>
#include <unity.h>

// Fonction à tester
int add(int a, int b)
{
    return a + b;
}

// Setup avant les tests
void setUp(void)
{
    // Initialise si nécessaire
}

// Nettoyage après les tests
void tearDown(void)
{
    // Nettoyage si nécessaire
}

// Tests unitaires
void test_add_positive_numbers(void)
{
    TEST_ASSERT_EQUAL(5, add(2, 3));
}

void test_add_negative_numbers(void)
{
    TEST_ASSERT_EQUAL(-5, add(-2, -3));
}

void setup()
{
    delay(2000); // Optionnel, parfois nécessaire pour éviter des problèmes de démarrage
    UNITY_BEGIN();
    RUN_TEST(test_add_positive_numbers);
    RUN_TEST(test_add_negative_numbers);
    UNITY_END();
    // Stoppe la carte après les tests
    // while (true);
}

void loop()
{
    // Ne rien mettre ici
}
