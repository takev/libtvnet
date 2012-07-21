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
#ifndef TVP_STDARG_DECODER_H
#define TVP_STDARG_DECODER_H

#include <stdarg.h>
#include <tvutils/tvutils.h>

/** Decode a buffer, and put the results in the fields.
 * @param fmt   Format string of the arguments.
 * @param ...   Pointers to fields to decode.
 */
int tvp_decode(tvu_buffer_t * restrict buffer, utf8_t const * restrict fmt, ...);

/**
 * @param fmt   Format string of the arguments.
 * @param ap    The fields to encode.
 */
int tvp_vdecode(tvu_buffer_t * restrict buffer, utf8_t const * restrict fmt, va_list ap);


#endif
