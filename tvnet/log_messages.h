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
#ifndef TVN_LOG_MESSAGES_H
#define TVN_LOG_MESSAGES_H

#include <tvutils/tvutils.h>

#define TVNL_LOG_TYPE_PRINT  1

#define TVNL_LOG_MSG_HDR\
    tvu_time_t      timestamp;\
    tvu_cuid_t      user;\
    tvu_cuid_t      session;\
    tvu_cuid_t      transaction;\
    uint64_t        type;

typedef struct {
    TVNL_LOG_MSG_HDR
    uint8_t     msg;            //< Pickled printf message.
} tvnl_log_msg_print_t;

#endif
