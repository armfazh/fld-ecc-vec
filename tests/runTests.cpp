#include <cpu_caps.h>
#include <gtest/gtest.h>

int main(int argc, char **argv)
{
    machine_info();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
