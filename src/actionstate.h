#ifndef _actionstate_h_
#define _actionstate_h_

#include <gctypes.h>

#include "gobj.h"
#include "player.h"

#define ACTIONSTATE_FALL 0x1D
#define ACTIONSTATE_HAMMERWAIT 0x133
#define ACTIONSTATE_HAMMERFALL 0x137
#define ACTIONSTATE_HAMMERLANDING 0x139
#define ACTIONSTATE_ENTRY 0x142
#define ACTIONSTATE_ENTRYEND 0x144

extern BOOL ActionState_IsDead(HSD_GObj *gobj);
extern BOOL ActionState_IsDeadUp(HSD_GObj *gobj);
extern BOOL ActionState_IsDeadStarUp(HSD_GObj *gobj);
extern BOOL ActionState_IsDeadUpFall(HSD_GObj *gobj);

extern BOOL ActionState_IsRebirth(HSD_GObj *gobj);
extern BOOL ActionState_IsEntry(HSD_GObj *gobj);

#endif