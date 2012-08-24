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
#include <tvnet/fmt_parser.h>

static inline size_t tvnp_fmt_parse_int(int h, int l, bool _unsigned)
{
    size_t size;

    if (h == 2) {
        size = sizeof (char);
    } else if (h == 1) {
        size = sizeof (short);
    } else if (l == 1) {
        size = sizeof (long);
    } else if (l == 2) {
        size = sizeof (long long);
    } else {
        size = sizeof (int);
    }

    switch (size) {
    case 1: return _unsigned ? 'B' : 'b';
    case 2: return _unsigned ? 'W' : 'w';
    case 4: return _unsigned ? 'D' : 'd';
    case 8: return _unsigned ? 'Q' : 'q';
    }
    abort();
}

static inline size_t tvnp_fmt_parse_char(int l)
{
    if (l == 1) {
        return 'W';
    } else {
        return 'B';
    }
}

static inline size_t tvnp_fmt_parse_float(int l)
{
    if (l == 2) {
        return 'G';
    } else {
        return 'F';
    }
}

/** Convert printf format string to type format string.
 * You can find out the size of the type format string by passing the
 * fmt string to tvnp_count_characters() and count '%' characters.
 * @param types     Pre allocated type format string.
 * @param fmt       printf format string.
 */
tvu_int tvnp_fmt_parse(utf8_t * restrict types, utf8_t const * restrict fmt)
{
    bool            format = false;
    utf8_t          c;
    unsigned int    l;
    unsigned int    h;

    while ((c = *fmt++) != '\0') {
        if (!format) {
            format = (c == '%');
            l = 0;
            h = 0;
        } else {
            switch (c) {
            case '%': format = false; break;
            case 's': format = false; *types++ = 's'; break;
            case 'i': format = false; *types++ = tvnp_fmt_parse_int(h, l, false); break;
            case 'd': format = false; *types++ = tvnp_fmt_parse_int(h, l, false); break;
            case 'o': format = false; *types++ = tvnp_fmt_parse_int(h, l, false); break;
            case 'u': format = false; *types++ = tvnp_fmt_parse_int(h, l, true); break;
            case 'x': format = false; *types++ = tvnp_fmt_parse_int(h, l, true); break;
            case 'X': format = false; *types++ = tvnp_fmt_parse_int(h, l, true); break;
            case 'c': format = false; *types++ = tvnp_fmt_parse_char(l); break;
            case 'f': format = false; *types++ = tvnp_fmt_parse_float(l); break;
            case 'F': format = false; *types++ = tvnp_fmt_parse_float(l); break;
            case 'e': format = false; *types++ = tvnp_fmt_parse_float(l); break;
            case 'E': format = false; *types++ = tvnp_fmt_parse_float(l); break;

            case 'l': l++;   break;
            case 'h': h++;   break;
            case 'j': l = 2; break;
            case 't': l = 2; break;
            case 'z': l = 2; break;
            case 'q': l = 2; break;
            case 'L': l = 2; break;
            default: break;
            }
            continue;
        }
    }

    *types++ = '\0';
    return 0;
}

