#include "hsd_cobj_ext.h"

//80013B14
HSD_CObj* CObj_Create(HSD_CObjDesc* cdesc){
    HSD_CObj* cobj = HSD_CObjLoadDesc(cdesc);
    if(HSD_CObjGetProjectionType(cobj) == PROJ_PERSPECTIVE){
        f32 aspect = HSD_CObjGetAspect(cobj);
        if(aspect == 1.18f){
            HSD_CObjSetAspect(cobj, 1.21733f);
        }
    }
    u16 scissors[4];
    HSD_CObjGetScissor(cobj, scissors);
    if(scissors[0] != 640){
        scissors[0] = 640;
    }
    if(scissors[2] != 480){
        scissors[2] = 480;
    }
    HSD_CObjSetScissor(cobj, scissors);
    return cobj;
}

//801A1814
void CObj_SetCurrent_Callback(HSD_GObj* gobj){
    
}

//801A18D4
void CObj_SetErase_Callback(HSD_GObj* gobj){
    HSD_CObj* cobj = GOBJ_HSD_COBJ(gobj);
    HSD_CObjSetCurrent(cobj);
    if(cobj != NULL){
        HSD_SetEraseColor(0x26, 0x26, 0x26, 0xFF); //This actually uses some stack allocated values, but I'm hard coding for now
        HSD_CObjEraseScreen(cobj, 1, 0, 1);
        HSD_CObjEndCurrent();
    }
}