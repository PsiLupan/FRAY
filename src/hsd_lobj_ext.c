#include "hsd_lobj_ext.h"

//80011AC4
HSD_LObj* LObj_LoadLightDescs(HSD_Light* light){
    HSD_LObj* lobj = NULL;
    HSD_LObj* prev = NULL;
    for(HSD_Light* i = light; i != NULL; i = i->next){
        lobj = HSD_LObjLoadDesc(i->desc);
        HSD_LightDesc* next = i->desc->next;
        if(i->anim != NULL){
            HSD_LObjAddAnim(lobj, i->anim);
        }
        if(prev != NULL){
            prev->next = lobj;   
        }
        prev = lobj;
    }
    return lobj;
}

//80391044
void LObj_Setup_Callback(HSD_GObj* gobj){
    HSD_LObj* lobj = GOBJ_HSD_LOBJ(gobj);
    HSD_LObjSetCurrentAll(lobj);
    HSD_LObjSetupInit(HSD_CObjGetCurrent());
}
