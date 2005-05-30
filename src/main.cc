#include <gamed/server.h>

using namespace Gamed;

int main(int argc, char *argv[]) {
    Server gamed;
	gamed.listen_on_port(GAMED_PORT);
	gamed.run_as_daemon();
}
