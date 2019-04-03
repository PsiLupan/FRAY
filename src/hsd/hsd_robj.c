#include "hsd_robj.h"

#include "hsd_aobj.h"
#include "hsd_mobj.h"
#include "hsd_wobj.h"

HSD_ObjDef robj_alloc_data;
HSD_ObjDef rvalue_alloc_data;

//8037AE34
void HSD_RObjInitAllocData(){
    HSD_ObjAllocInit(&robj_alloc_data, sizeof(HSD_RObj), 4); //size 0x1C
    HSD_ObjAllocInit(&rvalue_alloc_data, 0xc, 4); //size 0xC
}

//8037AE78
void HSD_RObjGetAllocData(){
    return &robj_alloc_data;
}

//8037AE84
void HSD_RvalueObjGetAllocData(){
    return &rvalue_alloc_data;
}

//8037AE90
void HSD_RObjSetFlags(HSD_RObj* robj, u32 flags){
    if(robj){
        robj->flags |= flags;
    }
}

//8037AEA8
HSD_RObj* HSD_RObjGetByType(HSD_RObj* robj, u32 type, u32 flags2){
    if(robj){
        for(HSD_RObj* curr = robj; curr != NULL; curr = curr->next){
            u32 flags = curr->flags;
            if( (flags & 0x80000000) != 0 && (flags & TYPE_MASK) == type && (!flags2 || flags2 == (flags & 0xFFFFFFF)) ){
                return curr;
            }
        }
    }
}

//8037AF14
static void RObjUpdateFunc(HSD_RObj* robj, u32 obj_type, FObjData data){
    if(robj != NULL && obj_type == TYPE_ROBJ){
        if(data.fv >= 1.75f){
            robj->flags = robj->flags | 0x80000000;
            return;
        }
        robj->flags = robj->flags & 0x7fffffff;
    }
}

//8037AF58
void HSD_RObjAnimAll(HSD_RObj* robj){
    if(robj != NULL){
        for(HSD_RObj* i = robj; i != NULL; i = i->next){
            HSD_AObjInterpretAnim(i->aobj, i, RObjUpdateFunc);
        }
    }
}

//8037AFC4
void HSD_RObjRemoveAnimAllByFlags(HSD_RObj* robj, u32 flags){
    if(robj != NULL){
        for(HSD_RObj* i = robj; i != NULL; i = i->next){
            if(i->aobj != NULL && (flags & 0x80) != 0){
                HSD_AObjRemove(i->aobj);
                i->aobj = NULL;
            }
        }
    }
}

//8037B044
void HSD_RObjRemoveAnimAll(HSD_RObj* robj){
    if(robj != NULL){
        for(HSD_RObj* i = robj; i != NULL; i = i->next){
            if(i->aobj != NULL){
                HSD_AObjRemove(i->aobj);
                i->aobj = NULL;
            }
        }
    }
}

//8037B0B0
void HSD_RObjReqAnimAllByFlags(HSD_RObj* robj, u32 flags, f32 frame){
    if(robj != NULL){
        for(HSD_RObj* i = robj; i != NULL; i = i->next){
            if(i->aobj != NULL && (flags & 0x80) != 0){
                HSD_AObjReqAnim(i->aobj, frame);
            }
        }
    }
}

//8037B134
void HSD_RObjReqAnimAllByFlags(HSD_RObj* robj, f32 frame){
    if(robj != NULL){
        for(HSD_RObj* i = robj; i != NULL; i = i->next){
            if(i->aobj != NULL){
                HSD_AObjReqAnim(i->aobj, frame);
            }
        }
    }
}

//8037B1A0
void HSD_RObjAddAnimAll(HSD_RObj* robj, HSD_InterestAnim* anim){ //The second parameter is assumed based on layout - It literally is never NULL in my experience
    if(robj != NULL && anim != NULL){
        HSD_RObj* i;
        HSD_MatAnim* j;
        for(i = robj, j = anim; i != NULL && j != NULL; i = i->next, j = j->next){
            if(i->aobj != NULL){
                HSD_AObjRemove(i->aobj);
            }
            i->aobj = HSD_AObjLoadDesc(j->aobjdesc);
        }
    }
}

