#ifndef _hsd_fog_h_
#define _hsd_fog_h_

#include <gctypes.h>

#include "hsd_object.h"

#include "hsd_aobj.h"
#include "hsd_cobj.h"

typedef struct _HSD_FogAdj {
    HSD_Obj parent;
    #ifdef NEW_LIB
    u32 flags;
    #endif
    u16 center; //0x08
    u16 width; //0xA
    Mtx44 mtx; //0x0C
    struct _HSD_AObj* aobj;
} HSD_FogAdj;

typedef struct _HSD_Fog {
    HSD_Obj parent;
    u8 type; //0x08
    struct _HSD_FogAdj* fog_adj; //0x0C
    f32 start; //0x10
    f32 end; //0x14
    GXColor color; //0x18
    struct _HSD_AObj* aobj; //0x1C
} HSD_Fog;

typedef struct _HSD_FogAdjDesc {
    u16 center; //0x00
    u16 width; //0x02
    Mtx44 mtx; //0x04
} HSD_FogAdjDesc;

typedef struct _HSD_FogInfo {
    HSD_ObjInfo parent;
} HSD_FogInfo;

typedef struct _HSD_FogAdjInfo {
    HSD_ObjInfo parent;
} HSD_FogAdjInfo;

typedef struct _HSD_FogDesc {
    u32 type;
    struct _HSD_FogAdjDesc* fogadjdesc; //0x04
    f32 start;
    f32 end;
    GXColor color;
} HSD_FogDesc;

void HSD_FogSet(HSD_Fog*);
HSD_Fog* HSD_FogLoadDesc(HSD_FogDesc*);
void HSD_FogInit(HSD_Fog*, HSD_FogDesc*);
HSD_FogAdj* HSD_FogAdjLoadDesc(HSD_FogAdjDesc*);
void HSD_FogAdjInit(HSD_FogAdj*, HSD_FogAdjDesc*);
void HSD_FogAddAnim(HSD_Fog*, HSD_AObjDesc*);
void HSD_FogReqAnim(HSD_Fog*, f32);
void HSD_FogReqAnimByFlags(HSD_Fog*, u32, f32);
void HSD_FogInterpretAnim(HSD_Fog*);
void FogUpdateFunc(HSD_Fog*, u32, f32*);

#endif
