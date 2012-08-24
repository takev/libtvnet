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
#ifndef TVN_FMT_PARSER_H
#define TVN_FMT_PARSER_H

#include <tvutils/tvutils.h>

/** Convert printf format string to type format string.
 * You can find out the size of the type format string by passing the
 *
 * fmt string to tvu_count_characters() and count '%' characters.
 * @param types     Pre allocated type format string. Simply count '%' signs in the fmt string.
 * @param fmt       printf format string.
 */
tvu_int tvnp_fmt_parse(utf8_t * restrict types, utf8_t const * restrict fmt);

#endif
