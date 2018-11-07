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
void FObjUpdateAnim(HSD_FObj* fobj, s32 r4, s32 (*callback)(s32, u8, f32)){
    f32 tVal = 0.0f;
    if(callback){
        u8 state = fobj->unk12;
        if(state == 2){
            u8 flags = fobj->flags;
            if(flags & 0x20){
                fobj->unk12 = flags & 0xDF;
                if(fobj->unk1A != 0){
                    f32* temp = ((u32)fobj->unk1A + 0x00004330); //Some of type of pointer - need to debug this @ 8036AF40
                    fobj->unk28 = (fobj->unk24 - fobj->unk20) / (*temp - (f32)/*r2 - 5376*/;
                }
            }else{
                fobj->unk28 = /*r2 - 5392*/;
                fobj->unk20 = fobj->unk24;
            }
            tVal = fobj->unk28 * fobj->unk1C + fobj->unk20;
        }else if(state == 1){
            tVal = fobj->unk1A - /*r2 - 5376*/;
            if(fobj->unk1C < tVal){
                tVal = fobj->unk24;
            }else{
                tVal = fobj->unk20;
            }
        }else if(state == 6){
            if(!(fobj->flags & 0x80))
                return;
            tVal = fobj->unk20;
            fobj->flags &= 0x7Fu;
        }else if(state < 6){
            if(fobj->unk1A != 0){
                f32* temp = ((u32)fobj->unk1A + 0x00004330); //Some of type of pointer - need to debug this @ 8036AFB0
                f32 val_1 = *temp - /*r2 - 5376*/;
                f32 val_2 = /*r2 - 5376*/ / val_1;
                tVal = splGetHelmite(val_1, val_2, fobj->unk1C, fobj->unk20, fobj->unk24, fobj->unk28, fobj->unk2C);
            }else{
                tVal = fobj->unk24;
            }
        }
        (callback)(r4, fobj->unk13, tVal);
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