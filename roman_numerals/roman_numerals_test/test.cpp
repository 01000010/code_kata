#include "pch.h"
#include "../roman_numerals/arabic_roman_converter.hpp"

TEST(ArabicToRomanNumeralsConverterTestCase, PreparationsCompleted) {
    GTEST_SUCCEED();
}

TEST(ArabicToRomanNumeralsConverterTestCase, 1_isConvertedTo_I) {
    ASSERT_EQ("I", to_roman_numeral(1));
}