#include "hsd_fobj.h"

//8036AA80
void HSD_FObjReqAnimAll(HSD_FObj* fobj, float frame){
	
}

//8036AE38
void FObjConditionalAdjust(HSD_FObj* fobj){
    if(fobj->flags & 0x40){
        fobj->unk12 = fobj->unk11;
        fobj->flags &= 0xBFu;
        fobj->flags |= 0x80u;
        fobj->unk20 = fobj->unk24;
    }
}

//8036AE70
void FObjUpdateAnim(HSD_FObj* fobj, int r4, void (*callback)()){
    if(callback){
        u8 state = fobj->unk12;
        if(state == 2){
            u8 flags = fobj->flags;
            if(flags & 0x20){
                fobj->unk12 = flags & 0xDF;
                if(fobj->unk1A != 0){
                    fobj->unk28 = (fobj->unk24- fobj->unk20) / ((f32)fobj->unk1A - (f32)/*r2 - 5376*/;
                }
            }else{
                fobj->unk28 = /*r2 - 5392*/;
                fobj->unk20 = fobj->unk24;
            }
            (callback)();
        }else if(state == 1){
            
        }else if(state == 6){

        }else if(state < 6){

        }else{

        }
    }
}

//8036B030
void HSD_FObjInterpretAnim(HSD_FObj* fobj, void* caller_obj, void (*callback)(), f32 frame){

}

//8036B6CC
void HSD_FObjInterpretAnimAll(HSD_FObj* fobj, void* caller_obj, void (*callback)(), f32 frame){
    for(HSD_FObj* curr = fobj; fobj != NULL; curr = fobj->next){
        HSD_FObjInterpretAnim(curr, caller_obj, callback, frame);
    }
}