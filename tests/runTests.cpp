#include <cpu_caps.h>
#include <gtest/gtest.h>

class MyEnv : public ::testing::Environment
{
    void SetUp() override
    {
        machine_info();
    }
};

testing::Environment* const myEnv = testing::AddGlobalTestEnvironment(new MyEnv);
