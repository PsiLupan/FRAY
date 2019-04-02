#ifndef _hsd_fog_h_
#define _hsd_fog_h_

#include <gctypes.h>

#include "hsd_aobj.h"
#include "hsd_cobj.h"

typedef struct _HSD_Fog {
    u8 type; //0x08
    u32 xC_unk; //0x0C
    f32 start; //0x10
    f32 end; //0x14
    GXColor color; //0x18
    HSD_AObj* aobj; //0x1C
} HSD_Fog;

typedef struct _HSD_FogDesc {
    u32 flags;
} HSD_FogDesc;

void HSD_FogAddAnim(HSD_Fog *, HSD_AObjDesc *);

#endif