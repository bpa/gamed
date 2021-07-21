#define _GNU_SOURCE
#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "server.h"

#ifndef DEFAULT_CONFIG
  #define DEFAULT_CONFIG "gamed.conf"
#endif

int main(int argc, char *argv[]) {
    char **p_ptr = NULL;
    char *config_file = NULL;
    int port = GAMED_PORT;
	int c, fork = 1;
    long requested_port = 0;
	struct rlimit limit;

	while ((c = getopt (argc, argv, "Xp:f:")) != -1) {
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
            case 'f':
			case '?':
				if (optopt == 'p') {
					/* getopt is printing errors for me, so this isn't needed
					fprintf(stderr, "No port specified with -p\n");
					*/
				}
				else {
					fprintf(stderr, "Usage: [-X] [-p port] [-f /path/to/config]\n");
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
    if (config_file == NULL) {
	    init_server(port, DEFAULT_CONFIG);
    }
    else {
	    init_server(port, config_file);
    }
    free(config_file);
	run_server();
	return 0;
}
