#include "pch.h"

#include <string>
#include <array>

struct ArabicToRomanMapping {
    unsigned int arabic_number;
    std::string roman_numeral;
};

const std::size_t number_of_mappings{ 13 };

using ArabicToRomanMappings = std::array<ArabicToRomanMapping, number_of_mappings>;

const ArabicToRomanMappings arabic_to_roman_mappings{ {
    { 1000, "M" },
    { 900, "XM" },
    { 500, "D" },
    { 400, "CD" },
    { 100, "C" },
    { 90, "XC" },
    { 50, "L" },
    { 40, "XL" },
    { 10, "X" },
    { 9, "IX" },
    { 5, "V" },
    { 4, "IV" },
    { 1, "I" }
} };

std::string to_roman_numeral(unsigned int arabic_number)
{
    std::string roman_numeral;

    for (const auto& mapping : arabic_to_roman_mappings) {
        while (arabic_number >= mapping.arabic_number) {
            roman_numeral += mapping.roman_numeral;
            arabic_number -= mapping.arabic_number;
        }
    }

    return roman_numeral;
}

class RomanNumeralAssert
{
private:
    const unsigned int arabic_number_to_convert;
public:
    RomanNumeralAssert() = delete;
    explicit RomanNumeralAssert(const unsigned int arabic_number)
        : arabic_number_to_convert{ arabic_number }
    {}

    void IsConvertedToRomanNumeral(const std::string& expected_roman_numeral) const
    {
        ASSERT_EQ(expected_roman_numeral, to_roman_numeral(arabic_number_to_convert));
    }
};

RomanNumeralAssert AssertThat(const unsigned int arabic_number)
{
    RomanNumeralAssert assert{ arabic_number };
    return assert;
}

TEST(ArabicToRomanNumeralsConverterTestCase, ConversionOfArabicNumbersToRomanNumerals_Works)
{
    AssertThat(1).IsConvertedToRomanNumeral("I");
    AssertThat(2).IsConvertedToRomanNumeral("II");
    AssertThat(3).IsConvertedToRomanNumeral("III");
    AssertThat(10).IsConvertedToRomanNumeral("X");
    AssertThat(20).IsConvertedToRomanNumeral("XX");
    AssertThat(30).IsConvertedToRomanNumeral("XXX");
    AssertThat(33).IsConvertedToRomanNumeral("XXXIII");
    AssertThat(100).IsConvertedToRomanNumeral("C");
    AssertThat(200).IsConvertedToRomanNumeral("CC");
    AssertThat(300).IsConvertedToRomanNumeral("CCC");
    AssertThat(1000).IsConvertedToRomanNumeral("M");
    AssertThat(2000).IsConvertedToRomanNumeral("MM");
    AssertThat(3000).IsConvertedToRomanNumeral("MMM");
    AssertThat(3333).IsConvertedToRomanNumeral("MMMCCCXXXIII");
    AssertThat(5).IsConvertedToRomanNumeral("V");
    AssertThat(6).IsConvertedToRomanNumeral("VI");
    AssertThat(37).IsConvertedToRomanNumeral("XXXVII");
    AssertThat(555).IsConvertedToRomanNumeral("DLV");
    AssertThat(4).IsConvertedToRomanNumeral("IV");
    AssertThat(3999).IsConvertedToRomanNumeral("MMMCMXCIX");
}