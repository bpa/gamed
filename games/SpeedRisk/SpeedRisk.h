#ifndef SPEED_RISK_H
#define SPEED_RISK_H

#include <gamed/game.h>
__BEGIN_DECLS

extern Game SpeedRisk;
extern bool borders(int, int);
extern void produce_armies(GameInstance *g, Server *s);

__END_DECLS
#endif
