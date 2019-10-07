#include "hsd_robj.h"

#include <string.h>

#include "hsd_aobj.h"
#include "hsd_jobj.h"
#include "hsd_mobj.h"
#include "hsd_wobj.h"

HSD_ObjAllocData robj_alloc_data;
HSD_ObjAllocData rvalue_alloc_data;

//8037AE34
void HSD_RObjInitAllocData(void)
{
    HSD_ObjAllocInit(&robj_alloc_data, sizeof(HSD_RObj), 4); //size 0x1C
    HSD_ObjAllocInit(&rvalue_alloc_data, 0xc, 4); //size 0xC
}

//8037AE78
HSD_ObjAllocData* HSD_RObjGetAllocData(void)
{
    return &robj_alloc_data;
}

//8037AE84
HSD_ObjAllocData* HSD_RvalueObjGetAllocData(void)
{
    return &rvalue_alloc_data;
}

//8037AE90
void HSD_RObjSetFlags(HSD_RObj* robj, u32 flags)
{
    if (robj != NULL) {
        robj->flags |= flags;
    }
}

//8037AEA8
HSD_RObj* HSD_RObjGetByType(HSD_RObj* robj, u32 type, u32 subtype)
{
    if (robj != NULL) {
        for (HSD_RObj* curr = robj; curr != NULL; curr = curr->next) {
            u32 flags = curr->flags;
            if ((flags & 0x80000000) != 0 && (flags & TYPE_MASK) == type && (!subtype || subtype == (flags & 0xFFFFFFF))) {
                return curr;
            }
        }
    }
}

//8037AF14
static void RObjUpdateFunc(void* obj, u32 type, FObjData val)
{
    if (obj != NULL && type == TYPE_ROBJ) {
        HSD_RObj* robj = (HSD_RObj*)obj;
        if (val.fv >= 1.75f) {
            robj->flags = robj->flags | 0x80000000;
            return;
        }
        robj->flags = robj->flags & 0x7fffffff;
    }
}

//8037AF58
void HSD_RObjAnimAll(HSD_RObj* robj)
{
    if (robj != NULL) {
        for (HSD_RObj* i = robj; i != NULL; i = i->next) {
            HSD_AObjInterpretAnim(i->aobj, i, RObjUpdateFunc);
        }
    }
}

//8037AFC0
void HSD_RObjRemoveAnimAllByFlags(HSD_RObj* robj, u32 flags)
{
    if (robj != NULL) {
        for (HSD_RObj* i = robj; i != NULL; i = i->next) {
            if (i->aobj != NULL && (flags & 0x80) != 0) {
                HSD_AObjRemove(i->aobj);
                i->aobj = NULL;
            }
        }
    }
}

//8037B044
void HSD_RObjRemoveAnimAll(HSD_RObj* robj)
{
    if (robj != NULL) {
        for (HSD_RObj* i = robj; i != NULL; i = i->next) {
            if (i->aobj != NULL) {
                HSD_AObjRemove(i->aobj);
                i->aobj = NULL;
            }
        }
    }
}

//8037B0B0
void HSD_RObjReqAnimAllByFlags(HSD_RObj* robj, u32 flags, f32 frame)
{
    if (robj != NULL) {
        for (HSD_RObj* i = robj; i != NULL; i = i->next) {
            if (i->aobj != NULL && (flags & 0x80) != 0) {
                HSD_AObjReqAnim(i->aobj, frame);
            }
        }
    }
}

//8037B134
void HSD_RObjReqAnimAll(HSD_RObj* robj, f32 frame)
{
    if (robj != NULL) {
        for (HSD_RObj* i = robj; i != NULL; i = i->next) {
            if (i->aobj != NULL) {
                HSD_AObjReqAnim(i->aobj, frame);
            }
        }
    }
}

//8037B1A0
void HSD_RObjAddAnimAll(HSD_RObj* robj, HSD_RObjAnimJoint* anim)
{ //The second parameter is assumed based on layout - It literally is never NULL in my experience
    if (robj != NULL && anim != NULL) {
        HSD_RObj* i;
        HSD_RObjAnimJoint* j;
        for (i = robj, j = anim; i != NULL && j != NULL; i = i->next, j = j->next) {
            if (i->aobj != NULL) {
                HSD_AObjRemove(i->aobj);
            }
            i->aobj = HSD_AObjLoadDesc(j->aobjdesc);
        }
    }
}

