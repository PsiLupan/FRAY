#ifndef _hsd_state_h_
#define _hsd_state_h_

#include <assert.h>

#include <gctypes.h>
#include <ogc/gx.h>

#include "hsd_aobj.h"
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
#define ENABLE_ALPHA_UPDATE 1 << 1
#define ENABLE_DST_ALPHA 1 << 2
#define BEFORE_TEX 1 << 3
#define ENABLE_COMPARE 1 << 4
#define ENABLE_ZUPDATE 1 << 5
#define ENABLE_DITHER 1 << 6

typedef s32 HSD_StateMask;

typedef struct _HSD_Chan {
    struct _HSD_Chan* next;
    u32 chan;
    u32 flags;
    GXColor amb_color;
    GXColor mat_color;
    u8 enable;
    u32 amb_src;
    u32 mat_src;
    u32 light_mask;
    u32 diff_fn;
    u32 attn_fn;
    HSD_AObj* aobj;
} HSD_Chan;

void HSD_SetupChannelMode(u32);
void HSD_SetupPEMode(u32, HSD_PEDesc*);
void HSD_SetupRenderModeWithCustomPE(u32, HSD_PEDesc*);
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
void HSD_SetupChannel(HSD_Chan*);
void HSD_StateSetNumChans(u8);
void HSD_StateInitTev(void);
u32 HSD_StateGetNumTevStages(void);
u8 HSD_StateAssignTev(void);
void HSD_StateSetNumTevStages(void);
void HSD_SetupTevStage(HSD_TevDesc*);
u8 HSD_Index2TevStage(u8);
u8 HSD_TevStage2Index(u8);

#endif
