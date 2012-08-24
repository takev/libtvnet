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
#ifndef TVN_COMMON_H
#define TVN_COMMON_H

#define TVL_SHM_KEY     0xf6d95fe2      //< Shared memory key for the tvlog system.
#define TVL_SHM_SIZE    0x00100000      //< 1 MB should be more than enough for a few log messages.

#include <tvutils/tvutils.h>

typedef struct {
    tvu_cuid_t  user;           //< The user for whom the transaction is executed.
    tvu_cuid_t  session;        //< The session this transaction is in.
    tvu_cuid_t  transaction;    //< The transaction.
    utf8_t      *service;       //< Name of service.
    uint64_t    session_flags;  //< The session flags, which contain information on how to handle messages.
} tvnl_log_info_t;

extern tvu_ringbuffer_t *tvnl_ringbuffer;

extern __thread tvnl_log_info_t tvnl_log_info;

int tvn_init(void);

#endif
