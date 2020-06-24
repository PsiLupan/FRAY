#ifndef _subaction_h_
#define _subaction_h_

#include <gctypes.h>

#include "hsd/hsd_gobj.h"

#include "hitbox.h"

typedef struct _SubactionInfo {
    u32 timer; //0x00
    f32 frame_count; //0x04
    u32* data_position; //0x08
    u32 loop_count; //0x0C
    void (*EventReturn)(); //0x10
    u32 loop_count_dup; //0x14
    u32 unk; //0x18
} SubactionInfo;

#endif
