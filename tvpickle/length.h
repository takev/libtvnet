#ifndef TVP_LENGTH_H
#define TVP_LENGTH_H

#include <stdint.h>
#include <math.h>
#include <tvutils/tvutils.h>

static inline int tvp_len_integer(int64_t x)
{
    if (x  > INT32_MAX) {
        return 9;
    } else if (x  > INT16_MAX) {
        return 5;
    } else if (x  > 63) {
        return 3;
    } else if (x >= -58) {
        return 1;
    } else if (x >= INT16_MIN) {
        return 3;
    } else if (x >= INT32_MIN) {
        return 5;
    } else {
        return 9;
    }
}

static inline int tvp_len_float(double x)
{
    float y = x;

    // Integer test.
    if (floor(x) == x) {
        return tvp_len_integer(x);

    // 32-bit float test.
    } else if ((double)y == x) {
        return 5;

    // 64 bit float.
    } else {
        return 9;
    }
}

static inline int tvp_len_null()
{
    return 1;
}

static inline int tvp_len_compound(uint32_t x)
{
    if (x > UINT16_MAX) {
        return 5;
    } else if (x > UINT8_MAX)  {
        return 3;
    } else if (x > 0x1c) {
        return 2;
    } else {
        return 1;
    }
}

static inline int tvp_len_string(uint32_t buffer_size)
{
    return tvp_len_compound(buffer_size) + buffer_size;
}

static inline int tvp_len_utf8_string(uint32_t buffer_size)
{
    return tvp_len_string(buffer_size);
}

static inline int tvp_len_binary_string(uint32_t buffer_size)
{
    return tvp_len_string(buffer_size);
}

static inline int tvp_len_list(uint32_t size)
{
    return tvp_len_compound(size);
}

static inline int tvp_len_dictionary(uint32_t size)
{
    return tvp_len_compound(size);
}

#endif
