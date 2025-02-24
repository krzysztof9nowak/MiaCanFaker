#include <miaui-utils.h>

void mui_UFixedPointToStr(char* out, uint32_t value, uint8_t dec_places)
{
    static const uint32_t DIGITS[] = { 1e9, 1e8, 1e7, 1e6, 1e5, 1e4, 1e3, 1e2, 1e1, 1e0 };
    char* start_out = out;

    for (register int index = 0; index < 10; index++) {
        register uint32_t this_digit = 0;
        register uint32_t this_mask = DIGITS[index];
        while (value >= this_mask) {
            value -= this_mask;
            ++this_digit;
        }

        *out = (char)('0' + this_digit);

        if (index == 9 - dec_places) {
            *(++out) = dec_places ? '.' : 0;
        }

        if (this_digit > 0 || out > start_out) {
            ++out;
        }
    }

    *out = 0;
}

void mui_FixedPointToStr(char* out, int32_t value, uint8_t dec_places)
{
    if (value < 0) {
        *(out++) = '-';
        value *= -1;
    }

    mui_UFixedPointToStr(out, (uint32_t)value, dec_places);
}
