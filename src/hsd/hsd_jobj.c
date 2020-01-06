#include "hsd_jobj.h"

#include <math.h>

#include "hsd_debug.h"
#include "hsd_display.h"
#include "hsd_memory.h"

static void JObjInfoInit(void);

HSD_JObjInfo hsdJObj = { JObjInfoInit };

static HSD_JObj* current_jobj = NULL; // r13_400C

static void (*ptcltgt_callback)(HSD_JObj*, s32) = NULL;
static void (*jsound_callback)(s32) = NULL; // r13_4014
static void (*dptcl_callback)(s32, s32, s32, HSD_JObj*) = NULL; // r13_4018
static HSD_SList* ufc_callbacks = NULL; // r13_401C
typedef void ufc_callback(HSD_JObj*, u32, f32);

static HSD_JObjInfo* default_class = NULL; // r13_4020

// 8036EC10
void HSD_JObjCheckDepend(HSD_JObj* jobj)
{
    if (jobj != NULL) {
        u32 flags = jobj->flags;
        if ((flags & USER_DEF_MTX) != 0 && (flags & MTX_DIRTY) == 0) {
            if ((flags & USER_DEF_MTX) == 0) {
                flags = flags & EFFECTOR;
                if ((jobj->prev != NULL && (jobj->prev->flags & MTX_DIRTY) != 0) || flags == JOINT1 || flags == JOINT2 || flags == EFFECTOR || jobj->robj != NULL) {
                    jobj->flags |= MTX_DIRTY;
                }
            } else {
                if ((flags & MTX_INDEP_PARENT) == 0) {
                    HSD_JObj* prev = jobj->prev;
                    if (prev != NULL) {
                        if ((prev->flags & USER_DEF_MTX) != 0 && (prev->flags & MTX_DIRTY) == 0) {
                            jobj->flags |= MTX_DIRTY;
                        }
                    }
                }
            }
        }
    }
}

// 8036ED3C
BOOL HSD_JObjMtxIsDirty(HSD_JObj* jobj)
{
    HSD_CheckAssert("HSD_JObjMtxIsDirty: jobj == NULL", jobj != NULL);
    if ((jobj->flags & USER_DEF_MTX) == 0 && (jobj->flags & MTX_DIRTY) != 0) {
        return TRUE;
    }
    return FALSE;
}

// 8036EE10
void HSD_JObjSetMtxDirty(HSD_JObj* jobj)
{
    if (jobj != NULL && HSD_JObjMtxIsDirty(jobj) == 0) {
        HSD_JObjSetMtxDirtySub(jobj);
    }
}

// 8036EFAC
static void HSD_JObjWalkTree0(HSD_JObj* jobj,
    void (*cb)(HSD_JObj*, void*, u32),
    void* args)
{
    if (jobj != NULL) {
        assert(jobj->prev != NULL);
        u32 type = 0;
        if (jobj->prev->child == jobj) {
            type = 1;
        } else {
            type = 2;
        }
        if (cb != NULL) {
            (*cb)(jobj, args, type);
        }
        if (JOBJ_INSTANCE(jobj)) {
            for (HSD_JObj* i = jobj->child; i != NULL; i = i->next) {
                assert(i->prev != NULL);
                type = 0;
                if (i->prev->child == i) {
                    type = 1;
                } else {
                    type = 2;
                }
                if (cb != NULL) {
                    (*cb)(i, args, type);
                }
                if (JOBJ_INSTANCE(i)) {
                    for (HSD_JObj* n = i->child; n != NULL; n = n->next) {
                        HSD_JObjWalkTree0(n, cb, args);
                    }
                }
            }
        }
    }
}

// 8036F0F0
void HSD_JObjWalkTree(HSD_JObj* jobj,
    void (*cb)(HSD_JObj*, void*, u32),
    void* args)
{
    if (jobj != NULL) {
        if (cb != NULL) {
            (*cb)(jobj, args, 0);
        }
        if (JOBJ_INSTANCE(jobj)) {
            for (HSD_JObj* i = jobj->child; i != NULL; i = i->next) {
                assert(i->prev != NULL);
                u32 type = 0;
                if (i->prev->child == i) {
                    type = 1;
                } else {
                    type = 2;
                }
                if (cb != NULL) {
                    (*cb)(i, args, type);
                }
                if (JOBJ_INSTANCE(i)) {
                    for (HSD_JObj* n = i->child; n != NULL; n = n->next) {
                        HSD_JObjWalkTree0(n, cb, args);
                    }
                }
            }
        }
    }
}

// 8036F1F8
void HSD_JObjMakeMatrix(HSD_JObj* jobj)
{
    HSD_JObj* parent = jobj->prev;
    HSD_JObjSetupMatrix(parent);
    if ((jobj->flags & CLASSICAL_SCALE) == 0) {
        if (jobj->pvec == NULL) {
            jobj->pvec = (guVector*)HSD_MemAlloc(sizeof(guVector));
        }
        if (parent == NULL || parent->pvec == NULL) {
            jobj->pvec->x = jobj->scale.x;
            jobj->pvec->y = jobj->scale.y;
            jobj->pvec->z = jobj->scale.z;
        } else {
            jobj->pvec->x = jobj->scale.x * parent->pvec->x;
            jobj->pvec->y = jobj->scale.y * parent->pvec->y;
            jobj->pvec->z = jobj->scale.z * parent->pvec->z;
        }
    } else {
        if (parent == NULL || parent->pvec == NULL) {
            if (jobj->pvec != NULL) {
                HSD_Free(jobj->pvec);
                jobj->pvec = NULL;
            }
        } else {
            if (jobj->pvec == NULL) {
                jobj->pvec = (guVector*)HSD_MemAlloc(sizeof(guVector));
            }
            jobj->pvec->x = parent->pvec->x;
            jobj->pvec->y = parent->pvec->y;
            jobj->pvec->z = parent->pvec->z;
        }
    }
    guVector* vec = NULL;
    if (parent != NULL && parent->pvec != NULL) {
        vec = parent->pvec;
    }

    if (JOBJ_USE_QUATERNION(jobj) != 0) {
        HSD_MtxSRTQuat(jobj->mtx, &jobj->scale, &jobj->rotation, &jobj->position, vec);
    } else {
        HSD_MtxSRT(jobj->mtx, &jobj->scale, (guVector*)&jobj->rotation, &jobj->position, vec);
    }

    if (parent != NULL) {
        guMtxConcat(parent->mtx, jobj->mtx, jobj->mtx);
    }

    if (jobj->aobj != NULL) {
        HSD_JObj* aj = (HSD_JObj*)jobj->aobj->hsd_obj;
        if (aj != NULL) {
            HSD_JObjSetupMatrix(parent);
            guVector rvec;
            guVecMultiply(aj->mtx, &jobj->position, &rvec);
            guMtxRowCol(jobj->mtx, 0, 3) = rvec.x;
            guMtxRowCol(jobj->mtx, 1, 3) = rvec.y;
            guMtxRowCol(jobj->mtx, 2, 3) = rvec.z;
        }
    }
}

