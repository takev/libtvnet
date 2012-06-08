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
#include <errno.h>
#include <tvpickle/length.h>
#include <tvpickle/encoder.h>
#include <tvpickle/stdarg_encoder.h>

ssize_t tvp_length(char const * restrict fmt, ...)
{
    va_list ap;
    ssize_t size;

    va_start(ap, fmt);
    size = tvp_vlength(fmt, ap);
    va_end(ap);

    return size;
}

int tvp_encoder(tvu_buffer_t * restrict buffer, char const * restrict fmt, ...)
{
    va_list ap;
    int     ret;

    va_start(ap, fmt);
    ret = tvp_vencode(buffer, fmt, ap);
    va_end(ap);

    return ret;
}

ssize_t tvp_vlength(char const * restrict fmt, va_list ap)
{
    char    c;
    uint8_t *s;
    ssize_t size = 0;

    while ((c = *fmt++) != '\0') {
        switch (c) {
        case 'b': size += tvp_len_integer(va_arg(ap, int32_t)); break;      // char promoted to int
        case 'B': size += tvp_len_integer(va_arg(ap, uint32_t)); break;     // char promoted to int
        case 'w': size += tvp_len_integer(va_arg(ap, int32_t)); break;      // short promoted to int
        case 'W': size += tvp_len_integer(va_arg(ap, uint32_t)); break;     // short promoted to int
        case 'd': size += tvp_len_integer(va_arg(ap, int32_t)); break;
        case 'D': size += tvp_len_integer(va_arg(ap, uint32_t)); break;
        case 'q': size += tvp_len_integer(va_arg(ap, int64_t)); break;
        case 'Q': size += tvp_len_integer(va_arg(ap, uint64_t)); break;
        case 'f': size += tvp_len_float(va_arg(ap, float64_t)); break;      // float promoted to double
        case 'F': size += tvp_len_float(va_arg(ap, float64_t)); break;
        case 's':
            s = va_arg(ap, uint8_t *);
            size+= tvp_len_utf8_string(strlen((char *)s));
            break;
        default:
            errno = EINVAL;
            return -1;
        }
    }
    return size;
}

int tvp_vencode(tvu_buffer_t * restrict buffer, char const * restrict fmt, va_list ap)
{
    char    c;
    uint8_t *s;

    while ((c = *fmt++) != '\0') {
        switch (c) {
        case 'b': tvp_enc_integer(buffer, va_arg(ap, int32_t)); break;
        case 'B': tvp_enc_integer(buffer, va_arg(ap, uint32_t)); break;
        case 'w': tvp_enc_integer(buffer, va_arg(ap, int32_t)); break;
        case 'W': tvp_enc_integer(buffer, va_arg(ap, uint32_t)); break;
        case 'd': tvp_enc_integer(buffer, va_arg(ap, int32_t)); break;
        case 'D': tvp_enc_integer(buffer, va_arg(ap, uint32_t)); break;
        case 'q': tvp_enc_integer(buffer, va_arg(ap, int64_t)); break;
        case 'Q': tvp_enc_integer(buffer, va_arg(ap, uint64_t)); break;
        case 'f': tvp_enc_float(buffer, va_arg(ap, float64_t)); break;
        case 'F': tvp_enc_float(buffer, va_arg(ap, float64_t)); break;
        case 's':
            s = va_arg(ap, uint8_t *);
            tvp_enc_utf8_string(buffer, s, strlen((char *)s));
            break;
        default:
            errno = EINVAL;
            return -1;
        }
    }
    return 0;
}



