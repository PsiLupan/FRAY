#include "hsd_fobj.h"

static HSD_FObj* init_fobj = NULL;

//8036A938
HSD_FObj* HSD_FObjGetAllocData(){
    return init_fobj;
}

//8036A944
HSD_FObj* HSD_FObjInitAllocData(){
    return HSD_ObjAllocInit(init_fobj, sizeof(HSD_FObj), 4);
}

//8036A974
void HSD_FObjRemove(HSD_FObj* fobj){
    if(fobj)
        HSD_FObjFree(fobj);
}

//8036A99C
void HSD_FObjRemoveAll(HSD_FObj* fobj){
    if(fobj){
        HSD_FObj* child = fobj->next;
        if(child){
            HSD_FObj* child_2 = child->next;
            if(child_2){
                HSD_FObj* child_3 = child_2->next;
                if(child_3){
                    HSD_FObjRemoveAll(child_3->next);
                    HSD_FObjRemove(child_3);
                }
                HSD_FObjFree(child_2);
            }
            HSD_FObjFree(child);
        }
        HSD_FObjFree(fobj);
    }
}

//8036AA44
u8 HSD_FObjSetState(HSD_FObj* fobj, u8 state){
    if(fobj){
        fobj->flags = state & 0xF | fobj->flags & 0xF0;
    }
    return state;
}

//8036AA64
u8 HSD_FObjGetState(HSD_FObj* fobj){
    if(fobj){
        return fobj->flags & 0xF;
    }
    return 0;
}

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
    u8 state = 0;
    if(fobj != NULL)
        state = FObjGetState(fobj);
    if(state == 0)
        return;

    f32 fVal = fobj->unk1C + frame;
    fobj->unk1C = fVal;
    if(fVal < /*r2 - 5360*/ )
        return;
    
    
}

//8036B6CC
void HSD_FObjInterpretAnimAll(HSD_FObj* fobj, void* caller_obj, void (*callback)(), f32 frame){
    for(HSD_FObj* curr = fobj; fobj != NULL; curr = fobj->next){
        HSD_FObjInterpretAnim(curr, caller_obj, callback, frame);
    }
}

//8036B848
HSD_FObj* HSD_FObjAlloc(){
    HSD_FObj* fobj = HSD_ObjAlloc(init_fobj);
    assert(fobj);
    memset(fobj, 0, sizeof(HSD_FObj));
    return fobj;
}

//8036B8A4
void HSD_FObjFree(HSD_FObj* fobj){
    HSD_ObjFree(init_fobj, fobj);
}