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

//8035DDE8
void HSD_DObjModifyFlags(HSD_DObj* dobj, u32 flags_1, u32 flags_2){
    if(dobj != NULL){
        dobj->flags = dobj->flags & ~flags_2 | flags_1 & flags_2;
    }
}

//8035DE08
void HSD_DObjRemoveAnimAllByFlags(HSD_DObj* dobj, u32 flags){
    if(dobj != NULL){
        for(HSD_DObj* i; i != NULL; i = i->next){
            if((flags & 2) != 0){
                HSD_AObjRemove(dobj->aobj);
                dobj->aobj = NULL;
            }
            HSD_PObjRemoveAnimAllByFlags(dobj->pobj, flags);
            HSD_MObjRemoveAnimByFlags(dobj->mobj, flags);
        }
    }
}