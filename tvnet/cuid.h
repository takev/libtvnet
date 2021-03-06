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
#ifndef TVN_CUID_H
#define TVN_CUID_H

#include <tvutils/tvutils.h>
#include <tvnet/common.h>


/* Get a tvid id.
 * A tvid number is a combination of the time and a node id. The tvid numbers can be sorted
 * accross node id, therefore the node id comprises the lower bits.
 *
 * This function is lock-free, since tvn_time is implemented as a lock-free* function in
 * Linux. *Actually clock_gettime own its own has no contention at all, only kernel threads
 * can content with clock_gettime.
 *
 * @param tvid_state    State used to share a counter/time with other threads and processes.
 * @param node_id       The node id within the cluster.
 * @param node_bits     The number of bits of the node id.
 * @returns             A cluster tvidue 64 bit signed positive integer number.
 */
static inline tvn_cuid_t tvn_cuid(void)
{
    int64_t  time_step = 0x0000000000001000; // 12 bits
    int64_t  node_mask = 0x0000000000000fff;
    int64_t  time_mask = 0xfffffffffffff000;

    // Combine the time and node id together.
    // Node id doesn't change, so it does not have to be volatile or have a memory boundary.
    tvn_cuid_t new_cuid = (tvu_time() & time_mask) | tvn_cuid_state->node_id;

    // Try to use the new time for the tvidue id.
    tvn_cuid_t old_cuid = tvu_atomic_read_i64(&tvn_cuid_state->prev_cuid);
    if (new_cuid > old_cuid && tvu_atomic_cas_i64(&tvn_cuid_state->prev_cuid, old_cuid, new_cuid)) {
        return new_cuid;
    }

    // We failed to use the new time, so we add 1 to the time part of the id.
    return tvu_atomic_add_read_i64(&tvn_cuid_state->prev_cuid, time_step);
}

/** Initialize the tvid system.
 * If this function is not called before any other function there will
 * be a high likelyhood of a NULL pointer dereference.
 *
 * @returns -1 on error, 0 if the system is already initialized, and 1 on sucess.
 */
int tvn_cuid_init(void);

#endif
