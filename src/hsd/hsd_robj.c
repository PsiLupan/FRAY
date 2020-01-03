#include "hsd_robj.h"

#include <math.h>
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
    return NULL;
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
{
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
    guVector global_position = { 0, 0, 0 };

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
                    HSD_JObjSetupMatrix(jobj);
                    pos_count += 1;
                    global_position.x += guMtxRowCol(jobj->mtx, 0, 3);
                    global_position.y += guMtxRowCol(jobj->mtx, 1, 3);
                    global_position.z += guMtxRowCol(jobj->mtx, 2, 3);
                }
            }
        }
        if (pos_count > 0) {
            pos->x = global_position.x;
            pos->y = global_position.y;
            pos->z = global_position.z;
        }
    }
    return pos_count;
}

static void resolveCnsPos(HSD_RObj* robj, void* obj, void (*obj_update)(void*, u32, FObjData*))
{
    guVector pos;
    FObjData gpos;

    u32 pos_count = HSD_RObjGetGlobalPosition(robj, 1, &pos);
    if (pos_count != 0) {
        gpos.p = pos;
        (*obj_update)(obj, 53, &gpos);
        (*obj_update)(obj, 56, NULL);
    }
}

//8037B400
static void set_dirup_matrix(guVector* dir_ptr, guVector* uv_ptr, guVector* scale_ptr, void* obj, void (*obj_update)(void*, u32, FObjData*))
{
    f32 dot;
    f32 rt;
    guVector vec;
    FObjData data;

    guVecCross(dir_ptr, uv_ptr, &vec);
    dot = guVecDotProduct(dir_ptr, dir_ptr);
    rt = sqrtf(1.0F / 1e-10F + dot);
    guVecScale(dir_ptr, dir_ptr, rt);

    dot = guVecDotProduct(&vec, &vec);
    rt = sqrtf(1.0F / 1e-10F + dot);
    guVecScale(&vec, &vec, rt);
    guVecCross(&vec, dir_ptr, uv_ptr);

    data.p.x = dir_ptr->x * scale_ptr->x;
    data.p.y = dir_ptr->y * scale_ptr->x;
    data.p.z = dir_ptr->z * scale_ptr->x;
    (*obj_update)(obj, 50, &data);

    data.p.x = dir_ptr->x * scale_ptr->y;
    data.p.y = dir_ptr->y * scale_ptr->y;
    data.p.z = dir_ptr->z * scale_ptr->y;
    (*obj_update)(obj, 51, &data);

    data.p.x = dir_ptr->x * scale_ptr->z;
    data.p.y = dir_ptr->y * scale_ptr->z;
    data.p.z = dir_ptr->z * scale_ptr->z;
    (*obj_update)(obj, 52, &data);

    (*obj_update)(obj, 55, NULL);
}

//8037B648
static void resolveCnsDirUp(HSD_RObj* robj, void* obj, void (*obj_update)(void*, u32, FObjData*))
{
    guVector dir;
    guVector scale = { 1.F, 1.F, 1.F };
    guVector up = { 0.F, 1.F, 0.F };
    HSD_JObj* jobj = (HSD_JObj*)obj;

    if (HSD_RObjGetGlobalPosition(robj, 2, &dir)) {
        guVector global = { guMtxRowCol(jobj->mtx, 0, 3), guMtxRowCol(jobj->mtx, 1, 3), guMtxRowCol(jobj->mtx, 2, 3) };
        guVecSub(&dir, &global, &dir);
        if (HSD_RObjGetGlobalPosition(robj, 3, &up)) {
            if (fabs(1.F - guVecDotProduct(&dir, &up)) < 1e-10F) {
                up.x = 0.F;
                up.y = 0.F;
                up.z = 1.F;
            }
        } else {
            guVecSub(&up, &global, &up);
        }

        guVector* pvec = jobj->pvec;
        if (pvec != NULL) {
            scale.x = pvec->x;
            scale.y = pvec->y;
            scale.z = pvec->z;
        }
        set_dirup_matrix(&dir, &up, &scale, obj, obj_update);
    }
}

