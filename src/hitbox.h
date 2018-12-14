#ifndef __HITBOX_H__
#define __HITBOX_H__

#include <gctypes.h>

#include "gobj.h"
#include "player.h"

typedef struct _Hitbox {
	BOOL x4_active;
} Hitbox;

void Hitbox_Deactivate(Hitbox*);
void Hitbox_Activate(Hitbox*);

#endif