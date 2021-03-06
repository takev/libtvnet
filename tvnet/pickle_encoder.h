/* libtvnet - Take Vos' Network tools.
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
#ifndef TVN_PICKLE_ENCODER_H
#define TVN_PICKLE_ENCODER_H

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <tvutils/tvutils.h>
#include <tvnet/pickle_length.h>

#define TVNP_ENCODE_INTEGER(buffer, contr, x, bit_size)\
        buffer->data[buffer->offset] = contr;\
        tvu_set_be_i ## bit_size (&buffer->data[buffer->offset + 1], x);\
        buffer->offset+= (bit_size / 8) + 1;

#define TVNP_ENCODE_FLOAT(buffer, contr, x, bit_size)\
        buffer->data[buffer->offset] = contr;\
        tvu_set_be_f ## bit_size (&buffer->data[buffer->offset + 1], x);\
        buffer->offset+= (bit_size / 8) + 1;

static inline void tvnp_encode_integer(tvu_buffer_t * restrict buffer, int64_t x)
{
    if (x  > INT32_MAX) {
        TVNP_ENCODE_INTEGER(buffer, 0x43, x, 64);
    } else if (x  > INT16_MAX) {
        TVNP_ENCODE_INTEGER(buffer, 0x44, x, 32);
    } else if (x  > 63)  {
        TVNP_ENCODE_INTEGER(buffer, 0x45, x, 16);
    } else if (x >= -58) {
        buffer->data[buffer->offset++] = x & 0x7f;
    } else if (x >= INT16_MIN) {
        TVNP_ENCODE_INTEGER(buffer, 0x45, x, 16);
    } else if (x >= INT32_MIN) {
        TVNP_ENCODE_INTEGER(buffer, 0x44, x, 32);
    } else {
        TVNP_ENCODE_INTEGER(buffer, 0x43, x, 64);
    }
}

static inline void tvnp_encode_float(tvu_buffer_t * restrict buffer, double x)
{
    float y = x;

    // Integer test.
    if (floor(x) == x) {
        tvnp_encode_integer(buffer, x);

    // 32-bit float test.
    } else if ((double)y == x) {
        TVNP_ENCODE_FLOAT(buffer, 0x42, x, 32);

    // 64 bit float.
    } else {
        TVNP_ENCODE_FLOAT(buffer, 0x41, x, 64);
    }
}

static inline void tvnp_encode_null(tvu_buffer_t * restrict buffer)
{
    buffer->data[buffer->offset++] = 0x40;
}

static inline void tvnp_encode_compound(tvu_buffer_t * restrict buffer, uint8_t c, uint32_t x)
{
    if (x > UINT16_MAX) {
        TVNP_ENCODE_INTEGER(buffer, c | 0x1f, x, 32);
    } else if (x > UINT8_MAX)  {
        TVNP_ENCODE_INTEGER(buffer, c | 0x1e, x, 16);
    } else if (x > 0x1c) {
        TVNP_ENCODE_INTEGER(buffer, c | 0x1d, x, 8);
    } else {
        buffer->data[buffer->offset++] = c | x;
    }
}

static inline void tvnp_encode_string(tvu_buffer_t * restrict buffer, uint8_t c, void const * restrict string, uint32_t string_size)
{
    tvnp_encode_compound(buffer, c, string_size);

    memcpy(&buffer->data[buffer->offset], string, string_size);
    buffer->offset+= string_size;
}

static inline void tvnp_encode_utf8_string_and_size(tvu_buffer_t * restrict buffer, utf8_t const * restrict string, uint32_t string_size)
{
    tvnp_encode_string(buffer, 0x80, string, string_size);
    // Terminate the UTF-8 string with a NUL.
    buffer->data[buffer->offset++] = 0;
}

static inline void tvnp_encode_utf8_string(tvu_buffer_t * restrict buffer, utf8_t const * restrict string)
{
    tvnp_encode_utf8_string_and_size(buffer, string, strlen(string));
}

static inline void tvnp_encode_binary_string(tvu_buffer_t * restrict buffer, void const * restrict string, uint32_t string_size)
{
    tvnp_encode_string(buffer, 0xa0, string, string_size);
}

static inline void tvnp_encode_list(tvu_buffer_t * restrict buffer, uint32_t size)
{
    tvnp_encode_compound(buffer, 0xc0, size);
}

static inline void tvnp_encode_dictionary(tvu_buffer_t * restrict buffer, uint32_t size)
{
    tvnp_encode_compound(buffer, 0xe0, size);
}


#endif
