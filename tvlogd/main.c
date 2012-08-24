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
#include <tvutils/tvutils.h>
#include <tvnet/tvnet.h>
#include "loop.h"
#include "main.h"

utf8_t *option_logdir = "/var/log/tvlog";
utf8_t *option_pidfile = "/var/run/tvlogd.pid";
bool option_usage = false;
bool option_verbose = false;
bool option_foreground = false;
bool option_kill = false;


struct option longopts[] = {
    {"help", no_argument, NULL, 'h'},
    {"verbose", no_argument, NULL, 'v'},
    {"foreground", no_argument, NULL, 'f'},
    {"kill", no_argument, NULL, 'k'},
    {"logdir", required_argument, NULL, 'l'},
    {"pidfile", required_argument, NULL, 'p'},
    {NULL, 0, NULL, 0}
};

static void usage(void)
{
    fprintf(stderr, "tvlogd - Take Vos' log daemon version %s.\n", tvn_version);
    fprintf(stderr, "\n");
    fprintf(stderr, "usage: tvlogd --help\n");
    fprintf(stderr, "   or: tvlogd [arguments]\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Arguments:\n");
    fprintf(stderr, "  --help, -h                   This usage information.\n");
    fprintf(stderr, "  --verbose, -v                Log more information about the log daemon process.\n");
    fprintf(stderr, "  --foreground, -f             Do not daemonize the process, for debugging.\n");
    fprintf(stderr, "  --kill, -k                   Kill the already running tvlogd process.\n");
    fprintf(stderr, "  --logdir=<dir>, -l <dir>     Use this directory to log into (default '%s').\n", option_logdir);
    fprintf(stderr, "  --pidfile=<file>, -p <file>  File to store the process id into (default '%s').\n", option_pidfile);
}

static void parse_options(int argc, char *argv[])
{
    int c;

    while ((c = getopt_long(argc, argv, "hkvfl:p:", longopts, NULL)) != -1) {
        switch (c) {
        case 'h':
            option_usage = true;
            break;
        case 'v':
            option_verbose = true;
            break;
        case 'f':
            option_foreground = true;
            break;
        case 'k':
            option_kill = true;
        case 'l':
            option_logdir = optarg;
            break;
        case 'p':
            option_pidfile = optarg;
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

    if (argc > 0) {
        fprintf(stderr, "%s: Unexpected non-option arguments.\n\n", tvu_program_name);
        option_usage = true;
    }
}


int main(int argc, char *argv[])
{
    tvu_init(argv[0]);

    parse_options(argc, argv);

    if (option_usage) {
        usage();
        return 0;
    }

    if (option_kill) {
        pid_t old_pid;

        if ((old_pid = tvu_daemon_kill(option_pidfile)) == -1) {
            tvu_perror("%s: Could not kill old process", tvu_program_name);
            return 1;
        }
        if (old_pid > 0) {
            fprintf(stderr, "%s: Killed old process '%lli'\n", tvu_program_name, (long long)old_pid);
        } else {
            fprintf(stderr, "%s: Process was not running.\n", tvu_program_name);
        }
        return 0;
    }

    if (!option_foreground) {
        if (tvu_daemon(option_pidfile) == -1) {
            tvu_perror("Could not create pid file '%s' to daemonize process", option_pidfile);
            return 2;
        }
    }

    loop();

    return 0;
}

