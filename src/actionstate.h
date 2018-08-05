#ifndef _actionstate_h_
#define _actionstate_h_

#include <gctypes.h>

#include "player.h"

//Death
extern bool ActionState_IsDead(struct Player*);
extern bool ActionState_IsDeadUp(struct Player*);
extern bool ActionState_IsDeadStarUp(struct Player*);
extern bool ActionState_IsDeadUpFall(struct Player*);

extern bool ActionState_IsRebirth(struct Player*);
extern bool ActionState_IsEntry(struct Player*);

#endif