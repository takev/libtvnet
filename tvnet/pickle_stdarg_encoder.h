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
#ifndef TVN_PICKLE_STDARG_ENCODER_H
#define TVN_PICKLE_STDARG_ENCODER_H

/** Encode data using a C-API.
 *
 * The format string gives the data types of the fields to encode.
 * Each character in the format string is a single field:
 * - 'b'  byte           int8_t
 * - 'B'  byte           uint8_t
 * - 'w'  word           int16_t
 * - 'W'  word           uint16_t
 * - 'd'  double word    int32_t
 * - 'D'  double word    uint32_t
 * - 'q'  quad word      int32_t
 * - 'Q'  quad word      uint32_t
 * - 'f'  float          float32_t
 * - 'F'  float          float64_t
 * - 's'  string         UTF-8 null terminated string.
 */

#include <stdarg.h>
#include <tvutils/tvutils.h>

/** Calculate the length for a buffer.
 * @param fmt   Format string of the arguments.
 * @param ...   The fields to encode.
 */
ssize_t tvnp_length(utf8_t const * restrict fmt, ...);

/**
 * @param fmt   Format string of the arguments.
 * @param ...   The fields to encode.
 */
int tvnp_encode(tvu_buffer_t * restrict buffer, utf8_t const * restrict fmt, ...);

/** Calculate the length for a buffer
 * @param fmt   Format string of the arguments.
 * @param ap    The fields to encode.
 */
ssize_t tvnp_vlength(utf8_t const * restrict fmt, va_list ap);

/**
 * @param fmt   Format string of the arguments.
 * @param ap    The fields to encode.
 */
int tvnp_vencode(tvu_buffer_t * restrict buffer, utf8_t const * restrict fmt, va_list ap);


#endif