// 8036F4C8
void HSD_JObjRemoveAnimByFlags(HSD_JObj* jobj, u32 flags)
{
    if (jobj != NULL) {
        if ((flags & SKELETON) != 0) {
            HSD_AObjRemove(jobj->aobj);
            jobj->aobj = NULL;
        }
        if (union_type_dobj(jobj)) {
            HSD_DObjRemoveAnimAllByFlags(jobj->u.dobj, flags);
        }
        HSD_RObjRemoveAnimAllByFlags(jobj->robj, flags);
    }
}

// 8036F550
void HSD_JObjRemoveAnimAllByFlags(HSD_JObj* jobj, u32 flags)
{
    if (jobj != NULL) {
        HSD_JObjRemoveAnimByFlags(jobj, flags);

        if (JOBJ_INSTANCE(jobj)) {
            for (HSD_JObj* i = jobj->child; i != NULL; i = i->next) {
                HSD_JObjRemoveAnimAllByFlags(i, flags);
            }
        }
    }
}

// 8036F644
void HSD_JObjRemoveAnim(HSD_JObj* jobj)
{
    if (jobj != NULL) {
        HSD_AObjRemove(jobj->aobj);
        jobj->aobj = NULL;
        if (union_type_dobj(jobj)) {
            HSD_DObjRemoveAnimAllByFlags(jobj->u.dobj, 0x7FF);
        }
        HSD_RObjRemoveAnimAllByFlags(jobj->robj, 0x7FF);
    }
}

// 8036F6B4
void HSD_JObjRemoveAnimAll(HSD_JObj* jobj)
{
    if (jobj != NULL) {
        HSD_JObjRemoveAnimByFlags(jobj, 0x7FF);
        if (JOBJ_INSTANCE(jobj)) {
            for (HSD_JObj* i = jobj->child; i != NULL; i = i->next) {
                HSD_JObjRemoveAnimAllByFlags(jobj, 0x7FF);
            }
        }
    }
}

// 8036F718
void HSD_JObjReqAnimByFlags(HSD_JObj* jobj, u32 flags, f32 frame)
{
    if (jobj != NULL) {
        if ((flags & SKELETON) != 0) {
            HSD_AObjReqAnim(jobj->aobj, frame);
        }
        if (union_type_dobj(jobj)) {
            HSD_DObjReqAnimAllByFlags(jobj->u.dobj, flags, frame);
        }
        HSD_RObjReqAnimAllByFlags(jobj->robj, flags, frame);
    }
}

// 8036F7B0
void HSD_JObjReqAnimAllByFlags(HSD_JObj* jobj, u32 flags, f32 frame)
{
    if (jobj != NULL) {
        HSD_JObjReqAnimByFlags(jobj, flags, frame);

        if (JOBJ_INSTANCE(jobj)) {
            for (HSD_JObj* i = jobj->child; i != NULL; i = i->next) {
                HSD_JObjReqAnimAllByFlags(i, flags, frame);
            }
        }
    }
}

// 8036F8BC
void HSD_JObjReqAnimAll(HSD_JObj* jobj, f32 frame)
{
    if (jobj != NULL) {
        HSD_JObjReqAnimByFlags(jobj, 0x7FF, frame);
        if (JOBJ_INSTANCE(jobj)) {
            for (HSD_JObj* i = jobj->child; i != NULL; i = i->next) {
                HSD_JObjReqAnimAllByFlags(i, 0x7FF, frame);
            }
        }
    }
}

// 8036F934
void HSD_JObjReqAnim(HSD_JObj* jobj, f32 frame)
{
    if (jobj != NULL) {
        HSD_AObjReqAnim(jobj->aobj, frame);
        if (union_type_dobj(jobj)) {
            HSD_DObjReqAnimAllByFlags(jobj->u.dobj, 0x7FF, frame);
        }
        HSD_RObjReqAnimAllByFlags(jobj->robj, 0x7FF, frame);
    }
}

// 8036F9B8
static void JObjSortAnim(HSD_AObj* aobj)
{
    if (aobj != NULL && aobj->fobj != NULL) {
        HSD_FObj* i = aobj->fobj;
        HSD_FObj* j = NULL;
        do {
            j = i;
            i = j->next;
            if (i == NULL) {
                return;
            }
        } while (i->obj_type != TYPE_JOBJ);
        j->next = i->next;
        i->next = aobj->fobj;
        aobj->fobj = i;
    }
}

// 8036FA10
void HSD_JObjAddAnim(HSD_JObj* jobj,
    HSD_AnimJoint* an_joint,
    HSD_MatAnimJoint* mat_joint,
    HSD_ShapeAnimJoint* sh_joint)
{
    if (jobj != NULL) {
        if (an_joint != NULL) {
            if (jobj->aobj != NULL) {
                HSD_AObjRemove(jobj->aobj);
            }
            jobj->aobj = HSD_AObjLoadDesc(an_joint->aobjdesc);
            JObjSortAnim(jobj->aobj);
            HSD_RObjAddAnimAll(jobj->robj, an_joint->robj_anim);
            if ((an_joint->flags & JOBJ_ANIM_CLASSICAL_SCALING) == 0) {
                HSD_JObjClearFlags(jobj, CLASSICAL_SCALE);
            } else {
                HSD_JObjSetFlags(jobj, CLASSICAL_SCALE);
            }
        }
        if (union_type_dobj(jobj)) {
            HSD_ShapeAnimDObj* sh_anim = NULL;
            HSD_MatAnim* mat_anim = NULL;
            if (sh_joint != NULL) {
                sh_anim = sh_joint->shapeanimdobj;
            }
            if (mat_joint != NULL) {
                mat_anim = mat_joint->matanim;
            }
            HSD_DObjAddAnimAll(jobj->u.dobj, mat_anim, sh_anim);
        }
    }
}

