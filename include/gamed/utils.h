#ifndef GAMED_UTILS_H
#define GAMED_UTILS_H

#include <stdlib.h>

namespace Gamed {

class Random {
public:
    Random(int min=0, int max=RAND_MAX);
    int next();
private:
    char rand_state[8];
#ifdef TESTING
public:
    void set_next(int value);
    void set_next_func(int(*)());
private:
    int next_value;
    int (*next_value_func)();
#endif
};

}
#endif
