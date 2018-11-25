#include "hsd_wobj.h"

static void HSD_WObjInfoInit();

HSD_WObjInfo hsdWObj = { HSD_WObjInfoInit };

//8037D050
void HSD_WObjRemoveAnim(HSD_WObj* wobj){
    if(wobj){
        HSD_AObjRemove(wobj->aobj);
        wobj->aobj = NULL;
        HSD_RObjRemoveAnimAll(wobj->robj);
    }
}

//8037D094
void HSD_WObjReqAnim(HSD_WObj* wobj, f32 frame){
    if(wobj){
        HSD_AObjReqAnim(wobj->aobj, frame);
        HSD_RObjReqAnimAll(wobj->robj, frame);
    }
}

//8037D0E4
void HSD_WObjAddAnim(HSD_WObj* wobj, HSD_AObjDesc* aobjdesc){ //TODO: Probably not an AObjDesc. Did show that the desc has issues.
    if(wobj && aobjdesc){
        if(wobj->aobj)
            HSD_AObjRemove(wobj->aobj);
        wobj->aobj = HSD_AObjLoadDesc(aobjdesc);
        HSD_RObjAddAnimAll(wobj->robj, /*aobjdesc->unk04*/);
    }
}

//8037D45C
void HSD_WObjSetPosition(HSD_WObj* wobj, guVector* pos){
    if(wobj && pos){
        wobj->x = pos->x;
        wobj->y = pos->y;
        wobj->z = pos->z;
        wobj->flags |= 0x2u;
        wobj->flags &= 0xFFFFFFFE;
    }
}

//8037D900
static void HSD_WObjInfoInit(){
    hsdInitClassInfo(HSD_CLASS_INFO(&hsdWObj), HSD_CLASS_INFO(&hsdClass), HSD_BASE_CLASS_LIBRARY, "hsd_wobj", sizeof(HSD_WObjInfo), sizeof(HSD_WObj));
} //TODO: remainder of func