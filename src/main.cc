#include <gamed/server.h>

using namespace Gamed;

int main(int argc, char *argv[]) {
  Server gamed;
  //gamed.run_as_daemon();
  gamed.run();
}
