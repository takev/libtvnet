/* libtvlog - Library of utilities.
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
#include <stdio.h>
#include <tvutils/tvutils.h>
#include <tvnet/pickle_print.h>


int test1(void)
{
    TVU_UNITTEST_INIT

    char            *s;
    tvu_buffer_t    buffer;

    buffer = tvnp_apprintf((utf8_t *)"%i", 25);
    TVU_UNITTEST_MEMCMP(buffer.data, "\x82%i\0\x19", 5);
    buffer.offset = 0;
    tvnp_asprintp((utf8_t **)&s, &buffer);
    TVU_UNITTEST_STRCMP(s, "25");
    free(s);
    tvu_buffer_free(buffer);

    buffer = tvnp_apprintf((utf8_t *)"%05i,%%,%.2f", 25, 12.4);
    buffer.offset = 0;
    tvnp_asprintp((utf8_t **)&s, &buffer);
    TVU_UNITTEST_STRCMP(s, "00025,%,12.40");
    free(s);
    tvu_buffer_free(buffer);

    buffer = tvnp_apprintf((utf8_t *)"%s", "hello");
    buffer.offset = 0;
    tvnp_asprintp((utf8_t **)&s, &buffer);
    TVU_UNITTEST_STRCMP(s, "hello");
    free(s);
    tvu_buffer_free(buffer);

    buffer = tvnp_apprintf((utf8_t *)"%s,%i,%s", "hello", 25, "world");
    buffer.offset = 0;
    tvnp_asprintp((utf8_t **)&s, &buffer);
    TVU_UNITTEST_STRCMP(s, "hello,25,world");
    free(s);
    tvu_buffer_free(buffer);

    buffer = tvnp_apprintf((utf8_t *)"%s,%.2f,%i,%s", "hello", 25.3, 12, "world");
    buffer.offset = 0;
    tvnp_asprintp((utf8_t **)&s, &buffer);
    TVU_UNITTEST_STRCMP(s, "hello,25.30,12,world");
    free(s);
    tvu_buffer_free(buffer);

    return 0;
}

int main(int argc __attribute__((unused)), char *argv[])
{
    int r;

    tvu_init(argv[0]);

    if ((r = test1()) != 0) {
        return r;
    }

    return 0; 
}