//8037B230
u32 HSD_RObjGetGlobalPosition(HSD_RObj* robj, u32 flag, guVector* pos){
    u32 pos_count = 0;
    guVector gpos;
    if(robj == NULL){
        return 0;
    }else{
        for(HSD_RObj* i = robj; i != NULL; i = i->next){
            u32 flags = i->flags;
            if((flags & TYPE_MASK) == REFTYPE_JOBJ && (flags & 0x80000000) != 0){
                flags &= 0xfffffff;
                if(flags == flag){
                    HSD_JObj* jobj = i->u.jobj;
                    assert(jobj != NULL);
                    BOOL need_mtx = FALSE;
                    if((jobj->flags & 0x800000) == 0 && (jobj->flags & 0x40) != 0){
                        need_mtx = TRUE;
                    }
                    if(need_mtx == TRUE){
                        HSD_JObjSetupMatrixSub(jobj);
                    }
                    pos_count += 1;
                    gpos.x = gpos.x + jobj->mtx[0][3];
                    gpos.y = gpos.y + jobj->mtx[1][3];
                    gpos.z = gpos.z + jobj->mtx[2][3];
                }
            }
        }
        if(pos_count > 0){
            pos->x = gpos.x;
            pos->y = gpos.y;
            pos->z = gpos.z;
        }
    }
    return pos_count;
}

//8037C1EC
HSD_RObj* HSD_RObjLoadDesc(HSD_RObjDesc* desc){
    if(desc == NULL){
        return NULL;
    }else{
        HSD_RObj* robj = HSD_RObjAlloc();
        HSD_RObj* next = HSD_RObjLoadDesc(desc->next);
        robj->next = next;
        robj->flags = desc->flags;
        u32 type = robj->flags & TYPE_MASK;
        switch (type){
            case 0:
                //expLoadDesc(&robj->u.rvalue, desc->u.rvalue);
                break;
            case REFTYPE_JOBJ:
                break;
            case 0x20000000:
                u32 flags = robj->flags & 0xfffffff;
                if(flags != 0 && flags < 7){
                    robj->u.fv = 0.01754533f * desc->u.fv;
                }else{
                    robj->u.fv = desc->u.fv;
                }
                break;
            case 0x30000000:
                robj->flags = robj->flags & 0x8fffffff;
                break;
            case 0x40000000:
                //bcexpLoadDesc(&robj->u.rvalue, desc->u.rvalue);
                robj->xC_unk = desc->xC_unk;
                break;
            default:
                HSD_Halt("Unexpected type of RObj");
        }
        return robj;
    }
}

//8037C334
void HSD_RObjRemove(HSD_RObj* robj){
    if(robj != NULL){
        u32 type = robj->flags & TYPE_MASK;
        if(type == REFTYPE_JOBJ){
            HSD_JObjUnrefThis(robj->u.jobj);
        }else if(type == 0){
            //HSD_RvalueRemoveAll(robj->xC_unk);
        }
        HSD_AObjRemove(robj->aobj);
        HSD_RObjFree(robj);
    }
}

//8037C3A8
void HSD_RObjRemoveAll(HSD_RObj* robj){
    if(robj != NULL){
        for(HSD_RObj* i = robj; i != NULL; i = i->next){
            u32 type = i->flags & TYPE_MASK;
            if(type == REFTYPE_JOBJ){
                HSD_JObjUnrefThis(i->u.jobj);
            }else if(type == 0){
                //HSD_RvalueRemoveAll(i->xC_unk);
            }
            HSD_AObjRemove(i->aobj);
            HSD_RObjFree(i);
        }
    }
}

//8037C444
HSD_RObj* HSD_RObjAlloc(){
    HSD_RObj* robj = (HSD_RObj*)HSD_ObjAlloc(&robj_alloc_data);
    if(robj == NULL){
        HSD_Halt("Not enough memory for RObjAlloc");
    }
    memset(robj, 0, sizeof(robj));
    return robj;
}

//8037C4A0
void HSD_RObjFree(HSD_RObj* robj){
    HSD_ObjFree(&robj_alloc_data, robj);
}