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

#include <tvutils/tvutils.h>
#include <tvnet/tvnet.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include "main.h"

utf8_t  *logfile_date[4] = {"", "", ""};
int     logfile_fd[4] = {-1, -1, -1};

bool    running = true;

#ifndef MAXPATHLEN
#define MAXPATHLEN  256
#endif

int open_logfile(tvu_time_t timestamp, tvnl_severity_t severity)
{
    utf8_t  date_s[9];
    utf8_t  logfile_filename[MAXPATHLEN];
    int     file_type;
    utf8_t  *file_type_s;

    tvu_fmt_shortdate(date_s, timestamp);

    switch (severity) {
    case TVNL_FATAL_SEVERITY:
    case TVNL_ERROR_SEVERITY:
    case TVNL_WARNING_SEVERITY:
        file_type = 0;
        file_type_s = "error";
        break;

    case TVNL_AUDIT_SEVERITY:
        file_type = 1;
        file_type_s = "audit";
        break;

    case TVNL_INFO_SEVERITY:
    case TVNL_DEBUG_SEVERITY:
        file_type = 2;
        file_type_s = "debug";
        break;
    default:
        file_type = 4;
        file_type_s = "unknown";
    }

    if (strcmp(logfile_date[file_type], date_s) != 0) {
        if (logfile_fd[file_type] != -1) {
            close(logfile_fd[file_type]);
        }
        sprintf(logfile_filename, "%s/%s-%s.log", option_logdir, file_type_s, date_s);

        if ((logfile_fd[file_type] = open(logfile_filename, O_WRONLY | O_APPEND | O_CREAT, 0644)) == -1) {
            abort();
        }
    }

    return logfile_fd[file_type];
}

/** Do a single iteration from the ring buffer, to logging into a file.
 * @returns     true when something was logged, false when the ring buffer was empty.
 */
bool iteration(void)
{
    tvu_ringpacket_t    *packet;
    tvu_buffer_t        buffer;
    int64_t             message_type;
    int64_t             timestamp;
    utf8_t              timestamp_s[30];
    int64_t             user;
    utf8_t              user_s[11];
    int64_t             session;
    utf8_t              session_s[11];
    int64_t             transaction;
    utf8_t              transaction_s[11];
    int64_t             severity;
    utf8_t              *severity_s;
    utf8_t              *filename;
    utf8_t              *service;
    int64_t             lineno;
    int                 fd;
    utf8_t              message_s[256];
    utf8_t              logline_s[256];
    tvu_int             logline_len;

    if ((packet = tvu_ring_tail(tvnl_ringbuffer)) == NULL) {
        return false;
    }

    buffer = tvu_ringpacket_to_buffer(packet);
    tvnp_decode(&buffer, "qqqqqsqsq",
        &message_type,
        &timestamp,
        &user,
        &session,
        &transaction,
        &service,
        &severity,
        &filename,  // borrowed reference, don't destroy the packet before using it.
        &lineno
    );

    tvu_fmt_timestamp(timestamp_s, timestamp);
    tvu_i64_to_base85(user_s, user);
    tvu_i64_to_base85(session_s, session);
    tvu_i64_to_base85(transaction_s, transaction);

    switch ((tvnl_severity_t)severity) {
    case TVNL_FATAL_SEVERITY:
        severity_s = "FATAL";
        break;
    case TVNL_ERROR_SEVERITY:
        severity_s = "ERROR";
        break;
    case TVNL_WARNING_SEVERITY:
        severity_s = "WARN ";
        break;
    case TVNL_AUDIT_SEVERITY:
        severity_s = "AUDIT";
        break;
    case TVNL_INFO_SEVERITY:
        severity_s = "INFO ";
        break;
    case TVNL_DEBUG_SEVERITY:
        severity_s = "DEBUG";
        break;
    default:
        severity_s = "UNKN ";
    }

    switch ((tvnl_type_t)message_type) {
    case TVNL_MESSAGE_TYPE:
        tvnp_snprintp(message_s, 256, &buffer);
        break;
    default:
        severity_s = "ERROR";
        snprintf(message_s, 256, "Unknown message type %i.", (int)message_type);
    }

    logline_len = snprintf(logline_s, 256, "%s %s %s %s %s %s:%s:%lli %s\n",
        timestamp_s,
        severity_s,
        user_s,
        session_s,
        transaction_s,
        service,
        filename,
        (long long)lineno,
        message_s
    );

    fd = open_logfile(timestamp, severity);
    write(fd, logline_s, logline_len);

    // This is the last thing that needs to be done.
    tvu_ringpacket_markfree(packet);
    tvu_ring_advance_free(tvnl_ringbuffer); // free advance is cheaper/latency to do on the log server side.
    return true;
}

void kill_handler(int sig __attribute__ ((__unused__)))
{
    TVNL_INFO("Logging service has terminated.");
    running = false;
}

void user_handler(int sig)
{
    switch (sig) {
    case SIGUSR1:
        TVNL_INFO("--- USR1 MARK ---");
        break;
    case SIGUSR2:
        TVNL_INFO("--- USR2 MARK ---");
        break;
    case SIGHUP:
        TVNL_INFO("--- HUP MARK ---");
        break;
    default:
        TVNL_INFO("--- UNKNOWN MARK ---");
        break;
    }
}

void loop(void)
{
    bool        opening_mark_send = false;
    tvu_int     last_hour_mark = tvu_to_hours(tvu_time());
    tvu_int     now;

    signal(SIGINT, kill_handler);
    signal(SIGTERM, kill_handler);
    signal(SIGHUP, user_handler);
    signal(SIGUSR1, user_handler);
    signal(SIGUSR2, user_handler);

    while (running) {
        if (!iteration()) {

            if (!opening_mark_send) {
                TVNL_INFO("Logging service has started.");
                opening_mark_send = true;
            }

            now = tvu_to_hours(tvu_time());
            if (last_hour_mark < now) {
                TVNL_INFO("--- HOUR MARK ---");
                last_hour_mark = now;
            }

            // Wait 10 ms if there wasn't anything logged.
            usleep(10000);
        }
    }
}

