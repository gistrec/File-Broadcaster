#include <gtest/gtest.h>

#include "../src/Utils.hpp"

class BytesConverter : public ::testing::TestWithParam<std::tuple<size_t, int>> {
public:
    size_t value;
    int    count;

protected:
    void SetUp() override {
        std::tie(value, count) = GetParam();
    }
};

INSTANTIATE_TEST_CASE_P(
    CombinationsTest, BytesConverter,
    ::testing::Combine(
        ::testing::Values(0U, 1U, 2U, 1000U, 65535U, 2147483647U),
        ::testing::Values(4, 8, 16)));

TEST_P(BytesConverter, getIntFromBytes) {
    char buffer[16] = { 0 };

    Utils::writeBytesFromNumber(buffer, value, count);

    size_t result = Utils::getNumberFromBytes(buffer, count);

    EXPECT_EQ(result, value);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}