// 8036FB5C
void HSD_JObjAddAnimAll(HSD_JObj* jobj,
    HSD_AnimJoint* an_joint,
    HSD_MatAnimJoint* mat_joint,
    HSD_ShapeAnimJoint* sh_joint)
{
    if (jobj != NULL) {
        HSD_JObjAddAnim(jobj, an_joint, mat_joint, sh_joint);
        if (JOBJ_INSTANCE(jobj)) {
            HSD_JObj* i = jobj->child;
            if (an_joint != NULL) {
                an_joint = an_joint->child;
            }
            if (mat_joint != NULL) {
                mat_joint = mat_joint->child;
            }
            if (sh_joint != NULL) {
                sh_joint = sh_joint->child;
            }
            while (i != NULL) {
                HSD_JObjAddAnimAll(i, an_joint, mat_joint, sh_joint);
                i = i->next;
                if (an_joint != NULL) {
                    an_joint = an_joint->next;
                }
                if (mat_joint != NULL) {
                    mat_joint = mat_joint->next;
                }
                if (sh_joint != NULL) {
                    sh_joint = sh_joint->next;
                }
            }
        }
    }
}

// 8036FDC0
void JObjUpdateFunc(void* obj, u32 type, FObjData* val)
{
    HSD_JObj* jobj = (HSD_JObj*)obj;
    if (jobj != NULL) {
        Mtx mtx;
        HSD_SList* callbacks = ufc_callbacks;

        switch (type) {
        case 1:
            if ((jobj->flags & JOINT1) != 0) {
                HSD_RObj* robj = HSD_RObjGetByType(jobj->robj, 0x40000000, 0);
                if (robj != NULL) {
                    robj->u.ik_hint.rotate_x = val->fv;
                }
            }
            assert(JOBJ_USE_QUATERNION(jobj) == 0);
            jobj->rotation.x = val->fv;
            if ((jobj->flags & MTX_INDEP_SRT) == 0) {
                HSD_JObjSetMtxDirty(jobj);
            }
            break;
        case 2:
            assert(JOBJ_USE_QUATERNION(jobj) == 0);
            jobj->rotation.y = val->fv;
            if ((jobj->flags & MTX_INDEP_SRT) == 0) {
                HSD_JObjSetMtxDirty(jobj);
            }
            break;
        case 3:
            assert(JOBJ_USE_QUATERNION(jobj) == 0);
            jobj->rotation.z = val->fv;
            if ((jobj->flags & MTX_INDEP_SRT) == 0) {
                HSD_JObjSetMtxDirty(jobj);
            }
            break;
        case 4:
            if (val->fv < 0.0F) {
                val->fv = 0.f;
            }
            if (1.0F < val->fv) {
                val->fv = 1.f;
            }
            assert(jobj->aobj != NULL);
            HSD_JObj* jp = (HSD_JObj*)jobj->aobj->hsd_obj;
            assert(jp->u.spline != NULL);
            guVector result;
            splArcLengthPoint(jp->u.spline, val->fv, &result);
            jobj->position = result;
            if ((jobj->flags & MTX_INDEP_SRT) == 0) {
                HSD_JObjSetMtxDirty(jobj);
            }
            break;
        case 5:
            jobj->position.x = val->fv;
            if ((jobj->flags & MTX_INDEP_SRT) == 0) {
                HSD_JObjSetMtxDirty(jobj);
            }
            break;
        case 6:
            jobj->position.y = val->fv;
            if ((jobj->flags & MTX_INDEP_SRT) == 0) {
                HSD_JObjSetMtxDirty(jobj);
            }
            break;
        case 7:
            jobj->position.z = val->fv;
            if ((jobj->flags & MTX_INDEP_SRT) == 0) {
                HSD_JObjSetMtxDirty(jobj);
            }
            break;
        case 8:
            if ((f32)((u32)val->fv & 0x7FFFFFFF) < 0.001f) {
                val->fv = 0.001f;
            }
            jobj->scale.x = val->fv;
            if ((jobj->flags & MTX_INDEP_SRT) == 0) {
                HSD_JObjSetMtxDirty(jobj);
            }
            break;
        case 9:
            if ((f32)((u32)val->fv & 0x7FFFFFFF) < 0.001f) {
                val->fv = 0.001f;
            }
            jobj->scale.y = val->fv;
            if ((jobj->flags & MTX_INDEP_SRT) == 0) {
                HSD_JObjSetMtxDirty(jobj);
            }
            break;
        case 10:
            if ((f32)((u32)val->fv & 0x7FFFFFFF) < 0.001f) {
                val->fv = 0.001f;
            }
            jobj->scale.z = val->fv;
            if ((jobj->flags & MTX_INDEP_SRT) == 0) {
                HSD_JObjSetMtxDirty(jobj);
            }
            break;
        case 11:
            if (val->fv <= 0.5) {
                HSD_JObjSetFlags(jobj, HIDDEN);
            } else {
                HSD_JObjClearFlags(jobj, HIDDEN);
            }
            break;
        case 12:
            if (val->fv <= 0.5) {
                HSD_JObjSetFlagsAll(jobj, HIDDEN);
            } else {
                HSD_JObjClearFlagsAll(jobj, HIDDEN);
            }
            break;
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
            while(callbacks != NULL){
                (*(ufc_callback*)callbacks->data)(jobj, type, val->iv);
                callbacks = callbacks->next;
            }
            break;
        case 30:
        case 31:
        case 32:
        case 33:
        case 34:
        case 35:
        case 36:
        case 37:
        case 38:
        case 39:
            while (callbacks != NULL) {
                ufc_callback* callback = callbacks->data;
                (callback)(jobj, type, val->fv);
                callbacks = callbacks->next;
            }
            break;
        case 40:
            if (dptcl_callback != NULL) {
                (*dptcl_callback)(0, val->iv & 0x3f, val->iv >> 6 & 0xffffff, jobj); // 8005DB70 during gameplay
            }
            break;
        case 41:
            if (jsound_callback != NULL) {
                (*jsound_callback)(val->iv);
            }
            break;
        case 42:
            if (ptcltgt_callback != NULL) {
                (*ptcltgt_callback)(jobj, val->iv);
            }
            break;
        case 50:
            guMtxRowCol(jobj->mtx, 0, 0) = val->p.x;
            guMtxRowCol(jobj->mtx, 1, 0) = val->p.y;
            guMtxRowCol(jobj->mtx, 2, 0) = val->p.z;
            break;
        case 51:
            guMtxRowCol(jobj->mtx, 0, 1) = val->p.x;
            guMtxRowCol(jobj->mtx, 1, 1) = val->p.y;
            guMtxRowCol(jobj->mtx, 2, 1) = val->p.z;
            break;
        case 52:
            guMtxRowCol(jobj->mtx, 0, 2) = val->p.x;
            guMtxRowCol(jobj->mtx, 1, 2) = val->p.y;
            guMtxRowCol(jobj->mtx, 2, 2) = val->p.z;
            break;
        case 53:
            guMtxRowCol(jobj->mtx, 0, 3) = val->p.x;
            guMtxRowCol(jobj->mtx, 1, 3) = val->p.y;
            guMtxRowCol(jobj->mtx, 2, 3) = val->p.z;
            break;
        case 54:
        case 55:
        case 56:
        case 57:
            if (jobj->prev == NULL) {
                guMtxCopy(jobj->mtx, mtx);
            } else {
                HSD_MtxInverseConcat(jobj->prev->mtx, jobj->mtx, mtx);
            }
            if (type == 54 || type == 56) {
                // HSD_MtxGetTranslate(mtx, &jobj->position);
                jobj->position.x = guMtxRowCol(mtx, 0, 3);
                jobj->position.y = guMtxRowCol(mtx, 1, 3);
                jobj->position.z = guMtxRowCol(mtx, 2, 3);
            }
            if ((type - 54) < 2) {
                HSD_MtxGetRotation(mtx, (guVector*)&jobj->rotation);
            }
            if ((type == 54) || type == 57) {
                // HSD_MtxGetScale(mtx, &jobj->scale);
                guMtxScale(mtx, jobj->scale.x, jobj->scale.y, jobj->scale.z);
            }
            break;
        }
    }
}

