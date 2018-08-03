#ifndef _actionstate_h_
#define _actionstate_h_

#include <gctypes.h>

#include "player.h"

//Death
bool ActionState_IsDead(struct Player*);
bool ActionState_IsDeadUp(struct Player*);
bool ActionState_IsDeadStarUp(struct Player*);
bool ActionState_IsDeadUpFall(struct Player*);

bool ActionState_IsRebirth(struct Player*);
bool ActionState_IsEntry(struct Player*);

#endif