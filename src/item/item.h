#ifndef _item_h_
#define _item_h_

#include <gctypes.h>

#include "hsd/hsd_gobj.h"
#include "hsd/ext/hsd_jobj_ext.h"
#include "fighter/player.h"

#define ITEM_HAMMER 0x1C
#define ITEM_MARIO_FIREBALL 0x30
#define ITEM_DRMARIO_PILL 0x31
#define ITEM_PEACH_PARASOL 0x67

#define GOBJ_ITEM(s) ((Item*)s->data)

typedef struct _Item {
    struct _HSD_GObj* next;
    struct _HSD_GObj* parent;
    u32 x08_flags;
    u32 x0C_flags;
    u32 x10_item_type;
    u32 x14_item_subtype;
    u32 x1C_spawn_ctr;
    u32 x20_flags;
    u32 x24_unk;
    u32 x28_state_idx;
    f32 x2C_direction;
    f32 x30_direction_s;
    f32 x34_unk;
    f32 x38_scale;
    f32 x40_velocity_x;
    f32 x44_velocity_y;
    f32 x48_velocity_z;
    f32 x4C_position_x;
    f32 x50_position_y;
    f32 x54_position_z;
    f32 xB0_direction_pos;
    void* xB8_static_data;
    void* xBC_static_data;
    f32** xC4_item_attribs;
    void* xCC_proj_attribs;

    HSD_GObj* x518_owner;

    u8 xDC8_flags;

} Item;

BOOL Item_PlayerHasHammer(HSD_GObj*);
void Item_DestroyItem(HSD_GObj*);
void Item_GetPosition(HSD_GObj*, f32*);
u32 Item_GetItemType(HSD_GObj*);
f32 Item_GetDefaultDuration(HSD_GObj*);

void Item_8026B73C(HSD_GObj*);
u32 Item_8026B7E8(HSD_GObj*);

void Item_Unhide(HSD_GObj* gobj);

#endif
