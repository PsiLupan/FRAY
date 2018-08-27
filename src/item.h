#ifndef _item_h_
#define _item_h_

#include <stdbool.h>

#include <gctypes.h>

#include "gobj.h"

struct Item
{
	struct GObj* next;
	struct GObj* parent;
	u32 flags08;
	u32 flags0C;
	u32 item_type;
	u32 item_subtype;
	u32 spawn_ctr;
	u32 flags20;
	u32 unk24;
	u32 state_idx;
	f32 direction;
	f32 direction_s;
	f32 unk34;
	f32 scale;
	f32 velocity[3];
	f32 position[3];
	f32 direction_pos;
	void* static_dataB8;
	void* static_dataBC;
	void* static_dataC4;
	void* proj_attribs;
};

extern u32 Item_GetItemType(struct Item*);

#endif