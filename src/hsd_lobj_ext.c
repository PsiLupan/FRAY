#include "hsd_lobj_ext.h"

//80011AC4
HSD_LObj* LObj_LoadLightDescs(HSD_Lights* lights)
{
    HSD_LObj* lobj = NULL;
    HSD_LObj* prev = NULL;
    HSD_LObj* not_prev = NULL;
    HSD_LObj* res = NULL;
    for (HSD_Lights* i = lights; i != NULL && i->list != NULL; ++i) {
        lobj = HSD_LObjLoadDesc(i->list->lightdesc);
        AnimList* alist = i->list->anims;
        if (alist != NULL) {
            HSD_LObjAddAnim(lobj, alist->lightanim);
        }
        not_prev = lobj;
        if (prev != NULL) {
            prev->next = lobj;
            not_prev = res;
        }
        res = not_prev;
        prev = lobj;
    }
    return res;
}

//80391044
void LObj_Setup_Callback(HSD_GObj* gobj)
{
    HSD_LObj* lobj = GOBJ_HSD_LOBJ(gobj);
    HSD_LObjSetCurrentAll(lobj);
    HSD_LObjSetupInit(HSD_CObjGetCurrent());
}