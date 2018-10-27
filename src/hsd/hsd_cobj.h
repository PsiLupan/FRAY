#ifndef _hsd_cobj_h_
#define _hsd_cobj_h_

#include <gctypes.h>
#include <ogc/gu.h>

typedef struct _HSD_CObj {
    u32 flags;
    f32 viewport_left;
    f32 viewport_right;
    f32 viewport_top;
    u16 viewport_bottom;
    u16 scissor_left;
    u16 scissor_right;
    u16 scissor_top;
    struct _HSD_WObj eye_position;
    struct _HSD_WObj interest;
    f32 roll;
    f32 pitch; //assumed
    f32 yaw; //assumed
    f32 frustrum_near;
    f32 frustrum_far;
    f32 frustrum_top;
    f32 frustrum_bottom;
    f32 frustrum_left;
    f32 frustrum_right;
    u8 projection_type;
    u8 unk51;
    u8 unk52;
    u8 unk53;
    u8 unk54;

    MtxP proj_mtx; //0x88
} HSD_CObj;

#endif