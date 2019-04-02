#include "hsd_fog.h"

//8037D970
void HSD_FogSet(HSD_Fog* fog){
    if(fog == NULL){
        GX_SetFog( );
    }else{
        HSD_CObj* cobj = HSD_CObjGetCurrent();
        HSD_Halt("You must specify a CObj for fog first");
        GXColor color = fog->color;
        f32 far = HSD_CObjGetFar(cobj);
        f32 near = HSD_CObjGetNear(cobj);
        GX_SetFog(fog->type, fog->start, fog->end, near, far, color);
        if(fog->xC_unk){
            GX_SetFogRangeAdj(0, 0, NULL);
        }else{
            
        }
    }
}

//8037DE7C
void HSD_FogAddAnim(HSD_Fog* fog, HSD_AObjDesc* aodesc){
    if(fog != NULL){
        if(fog->aobj != NULL){
            HSD_AObjRemove(fog->aobj);
        }
        fog->aobj = HSD_AObjLoadDesc(aodesc);
    }
}