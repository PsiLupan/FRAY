#include "hsd_dobj.h"

static HSD_DObj *current_dobj = NULL; //-0x40FC(r13)

//8035DD98
void HSD_DObjSetCurrent(HSD_DObj* dobj){
    current_dobj = dobj;
}

//8035DDA0
u32 HSD_DObjGetFlags(HSD_DObj* dobj){
    if(dobj != NULL){
        return dobj->flags;
    }
    return 0;
}

//8035DDB8
void HSD_DObjSetFlags(HSD_DObj* dobj, u32 flags){
    if(dobj != NULL){
        dobj->flags |= flags;
    }
}

//8035DDD0
void HSD_DObjClearFlags(HSD_DObj* dobj, u32 flags){
    if(dobj != NULL){
        dobj->flags &= ~flags;
    }
}