#ifndef __PLAYER_H
#define __PLAYER_H

#include <inttypes.h>

#include "phys.h"

struct player {
	vec2_t pos;
};

enum inputs {
	INPUT_MOVE_LEFT   = 0,
	INPUT_MOVE_RIGHT  = 1,
	INPUT_MOVE_UP     = 2,
	INPUT_MOVE_DOWN   = 3,
};

#endif /* __PLAYER_H */