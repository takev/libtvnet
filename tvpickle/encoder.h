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
#ifndef TVP_ENCODER_H
#define TVP_ENCODER_H

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <tvutils/tvutils.h>
#include <tvpickle/length.h>

#define TVP_ENC_INTEGER(buffer, contr, x, bit_size)\
        buffer->data[buffer->offset] = contr;\
        tvu_set_be_i ## bit_size (&buffer->data[buffer->offset + 1], x);\
        buffer->offset+= (bit_size / 8) + 1;

#define TVP_ENC_FLOAT(buffer, contr, x, bit_size)\
        buffer->data[buffer->offset] = contr;\
        tvu_set_be_f ## bit_size (&buffer->data[buffer->offset + 1], x);\
        buffer->offset+= (bit_size / 8) + 1;

static inline void tvp_enc_integer(tvu_buffer_t * restrict buffer, int64_t x)
{
    if (x  > INT32_MAX) {
        TVP_ENC_INTEGER(buffer, 0x43, x, 64);
    } else if (x  > INT16_MAX) {
        TVP_ENC_INTEGER(buffer, 0x44, x, 32);
    } else if (x  > 63)  {
        TVP_ENC_INTEGER(buffer, 0x45, x, 16);
    } else if (x >= -58) {
        buffer->data[buffer->offset++] = x & 0x7f;
    } else if (x >= INT16_MIN) {
        TVP_ENC_INTEGER(buffer, 0x45, x, 16);
    } else if (x >= INT32_MIN) {
        TVP_ENC_INTEGER(buffer, 0x44, x, 32);
    } else {
        TVP_ENC_INTEGER(buffer, 0x43, x, 64);
    }
}

static inline void tvp_enc_float(tvu_buffer_t * restrict buffer, double x)
{
    float y = x;

    // Integer test.
    if (floor(x) == x) {
        tvp_enc_integer(buffer, x);

    // 32-bit float test.
    } else if ((double)y == x) {
        TVP_ENC_FLOAT(buffer, 0x42, x, 32);

    // 64 bit float.
    } else {
        TVP_ENC_FLOAT(buffer, 0x41, x, 64);
    }
}

static inline void tvp_enc_null(tvu_buffer_t * restrict buffer)
{
    buffer->data[buffer->offset++] = 0x40;
}

static inline void tvp_enc_compound(tvu_buffer_t * restrict buffer, uint8_t c, uint32_t x)
{
    if (x > UINT16_MAX) {
        TVP_ENC_INTEGER(buffer, c | 0x1f, x, 32);
    } else if (x > UINT8_MAX)  {
        TVP_ENC_INTEGER(buffer, c | 0x1e, x, 16);
    } else if (x > 0x1c) {
        TVP_ENC_INTEGER(buffer, c | 0x1d, x, 8);
    } else {
        buffer->data[buffer->offset++] = c | x;
    }
}

static inline void tvp_enc_string(tvu_buffer_t * restrict buffer, uint8_t c, uint8_t const * restrict string, uint32_t string_size)
{
    tvp_enc_compound(buffer, c, string_size);

    memcpy(&buffer->data[buffer->offset], string, string_size);
    buffer->offset+= string_size;
}

static inline void tvp_enc_utf8_string_and_size(tvu_buffer_t * restrict buffer, utf8_t const * restrict string, uint32_t string_size)
{
    tvp_enc_string(buffer, 0x80, string, string_size);
    // Terminate the UTF-8 string with a NUL.
    buffer->data[buffer->offset++] = 0;
}

static inline void tvp_enc_utf8_string(tvu_buffer_t * restrict buffer, utf8_t const * restrict string)
{
    tvp_enc_utf8_string_and_size(buffer, string, tvu_strlen(string));
}

static inline void tvp_enc_binary_string(tvu_buffer_t * restrict buffer, uint8_t const * restrict string, uint32_t string_size)
{
    tvp_enc_string(buffer, 0xa0, string, string_size);
}

static inline void tvp_enc_list(tvu_buffer_t * restrict buffer, uint32_t size)
{
    tvp_enc_compound(buffer, 0xc0, size);
}

static inline void tvp_enc_dictionary(tvu_buffer_t * restrict buffer, uint32_t size)
{
    tvp_enc_compound(buffer, 0xe0, size);
}


#endif
