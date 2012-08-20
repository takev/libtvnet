/* libtvrpc - Take Vos' Pickle; object serializer and deserializer.
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
#ifndef TVR_PICKLE_DECODER_H
#define TVR_PICKLE_DECODER_H

#include <stdint.h>
#include <errno.h>
#include <tvutils/tvutils.h>

typedef enum {
    TVR_PICKLE_TOKEN_END         = -1,
    TVR_PICKLE_TOKEN_ERROR       = 0,
    TVR_PICKLE_TOKEN_NULL        = 1,
    TVR_PICKLE_TOKEN_INTEGER     = 2,
    TVR_PICKLE_TOKEN_FLOAT       = 3,
    TVR_PICKLE_TOKEN_UTF8_STRING = 4,
    TVR_PICKLE_TOKEN_BIN_STRING  = 5,
    TVR_PICKLE_TOKEN_LIST        = 6,
    TVR_PICKLE_TOKEN_DICTIONARY  = 7
} tvr_pickle_type_t;

typedef union {
    int64_t     i;
    double      f;
    uint64_t    u;
} tvr_pickle_value_t;

typedef struct {
    tvr_pickle_type_t   type;
    tvr_pickle_value_t  value;
    void                *data;
} tvr_pickle_token_t;


#define TVR_PICKLE_DECODE_BOUND_CHECK(token, buffer, _size, _type)\
    if (unlikely((buffer->offset + (_size)) > buffer->size)) {\
        token.type = _type;\
        errno = EINVAL;\
        return token;\
    }

#define TVR_PICKLE_DECODE_INTEGER(token, buffer, bit_size, _type)\
    TVR_PICKLE_DECODE_BOUND_CHECK(token, buffer, (bit_size / 8), TVR_PICKLE_TOKEN_ERROR)\
    token.type = _type;\
    token.value.i = tvu_get_be_i ## bit_size (&buffer->data[buffer->offset]);\
    buffer->offset+= (bit_size / 8);

#define TVR_PICKLE_DECODE_FLOAT(token, buffer, bit_size, _type)\
    TVR_PICKLE_DECODE_BOUND_CHECK(token, buffer, (bit_size / 8), TVR_PICKLE_TOKEN_ERROR)\
    token.type = _type;\
    token.value.f = tvu_get_be_f ## bit_size (&buffer->data[buffer->offset]);\
    buffer->offset+= (bit_size / 8);

#define TVR_PICKLE_DECODE_UNSIGNED(token, buffer, bit_size)\
    TVR_PICKLE_DECODE_BOUND_CHECK(token, buffer, (bit_size / 8), TVR_PICKLE_TOKEN_ERROR)\
    token.value.u = tvu_get_be_u ## bit_size (&buffer->data[buffer->offset]);\
    buffer->offset+= (bit_size / 8);

static inline tvr_pickle_token_t tvr_pickle_decode_number(tvu_buffer_t *buffer, uint8_t c)
{
    tvr_pickle_token_t token;

    switch (c) {
    case 0x40: // NULL
        token.type = TVR_PICKLE_TOKEN_NULL;
        break;
    case 0x41: // 64 bit float
        TVR_PICKLE_DECODE_FLOAT(token, buffer, 64, TVR_PICKLE_TOKEN_FLOAT)
        break;
    case 0x42: // 32 bit float
        TVR_PICKLE_DECODE_FLOAT(token, buffer, 32, TVR_PICKLE_TOKEN_FLOAT)
        break;
    case 0x43: // 64 bit integer
        TVR_PICKLE_DECODE_INTEGER(token, buffer, 64, TVR_PICKLE_TOKEN_INTEGER)
        break;
    case 0x44: // 32 bit integer
        TVR_PICKLE_DECODE_INTEGER(token, buffer, 32, TVR_PICKLE_TOKEN_INTEGER)
        break;
    case 0x45: // 16 bit integer
        TVR_PICKLE_DECODE_INTEGER(token, buffer, 16, TVR_PICKLE_TOKEN_INTEGER)
        break;
    default: // integer is encoded in the control word
        token.type = TVR_PICKLE_TOKEN_INTEGER;
        token.value.i = tvu_sign_extend_u8(c, 7);
        break;
    }
    return token;
}

static inline tvr_pickle_token_t tvr_pickle_decode_container(tvu_buffer_t *buffer, uint8_t c, uint8_t type)
{
    tvr_pickle_token_t token;

    switch (c & 0x1f) {
    case 0x1f: // 32 bit integer
        TVR_PICKLE_DECODE_UNSIGNED(token, buffer, 32);
        break;
    case 0x1e: // 16 bit integer
        TVR_PICKLE_DECODE_UNSIGNED(token, buffer, 16);
        break;
    case 0x1d: // 8 bit integer
        TVR_PICKLE_DECODE_UNSIGNED(token, buffer, 8);
        break;
    default:
        token.value.u = c & 0x1f;
        break;
    }
    token.type = type;
    return token;
}

static inline tvr_pickle_token_t tvr_pickle_decode_token(tvu_buffer_t *buffer)
{
    tvr_pickle_token_t     token;
    uint8_t         c;

    TVR_PICKLE_DECODE_BOUND_CHECK(token, buffer, 1, TVR_PICKLE_TOKEN_END);

    c = buffer->data[buffer->offset++];
    switch (c >> 5) {
    case 0:
    case 1:
    case 2:
    case 3:
        token = tvr_pickle_decode_number(buffer, c);
        break;
    case 4:
        token = tvr_pickle_decode_container(buffer, c, TVR_PICKLE_TOKEN_UTF8_STRING);
        TVR_PICKLE_DECODE_BOUND_CHECK(token, buffer, token.value.u, TVR_PICKLE_TOKEN_ERROR);
        token.data = &buffer->data[buffer->offset];
        // Skip over the string + NUL.
        buffer->offset+= token.value.u + 1;
        break;
    case 5:
        token = tvr_pickle_decode_container(buffer, c, TVR_PICKLE_TOKEN_BIN_STRING);
        TVR_PICKLE_DECODE_BOUND_CHECK(token, buffer, token.value.u, TVR_PICKLE_TOKEN_ERROR);
        token.data = &buffer->data[buffer->offset];
        // Skip over the binary data.
        buffer->offset+= token.value.u;
        break;
    case 6:
        token = tvr_pickle_decode_container(buffer, c, TVR_PICKLE_TOKEN_LIST);
        break;
    case 7:
        token = tvr_pickle_decode_container(buffer, c, TVR_PICKLE_TOKEN_DICTIONARY);
        break;
    }

    return token;
}


#endif
