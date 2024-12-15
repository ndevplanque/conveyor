#include <unity.h>

void setUp(void)
{
    // set stuff up here
}

void tearDown(void)
{
    // clean stuff up here
}

void test_config_is_good()
{
    TEST_ASSERT_TRUE(true);
    TEST_ASSERT_FALSE(false);
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_config_is_good);

    UNITY_END();
}