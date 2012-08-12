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
#include <stdint.h>
#include <errno.h>
#include <tvutils/tvutils.h>
#include <tvpickle/decompress.h>

tvu_int tvp_decompress_length(tvu_buffer_t in)
{
    tvu_int         command;
    tvu_int         length;
    tvu_int         rewind;
    tvu_int         out_size = 0;

    while (in.offset < in.size) {
        command = in.data[in.offset];
        if (command < 0x80) {
            // Literal, max 128 length.
            length = command + 1;
            in.offset += length + 1;    // length + the command byte.
            out_size += length;
        } else {
            // Lookback. max 65 bytes to copy, with rewind of 1024
            length = (command & 0x3f) + 2;
            in.offset += 2;             // length + command byte + rewind byte.
            out_size += length;
        }
    }
    return out_size;
}

void tvp_decompress(tvu_buffer_t * restrict _out, tvu_buffer_t * restrict _in)
{
    tvu_int         command;
    tvu_int         length;
    tvu_int         rewind;
    tvu_buffer_t    in = *_in;
    tvu_buffer_t    out = *_out;

    while (in.offset < in.size) {
        command = in.data[in.offset];
        if (command < 0x80) {
            // Literal max 128 length.
            length = command + 1;
            memcpy(&out.data[out.offset], &in.data[in.offset + 1], length);
            in.offset += length + 1;    // length + command byte.
            out.offset += length;
        } else {
            // Lookback. max 65 bytes to copy, with rewind of 1024
            length = (command & 0x3f) + 2;
            rewind = (((command & 0xc0) << 2) | in.data[in.offset + 1]) + length;
            memcpy(&out.data[out.offset], &out.data[out.offset - rewind], length);
            in.offset += 2;             // command byte + rewind byte.
            out.offset += length;
        }
    }
    _in->offset = in.offset;
    _out->offset = out.offset;
}

