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
#include <errno.h>
#include <stdint.h>
#include <tvnet/pickle_decoder.h>
#include <tvnet/pickle_stdarg_decoder.h>

int tvnp_decode(tvu_buffer_t * restrict buffer, utf8_t const * restrict fmt, ...)
{
    va_list ap;
    int     ret;

    va_start(ap, fmt);
    ret = tvnp_vdecode(buffer, fmt, ap);
    va_end(ap);

    return ret;
}

int tvnp_vdecode(tvu_buffer_t * restrict buffer, utf8_t const * restrict fmt, va_list ap)
{
    utf8_t      c;
    utf8_t      *s;
    tvnp_token_t token;
    int8_t      *pi8;
    uint8_t     *pu8;
    int16_t     *pi16;
    uint16_t    *pu16;
    int32_t     *pi32;
    uint32_t    *pu32;
    int64_t     *pi64;
    uint64_t    *pu64;
    float32_t   *pf32;
    float64_t   *pf64;
    utf8_t      **ps;


    while ((c = *fmt++) != '\0') {
        token = tvnp_decode_token(buffer);
        switch (c) {
        case 'b':
            pi8 = va_arg(ap, int8_t *);
            switch (token.type) {
            case TVNP_TOKEN_INTEGER: *pi8 = token.value.i; break;
            case TVNP_TOKEN_FLOAT  : *pi8 = token.value.f; break;
            default: errno = EINVAL; return -1;
            }
            break;
        case 'B':
            pu8 = va_arg(ap, uint8_t *);
            switch (token.type) {
            case TVNP_TOKEN_INTEGER: *pu8 = token.value.i; break;
            case TVNP_TOKEN_FLOAT  : *pu8 = token.value.f; break;
            default: errno = EINVAL; return -1;
            }
            break;
        case 'w':
            pi16 = va_arg(ap, int16_t *);
            switch (token.type) {
            case TVNP_TOKEN_INTEGER: *pi16 = token.value.i; break;
            case TVNP_TOKEN_FLOAT  : *pi16 = token.value.f; break;
            default: errno = EINVAL; return -1;
            }
            break;
        case 'W':
            pu16 = va_arg(ap, uint16_t *);
            switch (token.type) {
            case TVNP_TOKEN_INTEGER: *pu16 = token.value.i; break;
            case TVNP_TOKEN_FLOAT  : *pu16 = token.value.f; break;
            default: errno = EINVAL; return -1;
            }
            break;
        case 'd':
            pi32 = va_arg(ap, int32_t *);
            switch (token.type) {
            case TVNP_TOKEN_INTEGER: *pi32 = token.value.i; break;
            case TVNP_TOKEN_FLOAT  : *pi32 = token.value.f; break;
            default: errno = EINVAL; return -1;
            }
            break;
        case 'D':
            pu32 = va_arg(ap, uint32_t *);
            switch (token.type) {
            case TVNP_TOKEN_INTEGER: *pu32 = token.value.i; break;
            case TVNP_TOKEN_FLOAT  : *pu32 = token.value.f; break;
            default: errno = EINVAL; return -1;
            }
            break;
        case 'q':
            pi64 = va_arg(ap, int64_t *);
            switch (token.type) {
            case TVNP_TOKEN_INTEGER: *pi64 = token.value.i; break;
            case TVNP_TOKEN_FLOAT  : *pi64 = token.value.f; break;
            default: errno = EINVAL; return -1;
            }
            break;
        case 'Q':
            pu64 = va_arg(ap, uint64_t *);
            switch (token.type) {
            case TVNP_TOKEN_INTEGER: *pu64 = token.value.i; break;
            case TVNP_TOKEN_FLOAT  : *pu64 = token.value.f; break;
            default: errno = EINVAL; return -1;
            }
            break;
        case 'f':
            pf32 = va_arg(ap, float32_t *);
            switch (token.type) {
            case TVNP_TOKEN_INTEGER: *pf32 = token.value.i; break;
            case TVNP_TOKEN_FLOAT  : *pf32 = token.value.f; break;
            default: errno = EINVAL; return -1;
            }
            break;
        case 'F':
            pf64 = va_arg(ap, float64_t *);
            switch (token.type) {
            case TVNP_TOKEN_INTEGER: *pf64 = token.value.i; break;
            case TVNP_TOKEN_FLOAT  : *pf64 = token.value.f; break;
            default: errno = EINVAL; return -1;
            }
            break;
        case 's':
            ps = va_arg(ap, utf8_t **);
            switch (token.type) {
            case TVNP_TOKEN_UTF8_STRING: *ps = token.data; break;
            default: errno = EINVAL; return -1;
            }
            break;
        default:
            errno = EINVAL;
            return -1;
        }
    }
    return 0;
}



