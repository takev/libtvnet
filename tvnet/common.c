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

tvu_ringbuffer_t *tvnl_ringbuffer = NULL;

__thread tvnl_log_info_t tvnl_log_info = {
    .user = 0,
    .session = 0,
    .session_flags = 0,
    .transaction = 0,
    .service = "<unknown>"
};

int tvn_init(void)
{
    int shmid;

    tvnl_log_info.service = tvu_program_name;

    if (tvnl_ringbuffer) {
        // If the tvid_state is already allocated then don't initialize it again.
        return 0;
    }

    // Retrieve or create and attach the tvid_state from shared memory.
    if ((shmid = shmget(TVL_SHM_KEY, TVL_SHM_SIZE, IPC_CREAT | 0666)) == -1) {
        return -1;
    }

    if ((tvnl_ringbuffer = shmat(shmid, NULL, 0)) == (void *) -1) {
        return -1;
    }

    if (tvu_ring_is_initialized(tvnl_ringbuffer, TVL_SHM_SIZE)) {
        return 0;
    }

    if (tvu_ring_init(tvnl_ringbuffer, TVL_SHM_SIZE) == -1) {
        return -1;
    }

    return 1;
}

