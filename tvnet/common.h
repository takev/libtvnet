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

#define TVNL_RINGBUFFER_SIZE    0x00100000      //< 1 MB ring buffer for log messages.
#define TVN_SHM_KEY             0xf6d95fe2      //< Shared memory key for the tvlog system.
#define TVN_SHM_SIZE            (TVNL_RINGBUFFER_SIZE + sizeof (tvn_cuid_state_t))

typedef int64_t tvn_cuid_t;

//! Shared state between threads and processes.
struct tvn_cuid_state_s {
    int16_t     node_id;                                  //< Cluster node number.
    tvn_cuid_t  prev_cuid __attribute__((aligned(64)));   //< Last cluster node.
} __attribute__((aligned(64)));
typedef struct tvn_cuid_state_s tvn_cuid_state_t;

//! State shared between threads and processes.
typedef struct {
    tvn_cuid_t  user;           //< The user for whom the transaction is executed.
    tvn_cuid_t  session;        //< The session this transaction is in.
    tvn_cuid_t  transaction;    //< The transaction.
    utf8_t      *service;       //< Name of service.
    uint64_t    session_flags;  //< The session flags, which contain information on how to handle messages.
} tvn_info_t;

// These are located in shared-memory.
extern tvn_cuid_state_t *tvn_cuid_state;
extern tvu_ringbuffer_t *tvnl_ringbuffer;

// This information is kept on a thread by thread bases.
extern __thread tvn_info_t tvn_info;

int tvn_init(void);

#endif
