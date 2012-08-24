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
#include <tvnet/pickle_stdarg_encoder.h>
#include <tvnet/log.h>

__thread uint64_t tvnl_cause;

void tvnl_vmessage(utf8_t const * restrict filename, tvu_int lineno, tvnl_severity_t severity, utf8_t const * restrict fmt, va_list ap)
{
    tvu_buffer_t        buffer = {.data = NULL, .offset = 0, .size = 0, .fd = TVU_BUFFER_UNKNOWN};
    tvnl_log_info_t      *log_info = &tvnl_log_info;
    tvu_time_t          timestamp = tvu_time();
    utf8_t              types[tvu_count_character(fmt, '%') + 1];
    va_list             ap2;
    tvu_ringpacket_t    *packet;

    // Parse the types from the format string.
    if (tvnp_fmt_parse(types, fmt) == -1) {
        errno = EINVAL;
        return;
    }

    // Figure out how many bytes we need for the pickle buffer.
    buffer.size = tvnp_length("qqqqqsqsq",
        (int64_t)TVNL_MESSAGE_TYPE,
        (int64_t)timestamp,
        (int64_t)log_info->user,
        (int64_t)log_info->session,
        (int64_t)log_info->transaction,
        log_info->service,
        (int64_t)severity,
        filename,
        (int64_t)lineno
    );
    va_copy(ap2, ap);
    buffer.size+= tvnp_vpprintf_len(types, fmt, ap2);
    va_end(ap2);

    // Retrieve the pickle buffer from the ring buffer.
    if ((packet = tvu_ring_head(tvnl_ringbuffer, buffer.size)) == NULL) {
        errno = EAGAIN;
        return;
    }
    buffer.data = packet->data;

    // Write data in the pickle buffer.
    tvnp_encode(&buffer, "qqqqqsqsq",
        (int64_t)TVNL_MESSAGE_TYPE,
        (int64_t)timestamp,
        (int64_t)log_info->user,
        (int64_t)log_info->session,
        (int64_t)log_info->transaction,
        log_info->service,
        (int64_t)severity,
        filename,
        (int64_t)lineno
    );
    tvnp_vpprintf(&buffer, types, fmt, ap);

    // Return the pickle buffer back to the ring buffer.
    tvu_ringpacket_markdata(packet);
}

void tvnl_message(utf8_t *filename, tvu_int lineno, tvnl_severity_t severity, char const * restrict fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    tvnl_vmessage(filename, lineno, severity, fmt, ap);
    va_end(ap);
}

