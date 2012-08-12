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
#ifndef TVP_DECOMPRESS_H
#define TVP_DECOMPRESS_H

#include <stdint.h>
#include <errno.h>
#include <tvutils/tvutils.h>


/** Find out how big the result is.
 * This function is pretty fast, but it may be better to find out how
 * large the output buffer should be through out-of-band means.
 *
 * @param in    buffer where the compressed section is located.
 * @returns     Size of the decompressed data.
 */
tvu_int tvp_decompress_length(tvu_buffer_t in);

/** Decompress data.
 * This compression algorithm was build for speed, period.
 * The output buffer has to be pre allocated.
 *
 * @param in    buffer where the compressed section is located.
 * @param out   buffer where the decompression data should be put in.
 */
void tvp_decompress(tvu_buffer_t * restrict _out, tvu_buffer_t * restrict _in);

#endif
