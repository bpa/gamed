#include <gamed/utils.h>
#include <time.h>

Gamed::Random::Random(int min, int max) {
    initstate(time(NULL), &rand_state[0],8);
}

int Gamed::Random::next() {
#ifdef TESTING
    if (next_value_func != NULL) {
        return next_value_func();
    }
    else {
        return next_value;
    }
#else
    return random();
#endif
}

#ifdef TESTING
void Gamed::Random::set_next(int value) {
    next_value = value;
}

void Gamed::Random::set_next_func(int(*func)()) {
    next_value_func = func;
}
#endif
