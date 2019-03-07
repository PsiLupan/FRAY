#include "hsd_lobj_ext.h"

//80391044
void LObj_Setup_Callback(HSD_GObj* gobj){
    HSD_LObj* lobj = GOBJ_HSD_LOBJ(gobj);
    HSD_LObjSetCurrentAll(lobj);
    HSD_LObjSetupInit(HSD_CObjGetCurrent());
}