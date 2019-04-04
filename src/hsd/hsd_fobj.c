#include "hsd_fobj.h"

HSD_ObjDef fobj_alloc_data;

//8036A938
HSD_ObjDef* HSD_FObjGetAllocData(){
    return &fobj_alloc_data;
}

//8036A944
void HSD_FObjInitAllocData(){
    return HSD_ObjAllocInit(&fobj_alloc_data, sizeof(HSD_FObj), 4);
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
void HSD_FObjReqAnimAll(HSD_FObj* fobj, f32 frame){
    if(fobj){
        for(HSD_FObj* curr = fobj; curr != NULL; curr = curr->next){
            *curr->curr_parse = curr->parse_start;
            
            fobj->unk1C = (f32)fobj->unk18 - 176.f + frame;
            curr->flags &= 0xBFu;
            curr->parsed_state = 0;
            curr->obj_type = 0;
            fobj->flags &= 0xF0u;
            fobj->unk20 = 0.f;
            fobj->unk24 = 0.f;
            fobj->unk28 = 0.f;
            fobj->flags = fobj->flags & 0xF0 | 1;
        }
    }
}

//8036AB24
void HSD_FObjStopAnim(HSD_FObj* fobj, void* obj, void(*obj_update)(), f32 frame){
    if(fobj){
        if(fobj->obj_state == 6){
            HSD_FObjInterpretAnim(fobj, obj, obj_update, frame);
        }
        fobj->flags &= 0xF0u;
    }
}

//8036AB78
void HSD_FObjStopAnimAll(HSD_FObj* fobj, void* obj, void(*obj_update)(), f32 frame){
    for(HSD_FObj* curr = fobj; curr != NULL; curr = curr->next){
        if(fobj->obj_state == 6){
            HSD_FObjInterpretAnim(curr, obj, obj_update, frame);
        }
        curr->flags &= 0xF0u;
    }
}

//8036AC10
static f32 FObjLoadData(u8** curr_parse, u8 unk){
    if(unk){
        f32 fvar;
        u8 flag = unk & 0xE0;
        if(flag == 96){
            u8 val = (*curr_parse)++;
            fvar = val - 176.f;
        }else if(flag < 96){
            if(flag == 64){
                /*Converts curr_parse[0] & [1] to a double then subtracts 176.f*/
                *curr_parse += 2;
            }else{
                if(flag > 64 || flag != 32){
                    return 0.0f;
                }else{
                    /*Converts curr_parse[0] & [1] to a double then subtracts 176.f*/
                    *curr_parse += 2;
                }
            }
        }else{
            if(flag != 128){
                return 0.0f;
            }
            u8 val = (*curr_parse)++;
            fvar = val - 176.f;
        }
        return fvar / (1 << unk & 0x1f) - 176.f;
    }
    u8* parse_pos = (*curr_parse)++;
    u32 data = *parse_pos;
    parse_pos = (*curr_parse)++;
    data |= (*parse_pos << 8);
    parse_pos = (*curr_parse)++;
    data |= (*parse_pos << 16);
    parse_pos = (*curr_parse)++;
    return (f32)(data | (*parse_pos << 24));
}

//8036ADDC
static u16 sub_8036ADDC(u8** curr_parse){
    u8* temp = *curr_parse;
    *curr_parse += 1;
    u8 parse_start = *temp;
    
    u8* parse;
    u8 result;
    u32 lshift = 3;

    if((parse_start & 0x80) == 0){
        return ((parse_start >> 4) & 7) + 1;
    }
    result = ((parse_start >> 4) & 7) + 1;

    do {
        u8* parse = *curr_parse;
        *curr_parse += 1;
        result += *parse & 0x7F << lshift;
        lshift += 7;
    } while((*parse & 0x80) != 0);
    return result;
}

//8036AE38
static void FObjConditionalAdjust(HSD_FObj* fobj){
    if(fobj->flags & 0x40){
        fobj->obj_state = fobj->parsed_state;
        fobj->flags &= 0xBFu;
        fobj->flags |= 0x80u;
        fobj->unk20 = fobj->unk24;
    }
}

//8036AE70
void FObjUpdateAnim(HSD_FObj* fobj, void* obj, void (*obj_update)(void*, u32, FObjData)){
    FObjData fobjdata;
    if(obj_update){
        u8 state = fobj->obj_state;
        if(state == 2){
            u8 flags = fobj->flags;
            if(flags & 0x20){
                fobj->flags = flags & 0xDF;
                if(fobj->parse_res != 0){
                    fobj->unk28 = (fobj->unk24 - fobj->unk20) / 4.58594f; //Magic number
                }
            }else{
                fobj->unk28 = 0;
                fobj->unk20 = fobj->unk24;
            }
            fobjdata.fv = fobj->unk28 * fobj->unk1C + fobj->unk20;
        }else if(state == 1){
            if(fobj->unk1C < (f32)fobj->parse_res){
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
            if(fobj->parse_res != 0){
                f32 f0 = (f32)fobj->parse_res;
                f32 f1 = 1.0f / f0;
                fobjdata.fv = splGetHelmite(f0, f1, fobj->unk1C, fobj->unk20, fobj->unk24, fobj->unk28, fobj->unk2C);
            }else{
                fobjdata.fv = fobj->unk24;
            }
        }
        (obj_update)(obj, fobj->obj_type, fobjdata);
    }
}

//8036B030
void HSD_FObjInterpretAnim(HSD_FObj* fobj, void* obj, void (*obj_update)(), f32 frame){
    u8 result = 0;
    f32 v22 = 0;

    if(fobj != NULL)
        result = FObjGetState(fobj);
    if(result == 0)
        return;

    fobj->unk1C = fobj->unk1C + frame;
    if(fobj->unk1C < 0)
        return;
    
    while(true){
        while(result != 4){
            switch(result) {
            
            case 0:
            return;

            case 1:
            case 2:
            if((*fobj->curr_parse - fobj->parse_start) < fobj->parse_len){
                fobj->obj_state = fobj->parsed_state;
                if(fobj->unk16 == 0){
                    fobj->parsed_state = **fobj->curr_parse & 0xF;
                    fobj->unk16 = sub_8036ADDC(fobj->curr_parse);
                }
                fobj->unk16 -= 1;
                u8 state = fobj->parsed_state;
                u8 flags = fobj->flags & 0xF;
                assert((flags - 1) > 1);
                
                switch(state){
                    case 1:
                    case 2:
                    fobj->unk20 = fobj->unk24;
                    fobj->unk2C = FObjLoadData(fobj->curr_parse, fobj->unk14);
                    if(fobj->obj_state != 5){
                        fobj->unk28 = fobj->unk2C;
                        fobj->unk2C = 0;
                    }
                    if(flags == 1){
                        result = 3;
                    }else{
                        result = 4;
                    }
                    fobj->flags = result & 0xF | fobj->flags & 0xF0;
                    break;

                    case 3:
                    fobj->unk20 = fobj->unk24;
                    fobj->unk28 = fobj->unk2C;
                    fobj->unk24 = FObjLoadData(fobj->curr_parse, fobj->unk14);
                    fobj->unk2C = 0;
                    if(flags == 1){
                        result = HSD_FObjSetState(fobj, 3);
                    }else{
                        result = HSD_FObjSetState(fobj, 4);
                    }
                    break;

                    case 4:
                    fobj->unk20 = fobj->unk24;
                    fobj->unk24 = FObjLoadData(fobj->curr_parse, fobj->unk14);
                    fobj->unk28 = fobj->unk2C;
                    fobj->unk2C = FObjLoadData(fobj->curr_parse, fobj->unk15);
                    if(flags == 1){
                        result = HSD_FObjSetState(fobj, 3);
                    }else{
                        result = HSD_FObjSetState(fobj, 4);
                    }
                    break;

                    case 5:
                    fobj->unk20 = fobj->unk24;
                    fobj->unk24 = FObjLoadData(fobj->curr_parse, fobj->unk14);
                    result = HSD_FObjGetState(fobj);
                    break;

                    case 6:
                    FObjConditionalAdjust(fobj);
                    fobj->unk24 = FObjLoadData(fobj->curr_parse, fobj->unk14);
                    fobj->flags |= 0x40u;

                    if(flags == 1){
                        result = HSD_FObjSetState(fobj, 3);
                    }else{
                        result = HSD_FObjSetState(fobj, 4);
                    }
                    break;

                    default:
                    result = 0;
                    break;
                }
            }else{
                result = 6;
            }
            break;

            case 3:
            if(fobj->flags & 0x80)
                FObjUpdateAnim(fobj, obj, obj_update);
            assert(fobj->flags & 0xF != 3);

            if((*fobj->curr_parse - fobj->parse_start) < fobj->parse_len){
                u32 parse_res;
                u32 lshift = 0;
                u8 parse;
                do {
                    u8* pos = *fobj->curr_parse;
                    *fobj->curr_parse += 1;
                    parse = *pos;
                    parse_res |= (parse & 0x7F) << lshift;
                    lshift += 7;
                } while((parse & 0x80) != 0);
                fobj->parse_res = parse_res;
                fobj->flags |= 0x20u;
                result = 2;
                fobj->flags = fobj->flags & 0xF0 | result;
            }else{
                result = 6;
            }
            break;

            case 5:
            result = 4;
            fobj->flags = fobj->flags & 0xF0 | result;
            break;

            case 6:
            fobj->unk1C = fobj->unk1C + v22;
            FObjConditionalAdjust(fobj);
            FObjUpdateAnim(fobj, obj, obj_update);
            return;
            break;

            default:
            break;
            }
        }
        if((f32)fobj->parse_res > fobj->unk1C)
            break;
        
        fobj->unk1C = fobj->unk1C - fobj->parse_res;
        v22 = fobj->unk1C;
        result = 3;
        fobj->flags = fobj->flags & 0xF0 | result;
    }
    FObjUpdateAnim(fobj, obj, obj_update);
    result = 5;
    fobj->flags = fobj->flags & 0xF0 | result;
}

//8036B6CC
void HSD_FObjInterpretAnimAll(HSD_FObj* fobj, void* caller_obj, void (*callback)(), f32 frame){
    for(HSD_FObj* curr = fobj; fobj != NULL; curr = fobj->next){
        HSD_FObjInterpretAnim(curr, caller_obj, callback, frame);
    }
}

//8036B848
HSD_FObj* HSD_FObjAlloc(){
    HSD_FObj* fobj = HSD_ObjAlloc(&fobj_alloc_data);
    assert(fobj);
    memset(fobj, 0, sizeof(HSD_FObj));
    return fobj;
}

//8036B8A4
void HSD_FObjFree(HSD_FObj* fobj){
    HSD_ObjFree(&fobj_alloc_data, fobj);
}