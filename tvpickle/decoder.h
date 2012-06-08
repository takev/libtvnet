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
#ifndef TVP_DECODER_H
#define TVP_DECODER_H

#include <stdint.h>
#include <errno.h>
#include <tvutils/tvutils.h>

typedef enum {
    TVP_TOKEN_END         = -1,
    TVP_TOKEN_ERROR       = 0,
    TVP_TOKEN_NULL        = 1,
    TVP_TOKEN_INTEGER     = 2,
    TVP_TOKEN_FLOAT       = 3,
    TVP_TOKEN_UTF8_STRING = 4,
    TVP_TOKEN_BIN_STRING  = 5,
    TVP_TOKEN_LIST        = 6,
    TVP_TOKEN_DICTIONARY  = 7
} tvp_type_t;

typedef union {
    int64_t     i;
    double      f;
    uint64_t    u;
} tvp_value_t;

typedef struct {
    tvp_type_t  type;
    tvp_value_t value;
    uint8_t     *data;
} tvp_token_t;


#define TVP_DEC_BOUND_CHECK(token, buffer, _size, _type)\
    if (unlikely((buffer->offset + (_size)) > buffer->size)) {\
        token.type = _type;\
        errno = EINVAL;\
        return token;\
    }

#define TVP_DEC_INTEGER(token, buffer, bit_size, _type)\
    TVP_DEC_BOUND_CHECK(token, buffer, (bit_size / 8), TVP_TOKEN_ERROR)\
    token.type = _type;\
    token.value.i = tvu_get_be_i ## bit_size (&buffer->data[buffer->offset]);\
    buffer->offset+= (bit_size / 8);

#define TVP_DEC_FLOAT(token, buffer, bit_size, _type)\
    TVP_DEC_BOUND_CHECK(token, buffer, (bit_size / 8), TVP_TOKEN_ERROR)\
    token.type = _type;\
    token.value.f = tvu_get_be_f ## bit_size (&buffer->data[buffer->offset]);\
    buffer->offset+= (bit_size / 8);

#define TVP_DEC_UNSIGNED(token, buffer, bit_size)\
    TVP_DEC_BOUND_CHECK(token, buffer, (bit_size / 8), TVP_TOKEN_ERROR)\
    token.value.u = tvu_get_be_u ## bit_size (&buffer->data[buffer->offset]);\
    buffer->offset+= (bit_size / 8);

static inline tvp_token_t tvp_dec_number(tvu_buffer_t *buffer, uint8_t c)
{
    tvp_token_t token;

    switch (c) {
    case 0x40: // NULL
        token.type = TVP_TOKEN_NULL;
        break;
    case 0x41: // 64 bit float
        TVP_DEC_FLOAT(token, buffer, 64, TVP_TOKEN_FLOAT)
        break;
    case 0x42: // 32 bit float
        TVP_DEC_FLOAT(token, buffer, 32, TVP_TOKEN_FLOAT)
        break;
    case 0x43: // 64 bit integer
        TVP_DEC_INTEGER(token, buffer, 64, TVP_TOKEN_INTEGER)
        break;
    case 0x44: // 32 bit integer
        TVP_DEC_INTEGER(token, buffer, 32, TVP_TOKEN_INTEGER)
        break;
    case 0x45: // 16 bit integer
        TVP_DEC_INTEGER(token, buffer, 16, TVP_TOKEN_INTEGER)
        break;
    default: // integer is encoded in the control word
        token.type = TVP_TOKEN_INTEGER;
        token.value.i = tvu_sign_extend_u8(c, 7);
        break;
    }
    return token;
}

static inline tvp_token_t tvp_dec_container(tvu_buffer_t *buffer, uint8_t c, uint8_t type)
{
    tvp_token_t token;

    switch (c & 0x1f) {
    case 0x1f: // 32 bit integer
        TVP_DEC_UNSIGNED(token, buffer, 32);
        break;
    case 0x1e: // 16 bit integer
        TVP_DEC_UNSIGNED(token, buffer, 16);
        break;
    case 0x1d: // 8 bit integer
        TVP_DEC_UNSIGNED(token, buffer, 8);
        break;
    default:
        token.value.u = c & 0x1f;
        break;
    }
    token.type = type;
    return token;
}

static inline tvp_token_t tvp_dec_token(tvu_buffer_t *buffer)
{
    tvp_token_t     token;
    uint8_t         c;

    TVP_DEC_BOUND_CHECK(token, buffer, 1, TVP_TOKEN_END);

    c = buffer->data[buffer->offset++];
    switch (c >> 5) {
    case 0:
    case 1:
    case 2:
    case 3:
        token = tvp_dec_number(buffer, c);
        break;
    case 4:
        token = tvp_dec_container(buffer, c, TVP_TOKEN_UTF8_STRING);
        TVP_DEC_BOUND_CHECK(token, buffer, token.value.u, TVP_TOKEN_ERROR);
        token.data = &buffer->data[buffer->offset];
        buffer->offset+= token.value.u;
        break;
    case 5:
        token = tvp_dec_container(buffer, c, TVP_TOKEN_BIN_STRING);
        TVP_DEC_BOUND_CHECK(token, buffer, token.value.u, TVP_TOKEN_ERROR);
        token.data = &buffer->data[buffer->offset];
        buffer->offset+= token.value.u;
        break;
    case 6:
        token = tvp_dec_container(buffer, c, TVP_TOKEN_LIST);
        break;
    case 7:
        token = tvp_dec_container(buffer, c, TVP_TOKEN_DICTIONARY);
        break;
    }

    return token;
}


#endif