//8037B230
u32 HSD_RObjGetGlobalPosition(HSD_RObj* robj, u32 flag, guVector* pos)
{
    u32 pos_count = 0;
    guVector gpos;
    if (robj == NULL) {
        return 0;
    } else {
        for (HSD_RObj* i = robj; i != NULL; i = i->next) {
            u32 flags = i->flags;
            if ((flags & TYPE_MASK) == REFTYPE_JOBJ && (flags & 0x80000000) != 0) {
                flags &= 0xfffffff;
                if (flags == flag) {
                    HSD_JObj* jobj = i->u.jobj;
                    assert(jobj != NULL);
                    BOOL need_mtx = FALSE;
                    if ((jobj->flags & USER_DEF_MTX) == 0 && (jobj->flags & MTX_DIRTY) != 0) {
                        need_mtx = TRUE;
                    }
                    if (need_mtx == TRUE) {
                        HSD_JObjSetupMatrixSub(jobj);
                    }
                    pos_count += 1;
                    gpos.x = gpos.x + jobj->mtx[0][3];
                    gpos.y = gpos.y + jobj->mtx[1][3];
                    gpos.z = gpos.z + jobj->mtx[2][3];
                }
            }
        }
        if (pos_count > 0) {
            pos->x = gpos.x;
            pos->y = gpos.y;
            pos->z = gpos.z;
        }
    }
    return pos_count;
}

//8037BFB0
void HSD_RObjUpdateAll(HSD_RObj* robj, void* jobj, void (*obj_update)(void*, u32, update*))
{
    if (robj != NULL) {
        guVector pos;
        u32 pos_count = HSD_RObjGetGlobalPosition(robj, 1, &pos);
        if (pos_count != 0) {
            update up;
            up.p = pos;
            (*obj_update)(jobj, 0x35, &up);
            (*obj_update)(jobj, 0x38, NULL);
        }
        /* sub_8037b648(robj, obj, obj_update);
        resolveCnsDirUp(robj, obj, obj_update);
        resolveLimits(robj, obj, obj_update);
         */
        for (HSD_RObj* r = robj; r != NULL; r = r->next) {
            if ((r->flags & 0x70000000) == 0 && (r->flags & 0x80000000) != 0) {
                //expEvaluate(robj->u.exp.rvalue, r->flags & 0xFFFFFFF, obj, obj_update);
            }
        }
    }
}

//8037C0CC
void HSD_RObjResolveRefsAll(HSD_RObj* robj, HSD_RObjDesc* robjdesc)
{
    HSD_RObj* ro;
    HSD_RObjDesc* rdesc;
    for (ro = robj, rdesc = robjdesc; ro != NULL && rdesc != NULL; ro = ro->next, rdesc = rdesc->next) {
        if (ro != NULL && rdesc != NULL) {
            u32 flags = ro->flags & TYPE_MASK;
            if (flags == REFTYPE_JOBJ) {
                HSD_JObjUnrefThis(ro->u.jobj);
                HSD_JObj* jobj = HSD_IDGetDataFromTable(NULL, (u32)rdesc->u.joint, NULL);
                ro->u.jobj = jobj;
                assert(ro->u.jobj != NULL);
                ro->u.jobj->parent.ref_count_individual += 1;
                assert(ro->u.jobj->parent.ref_count_individual != 0);
            } else {
                if (flags == 0) {
                    HSD_RvalueResolveRefsAll(ro->u.exp.rvalue, rdesc->u.exp->rvalue);
                }
            }
        }
    }
}

//8037C1EC
HSD_RObj* HSD_RObjLoadDesc(HSD_RObjDesc* desc)
{
    if (desc == NULL) {
        return NULL;
    } else {
        HSD_RObj* robj = HSD_RObjAlloc();
        HSD_RObj* next = HSD_RObjLoadDesc(desc->next);
        robj->next = next;
        robj->flags = desc->flags;
        u32 type = robj->flags & TYPE_MASK;
        u32 flags = robj->flags & 0xfffffff;
        switch (type) {
        case 0:
            expLoadDesc(&robj->u.exp, desc->u.exp);
            break;
        case REFTYPE_JOBJ:
            break;
        case 0x20000000:
            if (flags != 0 && flags < 7) {
                robj->u.limit = 0.01754533f * desc->u.limit;
            } else {
                robj->u.limit = desc->u.limit;
            }
            break;
        case 0x30000000:
            bcexpLoadDesc(&robj->u.exp, desc->u.bcexp);
            robj->flags = robj->flags & 0x8fffffff;
            break;
        case REFTYPE_IKHINT:
            robj->u.ik_hint.bone_length = desc->u.ik_hint->bone_length;
            robj->u.ik_hint.rotate_x = desc->u.ik_hint->bone_length;
            break;
        default:
            HSD_Halt("Unexpected type of RObj");
        }
        return robj;
    }
}