// 80370780
void HSD_JObjAnim(HSD_JObj* jobj)
{
    if (jobj != NULL) {
        HSD_JObjCheckDepend(jobj);
        HSD_AObjInterpretAnim(jobj->aobj, jobj, JObjUpdateFunc);
        HSD_RObjAnimAll(jobj->robj);
        if (union_type_dobj(jobj)) {
            HSD_DObjAnimAll(jobj->u.dobj);
        }
    }
}

// 803707F8
static void JObjAnimAll(HSD_JObj* jobj)
{
    if (jobj != NULL) {
        HSD_JObjAnim(jobj);
        if (JOBJ_INSTANCE(jobj)) {
            for (HSD_JObj* i = jobj->child; i != NULL; i = i->next) {
                JObjAnimAll(i);
            }
        }
    }
}

// 80370928
void HSD_JObjAnimAll(HSD_JObj* jobj)
{
    if (jobj != NULL) {
        HSD_AObjInitEndCallBack();
        JObjAnimAll(jobj);
        HSD_AObjInvokeCallBacks();
    }
}

static void setupInstanceMtx(HSD_JObj* jobj, MtxP vmtx, Mtx mtx)
{
    HSD_JObjSetupMatrix(jobj);

    HSD_JObj* child = jobj->child;
    HSD_JObjSetupMatrix(child);

    guMtxInverse(child->mtx, mtx);
    guMtxConcat(jobj->mtx, mtx, mtx);
    if (vmtx == NULL) {
        HSD_CObj* cobj = HSD_CObjGetCurrent();
        assert(cobj != NULL);
        vmtx = HSD_CObjGetViewingMtxPtrDirect(cobj);
        guMtxConcat(vmtx, mtx, mtx);
    } else {
        guMtxConcat(vmtx, mtx, mtx);
    }
}

// 803709DC
void HSD_JObjDispAll(HSD_JObj* jobj, MtxP vmtx, u32 flags, u32 rendermode)
{
    if (jobj != NULL) {
        if (JOBJ_INSTANCE(jobj)) {
            if ((jobj->flags & flags << 18) != 0) {
                HSD_JObjDisp(jobj, vmtx, flags, rendermode);
            }
            if ((jobj->flags & flags << 28) != 0) {
                for (HSD_JObj* child = jobj->child; child != NULL; child = child->next) {
                    HSD_JObjDispAll(child, vmtx, flags, rendermode);
                }
            }
        } else {
            if ((jobj->flags & HIDDEN) == 0) {
                Mtx mtx;
                setupInstanceMtx(jobj, vmtx, mtx);
                HSD_JObjDispAll(jobj->child, mtx, flags, rendermode);
            }
        }
    }
}

// 80370B90
void HSD_JObjSetDefaultClass(HSD_JObjInfo* info)
{
    if (info != NULL) {
        assert(hsdIsDescendantOf((HSD_ClassInfo*)info, (HSD_ClassInfo*)&hsdJObj));
    }
    default_class = info;
}

static HSD_JObj* JObjLoadJointSub(HSD_JObjDesc* joint, HSD_JObj* parent)
{
    HSD_JObj* child = NULL;
    if (joint != NULL) {
        child = (HSD_JObj*)HSD_JObjAlloc();
        HSD_JOBJ_METHOD(child)->load(child, joint, parent);
    }
    return child;
}

// 80370BEC
static s32 JObjLoad(HSD_JObj* jobj, HSD_JObjDesc* desc, HSD_JObj* prev)
{
    if (JOBJ_INSTANCE(desc)) {
        jobj->child = JObjLoadJointSub(desc->child, jobj);
    }

    jobj->next = JObjLoadJointSub(desc->next, prev);
    jobj->prev = prev;
    jobj->flags |= desc->flags;
    if ((jobj->flags & SPLINE) == 0) {
        if ((jobj->flags & PTCL) == 0) {
            jobj->u.dobj = HSD_DObjLoadDesc(desc->u.dobjdesc);
        } else {
            jobj->u.ptcl = desc->u.ptcl;
            for (HSD_SList* slist = desc->u.ptcl; slist != NULL; slist = slist->next) {
                slist->data = (void*)((u32)slist->data | 0x80000000);
            }
        }
    } else {
        jobj->u.spline = desc->u.spline;
    }
    jobj->robj = HSD_RObjLoadDesc(desc->robjdesc);

    jobj->rotation.x = desc->rotation.x;
    jobj->rotation.y = desc->rotation.y;
    jobj->rotation.z = desc->rotation.z;

    jobj->scale.x = desc->scale.x;
    jobj->scale.y = desc->scale.y;
    jobj->scale.z = desc->scale.z;

    jobj->position.x = desc->position.x;
    jobj->position.y = desc->position.y;
    jobj->position.z = desc->position.z;

    guMtxIdentity(jobj->mtx);
    jobj->pvec = NULL;
    if (desc->mtx != NULL) {
        jobj->vmtx = (MtxP)HSD_MemAlloc(sizeof(Mtx));
        memcpy(jobj->vmtx, desc->mtx, sizeof(Mtx));
    }
    HSD_IDInsertToTable(NULL, (u32)desc, jobj);
    jobj->desc = desc;

    return 0;
}

