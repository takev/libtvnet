/* libtvpickle - Take Vos' Pickle; object serializer and deserializer.
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
#ifndef TVP_COMMON_H
#define TVP_COMMON_H

#define TVP_SHM_KEY     0x411b40e7      //< Shared memory key for the tvpickle system

typedef struct {
    uint64_t        pages[2][65536];
    tvu_int         selected_page;      //< 0 - No Encryption, 1, 2 - Page number.
} tvp_shared_keys_t;

extern tvp_shared_keys_t *tvp_shared_keys;

int tvp_init(void);

/** Return the key page.
 * This function is used to decrypt data, the page_nr
 * comes from the received message.
 *
 * @param page_nr   0 - No encryption, 1,2 - Page number
 * @returns         65536 uint64_t key to use as a pad for decryption.
 */
static inline uint64_t *tvp_get_page(tvu_int page_nr)
{
    switch (page_nr) {
    case 0:  return NULL;
    case 1:  return tvp_shared_keys->pages[0];
    case 2:  return tvp_shared_keys->pages[1];
    default: return NULL;
    }
}

/** Return the currently system selected page.
 * @returns   0 - No encryption, 1,2 - Page number
 */
static inline tvu_int tvp_get_current_page_nr(void)
{
    if (tvp_shared_keys == NULL) {
        return 0;
    } else {
        return tvp_shared_keys->selected_page;
    }
}


#endif
