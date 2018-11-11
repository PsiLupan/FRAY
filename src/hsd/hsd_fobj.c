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
void FObjUpdateAnim(HSD_FObj* fobj, void* obj, void (*obj_update)(s32, u8, FObjData)){
    FObjData fobjdata;
    if(obj_update){
        u8 state = fobj->unk12;
        if(state == 2){
            u8 flags = fobj->flags;
            if(flags & 0x20){
                fobj->flags = flags & 0xDF;
                if(fobj->unk1A != 0){
                    fobj->unk28 = (fobj->unk24 - fobj->unk20) / 4.58594f; //Magic number
                }
            }else{
                fobj->unk28 = 0;
                fobj->unk20 = fobj->unk24;
            }
            fobjdata.fv = fobj->unk28 * fobj->unk1C + fobj->unk20;
        }else if(state == 1){
            f64 fVal = 176.0;
            f64 val = (fVal + fobj->unk1A) - fVal;
            if(fobj->unk1C < val){
                fobjdata.fv = fobj->unk24;
            }else{
                fobjdata.fv = fobj->unk20;
            }
        }else if(state == 6){
            if(!(fobj->flags & 0x80))
                return;
            fobjdata.fv = fobj->unk20;
            fobj->flags &= 0x7Fu;
        }else if(state < 6){
            if(fobj->unk1A != 0){
                f64 fVal = 176.0;
                f64 fVal2 = 1.0;
                f64 f0 = (fVal + fobj->unk1A) - fVal;
                f32 f1 = fVal2 / f0;
                fobjdata.fv = splGetHelmite(f0, f1, fobj->unk1C, fobj->unk20, fobj->unk24, fobj->unk28, fobj->unk2C);
            }else{
                fobjdata.fv = fobj->unk24;
            }
        }
        (obj_update)(obj, fobj->obj_type, fobjdata);
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