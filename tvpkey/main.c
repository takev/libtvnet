/* libtvpickle - Take Vos's serialization library
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

#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <getopt.h>
#include <libgen.h>
#include <strings.h>
#include <tvutils/tvutils.h>
#include <tvpickle/tvpickle.h>
#include "main.h"

utf8_t  option_message[256];
bool    option_usage = false;
bool    option_verbose = false;
tvu_int option_page = 0;
bool    option_switch_page = false;
bool    option_stdin = "true";
utf8_t  *option_file = "<stdin>";

utf8_t  key[256];
tvu_int key_size;

struct option longopts[] = {
    {"help", no_argument, NULL, 'h'},
    {"verbose", no_argument, NULL, 'v'},
    {"switch", no_argument, NULL, 's'},
    {"file", no_argument, NULL, 'f'},
    {NULL, 0, NULL, 0}
};

static void usage(void)
{
    fprintf(stderr, "tvpkey - Set key for RPC encryption, version %s.\n", tvp_version);
    fprintf(stderr, "\n");
    fprintf(stderr, "usage: tvpkey --help\n");
    fprintf(stderr, "   or: tvpkey <page>                 Set the key for <page>, from stdin\n");
    fprintf(stderr, "   or: tvpkey -s <page>              Switch RPC encryption to <page>\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Description:\n");
    fprintf(stderr, "  All RPC messages are encrypted by a key stored in a page. This program can be used\n");
    fprintf(stderr, "  to change the page for all the applications on this machine. After the page has switched\n");
    fprintf(stderr, "  the programs will use the key for new messages. Afterwards you may update the key on the\n");
    fprintf(stderr, "  page which is not in use anymore.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "  It is recommended to change the key and flip the page very often (every few minutes)\n");
    fprintf(stderr, "  on all the servers in the cluster. A central server can randomly generate keys and\n");
    fprintf(stderr, "  distribute these with ssh.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Arguments:\n");
    fprintf(stderr, "  <page>                        Must be either '1' or '2'. Or '0' to turn off encryption.\n");
    fprintf(stderr, "   --help, -h                   This usage information.\n");
    fprintf(stderr, "   --verbose, -v                Log more information about the log daemon process.\n");
    fprintf(stderr, "   --switch, -s                 Switch RPC encryption.\n");
    fprintf(stderr, "   --file=<file>, -d            Extract the key from the <file> specified.\n");
}

static void parse_options(int argc, char *argv[])
{
    int c;
    int i;

    while ((c = getopt_long(argc, argv, "hvsf:", longopts, NULL)) != -1) {
        switch (c) {
        case 'h':
            option_usage = true;
            goto done;
        case 'v':
            option_verbose = true;
            break;
        case 's':
            option_switch_page = true;
            break;
        case 'f':
            option_stdin = false;
            option_file = optarg;
            break;
        case ':':
            fprintf(stderr, "\n");
            option_usage = true;
            goto done;
        case '?':
            fprintf(stderr, "\n");
            option_usage = true;
            goto done;
        case 0:
            // Long option sets its flag by its self.
            break;
        default:
            option_usage = true;
            goto done;
        }
    }
    argc -= optind;
    argv += optind;

    if (argc != 1) {
        fprintf(stderr, "%s: Expected a single <page> argument.\n\n", tvu_program_name);
        option_usage = true;
        goto done;
    }

    if (strcmp(argv[0], "0") == 0 && option_switch_page) {
        option_page = 0;
    } else if (strcmp(argv[0], "1") == 0) {
        option_page = 1;
    } else if (strcmp(argv[0], "2") == 0) {
        option_page = 2;
    } else {
        fprintf(stderr, "%s: Expected a 1 or 2 as <page> argument, or 0 to turn of encryption.\n\n", tvu_program_name);
        option_usage = true;
        goto done;
    }

done:
    if (option_usage) {
        usage();
        exit(1);
    }
}

void read_key(void)
{
    int     fd = 0;     // Stdin is the first '0' file descriptor.
    tvu_int r;
    tvu_int todo = 256;
    tvu_int done = 0;

    if (!option_stdin) {
        if ((fd = open(option_file, O_RDONLY)) == -1) {
            fprintf(stderr, "Could not open key file '%s': %s.\n", option_file, strerror(errno));
            exit(1);
        }
    }

    while (todo) {
        switch ((r = read(fd, &key[done], todo))) {
        case -1: // Error.
            switch (errno) {
            case EAGAIN:    // This should not happen, only on non blocking io. Lets poll after a short delay.
                sleep(1);
            case EINTR:     // Just a signal, retry.
                continue;
            default:
                fprintf(stderr, "Could not read from key file '%s': %s.\n", option_file, strerror(errno));
                exit(1);
            }
            break;
        case 0: // End of file.
            todo = 0;
            break;
        default:
            todo -= r;
            done += r;
        }
    }
    key_size = done;

    if (!option_stdin) {
        if (close(fd) == -1) {
            fprintf(stderr, "Could not close key file '%s': %s.\n", option_file, strerror(errno));
            exit(1);
        }
    }
}

int main(int argc, char *argv[])
{
    if (tvu_init(argv[0]) == -1) { perror("Can not initialize tvutils"); return 1; }
    parse_options(argc, argv);

    if (!option_switch_page) {
        read_key();
    }
   
    fprintf(stderr, "key:\n****\n%.256s\n***\n", key);

    return 0;
}

