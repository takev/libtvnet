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
#include <limits.h>
#include <getopt.h>
#include <libgen.h>
#include <strings.h>
#include <tvutils/tvutils.h>
#include <tvnet/tvnet.h>
#include "main.h"

utf8_t  option_message[256];
bool    option_usage = false;
bool    option_verbose = false;
bool    option_foreground = false;


struct option longopts[] = {
    {"help", no_argument, NULL, 'h'},
    {"verbose", no_argument, NULL, 'v'},
    {NULL, 0, NULL, 0}
};

static void usage(void)
{
    fprintf(stderr, "tvlog - Take Vos' log tool version %s.\n", tvn_version);
    fprintf(stderr, "\n");
    fprintf(stderr, "usage: tvlog --help\n");
    fprintf(stderr, "   or: tvlog [arguments] <message>\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Arguments:\n");
    fprintf(stderr, "  --help, -h                   This usage information.\n");
    fprintf(stderr, "  --verbose, -v                Log more information about the log daemon process.\n");
}

static void parse_options(int argc, char *argv[])
{
    int c;
    int i;

    while ((c = getopt_long(argc, argv, "hv", longopts, NULL)) != -1) {
        switch (c) {
        case 'h':
            option_usage = true;
            break;
        case 'v':
            option_verbose = true;
            break;
        case ':':
            fprintf(stderr, "\n");
            option_usage = true;
            break;
        case '?':
            fprintf(stderr, "\n");
            option_usage = true;
            break;
        case 0:
            // Long option sets its flag by its self.
            break;
        default:
            option_usage = true;
            break;
        }
    }
    argc -= optind;
    argv += optind;

    option_message[0] = 0;
    for (i = 0; i < argc; i++) {
        if (i > 0) {
            strcat(option_message, " ");
        }
        strcat(option_message, argv[i]);
    }
}


int main(int argc, char *argv[])
{
    if (tvu_init(argv[0]) == -1) { perror("Can not initialize tvutils"); return 1; }

    parse_options(argc, argv);

    if (option_usage) {
        usage();
        return 0;
    }

    TVNL_DEBUG("%s", option_message);

    return 0;
}

