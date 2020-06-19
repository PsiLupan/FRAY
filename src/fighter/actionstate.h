#ifndef _actionstate_h_
#define _actionstate_h_

#include <gctypes.h>

#include "hsd/hsd_aobj.h"
#include "hsd/hsd_gobj.h"

#include "item.h"
#include "player.h"

#include "ftPeach.h"

#define ACTIONSTATE_WAIT 0x0E
#define ACTIONSTATE_FALL 0x1D
#define ACTIONSTATE_HAMMERWAIT 0x133
#define ACTIONSTATE_HAMMERFALL 0x137
#define ACTIONSTATE_HAMMERLANDING 0x139
#define ACTIONSTATE_ENTRY 0x142
#define ACTIONSTATE_ENTRYEND 0x144

BOOL ActionState_IsDead(HSD_GObj*);
BOOL ActionState_IsDeadUp(HSD_GObj*);
BOOL ActionState_IsDeadStarUp(HSD_GObj*);
BOOL ActionState_IsDeadUpFall(HSD_GObj*);

BOOL ActionState_IsRebirth(HSD_GObj*);
BOOL ActionState_IsEntry(HSD_GObj*);

void ActionState_Wait_CheckPlayer(HSD_GObj*);
void ActionState_Wait_NoCheck(HSD_GObj*);
void ActionState_Wait(HSD_GObj*, const f32);

void ActionState_HammerWait(HSD_GObj*);
f32 ActionState_HammerFrameCount(Player*);
BOOL ActionState_IsHammer(Player*);
void ActionState_Hammer_ActivateHitbox(Player*);
void ActionState_800C8B74(HSD_GObj*);
void ActionState_Fall(HSD_GObj*);

#endif
