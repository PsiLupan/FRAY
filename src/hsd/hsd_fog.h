#ifndef _hsd_fog_h_
#define _hsd_fog_h_

#include <gctypes.h>

#include "hsd_object.h"

#include "hsd_aobj.h"
#include "hsd_cobj.h"

typedef struct _HSD_FogAdj {
    HSD_Class class_parent;
    u16 x8_unk; //0x08
    u16 xA_unk; //0xA
    Mtx mtx; //0x0C
    f32 x3C_unk; //0x3C
    f32 x40_unk; //0x40
    f32 x44_unk; //0x44
    f32 x48_unk; //0x48
    u32 x4C_unk;
    u32 x50_unk;
} HSD_FogAdj;

typedef struct _HSD_Fog {
    HSD_Class class_parent;
    u8 type; //0x08
    struct _HSD_FogAdj* fog_adj; //0x0C
    f32 start; //0x10
    f32 end; //0x14
    GXColor color; //0x18
    struct _HSD_AObj* aobj; //0x1C
    u32 unk;
} HSD_Fog;

typedef struct _HSD_FogAdjDesc {
    u16 x0_unk; //0x00
    u16 x2_unk; //0x02
    MtxP pmtx; //0x04
} HSD_FogAdjDesc;

typedef struct _HSD_FogInfo {
    HSD_ClassInfo parent;
} HSD_FogInfo;

typedef struct _HSD_FogAdjInfo {
    HSD_ClassInfo parent;
} HSD_FogAdjInfo;

typedef struct _HSD_FogDesc {
    u32 unk;
    struct _HSD_FogAdjDesc* fogadjdesc; //0x04
} HSD_FogDesc;

void HSD_FogSet(HSD_Fog *);
HSD_Fog* HSD_FogLoadDesc(HSD_FogDesc *);
HSD_FogAdj* HSD_FogAdjLoadDesc(HSD_FogAdjDesc *);
void HSD_FogAdjInit(HSD_FogAdj *, HSD_FogAdjDesc *);
void HSD_FogAddAnim(HSD_Fog *, HSD_AObjDesc *);
void HSD_FogReqAnim(HSD_Fog *, f32);
void HSD_FogReqAnimByFlags(HSD_Fog *, u32, f32);
void HSD_FogInterpretAnim(HSD_Fog *);
void FogUpdateFunc(HSD_Fog *, u32, f32 *);

#endif
