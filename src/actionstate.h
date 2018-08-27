#ifndef _actionstate_h_
#define _actionstate_h_

#include <gctypes.h>

#include "gobj.h"
#include "player.h"

//Death
extern bool ActionState_IsDead(HSD_GObj *gobj);
extern bool ActionState_IsDeadUp(HSD_GObj *gobj);
extern bool ActionState_IsDeadStarUp(HSD_GObj *gobj);
extern bool ActionState_IsDeadUpFall(HSD_GObj *gobj);

extern bool ActionState_IsRebirth(HSD_GObj *gobj);
extern bool ActionState_IsEntry(HSD_GObj *gobj);

#endif