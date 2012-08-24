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

#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <tvutils/tvutils.h>
#include <tvnet/common.h>
#ifdef TVU_HAVE_TIPC
#include <unistd.h>
#include <sys/socket.h>
#include <linux/tipc.h>
#endif

tvu_ringbuffer_t *tvnl_ringbuffer = NULL;
tvn_cuid_state_t *tvn_cuid_state = NULL;

__thread tvn_info_t tvn_info = {
    .user = 0,
    .session = 0,
    .session_flags = 0,
    .transaction = 0,
    .service = "<unknown>"
};

int tvn_init(void)
{
    int shmid;
#ifdef TVU_HAVE_TIPC
    int                     fd;
    struct sockaddr_tipc    localaddr;
    socklen_t               addrlen = sizeof (struct sockaddr_tipc);
#endif

    tvn_info.service = tvu_program_name;

    if (tvnl_ringbuffer) {
        // If the tvid_state is already allocated then don't initialize it again.
        return 0;
    }

    // Retrieve or create and attach the tvid_state from shared memory.
    if ((shmid = shmget(TVN_SHM_KEY, TVN_SHM_SIZE, IPC_CREAT | 0666)) == -1) {
        return -1;
    }

    if ((tvnl_ringbuffer = shmat(shmid, NULL, 0)) == (void *) -1) {
        return -1;
    }
    tvn_cuid_state = (tvn_cuid_state_t *)((char *)tvnl_ringbuffer + TVNL_RINGBUFFER_SIZE);

    if (tvu_ring_is_initialized(tvnl_ringbuffer, TVN_SHM_SIZE)) {
        return 0;
    }

    if (tvu_ring_init(tvnl_ringbuffer, TVN_SHM_SIZE) == -1) {
        return -1;
    }

    // Now find out what the node id is of this server. We can overwrite it in shared
    // memory any time we like.
#ifdef TVU_HAVE_TIPC
    if ((fd = socket(AF_TIPC, SOCK_RDM, 0)) == -1) {
        return -1;
    }

    if (getsockname(fd, (struct sockaddr *)&localaddr, &addrlen) == -1) {
        return -1;
    }

    if (close(fd) == -1) {
        return -1;
    }

    // This may not use more than 12 bits.
    tvn_cuid_state->node_id = tipc_node(localaddr.addr.id.node);
#else
    // Unix domain sockets will be local host, so zero.
    tvn_cuid_state->node_id = 0;
#endif

    return 1;
}

