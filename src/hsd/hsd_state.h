#ifndef _hsd_state_h_
#define _hsd_state_h_

#include <gctypes.h>
#include <ogc/gx.h>

#include "hsd_mobj.h"

#define HSD_STATE_ALL -1

#define ENABLE_COLOR_UPDATE 1
#define ENABLE_ALPHA_UPDATE 1<<1
#define ENABLE_DST_ALPHA    1<<2
#define BEFORE_TEX          1<<3
#define ENABLE_COMPARE      1<<4
#define ENABLE_ZUPDATE      1<<5
#define ENABLE_DITHER       1<<6

#define RENDER_DIFFUSE_BITS 0x00000008
#define RENDER_ZMODE_ALWAYS 0x08000000
#define RENDER_NO_ZUPDATE   0x20000000
#define RENDER_XLU          0x40000000

#define TEVOP_MODE          0

typedef struct _HSD_Chan {
    void* unk;
    u8 light_mask;
    GXColor color;
    GXColor ambient;
    GXColor mat;
    u8 gx_bool;
    u8 src_reg1;
    u8 src_reg2;
    u8 light_id;
    u8 diffuse;
    u8 attn;
} HSD_Chan;

typedef struct _HSD_TevDesc {
    u32 flags;
    u8  stage;
    u8  coord;
    u8  map;
    u8  color;
    union {
        struct {
            u8 tevmode;
        } tevop;
    } u;
} HSD_TevDesc;

void HSD_SetupChannelMode(u32 rendermode);
void HSD_SetupPEMode(u32 rendermode, HSD_PEDesc *pe);
void HSD_SetupRenderModeWithCustomPE(u32 rendermode, HSD_PEDesc *pe);
void HSD_SetupRenderMode(u32 rendermode);
void HSD_StateInvalidate(s32 state);
void HSD_StateSetNumChans(u8 num);
u32 HSD_StateGetNumTevStages();
u8 HSD_StateAssignTev();
void HSD_StateSetNumTevStages();
u8 HSD_Index2TevStage(u8 idx);
u8 HSD_TevStage2Index(u8 idx);

#endif