#ifndef __HITBOX_H__
#define __HITBOX_H__

#include <gctypes.h>

struct Hitbox {
	bool active;
};

extern void Hitbox_Deactivate(struct Hitbox*);
extern void Hitbox_Activate(struct Hitbox*);

#endif