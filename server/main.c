#define _GNU_SOURCE
#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "server.h"

int main(int argc, char *argv[]) {
    char **p_ptr = NULL;
    int port = GAMED_PORT;
	int c, fork = 1;
    long requested_port = 0;
	struct rlimit limit;

	while ((c = getopt (argc, argv, "Xp:")) != -1) {
		switch(c) {
			case 'X':
				fork = 0;
				break;
			case 'p':
				requested_port = strtol(optarg, p_ptr, 10);
				if (p_ptr != NULL || requested_port > 65535) {
					printf("%s is not a valid port\n", optarg);
					return 1;
				}
				port = requested_port;
				break;
			case '?':
				if (optopt == 'p') {
					/* getopt is printing errors for me, so this isn't needed
					fprintf(stderr, "No port specified with -p\n");
					*/
				}
				else {
					fprintf(stderr, "Usage: [-X] [-p port]\n");
				}
				return 1;
			default:
				abort();
		}
	}

	limit.rlim_cur = RLIM_INFINITY;
	limit.rlim_max = RLIM_INFINITY;
	setrlimit(RLIMIT_CORE, &limit);
    if (fork) {
        if (daemon(1, 0) == -1) {
            perror("daemonize");
            return 1;
        }
    }
	init_server(port);
	run_server();
	return 0;
}
