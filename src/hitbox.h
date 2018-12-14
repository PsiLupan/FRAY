#ifndef __HITBOX_H__
#define __HITBOX_H__

#include <gctypes.h>

typedef struct _Hitbox {
	BOOL x4_active;
} Hitbox;

extern void Hitbox_Deactivate(struct Hitbox*);
extern void Hitbox_Activate(struct Hitbox*);

#endif