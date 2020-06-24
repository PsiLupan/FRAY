#ifndef __hitbox_h__
#define __hitbox_h__

#include <gctypes.h>

#include "hsd/hsd_gobj.h"

typedef struct _Hitbox {
    u32 x0_status;
    u32 x4_active;
    guVector x10_x18_position;
    f32 x1C_size;
    u32 x24_kb_growth;
    u32 x28_kb_weight_scale;
    u32 x2C_kb_base;
    u32 x30_element;
    u32 x34_shield_damage;
    u32 x38_hit_sfx;
    u32 x3C_hit_sfx_type;
    u8 x40_flags;
    u8 x41_flags;
    u8 x42_flags;
    u8 x43_flags;
} Hitbox;

void Hitbox_Deactivate(Hitbox*);
void Hitbox_Activate(Hitbox*);
void Hitbox_DeactivateByIndex(HSD_GObj*, u32);
void Hitbox_DeactivateAll(HSD_GObj*);

#endif
