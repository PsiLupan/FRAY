#ifndef _hsd_state_h_
#define _hsd_state_h_

#include <assert.h>

#include <gctypes.h>
#include <ogc/gx.h>

#include "hsd_mobj.h"

#define HSD_STATE_ALL -1
#define HSD_STATE_NONE 0
#define HSD_STATE_PRIMITIVE 0x1
#define HSD_STATE_VTX_ATTR 0x2
#define HSD_STATE_COLOR_CHANNEL 0x4
#define HSD_STATE_TEV_STAGE 0x8
#define HSD_STATE_TEV_REGISTER 0x10
#define HSD_STATE_TEX_COORD_GEN 0x20
#define HSD_STATE_RENDER_MODE 0x40

#define ENABLE_COLOR_UPDATE 1
#define ENABLE_ALPHA_UPDATE 1<<1
#define ENABLE_DST_ALPHA    1<<2
#define BEFORE_TEX          1<<3
#define ENABLE_COMPARE      1<<4
#define ENABLE_ZUPDATE      1<<5
#define ENABLE_DITHER       1<<6

typedef s32 HSD_StateMask;

typedef HSD_TExpTevDesc HSD_TevDesc;

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

void HSD_SetupChannelMode(u32);
void HSD_SetupPEMode(u32, HSD_PEDesc *);
void HSD_SetupRenderModeWithCustomPE(u32, HSD_PEDesc *);
void HSD_SetupRenderMode(u32);
void HSD_SetMaterialColor(GXColor, GXColor, GXColor, f32);
void HSD_SetMaterialShininess(f32);
void HSD_StateSetLineWidth(u8, u8);
void HSD_StateSetCullMode(u8);
void HSD_StateSetBlendMode(u8, u8, u8, u8);
void HSD_StateSetZMode(s32, u8, s32);
void HSD_StateSetAlphaCompare(u8, u8, u8, u8, u8);
void HSD_StateSetColorUpdate(s32);
void HSD_StateSetAlphaUpdate(s32);
void HSD_StateSetDstAlpha(s32, u8);
void HSD_StateSetZCompLoc(s32);
void HSD_StateSetDither(s32);
void HSD_StateInvalidate(s32);
void HSD_StateSetNumChans(u8);
void HSD_StateInitTev();
u32 HSD_StateGetNumTevStages();
u8 HSD_StateAssignTev();
void HSD_StateSetNumTevStages();
void HSD_SetupTevStage(HSD_TevDesc *);
u8 HSD_Index2TevStage(u8);
u8 HSD_TevStage2Index(u8);

#endif
