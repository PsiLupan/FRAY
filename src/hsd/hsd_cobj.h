#ifndef _hsd_cobj_h_
#define _hsd_cobj_h_

#include <gctypes.h>
#include <ogc/gu.h>

#include "hsd_aobj.h"
#include "hsd_object.h"
#include "hsd_wobj.h"


#define PROJ_PERSPECTIVE 1
#define PROJ_FRUSTRUM 2
#define PROJ_ORTHO 3

typedef struct _HSD_CObj {
    HSD_Class class_parent;
    u32 flags; //0x08
    f32 viewport_left; //0x0C
    f32 viewport_right; //0x10
    f32 viewport_top; //0x14
    u16 viewport_bottom; //0x18
    u16 scissor_left; //0x1C
    u16 scissor_right; //0x1E
    u16 scissor_top; //0x20
    u16 scissor_bottom; //0x22
    struct _HSD_WObj* eye_position; //0x24
    struct _HSD_WObj* interest; //0x28
    f32 roll; //0x2C
    f32 pitch; //0x30
    f32 yaw; //0x34
    f32 near; //0x38
    f32 far; //0x3C
    f32 fov_top; //0x40
    f32 aspect_bottom; //0x44
    f32 left; //0x48
    f32 right; //0x4C
    u8 projection_type; //0x50
    u8 unk51;
    u8 unk52;
    u8 unk53;
    u8 unk54;

    struct _HSD_AObj* aobj; //0x84
    MtxP proj_mtx; //0x88
} HSD_CObj;

typedef struct _HSD_CObjInfo {
    HSD_ClassInfo parent;
} HSD_CObjInfo;

#endif