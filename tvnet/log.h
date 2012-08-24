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
#ifndef TVN_LOG_H
#define TVN_LOG_H

#include <stdint.h>
#include <stdarg.h>
#include <tvutils/tvutils.h>
#include <tvnet/fmt_parser.h>
#include <tvnet/pickle_print.h>
#include <tvnet/common.h>

typedef enum {
    TVNL_FATAL_SEVERITY = 0,
    TVNL_ERROR_SEVERITY,
    TVNL_WARNING_SEVERITY,
    TVNL_AUDIT_SEVERITY,
    TVNL_INFO_SEVERITY,
    TVNL_DEBUG_SEVERITY
} tvnl_severity_t;

typedef enum {
    TVNL_MESSAGE_TYPE = 0
} tvnl_type_t;

#define TVNL_FATAL(...)     tvnl_message(__FILE__, __LINE__, TVNL_FATAL_SEVERITY,   __VA_ARGS__)
#define TVNL_ERROR(...)     tvnl_message(__FILE__, __LINE__, TVNL_ERROR_SEVERITY,   __VA_ARGS__)
#define TVNL_WARNING(...)   tvnl_message(__FILE__, __LINE__, TVNL_WARNING_SEVERITY, __VA_ARGS__)
#define TVNL_INFO(...)      tvnl_message(__FILE__, __LINE__, TVNL_INFO_SEVERITY,    __VA_ARGS__)
#define TVNL_DEBUG(...)     tvnl_message(__FILE__, __LINE__, TVNL_DEBUG_SEVERITY,   __VA_ARGS__)

void tvnl_vmessage(utf8_t const * restrict filename, tvu_int lineno, tvnl_severity_t severity, utf8_t const * restrict fmt, va_list ap);

void tvnl_message(utf8_t *filename, tvu_int lineno, tvnl_severity_t severity, char const * restrict fmt, ...);

#endif
