#ifndef _ftMario_h_
#define _ftMario_h_

#include <gctypes.h>

#include "hsd/hsd_gobj.h"
#include "hsd/hsd_jobj.h"

#include "actionstate.h"
#include "item.h"
#include "player.h"

#define MARIO_ACTIONSTATE_SPECIAL_NEUTRAL_157 0x157
#define MARIO_ACTIONSTATE_SPECIAL_NEUTRAL_158 0x158

void Mario_OnLoad(HSD_GObj* gobj);
void Mario_OnDeath(HSD_GObj* gobj);

void Mario_SpawnProjectile(HSD_GObj*, guVector);

void Mario_Special_Neutral_Air_AnimationInterrupt(HSD_GObj*);
void Mario_Special_Neutral_Air_IASA(HSD_GObj*);
void Mario_Special_Neutral_Air_Physics(HSD_GObj*);
void Mario_Special_Neutral_Air_CollInterrupt(HSD_GObj*);
void Mario_Special_Neutral_SetActionState_x158(HSD_GObj*);
void Mario_Special_Neutral_Air_SetActionState_x157(HSD_GObj*);

void DocMario_OnDeath(HSD_GObj* gobj);

#endif
