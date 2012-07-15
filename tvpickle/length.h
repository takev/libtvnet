/* libtvpickle - Take Vos' Pickle; object serializer and deserializer.
 * Copyright (C) 2012  Take Vos <take.vos@vosgames.nl>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; of version 2 of the License.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef TVP_LENGTH_H
#define TVP_LENGTH_H

#include <stdint.h>
#include <math.h>
#include <tvutils/tvutils.h>

static inline size_t tvp_len_integer(int64_t x)
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

static inline size_t tvp_len_float(double x)
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

static inline size_t tvp_len_null()
{
    return 1;
}

static inline size_t tvp_len_compound(size_t x)
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

static inline size_t tvp_len_string(size_t buffer_size)
{
    return tvp_len_compound(buffer_size) + buffer_size;
}

static inline size_t tvp_len_utf8_string(size_t buffer_size)
{
    // an UTF-8 string has a nul terminator outside of the tag/length field.
    return tvp_len_string(buffer_size) + 1;
}

static inline size_t tvp_len_binary_string(size_t buffer_size)
{
    return tvp_len_string(buffer_size);
}

static inline size_t tvp_len_list(size_t size)
{
    return tvp_len_compound(size);
}

static inline size_t tvp_len_dictionary(size_t size)
{
    return tvp_len_compound(size);
}

#endif