//8037C334
void HSD_RObjRemove(HSD_RObj* robj)
{
    if (robj != NULL) {
        u32 type = robj->flags & TYPE_MASK;
        if (type == REFTYPE_JOBJ) {
            HSD_JObjUnrefThis(robj->u.jobj);
        } else if (type == 0) {
            HSD_RvalueRemoveAll(robj->u.exp.rvalue);
        }
        HSD_AObjRemove(robj->aobj);
        HSD_RObjFree(robj);
    }
}

//8037C3A8
void HSD_RObjRemoveAll(HSD_RObj* robj)
{
    if (robj != NULL) {
        for (HSD_RObj* i = robj; i != NULL; i = i->next) {
            u32 type = i->flags & TYPE_MASK;
            if (type == REFTYPE_JOBJ) {
                HSD_JObjUnrefThis(i->u.jobj);
            } else if (type == 0) {
                HSD_RvalueRemoveAll(i->u.exp.rvalue);
            }
            HSD_AObjRemove(i->aobj);
            HSD_RObjFree(i);
        }
    }
}

//8037C444
HSD_RObj* HSD_RObjAlloc(void)
{
    HSD_RObj* robj = (HSD_RObj*)HSD_MemAlloc(sizeof(HSD_RObj)); //HSD_ObjAlloc(&robj_alloc_data);
    HSD_CheckAssert("Not enough memory for RObjAlloc", robj != NULL);
    memset(robj, 0, sizeof(robj));
    return robj;
}

//8037C4A0
void HSD_RObjFree(HSD_RObj* robj)
{
    HSD_ObjFree(&robj_alloc_data, (HSD_ObjAllocLink*)robj);
}

//8037C950
static f32 dummy_func()
{
    return 0.0f;
}

//8037C958
void HSD_RvalueRemoveAll(HSD_Rvalue* rvalue)
{
    HSD_Rvalue* next;
    while (next = rvalue, next != NULL) {
        rvalue = next->next;
        if (next != NULL) {
            HSD_JObjUnrefThis(next->jobj);
            HSD_ObjFree(&rvalue_alloc_data, (HSD_ObjAllocLink*)next);
        }
    }
}

//8037C9C8
void expLoadDesc(HSD_Exp* exp, HSD_ExpDesc* desc)
{
    memset(exp, 0, sizeof(HSD_Exp));
    if (desc != NULL) {
        if (desc->func == NULL) {
            exp->expr.func = dummy_func;
        } else {
            exp->expr.func = desc->func;
        }

        HSD_Rvalue* rv = NULL;
        if (desc->rvalue != NULL) {
            //TODO
        }
        exp->rvalue = rv;
        exp->nb_args = -1;
    }
}

//8037CAB4
void bcexpLoadDesc(HSD_Exp* exp, HSD_ByteCodeExpDesc* desc)
{
    memset(exp, 0, sizeof(HSD_Exp));
    if (desc != NULL) {
        if (desc->bytecode == NULL) {
            exp->expr.bytecode = 0;
        } else {
            exp->expr.bytecode = desc->bytecode;
        }

        HSD_Rvalue* rv = NULL;
        if (desc->rvalue != NULL) {
            //TODO
        }
        exp->rvalue = rv;
        exp->nb_args = -1;
        exp->is_bytecode = TRUE;
    }
}

//8037CBA4
void HSD_RvalueResolveRefsAll(HSD_Rvalue* rval, HSD_RvalueList* desc)
{
    if (desc != NULL) {
        while (rval != NULL && desc->joint != NULL) {
            if (rval != NULL && desc != NULL) {
                HSD_JObjUnrefThis(rval->jobj);
                HSD_JObj* jobj = HSD_IDGetDataFromTable(NULL, (u32)desc->joint, NULL);
                rval->jobj = jobj;
                assert(rval->jobj != NULL);
                HSD_JObjRefThis(jobj);
            }
            rval = rval->next;
            desc = desc->next;
        }
    }
}