//8037B7B0
static void resolveCnsOrientation(HSD_RObj* robj, void* obj, void (*obj_update)(void*, u32, FObjData*))
{
    Mtx mtx;
    f32 mag, jmag;
    FObjData data;

    HSD_JObj* jobj = (HSD_JObj*)obj;
    assert(jobj != NULL);

    HSD_RObj* ro = HSD_RObjGetByType(robj, REFTYPE_JOBJ, 4);
    if (ro != NULL) {
        MtxP pmtx = HSD_JObjGetMtxPtr(ro->u.jobj);
        guMtxCopy(pmtx, mtx);
        for (u32 i = 0; i < 3; ++i) {
            guVector vec = { guMtxRowCol(mtx, 0, i), guMtxRowCol(mtx, 1, i), guMtxRowCol(mtx, 2, i) };
            mag = vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
            if(1e-10F < mag){
                mag = 1.0F / mag;
            }
            
            vec.x = guMtxRowCol(jobj->mtx, 0, i);
            vec.y = guMtxRowCol(jobj->mtx, 1, i);
            vec.z = guMtxRowCol(jobj->mtx, 2, i);
            jmag = vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;

            f32 scale = mag * jmag;

            data.p.x *= scale;
            data.p.y *= scale;
            data.p.z *= scale;
            (*obj_update)(obj, 50 + i, &data);
        }
        (*obj_update)(obj, 55, NULL);
    }
}

//8037BD84
static void resolveLimits(HSD_RObj* robj, void* obj)
{
    HSD_JObj* jobj = (HSD_JObj*)obj;
    HSD_RObj* ro = robj;
    BOOL dirty = FALSE;

    assert(jobj != NULL);

    while (ro != NULL && (ro->flags & 0x70000000) != 0x20000000) {
        ro = ro->next;
    }

    if (ro != NULL) {
        while (robj != NULL) {
            if ((robj->flags & 0x70000000) == 0x20000000) {
                switch (robj->flags & 0xFFFFFFF) {
                case 1:
                    if (jobj->rotation.x < robj->u.limit) {
                        jobj->rotation.x = robj->u.limit;
                    }
                    dirty = TRUE;
                    break;

                case 2:
                    if (robj->u.limit < jobj->rotation.x) {
                        (jobj->rotation).x = robj->u.limit;
                    }
                    dirty = TRUE;
                    break;

                case 3:
                    if (jobj->rotation.y < robj->u.limit) {
                        jobj->rotation.y = robj->u.limit;
                    }
                    dirty = TRUE;
                    break;

                case 4:
                    if (robj->u.limit < jobj->rotation.y) {
                        jobj->rotation.y = robj->u.limit;
                    }
                    dirty = TRUE;
                    break;

                case 5:
                    if (jobj->rotation.z < robj->u.limit) {
                        jobj->rotation.z = robj->u.limit;
                    }
                    dirty = TRUE;
                    break;

                case 6:
                    if (robj->u.limit < jobj->rotation.z) {
                        jobj->rotation.z = robj->u.limit;
                    }
                    dirty = TRUE;
                    break;

                case 7:
                    if (jobj->position.x < robj->u.limit) {
                        jobj->position.x = robj->u.limit;
                    }
                    dirty = TRUE;
                    break;

                case 8:
                    if (robj->u.limit < jobj->position.x) {
                        jobj->position.x = robj->u.limit;
                    }
                    dirty = TRUE;
                    break;

                case 9:
                    if (jobj->position.y < robj->u.limit) {
                        jobj->position.y = robj->u.limit;
                    }
                    dirty = TRUE;
                    break;

                case 10:
                    if (robj->u.limit < jobj->position.y) {
                        jobj->position.y = robj->u.limit;
                    }
                    dirty = TRUE;
                    break;

                case 11:
                    if (jobj->position.z < robj->u.limit) {
                        jobj->position.z = robj->u.limit;
                    }
                    dirty = TRUE;
                    break;

                case 12:
                    if (robj->u.limit < jobj->position.z) {
                        jobj->position.z = robj->u.limit;
                    }
                    dirty = TRUE;
                    break;
                }
            }
            robj = robj->next;
        }
        if (dirty) {
            HSD_JObjMakeMatrix(jobj);
        }
    }
}

//8037BFB0
void HSD_RObjUpdateAll(HSD_RObj* robj, void* obj, void (*obj_update)(void*, u32, FObjData*))
{
    if (robj != NULL) {
        resolveCnsPos(robj, obj, obj_update);
        resolveCnsDirUp(robj, obj, obj_update);
        resolveCnsOrientation(robj, obj, obj_update);
        resolveLimits(robj, obj);
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
                HSD_JObjRefThis(ro->u.jobj);
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
    HSD_RObj* robj = NULL;

    if (desc == NULL) {
        return NULL;
    } else {
        robj = HSD_RObjAlloc();
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
            return robj;
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
    }
    return robj;
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
    memset(robj, 0, sizeof(HSD_RObj));
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
    for (HSD_Rvalue* next = rvalue; next != NULL; next = rvalue) {
        rvalue = next->next;
        HSD_JObjUnrefThis(next->jobj);
        HSD_ObjFree(&rvalue_alloc_data, (HSD_ObjAllocLink*)next);
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