// 80370E44
HSD_JObj* HSD_JObjLoadJoint(HSD_JObjDesc* desc)
{
    HSD_JObj* jobj = JObjLoadJointSub(desc, NULL);
    HSD_JObjResolveRefsAll(jobj, desc);
    return jobj;
}

// 80370EEC
void HSD_JObjResolveRefs(HSD_JObj* jobj, HSD_JObjDesc* desc)
{
    if (jobj != NULL && desc != NULL) {
        HSD_RObjResolveRefsAll(jobj->robj, desc->robjdesc);
        if (!JOBJ_INSTANCE(jobj)) {
            HSD_JObjUnref(jobj->child);
            HSD_JObj* child = (HSD_JObj*)HSD_IDGetDataFromTable(NULL, (u32)desc->child, NULL);
            jobj->child = child;
            HSD_CheckAssert("JObjResolveRefs child == NULL", jobj->child != NULL);
            HSD_JObjRef(jobj->child);
        }
        if (union_type_dobj(jobj)) {
            HSD_DObjResolveRefsAll(jobj->u.dobj, desc->u.dobjdesc);
        }
    }
}

// 80370FF4
void HSD_JObjResolveRefsAll(HSD_JObj* jobj, HSD_JObjDesc* desc)
{
    while (jobj != NULL && desc != NULL) {
        HSD_JObjResolveRefs(jobj, desc);
        if (JOBJ_INSTANCE(jobj)) {
            HSD_JObjResolveRefsAll(jobj->child, desc->child);
        }
        jobj = jobj->next;
        desc = desc->next;
    }
}

void HSD_JObjRef(HSD_JObj* jobj){
    if (jobj != NULL) {
        ref_INC(&jobj->parent);
    }
}

void HSD_JObjRefThis(HSD_JObj* jobj)
{
    if (jobj != NULL) {
        iref_INC(&jobj->parent);
    }
}

// 8037115C
void HSD_JObjUnref(HSD_JObj* jobj)
{
    if (jobj != NULL) {
        if (ref_DEC(&jobj->parent) != 0) {
            if (iref_CNT(&jobj->parent) < 0) {
                HSD_OBJECT_METHOD(jobj)->release((HSD_Class*)jobj);
                HSD_OBJECT_METHOD(jobj)->destroy((HSD_Class*)jobj);
            } else {
                iref_INC(&jobj->parent);
                HSD_JOBJ_METHOD(jobj)->release_child(jobj);
                if (iref_DEC(&jobj->parent) != 0) {
                    HSD_OBJECT_METHOD(jobj)->release((HSD_Class*)jobj);
                    HSD_OBJECT_METHOD(jobj)->destroy((HSD_Class*)jobj);
                }
            }
        }
    }
}

// 803712C0
void HSD_JObjUnrefThis(HSD_JObj* jobj)
{
    if (jobj != NULL) {
        if (iref_DEC(&jobj->parent) != 0) {
            if(ref_CNT(&jobj->parent) < 0){
                HSD_OBJECT_METHOD(jobj)->release((HSD_Class*)jobj);
                HSD_OBJECT_METHOD(jobj)->destroy((HSD_Class*)jobj);
            }
        }
    }
}

// 80371370
void HSD_JObjRemove(HSD_JObj* jobj)
{
    if (jobj != NULL) {
        HSD_JObj* child = jobj->child;
        if (child != NULL && child->next != NULL) {
            HSD_Halt("child->next == NULL");
        }
        HSD_JObj* next = child;
        if (child == NULL) {
            next = jobj->next;
        }
        HSD_JObj* prev = HSD_JObjGetPrev(jobj);
        if (prev == NULL) {
            if (jobj->prev != NULL) {
                jobj->prev->child = next;
            }
        } else {
            prev->next = next;
        }
        if (next != NULL && next == child) {
            next->next = jobj->next;
            next->prev = jobj->prev;
        }
        jobj->prev = NULL;
        jobj->child = NULL;
        jobj->next = NULL;

        HSD_JObjUnref(jobj);
    }
}

// 80371590
void HSD_JObjRemoveAll(HSD_JObj* jobj)
{
    if (jobj != NULL) {
        if (jobj->prev) {
            HSD_JObj* prev = HSD_JObjGetPrev(jobj);
            if (prev != NULL) {
                prev->next = NULL;
            } else {
                jobj->prev->next = NULL;
            }
        }

        while (jobj != NULL) {
            HSD_JObj* next = jobj->next;
            jobj->prev = NULL;
            jobj->next = NULL;
            HSD_JObjUnref(jobj);
            jobj = next;
        }
    }
}

// 80371750
void RecalcParentTrspBits(HSD_JObj* jobj)
{
    if (jobj != NULL) {
        for (HSD_JObj* i = jobj; i != NULL; i = i->next) {
            u32 mask = 0x8FFFFFFF;
            for (HSD_JObj* child = jobj->child; child != NULL; child = child->next) {
                mask |= (child->flags | (child->flags << 10)) & (ROOT_OPA | ROOT_TEXEDGE | ROOT_XLU);
            }
            if ((i->flags & ~mask) == 0)
                break;
            i->flags = i->flags & mask;
        }
    }
}

static void UpdateParentTrspBits(HSD_JObj* jobj, HSD_JObj* child)
{
    u32 flags = (child->flags | (child->flags << 10)) & (ROOT_OPA | ROOT_TEXEDGE | ROOT_XLU);
    while (jobj != NULL && (flags & ~jobj->flags) != 0) {
        jobj->flags |= flags;
        jobj = jobj->prev;
    }
}

