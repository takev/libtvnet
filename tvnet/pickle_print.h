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
#ifndef TVN_PICKLE_PRINT_H
#define TVN_PICKLE_PRINT_H

#include <unistd.h>
#include <stdarg.h>
#include <stdint.h>
#include <tvnet/fmt_parser.h>

/** Determine the size of the pickle buffer for the format string and parameters.
 *
 * @param types     A list of types to encode.
 * @param nr_types  The size of the list of types to encode.
 * @param fmt       Format string.
 * @param ap        Parameters.
 * @returns         The size the pickle buffer needs to be.
 */
size_t tvnp_vpprintf_len(utf8_t const * restrict types, utf8_t const * restrict fmt, va_list ap) __attribute ((format (printf, 2, 0)));

/** Pickle the format string and parameters.
 * 
 * @param buffer    Pre allocated pickle buffer.
 * @param types     A list of types to encode.
 * @param nr_types  The size of the list of types to encode.
 * @param fmt       Format string.
 * @param ap        Parameters.
 */
void tvnp_vpprintf(tvu_buffer_t * restrict buffer, utf8_t const * restrict types, utf8_t const * restrict fmt, va_list ap) __attribute ((format (printf, 3, 0)));

/** Pickle the format string and parameters.
 *
 * @param fmt       Format string.
 * @param ap        Parameters.
 * @returns         An allocated buffer.
 */
tvu_buffer_t tvnp_vapprintf(utf8_t const * restrict fmt, va_list ap) __attribute ((format (printf, 1, 0)));

/** Pickle the format string and parameters.
 *
 * @param fmt       Format string.
 * @param ...       Parameters.
 * @returns         An allocated buffer.
 */
tvu_buffer_t tvnp_apprintf(utf8_t const * restrict fmt, ...) __attribute__ ((format (printf, 1, 2)));

/** Unpickle format string and parameters and print to string.
 *
 * @param out       Output string
 * @param int       Pickle buffer.
 * @returns         The size of the output string, or -1 on error.
 */
int tvnp_fprintp(FILE * restrict stream, tvu_buffer_t * restrict in);
int tvnp_sprintp(utf8_t * restrict str, tvu_buffer_t * restrict in);
int tvnp_snprintp(utf8_t * restrict str, size_t size, tvu_buffer_t * restrict in);
int tvnp_asprintp(utf8_t **out, tvu_buffer_t * restrict in);
int tvnp_dprintp(int fd, tvu_buffer_t * restrict in);

#endif
