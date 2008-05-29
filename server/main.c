#include <stdio.h>
#include <stdlib.h>
#include "server.h"

int main(int argc, char *argv[]) {
    char **p_ptr = NULL;
    int port = GAMED_PORT;
    long requested_port = 0;
    if (argc > 1) {
        requested_port = strtol(argv[1], p_ptr, 10);
        if (p_ptr != NULL || requested_port > 65535) {
            printf("%s is not a valid port\n", argv[1]);
        }
        else {
            port = requested_port;
        }
    }
    /* run_as_daemon(port); */
    run_server(port);
    return 0;
}