// 803717A8
void HSD_JObjAddChild(HSD_JObj* jobj, HSD_JObj* child)
{
    if (jobj != NULL && child != NULL) {
        HSD_CheckAssert("HSD_JObjAddChild: child should be an orphan", child->prev == NULL);
        HSD_CheckAssert("HSD_JObjAddChild: child should not have siblings", child->next == NULL);
        if (jobj->child != NULL) {
            HSD_JObj* i;
            for (i = jobj->child; i->next != NULL; i = i->next) {
                HSD_CheckAssert("HSD_JObjAddChild: last == child", i != child);
            }
            i->next = child;
        } else {
            jobj->child = child;
        }
        child->prev = jobj;
        UpdateParentTrspBits(jobj, child);
    }
}

// 803718F4
void HSD_JObjReparent(HSD_JObj* jobj, HSD_JObj* pjobj)
{
    if (jobj != NULL) {
        if (jobj->prev != NULL) {
            HSD_JObj** prchild = &jobj->prev->child;
            if (*prchild == jobj) {
                *prchild = jobj->next;
            } else {
                HSD_JObj* prev = HSD_JObjGetPrev(jobj);
                assert(prev);
                prev->next = jobj->next;
            }

            RecalcParentTrspBits(jobj->prev);
            jobj->prev = NULL;
        }
        jobj->next = NULL;
        HSD_JObjAddChild(pjobj, jobj);
    }
}

// 80371A04
void HSD_JObjAddNext(HSD_JObj* jobj, HSD_JObj* next)
{
    HSD_JObj* curr;

    if (jobj != NULL && next != NULL) {
        curr = jobj;
        if (jobj->prev != NULL) {
            curr = jobj->prev->child;
            jobj->prev->child = NULL;
            jobj->flags = jobj->flags & 0x8FFFFFFF;
        }

        HSD_JObjReparent(next, jobj->prev);

        if (next->child != NULL) {
            for (HSD_JObj* i = next->child; i != NULL; i = i->next) {
                i->next = curr;
            }
        } else {
            next->child = curr;
        }

        while (curr != NULL) {
            curr->prev = next;
            UpdateParentTrspBits(next, curr);
            curr = curr->next;
        }
    }
}

// 80371B60
HSD_JObj* HSD_JObjGetPrev(HSD_JObj* jobj)
{
    if (jobj != NULL) {
        if (jobj->prev == NULL){
            return NULL;
        }

        if (jobj->prev->child == jobj){
            return NULL;
        }

        for (HSD_JObj* i = jobj->prev->child; i != NULL; i = i->next) {
            if (i->next == jobj) {
                return i;
            }
        }
    }
    return NULL;
}

// 80371BEC
HSD_DObj* HSD_JObjGetDObj(HSD_JObj* jobj)
{
    if (jobj != NULL && union_type_dobj(jobj)) {
        return jobj->u.dobj;
    }
    return NULL;
}

// 80371C24
void HSD_JObjAddDObj(HSD_JObj* jobj, HSD_DObj* dobj)
{
    if (jobj != NULL && dobj != NULL && union_type_dobj(jobj)) {
        dobj->next = jobj->u.dobj;
        jobj->u.dobj = dobj;
    }
}

// 80371C68
void HSD_JObjPrependRObj(HSD_JObj* jobj, HSD_RObj* robj)
{
    if (jobj != NULL && robj != NULL) {
        robj->next = jobj->robj;
        jobj->robj = robj;
    }
}

// 80371C98
void HSD_JObjDeleteRObj(HSD_JObj* jobj, HSD_RObj* robj)
{
    if (jobj != NULL && robj != NULL) {
        for (HSD_RObj** rp = &jobj->robj; *rp != NULL; rp = &(*rp)->next) {
            if (*rp == robj) {
                *rp = robj->next;
                robj->next = NULL;
                return;
            }
        }
    }
}

// 80371CE8
u32 HSD_JObjGetFlags(HSD_JObj* jobj)
{
    return (jobj != NULL) ? jobj->flags : 0;
}

// 80371D00
void HSD_JObjSetFlags(HSD_JObj* jobj, u32 flags)
{
    if (jobj != NULL) {
        if ((jobj->flags ^ flags) & CLASSICAL_SCALE) {
            HSD_JObjSetMtxDirty(jobj);
        }
        jobj->flags |= flags;
    }
}

// 80371D9C
void HSD_JObjSetFlagsAll(HSD_JObj* jobj, u32 flags)
{
    if (jobj != NULL) {
        HSD_JObjSetFlags(jobj, flags);

        if (JOBJ_INSTANCE(jobj)) {
            for (HSD_JObj* i = jobj->child; i != NULL; i = i->next) {
                HSD_JObjSetFlagsAll(i, flags);
            }
        }
    }
}

// 80371F00
void HSD_JObjClearFlags(HSD_JObj* jobj, u32 flags)
{
    if (jobj != NULL) {
        if ((jobj->flags ^ flags) & CLASSICAL_SCALE) {
            HSD_JObjSetMtxDirty(jobj);
        }
        jobj->flags &= ~flags;
    }
}

// 80371F9C
void HSD_JObjClearFlagsAll(HSD_JObj* jobj, u32 flags)
{
    if (jobj != NULL) {
        HSD_JObjClearFlags(jobj, flags);

        if (JOBJ_INSTANCE(jobj)) {
            for (HSD_JObj* i = jobj->child; i != NULL; i = i->next) {
                HSD_JObjClearFlagsAll(i, flags);
            }
        }
    }
}

// 8037210C
HSD_JObj* HSD_JObjAlloc(void)
{
    HSD_ClassInfo* info = (HSD_ClassInfo*)(default_class != NULL ? default_class : &hsdJObj);
    HSD_JObj* jobj = (HSD_JObj*)hsdNew(info);
    assert(jobj != NULL);
    return jobj;
}

// 80372168
void HSD_JObjSetCurrent(HSD_JObj* jobj)
{
    if (jobj != NULL) {
        HSD_JObjRef(jobj);
        HSD_JObjUnref(current_jobj);
        current_jobj = jobj;
    }
}

// 80372314
HSD_JObj* HSD_JObjGetCurrent(void)
{
    return current_jobj;
}

