/* libtvlog - Take Vos's log library
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
#include <string.h>
#include <tvutils/tvutils.h>
#include <tvnet/pickle_length.h>
#include <tvnet/pickle_encoder.h>
#include <tvnet/pickle_decoder.h>
#include <tvnet/pickle_stdarg_encoder.h>
#include <tvnet/fmt_parser.h>
#include <tvnet/pickle_print.h>

size_t tvnp_vpprintf_len(utf8_t const * restrict types, utf8_t const * restrict fmt, va_list ap)
{
    size_t  pickle_size;
    utf8_t  c;

    pickle_size = tvnp_length_utf8_string(strlen(fmt));
    pickle_size+= tvnp_vlength(types, ap);
    return pickle_size;
}

void tvnp_vpprintf(tvu_buffer_t * restrict buffer, utf8_t const * restrict types, utf8_t const * restrict fmt, va_list ap)
{
    tvnp_encode_utf8_string(buffer, fmt);
    tvnp_vencode(buffer, types, ap);
}

tvu_buffer_t tvnp_vapprintf(utf8_t const * restrict fmt, va_list ap)
{
    utf8_t          types[tvu_count_character(fmt, '%') + 1];
    va_list         ap2;
    tvu_buffer_t    buffer;

    va_copy(ap2, ap);

    // Parse the format string into types.
    if (tvnp_fmt_parse(types, fmt) == -1) {
        return tvu_buffer_error();
    }

    // Determine pickle format string and arguments.
    buffer = tvu_buffer_alloc(tvnp_vpprintf_len(types, fmt, ap));

    // Pickle format string and arguments.
    tvnp_vpprintf(&buffer, types, fmt, ap2);
    return buffer;
}

tvu_buffer_t tvnp_apprintf(utf8_t const * restrict fmt, ...)
{
    va_list         ap;
    tvu_buffer_t    ret;

    va_start(ap, fmt);
    ret = tvnp_vapprintf(fmt, ap);
    va_end(ap);
    return ret;
}

utf8_t *tvnp_pickle_to_va(va_list ap, char *extern_ap_buf, tvu_buffer_t * restrict in)
{
    tvnp_token_t token;
    int         i = 0;
    utf8_t      c;
    utf8_t      *fmt;

    // First extract the format from the pickle.
    token = tvnp_decode_token(in);
    switch (token.type) {
    case TVNP_TOKEN_UTF8_STRING:
        fmt = token.data;
        break;
    default:
        errno = EINVAL;
        return NULL;
    }

    // Convert the format into C types.
    utf8_t fmt_types[tvu_count_character(fmt, '%')];
    tvnp_fmt_parse(fmt_types, fmt);

    // Build up va_list.
    tvu_va_make2(ap, extern_ap_buf);
    while ((c = fmt_types[i++]) != '\0') {
        token = tvnp_decode_token(in);
        switch (c) {
        case 'b':   // All integer va_list parameters are converted to at least int32_t.
        case 'w':
        case 'd':
            switch (token.type) {
            case TVNP_TOKEN_INTEGER: tvu_va_set_gp(ap, int32_t, token.value.i); break;
            case TVNP_TOKEN_FLOAT:   tvu_va_set_gp(ap, int32_t, token.value.f); break;
            default: errno = EINVAL; return NULL;
            }
            break;
        case 'B':
        case 'W':
        case 'D':
            switch (token.type) {
            case TVNP_TOKEN_INTEGER: tvu_va_set_gp(ap, uint32_t, token.value.i); break;
            case TVNP_TOKEN_FLOAT:   tvu_va_set_gp(ap, uint32_t, token.value.f); break;
            default: errno = EINVAL; return NULL;
            }
            break;
        case 'q':
            switch (token.type) {
            case TVNP_TOKEN_INTEGER: tvu_va_set_gp(ap, int64_t, token.value.i); break;
            case TVNP_TOKEN_FLOAT:   tvu_va_set_gp(ap, int64_t, token.value.f); break;
            default: errno = EINVAL; return NULL;
            }
            break;
        case 'Q':
            switch (token.type) {
            case TVNP_TOKEN_INTEGER: tvu_va_set_gp(ap, uint64_t, token.value.i); break;
            case TVNP_TOKEN_FLOAT:   tvu_va_set_gp(ap, uint64_t, token.value.f); break;
            default: errno = EINVAL; return NULL;
            }
            break;
        case 'f':
        case 'F':
            switch (token.type) {
            case TVNP_TOKEN_INTEGER: tvu_va_set_fp(ap, float64_t, token.value.i); break;
            case TVNP_TOKEN_FLOAT:   tvu_va_set_fp(ap, float64_t, token.value.f); break;
            default: errno = EINVAL; return NULL;
            }
            break;
        case 'G':
            switch (token.type) {
            case TVNP_TOKEN_INTEGER: tvu_va_set_fp(ap, float80_t, token.value.i); break;
            case TVNP_TOKEN_FLOAT:   tvu_va_set_fp(ap, float80_t, token.value.f); break;
            default: errno = EINVAL; return NULL;
            }
            break;
        case 's':
            switch (token.type) {
            case TVNP_TOKEN_UTF8_STRING: tvu_va_set_gp(ap, utf8_t *, token.data);         break;
            case TVNP_TOKEN_BIN_STRING:  tvu_va_set_gp(ap, utf8_t *, token.data);         break;
            case TVNP_TOKEN_NULL:        tvu_va_set_gp(ap, utf8_t *, (utf8_t *)"(NULL)"); break;
            default: errno = EINVAL; return NULL;
            }
            break;
        default:
            errno = EINVAL;
            return NULL;
        }
    }
    return fmt;
}


int tvnp_fprintp(FILE * restrict stream, tvu_buffer_t * restrict in)
{
    va_list     ap;
    char        ap_buf[TVU_VA_BUFFER_SIZE];
    utf8_t      *fmt;

    if ((fmt = tvnp_pickle_to_va(ap, ap_buf, in)) == NULL) {
        return -1;
    }

    return vfprintf(stream, (char *)fmt, ap);
}

int tvnp_sprintp(utf8_t * restrict str, tvu_buffer_t * restrict in)
{
    va_list     ap;
    char        ap_buf[TVU_VA_BUFFER_SIZE];
    utf8_t      *fmt;

    if ((fmt = tvnp_pickle_to_va(ap, ap_buf, in)) == NULL) {
        return -1;
    }

    return vsprintf((char *)str, (char *)fmt, ap);
}

int tvnp_snprintp(utf8_t * restrict str, size_t size, tvu_buffer_t * restrict in)
{
    va_list     ap;
    char        ap_buf[TVU_VA_BUFFER_SIZE];
    utf8_t      *fmt;

    if ((fmt = tvnp_pickle_to_va(ap, ap_buf, in)) == NULL) {
        return -1;
    }

    return vsnprintf((char *)str, size, (char *)fmt, ap);
}

int tvnp_asprintp(utf8_t **out, tvu_buffer_t * restrict in)
{
    va_list     ap;
    char        ap_buf[TVU_VA_BUFFER_SIZE];
    utf8_t      *fmt;

    if ((fmt = tvnp_pickle_to_va(ap, ap_buf, in)) == NULL) {
        return -1;
    }

    return vasprintf((char **)out, (char *)fmt, ap);
}

int tvnp_dprintp(int fd, tvu_buffer_t * restrict in)
{
    va_list     ap;
    char        ap_buf[TVU_VA_BUFFER_SIZE];
    utf8_t      *fmt;

    if ((fmt = tvnp_pickle_to_va(ap, ap_buf, in)) == NULL) {
        return -1;
    }

    return vdprintf(fd, (char *)fmt, ap);
}

