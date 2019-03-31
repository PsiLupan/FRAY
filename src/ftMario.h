#ifndef _ftMario_h_
#define _ftMario_h_

#include <gctypes.h>

#include "hsd/hsd_jobj.h"

#include "gobj.h"
#include "item.h"
#include "player.h"

#define MARIO_ACTIONSTATE_SPECIAL_NEUTRAL_157 0x157
#define MARIO_ACTIONSTATE_SPECIAL_NEUTRAL_158 0x158

void Mario_SpawnProjectile(HSD_GObj *, guVector);

#endif