// 8037231C
static void resolveIKJoint1(HSD_JObj* jobj)
{
    HSD_JObj* child;
    bool has_flag = false;
    bool has_mtx = false;

    HSD_CheckAssert("resolveIKJoint1: jobj == NULL", jobj != NULL);

    for (child = jobj->child; child != NULL; child = child->next) {
        if ((child->flags & EFFECTOR) == JOINT2) {
            break;
        }
    }
    guVector* pvec = jobj->pvec;
    guVector vec = { 0.f, 0.f, 0.f };
    if (pvec != NULL) {
        vec.x = pvec->x;
        vec.y = pvec->y;
        vec.z = pvec->z;
    }
    HSD_RObj* robj = HSD_RObjGetByType(jobj->robj, REFTYPE_IKHINT, 0);
    assert(robj != NULL);

    f32 rotate_x = robj->u.ik_hint.rotate_x;
    f32 rx_scale = robj->u.ik_hint.bone_length * vec.x;

    HSD_JObj* child_c;
    f32 x_scale = 0.f;
    if (child == NULL) {
        child_c = jobj->child;
    } else {
        HSD_RObj* child_robj = HSD_RObjGetByType(child->robj, REFTYPE_IKHINT, 0);
        assert(child_robj != NULL);
        has_flag = (child_robj->flags & 4) != 0;
        x_scale = vec.x * child_robj->u.ik_hint.bone_length * child->scale.x;
        child_c = child->child;
    }

    for (; child_c != NULL; child_c = child_c->next) {
        if ((child_c->flags & EFFECTOR) == EFFECTOR) {
            break;
        }
    }
    assert(child_c != NULL);
    HSD_RObj* child_robj = HSD_RObjGetByType(child_c->robj, REFTYPE_JOBJ, 1);
    if (child_robj == NULL) {
        child_c = NULL;
    }

    if (child_c != NULL) {
        HSD_RObj* robj_t3 = HSD_RObjGetByType(jobj->robj, REFTYPE_JOBJ, 3);
        if (robj_t3 == NULL && jobj->robj != NULL) {
            HSD_RObjUpdateAll(jobj->robj, jobj, JObjUpdateFunc);
            if ((jobj->flags & USER_DEF_MTX) == 0 && (jobj->flags & MTX_DIRTY) != 0) {
                has_mtx = true;
                HSD_JOBJ_METHOD(jobj)->make_mtx(jobj);
                jobj->flags &= 0xFFFFFFBF;
            }
        }

        guVector trans = { 0.f, 0.f, 0.f };
        guVector ptran;
        guVector c_scale;
        guVector cvec;
        guVector z_scale;
        guVector zvec;

        if (jobj->prev != NULL) {
            trans.x = jobj->prev->mtx[0][3];
            trans.y = jobj->prev->mtx[1][3];
            trans.z = jobj->prev->mtx[2][3];
        }
        HSD_RObjGetGlobalPosition(child_c->robj, 1, &child_c->position);
        guVecSub(&child_c->position, &trans, &ptran);
        f32 dot = guVecDotProduct(&ptran, &ptran);
        if (dot <= 1.0e-8f) {
            dot = 0.f;
        } else {
            guVector trans_2 = trans;
            u32 res = HSD_RObjGetGlobalPosition(jobj->robj, 3, &trans_2);
            if (res == 0) {
                zvec.x = jobj->mtx[0][2];
                zvec.y = jobj->mtx[1][2];
                zvec.z = jobj->mtx[2][2];
                guVecCross(&zvec, &trans_2, &cvec);
                guVecCross(&trans_2, &cvec, &zvec);
            } else {
                guVecSub(&cvec, &trans, &cvec);
                Mtx mtx;
                if (rotate_x != 0) {
                    guMtxRotAxisRad(mtx, &trans_2, rotate_x);
                    guVecMultiply(mtx, &cvec, &cvec);
                }
                guVecCross(&trans_2, &cvec, &zvec);
                guVecCross(&zvec, &trans_2, &cvec);
            }

            f32 z_scalar = 1.0f / (1e-10f + guVecDotProduct(&zvec, &zvec));
            if (0.f < z_scalar) {
            }
            guVecScale(&zvec, &z_scale, z_scalar);

            f32 c_scalar = 1.0f / (1e-10f + guVecDotProduct(&cvec, &cvec));
            if (0.f < c_scalar) {
            }
            guVecScale(&cvec, &c_scale, c_scalar);

            f32 rx_squared = rx_scale * rx_scale;
            f32 x_squared = x_scale * x_scale;
            f32 n_val = 0.25f * (2.f * (rx_squared + x_squared) - dot) - (((rx_squared - x_squared) * (rx_squared - x_squared)) / dot);
            if (n_val < 0.f) {
                n_val = 0.f;
            }
        }

        guVector fvec;

        guVecScale(&fvec, &fvec, dot);
        guMtxRowCol(jobj->mtx, 0, 0) = fvec.x * vec.x;
        guMtxRowCol(jobj->mtx, 1, 0) = fvec.y * vec.x;
        guMtxRowCol(jobj->mtx, 2, 0) = fvec.z * vec.x;
        guVecCross(&z_scale, &fvec, &cvec);
        guMtxRowCol(jobj->mtx, 0, 1) = cvec.x * vec.y;
        guMtxRowCol(jobj->mtx, 1, 1) = cvec.y * vec.y;
        guMtxRowCol(jobj->mtx, 2, 1) = cvec.z * vec.y;
        guMtxRowCol(jobj->mtx, 0, 2) = z_scale.x * vec.z;
        guMtxRowCol(jobj->mtx, 1, 2) = z_scale.y * vec.z;
        guMtxRowCol(jobj->mtx, 2, 2) = z_scale.z * vec.z;
        guMtxRowCol(jobj->mtx, 0, 3) = trans.x;
        guMtxRowCol(jobj->mtx, 1, 3) = trans.y;
        guMtxRowCol(jobj->mtx, 2, 3) = trans.z;
    }
}

// 80372B08
static void resolveIKJoint2(HSD_JObj* jobj) {}

static void resolveIKEffector(HSD_JObj* jobj)
{
    HSD_JObj* prev = jobj->prev;
    if (prev != NULL) {
        HSD_RObj* robj = prev->robj;
        if (robj != NULL) {
            if (HSD_RObjGetByType(prev->robj, 0x40000000, 0) != 0) {
                guVector w_vec = { guMtxRowCol(prev->mtx, 0, 3), guMtxRowCol(prev->mtx, 1, 3), guMtxRowCol(prev->mtx, 2, 3) };
                guVector x_vec = { guMtxRowCol(prev->mtx, 0, 0), guMtxRowCol(prev->mtx, 1, 0), guMtxRowCol(prev->mtx, 2, 0) };
                f32 dot = guVecDotProduct(&x_vec, &x_vec);
                dot = sqrtf(1.f / (1e-10f + dot));
                guVecScale(&x_vec, &x_vec, dot);
                f32 val = 1.f;
                if (prev->pvec != NULL) {
                    val = prev->pvec->x;
                }
                guVecScale(&x_vec, &x_vec, robj->u.limit * val);
                guVector res;
                guVecAdd(&w_vec, &x_vec, &res);
                guMtxRowCol(jobj->mtx, 0, 3) = res.x;
                guMtxRowCol(jobj->mtx, 1, 3) = res.y;
                guMtxRowCol(jobj->mtx, 2, 3) = res.z;
            }
        }
    }
}

