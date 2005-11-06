#include <gamed/utils.h>
#include "HiLo.h"

HiLo::HiLo() {
    Gamed::Random r = Gamed::Random(1,100);
    number = r.next();
}

void HiLo::handle_request(Gamed::Player *, Gamed::Request *) {
}
