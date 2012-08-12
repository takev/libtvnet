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
#ifndef TVLOGD_MAIN_H
#define TVLOGD_MAIN_H

#include <tvutils/tvutils.h>

extern utf8_t   *option_progname;
extern utf8_t   *option_progpath;
extern utf8_t   *option_progdir;
extern utf8_t   option_message[];
extern bool     option_usage;
extern bool     option_verbose;
extern bool     option_foreground;

#endif