static void HSD_JObjUpdateRObj(HSD_JObj* jobj)
{
    HSD_RObj* robj = jobj->robj;
    if (robj != NULL) {
        HSD_RObjUpdateAll(robj, jobj, JObjUpdateFunc);
        if (HSD_JObjMtxIsDirty(jobj)) {
            HSD_JOBJ_METHOD(jobj)->make_mtx(jobj);
            jobj->flags &= 0xFFFFFFBF;
        }
    }
}

void HSD_JObjSetupMatrix(HSD_JObj* jobj)
{
    if (jobj != NULL && HSD_JObjMtxIsDirty(jobj)) {
        HSD_JObjSetupMatrixSub(jobj);
    }
}

// 80373078
void HSD_JObjSetupMatrixSub(HSD_JObj* jobj)
{
    HSD_JOBJ_METHOD(jobj)->make_mtx(jobj);
    jobj->flags &= 0xFFFFFFBF;
    if ((jobj->flags & USER_DEF_MTX) != 0) {
        return;
    }
    u32 flags = jobj->flags & EFFECTOR;
    switch (flags) {
    case JOINT1:
        resolveIKJoint1(jobj);
        break;

    case JOINT2:
        resolveIKJoint2(jobj);
        break;

    case EFFECTOR:
        resolveIKEffector(jobj);
        break;
    default:
        HSD_JObjUpdateRObj(jobj);
    }
    jobj->flags &= 0xFFFFFFBF;
    return;
}

MtxP HSD_JObjGetMtxPtr(HSD_JObj* jobj){
    assert(jobj != NULL);
    HSD_JObjSetupMatrix(jobj);
    return jobj->mtx;
}

// 803732E8
void HSD_JObjSetMtxDirtySub(HSD_JObj* jobj)
{
    jobj->flags |= 0x40;
    if (JOBJ_INSTANCE(jobj)) {
        for (HSD_JObj* j = jobj->child; j != NULL; j = j->next) {
            if ((j->flags & MTX_INDEP_PARENT) == 0 && HSD_JObjMtxIsDirty(j) == 0) {
                HSD_JObjSetMtxDirtySub(j);
            }
        }
    }
}

// 80373404
void HSD_JObjSetDPtclCallback(void (*cb)(s32, s32, s32, HSD_JObj*))
{
    dptcl_callback = cb;
}

// 8037340C
static void JObjInit(HSD_Class* o)
{
    HSD_OBJECT_PARENT_INFO(&hsdJObj)->init(o);

    if (o != NULL) {
        HSD_JObj* jobj = HSD_JOBJ(o);
        jobj->flags = MTX_DIRTY;
        jobj->scale.x = 1.0f;
        jobj->scale.y = 1.0f;
        jobj->scale.z = 1.0f;
    }
}

// 80373470
static void JObjReleaseChild(HSD_JObj* jobj)
{
    HSD_JObj* child = jobj->child;
    if (child != NULL) {
        if (JOBJ_INSTANCE(jobj)) {
            child->prev = NULL;
            HSD_JObjRemoveAll(jobj->child);
        } else {
            HSD_JObjUnref(child);
        }
        jobj->child = NULL;
    }

    if (jobj->prev != NULL) {
        HSD_JObjReparent(jobj, NULL);
    }
    if (union_type_dobj(jobj) && jobj->u.dobj != NULL) {
        HSD_DObjRemoveAll(jobj->u.dobj);
        jobj->u.dobj = NULL;
    }
    if (jobj->robj != NULL) {
        HSD_RObjRemoveAll(jobj->robj);
        jobj->robj = NULL;
    }
    if (jobj->aobj != NULL) {
        HSD_AObjRemove(jobj->aobj);
        jobj->aobj = NULL;
    }
}

// 803736F8
static void JObjRelease(HSD_Class* o)
{
    HSD_JObj* jobj = HSD_JOBJ(o);
    HSD_JOBJ_METHOD(jobj)->release_child(jobj);
    if ((HSD_JObj*)(HSD_IDGetDataFromTable(NULL, (u32)jobj->desc, NULL)) == jobj) {
        HSD_IDRemoveByIDFromTable(NULL, (u32)jobj->desc);
    }
    if (jobj->pvec != NULL) {
        HSD_Free(jobj->pvec);
    }
    if (jobj->vmtx != NULL) {
        HSD_Free(jobj->vmtx);
    }
    HSD_OBJECT_PARENT_INFO(&hsdJObj)->release(o);
}

// 80373790
static void JObjAmnesia(HSD_ClassInfo* info)
{
    if (info == HSD_CLASS_INFO(default_class)) {
        default_class = NULL;
    }
    if (info == HSD_CLASS_INFO(&hsdJObj)) {
        ufc_callbacks = NULL;
        current_jobj = NULL;
    }
    HSD_OBJECT_PARENT_INFO(&hsdJObj)->amnesia(info);
}

// 803737F4
static void JObjInfoInit(void)
{
    hsdInitClassInfo(HSD_CLASS_INFO(&hsdJObj), HSD_CLASS_INFO(&hsdObj),
        HSD_BASE_CLASS_LIBRARY, "hsd_jobj", sizeof(HSD_JObjInfo),
        sizeof(HSD_JObj));
    HSD_CLASS_INFO(&hsdJObj)->init = JObjInit;
    HSD_CLASS_INFO(&hsdJObj)->release = JObjRelease;
    HSD_CLASS_INFO(&hsdJObj)->amnesia = JObjAmnesia;
    HSD_JOBJ_INFO(&hsdJObj)->disp = HSD_JObjDispSub;
    HSD_JOBJ_INFO(&hsdJObj)->make_mtx = HSD_JObjMakeMatrix;
    HSD_JOBJ_INFO(&hsdJObj)->make_pmtx = HSD_JObjMakePositionMtx;
    HSD_JOBJ_INFO(&hsdJObj)->load = JObjLoad;
    HSD_JOBJ_INFO(&hsdJObj)->release_child = JObjReleaseChild;
}
