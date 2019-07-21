#include "hsd_lobj_ext.h"

//80011AC4
/*HSD_LObj* LObj_LoadLightDescs(HSD_LightDesc** lightdescs){
    HSD_LObj* lobj = NULL;
    HSD_LObj* lobj_prev = NULL;
    u32 n = 0;
    for(HSD_LightDesc* i = lightdescs[n]; i != NULL; i = lightdescs[n]){
        lobj = HSD_LObjLoadDesc(i);
        if(i->anim != NULL){
            HSD_LObjAddAnim(lobj, i->anim);
        }
        if(lobj_prev != NULL){
            lobj_prev->next = lobj;   
        }
        lobj_prev = lobj;
        n += 1;
    }
    return lobj;
} */

//80391044
void LObj_Setup_Callback(HSD_GObj* gobj){
    HSD_LObj* lobj = GOBJ_HSD_LOBJ(gobj);
    HSD_LObjSetCurrentAll(lobj);
    HSD_LObjSetupInit(HSD_CObjGetCurrent());
}
