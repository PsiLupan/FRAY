#include "hsd_cobj_ext.h"

//80013B14
HSD_CObj* CObj_Create(HSD_CObjDesc* cdesc)
{
    HSD_CObj* cobj = HSD_CObjLoadDesc(cdesc);
    if (HSD_CObjGetProjectionType(cobj) == PROJ_PERSPECTIVE) {
        f32 aspect = HSD_CObjGetAspect(cobj);
        if (aspect == 1.18f) {
            HSD_CObjSetAspect(cobj, 1.21733f);
        }
    }
    u16 scissors[4];
    HSD_CObjGetScissor(cobj, scissors);
    scissors[0] = 640;
    scissors[2] = 480;
    HSD_CObjSetScissor(cobj, scissors);
    return cobj;
}

//801A1814
void CObj_Texture_Callback(HSD_GObj* gobj, s32 unused)
{
    HSD_CObj* cobj = GOBJ_HSD_COBJ(gobj);
    if(HSD_CObjSetCurrent(cobj)){
        GObj_SetTextureCamera(gobj, 7);
        HSD_CObjEndCurrent();
    }
}

//801A18D4
void CObj_SetErase_Callback(HSD_GObj* gobj, s32 unused)
{
    GXColor erase = {0x26, 0x26, 0x26, 0xFF};
    HSD_CObj* cobj = GOBJ_HSD_COBJ(gobj);

    if (HSD_CObjSetCurrent(cobj)) {
        HSD_SetEraseColor(erase.r, erase.g, erase.b, erase.a);
        HSD_CObjEraseScreen(cobj, 1, 0, 1);
        HSD_CObjEndCurrent();
    }
}
