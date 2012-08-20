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
#include <errno.h>
#include <tvrpc/pickle_length.h>
#include <tvrpc/pickle_encoder.h>
#include <tvrpc/pickle_stdarg_encoder.h>

ssize_t tvr_length(utf8_t const * restrict fmt, ...)
{
    va_list ap;
    ssize_t size;

    va_start(ap, fmt);
    size = tvr_vlength(fmt, ap);
    va_end(ap);

    return size;
}

int tvr_encode(tvu_buffer_t * restrict buffer, utf8_t const * restrict fmt, ...)
{
    va_list ap;
    int     ret;

    va_start(ap, fmt);
    ret = tvr_vencode(buffer, fmt, ap);
    va_end(ap);

    return ret;
}

ssize_t tvr_vlength(utf8_t const * restrict fmt, va_list ap)
{
    utf8_t   c;
    utf8_t   *s;
    ssize_t  size = 0;

    while ((c = *fmt++) != '\0') {
        switch (c) {
        case 'b': size += tvr_len_integer(va_arg(ap, int32_t)); break;      // char promoted to int
        case 'B': size += tvr_len_integer(va_arg(ap, uint32_t)); break;     // char promoted to int
        case 'w': size += tvr_len_integer(va_arg(ap, int32_t)); break;      // short promoted to int
        case 'W': size += tvr_len_integer(va_arg(ap, uint32_t)); break;     // short promoted to int
        case 'd': size += tvr_len_integer(va_arg(ap, int32_t)); break;
        case 'D': size += tvr_len_integer(va_arg(ap, uint32_t)); break;
        case 'q': size += tvr_len_integer(va_arg(ap, int64_t)); break;
        case 'Q': size += tvr_len_integer(va_arg(ap, uint64_t)); break;
        case 'f': size += tvr_len_float(va_arg(ap, float64_t)); break;      // float promoted to double
        case 'F': size += tvr_len_float(va_arg(ap, float64_t)); break;
        case 'G': size += tvr_len_float(va_arg(ap, float80_t)); break;
        case 's':
            s = va_arg(ap, utf8_t *);
            size+= tvr_len_utf8_string(strlen(s));
            break;
        default:
            errno = EINVAL;
            return -1;
        }
    }
    return size;
}

int tvr_vencode(tvu_buffer_t * restrict buffer, utf8_t const * restrict fmt, va_list ap)
{
    utf8_t    c;
    utf8_t    *s;

    while ((c = *fmt++) != '\0') {
        switch (c) {
        case 'b': tvr_enc_integer(buffer, va_arg(ap, int32_t)); break;
        case 'B': tvr_enc_integer(buffer, va_arg(ap, uint32_t)); break;
        case 'w': tvr_enc_integer(buffer, va_arg(ap, int32_t)); break;
        case 'W': tvr_enc_integer(buffer, va_arg(ap, uint32_t)); break;
        case 'd': tvr_enc_integer(buffer, va_arg(ap, int32_t)); break;
        case 'D': tvr_enc_integer(buffer, va_arg(ap, uint32_t)); break;
        case 'q': tvr_enc_integer(buffer, va_arg(ap, int64_t)); break;
        case 'Q': tvr_enc_integer(buffer, va_arg(ap, uint64_t)); break;
        case 'f': tvr_enc_float(buffer, va_arg(ap, float64_t)); break;
        case 'F': tvr_enc_float(buffer, va_arg(ap, float64_t)); break;
        case 's':
            s = va_arg(ap, utf8_t *);
            tvr_enc_utf8_string(buffer, s);
            break;
        default:
            errno = EINVAL;
            return -1;
        }
    }
    return 0;
}



