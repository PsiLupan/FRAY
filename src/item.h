#ifndef _item_h_
#define _item_h_

#include <stdbool.h>

#include <gctypes.h>

#include "gobj.h"
#include "player.h"

#define ITEM_HAMMER 0x1C

typedef struct _Item
{
	struct _HSD_GObj* next;
	struct _HSD_GObj* parent;
	u32 x08_flags;
	u32 x0C_flags;
	u32 item_type;
	u32 item_subtype;
	u32 spawn_ctr;
	u32 x20_flags;
	u32 x24_unk;
	u32 state_idx;
	f32 direction;
	f32 direction_s;
	f32 x34_unk;
	f32 scale;
	guVector velocity;
	guVector position;
	f32 direction_pos;
	void* xB8_static_data;
	void* xBC_static_data;
	void* xC4_static_data;
	void* proj_attribs;
} Item;

extern u32 Item_GetItemType(struct Item*);

#endif