#include <gtest/gtest.h>

class ControlVolumeTest : public testing::Test {
protected:
    virtual void SetUp() {
    }
};

TEST_F(ControlVolumeTest, updateTest){
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
