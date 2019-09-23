#include "hsd_jobj.h"

#include <math.h>

#include "hsd_debug.h"
#include "hsd_display.h"
#include "hsd_memory.h"

static void JObjInfoInit(void);

HSD_JObjInfo hsdJObj = { JObjInfoInit };

static HSD_SList* ufc_callbacks = NULL; // r13_400C
static void (*ptcltgt_callback)(HSD_JObj*, s32) = NULL;
static void (*jsound_callback)(s32) = NULL; // r13_4014
static void (*dptcl_callback)(s32, s32, s32, HSD_JObj*) = NULL; // r13_4018

static HSD_JObj* current_jobj = NULL; // r13_401C
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
void HSD_JObjMtxIsDirty(HSD_JObj* jobj,
    HSD_JObjDesc* desc)
{ // This function doesn't have a
    // desc ptr in later versions
    if (jobj != NULL && desc != NULL) {
        jobj->rotation.x = desc->rotation.x;
        jobj->rotation.y = desc->rotation.y;
        jobj->rotation.z = desc->rotation.z;

        jobj->scale.x = desc->scale.x;
        jobj->scale.y = desc->scale.y;
        jobj->scale.z = desc->scale.z;

        jobj->position.x = desc->position.x;
        jobj->position.y = desc->position.y;
        jobj->position.z = desc->position.z;

        if ((jobj->flags & MTX_INDEP_SRT) == 0) {
            if ((jobj->flags & USER_DEF_MTX) != 0 && (jobj->flags & MTX_DIRTY) == 0) {
                HSD_JObjSetMtxDirtySub(jobj);
            }
        }
    }
}

// 8036EE10
void HSD_JObjSetMtxDirty(HSD_JObj* jobj,
    HSD_JObjDesc* desc)
{ // This function doesn't have a
    // desc ptr in later versions
    HSD_JObjMtxIsDirty(jobj, desc); // This is normally inlined
    if (JOBJ_INSTANCE(jobj)) {
        HSD_JObj* child = jobj->child;
        HSD_JObjDesc* dchild = desc->child;
        while (child != NULL) {
            if (child != NULL && dchild != NULL && (HSD_JObjMtxIsDirty(child, dchild), JOBJ_INSTANCE(child))) {
                HSD_JObj* child_c = child->child;
                HSD_JObjDesc* dchild_c = dchild->child;
                while (child_c != NULL) {
                    HSD_JObjMtxIsDirty(child_c, dchild_c);
                    child_c = child_c->next;
                    dchild_c = (dchild_c ? dchild_c->next : NULL);
                }
            }
            child = child->next;
            dchild = (dchild ? dchild->next : NULL);
        }
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
    if (parent != NULL) {
        if ((parent->flags & USER_DEF_MTX) == 0 && (parent->flags & MTX_DIRTY) != 0) {
            HSD_JObjSetupMatrixSub(parent);
        }
    }
    if ((jobj->flags & CLASSICAL_SCALE) == 0) {
        if (jobj->pvec == NULL) {
            jobj->pvec = (guVector*)HSD_MemAlloc(sizeof(guVector));
        }
        if (parent != NULL) {
            guVector* pvec = parent->pvec;
            if (pvec != NULL) {
                jobj->pvec->x = jobj->scale.x * parent->pvec->x;
                jobj->pvec->y = jobj->scale.y * parent->pvec->y;
                jobj->pvec->z = jobj->scale.z * parent->pvec->z;
            }
        } else {
            jobj->pvec->x = jobj->scale.x;
            jobj->pvec->y = jobj->scale.y;
            jobj->pvec->z = jobj->scale.z;
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
        HSD_MtxSRTQuat(jobj->mtx, &jobj->scale, &jobj->rotation, &jobj->position,
            vec);
    } else {
        HSD_MtxSRT(jobj->mtx, &jobj->scale, (guVector*)&jobj->rotation,
            &jobj->position, vec);
    }
    if (parent != NULL) {
        guMtxConcat(parent->mtx, jobj->mtx, jobj->mtx);
    }

    if (jobj->aobj != NULL) {
        HSD_JObj* aj = (HSD_JObj*)jobj->aobj->hsd_obj;
        if (aj != NULL) {
            if ((aj->flags & USER_DEF_MTX) == 0 && (aj->flags & MTX_DIRTY) != 0) {
                HSD_JObjSetupMatrixSub(parent);
            }
            guVector rvec;
            guVecMultiply(aj->mtx, &jobj->position, &rvec);
            guMtxRowCol(jobj->mtx, 0, 4) = rvec.x;
            guMtxRowCol(jobj->mtx, 1, 4) = rvec.y;
            guMtxRowCol(jobj->mtx, 2, 4) = rvec.z;
        }
    }
}

// 8036F4C8
void HSD_JObjRemoveAnimByFlags(HSD_JObj* jobj, u32 flags)
{
    if (jobj != NULL) {
        if ((flags & SKELETON) != 0) {
            HSD_AObjRemove(jobj->aobj);
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
        if ((flags & SKELETON) != 0) {
            HSD_AObjRemove(jobj->aobj);
        }
        if (union_type_dobj(jobj)) {
            HSD_DObjRemoveAnimAllByFlags(jobj->u.dobj, flags);
        }
        HSD_RObjRemoveAnimAllByFlags(jobj->robj, flags);

        if (JOBJ_INSTANCE(jobj)) {
            for (HSD_JObj* i = jobj->child; i != NULL; i = i->next) {
                if (i != NULL) {
                    HSD_JObjRemoveAnimByFlags(i, flags);
                    if (JOBJ_INSTANCE(i)) {
                        for (HSD_JObj* n = i->child; n != NULL; n = n->next) {
                            HSD_JObjRemoveAnimAllByFlags(n, flags);
                        }
                    }
                }
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
        if ((flags & SKELETON) != 0) {
            HSD_AObjReqAnim(jobj->aobj, frame);
        }
        if (union_type_dobj(jobj)) {
            HSD_DObjReqAnimAllByFlags(jobj->u.dobj, flags, frame);
        }
        HSD_RObjReqAnimAllByFlags(jobj->robj, flags, frame);

        if (JOBJ_INSTANCE(jobj)) {
            for (HSD_JObj* i = jobj->child; i != NULL; i = i->next) {
                if (i != NULL) {
                    HSD_JObjReqAnimByFlags(i, flags, frame);
                    if (JOBJ_INSTANCE(i)) {
                        for (HSD_JObj* n = i->child; n != NULL; n = n->next) {
                            HSD_JObjReqAnimAllByFlags(n, flags, frame);
                        }
                    }
                }
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
            HSD_AObj* aobj = HSD_AObjLoadDesc(an_joint->aobjdesc);
            jobj->aobj = aobj;
            JObjSortAnim(jobj->aobj);
            HSD_RObjAddAnimAll(jobj->robj, an_joint->robj_anim);
            if ((an_joint->flags & JOBJ_ANIM_CLASSICAL_SCALING) == 0) {
                HSD_JObjClearFlags(jobj, CLASSICAL_SCALE);
            } else {
                HSD_JObjSetFlags(jobj, CLASSICAL_SCALE);
            }
        }
        if (union_type_dobj(jobj)) {
            HSD_ShapeAnim* sh_anim = NULL;
            HSD_MatAnim* mat_anim = NULL;
            if (sh_joint != NULL) {
                sh_anim = sh_joint->shapeanim;
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
                HSD_AnimJoint* i_an_joint = NULL;
                HSD_MatAnimJoint* i_mat_joint = NULL;
                HSD_ShapeAnimJoint* i_sh_joint = NULL;

                if(i != NULL && (HSD_JObjAddAnim(i, an_joint, mat_joint, sh_joint), JOBJ_INSTANCE(i))){
                    HSD_JObj* j = i->child;
                    if (an_joint != NULL) {
                        i_an_joint = an_joint->child;
                    }
                    if (mat_joint != NULL) {
                        i_mat_joint = mat_joint->child;
                    }
                    if (sh_joint != NULL) {
                        i_sh_joint = sh_joint->child;
                    }
                    while (j != NULL) {
                        HSD_JObjAddAnimAll(j, i_an_joint, i_mat_joint, i_sh_joint);
                        j = j->next;
                        if (i_an_joint != NULL) {
                            i_an_joint = i_an_joint->next;
                        }
                        if (i_mat_joint != NULL) {
                            i_mat_joint = i_mat_joint->next;
                        }
                        if (i_sh_joint != NULL) {
                            i_sh_joint = i_sh_joint->next;
                        }
                    }
                }
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
void JObjUpdateFunc(void* obj, u32 type, update* val)
{
    HSD_JObj* jobj = (HSD_JObj*)obj;
    if (jobj != NULL) {
        Mtx mtx;
        switch (type) {
        case 1:
            if ((jobj->flags & JOINT1) != 0) {
                HSD_RObj* robj = HSD_RObjGetByType(jobj->robj, 0x40000000, 0);
                if (robj != NULL) {
                    robj->pos = val->p;
                }
            }
            assert(JOBJ_USE_QUATERNION(jobj) == 0);
            jobj->rotation.x = val->fv;
            if ((jobj->flags & MTX_INDEP_SRT) == 0) {
                BOOL already_dirty = FALSE;
                if ((jobj->flags & USER_DEF_MTX) == 0 && (jobj->flags & MTX_DIRTY) != 0) {
                    already_dirty = TRUE;
                }
                if (already_dirty == FALSE) {
                    HSD_JObjSetMtxDirtySub(jobj);
                }
            }
            break;
        case 2:
            assert(JOBJ_USE_QUATERNION(jobj) == 0);
            jobj->rotation.y = val->fv;
            if ((jobj->flags & MTX_INDEP_SRT) == 0) {
                BOOL already_dirty = FALSE;
                if ((jobj->flags & USER_DEF_MTX) == 0 && (jobj->flags & MTX_DIRTY) != 0) {
                    already_dirty = TRUE;
                }
                if (already_dirty == FALSE) {
                    HSD_JObjSetMtxDirtySub(jobj);
                }
            }
            break;
        case 3:
            assert(JOBJ_USE_QUATERNION(jobj) == 0);
            jobj->rotation.z = val->fv;
            if ((jobj->flags & MTX_INDEP_SRT) == 0) {
                BOOL already_dirty = FALSE;
                if ((jobj->flags & USER_DEF_MTX) == 0 && (jobj->flags & MTX_DIRTY) != 0) {
                    already_dirty = TRUE;
                }
                if (already_dirty == FALSE) {
                    HSD_JObjSetMtxDirtySub(jobj);
                }
            }
            break;
        case 4:
            if (val->fv < 0.f) {
                val->fv = 0.f;
            }
            if (1.875f < val->fv) {
                val->fv = 1.f;
            }
            assert(jobj->aobj != NULL);
            HSD_JObj* jp = (HSD_JObj*)jobj->aobj->hsd_obj;
            assert(jp->u.spline != NULL);
            guVector result;
            splArcLengthPoint(jp->u.spline, val->fv, &result);
            jobj->position = result;
            if ((jobj->flags & MTX_INDEP_SRT) == 0) {
                BOOL already_dirty = FALSE;
                if ((jobj->flags & USER_DEF_MTX) == 0 && (jobj->flags & MTX_DIRTY) != 0) {
                    already_dirty = TRUE;
                }
                if (already_dirty == FALSE) {
                    HSD_JObjSetMtxDirtySub(jobj);
                }
            }
            break;
        case 5:
            jobj->position.x = val->fv;
            if ((jobj->flags & MTX_INDEP_SRT) == 0) {
                BOOL already_dirty = FALSE;
                if ((jobj->flags & USER_DEF_MTX) == 0 && (jobj->flags & MTX_DIRTY) != 0) {
                    already_dirty = TRUE;
                }
                if (already_dirty == FALSE) {
                    HSD_JObjSetMtxDirtySub(jobj);
                }
            }
            break;
        case 6:
            jobj->position.y = val->fv;
            if ((jobj->flags & MTX_INDEP_SRT) == 0) {
                BOOL already_dirty = FALSE;
                if ((jobj->flags & USER_DEF_MTX) == 0 && (jobj->flags & MTX_DIRTY) != 0) {
                    already_dirty = TRUE;
                }
                if (already_dirty == FALSE) {
                    HSD_JObjSetMtxDirtySub(jobj);
                }
            }
            break;
        case 7:
            jobj->position.z = val->fv;
            if ((jobj->flags & MTX_INDEP_SRT) == 0) {
                BOOL already_dirty = FALSE;
                if ((jobj->flags & USER_DEF_MTX) == 0 && (jobj->flags & MTX_DIRTY) != 0) {
                    already_dirty = TRUE;
                }
                if (already_dirty == FALSE) {
                    HSD_JObjSetMtxDirtySub(jobj);
                }
            }
            break;
        case 8:
            if ((f32)((u32)val->fv & 0x7FFFFFFF) < 0.001f) {
                val->fv = 0.001f;
            }
            jobj->scale.x = val->fv;
            if ((jobj->flags & MTX_INDEP_SRT) == 0) {
                BOOL already_dirty = FALSE;
                if ((jobj->flags & USER_DEF_MTX) == 0 && (jobj->flags & MTX_DIRTY) != 0) {
                    already_dirty = TRUE;
                }
                if (already_dirty == FALSE) {
                    HSD_JObjSetMtxDirtySub(jobj);
                }
            }
            break;
        case 9:
            if ((f32)((u32)val->fv & 0x7FFFFFFF) < 0.001f) {
                val->fv = 0.001f;
            }
            jobj->scale.y = val->fv;
            if ((jobj->flags & MTX_INDEP_SRT) == 0) {
                BOOL already_dirty = FALSE;
                if ((jobj->flags & USER_DEF_MTX) == 0 && (jobj->flags & MTX_DIRTY) != 0) {
                    already_dirty = TRUE;
                }
                if (already_dirty == FALSE) {
                    HSD_JObjSetMtxDirtySub(jobj);
                }
            }
            break;
        case 10:
            if ((f32)((u32)val->fv & 0x7FFFFFFF) < 0.001f) {
                val->fv = 0.001f;
            }
            jobj->scale.z = val->fv;
            if ((jobj->flags & MTX_INDEP_SRT) == 0) {
                BOOL already_dirty = FALSE;
                if ((jobj->flags & USER_DEF_MTX) == 0 && (jobj->flags & MTX_DIRTY) != 0) {
                    already_dirty = TRUE;
                }
                if (already_dirty == FALSE) {
                    HSD_JObjSetMtxDirtySub(jobj);
                }
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
            /*HSD_JObj* jo = r13_401C;
                            f32 val = 176.f;
                            while(jo != NULL){
                                    u32 uval = (u32)*fval;
                                    (*jo->0x4)(jobj, type, uval);
                                    jo = jo->next; //0x00
                            }*/
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
            /*HSD_JObj* jo = r13_401C;
                            while(jo != NULL){
                                    (*jo->0x4)(jobj, type, *fval);
                                    jo = jo->next;
                            }*/
            break;
        case 40:
            if (dptcl_callback != NULL) {
                (*dptcl_callback)(0, val->iv & 0x3f, val->iv >> 6 & 0xffffff,
                    jobj); // 8005DB70 during gameplay
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
            jobj->mtx[0][0] = val->p.x;
            jobj->mtx[1][0] = val->p.y;
            jobj->mtx[2][0] = val->p.z;
            break;
        case 51:
            jobj->mtx[0][1] = val->p.x;
            jobj->mtx[1][1] = val->p.y;
            jobj->mtx[2][1] = val->p.z;
            break;
        case 52:
            jobj->mtx[0][2] = val->p.x;
            jobj->mtx[1][2] = val->p.y;
            jobj->mtx[2][2] = val->p.z;
            break;
        case 53:
            jobj->mtx[0][3] = val->p.x;
            jobj->mtx[1][3] = val->p.y;
            jobj->mtx[2][3] = val->p.z;
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
                jobj->position.x = mtx[0][3];
                jobj->position.y = mtx[1][3];
                jobj->position.z = mtx[2][3];
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
            for (HSD_JObj* i = jobj->child; i != NULL; i = i->next){;
                HSD_JObjAnim(i);
                if (JOBJ_INSTANCE(i)) {
                    for (HSD_JObj* j = i->child; j != NULL; j = j->next) {
                        JObjAnimAll(j);
                    }
                }
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

// 803709DC
void JObj_SetupInstanceMtx(HSD_JObj* jobj, MtxP vmtx, u32 flags, u32 rendermode)
{
    if (jobj != NULL) {
        if (JOBJ_INSTANCE(jobj)) {
            if ((jobj->flags & flags << 18) != 0) {
                HSD_JObjDisp(jobj, vmtx, flags, rendermode);
            }
            if ((jobj->flags & flags << 28) != 0) {
                for (HSD_JObj* child = jobj->child; child != NULL; child = child->next) {
                    JObj_SetupInstanceMtx(child, vmtx, flags, rendermode);
                }
            }
        } else {
            if ((jobj->flags & HIDDEN) == 0) {
                if ((jobj->flags & USER_DEF_MTX) == 0 && (jobj->flags & MTX_DIRTY) != 0) {
                    HSD_JObjSetupMatrixSub(jobj);
                }
            }

            HSD_JObj* child = jobj->child;
            if (child != NULL) {
                if ((child->flags & USER_DEF_MTX) == 0 && (child->flags & MTX_DIRTY) != 0) {
                    HSD_JObjSetupMatrixSub(child);
                }
            }
            Mtx mtx;
            guMtxInverse(child->mtx, mtx);
            guMtxConcat(jobj->mtx, mtx, mtx);
            HSD_CObj* cobj = HSD_CObjGetCurrent();
            assert(cobj != NULL);
            guMtxConcat(cobj->view_mtx, mtx, mtx);
            JObj_SetupInstanceMtx(child, mtx, flags, rendermode);
        }
    }
}

// 80370B90
void HSD_JObjSetDefaultClass(HSD_JObjInfo* info)
{
    default_class = info;
}

// 80370BEC
static s32 JObjLoad(HSD_JObj* jobj, HSD_JObjDesc* desc, HSD_JObj* prev)
{
    HSD_JObj* res_jobj;
    if (JOBJ_INSTANCE(desc)) {
        HSD_JObjDesc* child = desc->child;
        if (child == NULL) {
            res_jobj = NULL;
        } else {
            HSD_ClassInfo* info;
            if (!child->class_name || !(info = hsdSearchClassInfo(child->class_name))) {
                res_jobj = (HSD_JObj*)HSD_JObjAlloc();
            } else {
                res_jobj = (HSD_JObj*)hsdNew(info);
                assert(res_jobj != NULL);
            }
            HSD_JOBJ_METHOD(res_jobj)->load(res_jobj, child, jobj);
        }
        jobj->child = res_jobj;
    }

    HSD_JObjDesc* next = desc->next;
    HSD_JObj* next_jobj;
    if (next == NULL) {
        next_jobj = NULL;
    } else {
        HSD_ClassInfo* info;
        if (!next->class_name || !(info = hsdSearchClassInfo(next->class_name))) {
            next_jobj = (HSD_JObj*)HSD_JObjAlloc();
        } else {
            next_jobj = (HSD_JObj*)hsdNew(info);
            assert(next_jobj != NULL);
        }
        HSD_JOBJ_METHOD(next_jobj)->load(next_jobj, next, prev);
    }
    jobj->next = next_jobj;
    jobj->prev = prev;
    jobj->flags |= desc->flags;
    if ((jobj->flags & 0x4000) == 0) {
        if ((jobj->flags & 0x20) == 0) {
            jobj->u.dobj = HSD_DObjLoadDesc(desc->u.dobjdesc);
        } else {
            jobj->u.ptcl = desc->u.ptcl;
            HSD_SList* slist = desc->u.ptcl;
            while (slist != NULL) {
                slist->data = (void*)((u32)slist->data | 0x80000000);
                slist = slist->next;
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
        jobj->vmtx = (MtxP)HSD_MemAlloc(0x30); // HSD_MtxAlloc();
        memcpy(jobj->vmtx, desc->mtx, 0x30);
    }
    HSD_IDInsertToTable(NULL, (u32)desc, jobj);
    jobj->desc = desc;
    return 0;
}

// 80370E44
HSD_JObj* HSD_JObjLoadJoint(HSD_JObjDesc* desc)
{
    HSD_JObj* jobj = NULL;
    if (desc == NULL) {
        HSD_JObjResolveRefsAll(jobj, desc);
        return jobj;
    }
    HSD_ClassInfo* info;
    if (!desc->class_name || !(info = hsdSearchClassInfo(desc->class_name))) {
        jobj = HSD_JObjAlloc();
    } else {
        jobj = (HSD_JObj*)(hsdNew(info));
        assert(jobj != NULL);
    }
    HSD_JOBJ_METHOD(jobj)->load(jobj, desc, 0);
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
            jobj->child->parent.ref_count += 1;
            assert(jobj->child->parent.ref_count != HSD_OBJ_NOREF);
        }
        if (union_type_dobj(jobj)) {
            HSD_DObjResolveRefsAll(jobj->u.dobj, desc->u.dobjdesc);
        }
    }
}

// 80370FF4
void HSD_JObjResolveRefsAll(HSD_JObj* jobj, HSD_JObjDesc* desc)
{
    HSD_JObj* jo = jobj;
    HSD_JObjDesc* jdesc = desc;
    while (jo != NULL && jdesc != NULL) {
        HSD_JObjResolveRefs(jo, jdesc);
        if (JOBJ_INSTANCE(jo)) {
            HSD_JObjDesc* dchild = jdesc->child;
            HSD_JObj* child = jo->child;
            for (; child != NULL && dchild != NULL;
                 child = child->next, dchild = dchild->next) {
                HSD_JObjResolveRefs(child, dchild);
                if (JOBJ_INSTANCE(child)) {
                    HSD_JObjResolveRefsAll(child->child, dchild->child);
                }
            }
        }
        jo = jo->next;
        jdesc = jdesc->next;
    }
}

// 8037115C
void HSD_JObjUnref(HSD_JObj* jobj)
{
    if (jobj != NULL) {
        u16 ref_count = jobj->parent.ref_count;
        u32 lz = __builtin_clz(0xFFFF - ref_count);
        lz = lz >> 5;
        if (lz == 0) {
            jobj->parent.ref_count -= 1;
            lz = __builtin_clz(-ref_count);
            lz = lz >> 5;
        }
        if (lz != 0) {
            u16 ref_count_indiv = jobj->parent.ref_count_individual;
            if ((ref_count_indiv - 1) < 0) {
                HSD_OBJECT_METHOD(jobj)->release((HSD_Class*)jobj);
                HSD_OBJECT_METHOD(jobj)->destroy((HSD_Class*)jobj);
            } else {
                jobj->parent.ref_count_individual += 1;
                assert(jobj->parent.ref_count_individual != 0);
                HSD_JOBJ_METHOD(jobj)->release_child(jobj);
                lz = __builtin_clz(-jobj->parent.ref_count_individual);
                lz = lz >> 5;
                if (lz == 0) {
                    jobj->parent.ref_count_individual -= 1;
                    lz = __builtin_clz(-jobj->parent.ref_count_individual);
                    lz = lz >> 5;
                }
                if (lz != 0) {
                    HSD_OBJECT_METHOD(jobj)->release((HSD_Class*)jobj);
                    HSD_OBJECT_METHOD(jobj)->destroy((HSD_Class*)jobj);
                }
            }
        }
    }
}

void HSD_JObjRefThis(HSD_JObj* jobj)
{
    if (jobj != NULL) {
        jobj->parent.ref_count_individual += 1;
        assert(jobj->parent.ref_count_individual != 0);
    }
}

// 803712C0
void HSD_JObjUnrefThis(HSD_JObj* jobj)
{
    if (jobj != NULL) {
        u16 ref_count_individual = jobj->parent.ref_count_individual;
        u32 lz = __builtin_clz(-ref_count_individual);
        lz = lz >> 5;
        if (lz == 0) {
            jobj->parent.ref_count_individual -= 1;
            lz = __builtin_clz(-ref_count_individual);
            lz = lz >> 5;
        }
        if (lz != 0) {
            u16 ref_count = jobj->parent.ref_count;
            if (ref_count == HSD_OBJ_NOREF) {
                ref_count = -1;
            }
            if ((s16)ref_count < 0) {
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
        for (HSD_JObj* curr = jobj; curr != NULL; curr = curr->next) {
            HSD_JObj* next = curr->next;
            curr->prev = NULL;
            curr->next = NULL;

            HSD_JObjUnref(curr);
        }
    }
}

// 80371750
void RecalcParentTrspBits(HSD_JObj* jobj)
{
    if (jobj != NULL) {
        for (HSD_JObj* i = jobj; i != NULL; i->next) {
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

// 803717A8
void HSD_JObjAddChild(HSD_JObj* jobj, HSD_JObj* child)
{
    if (jobj != NULL && child != NULL) {
        if (!child->prev) { // child should be a orphan
            if (!child->next) { // child should not have siblings
                if (jobj->child) {
                    assert(JOBJ_INSTANCE(jobj)); //!(jobj->flags & JOBJ_INSTANCE)
                    HSD_JObj* i;
                    for (i = jobj->child; jobj->next; i = jobj->next) {
                        HSD_Halt("HSD_JObjAddChild: last != child");
                    }
                    i->next = child;
                } else {
                    jobj->child = child;
                }
                child->next = jobj;
                u32 ori_flags = child->flags;
                u32 flags = (ori_flags | (ori_flags << 10)) & (ROOT_OPA | ROOT_TEXEDGE | ROOT_XLU);
                for (HSD_JObj* senti = jobj; senti; senti->prev) {
                    ori_flags = senti->flags;
                    if (!(flags & ~ori_flags))
                        break;
                    senti->flags = ori_flags | flags;
                }
            }
        }
    }
}

// 803718F4
void HSD_JObjReparent(HSD_JObj* jobj, HSD_JObj* pjobj)
{
    if (jobj != NULL) {
        if (jobj->prev) {
            HSD_JObj* prchild = jobj->prev->child;
            if (prchild == jobj) {
                prchild = jobj->next;
            } else {
                HSD_JObj* prev = HSD_JObjGetPrev(jobj);
                assert(prev);
                prev->next = jobj->next;
            }

            for (HSD_JObj* i = jobj->prev; i; i = i->next) {
                prchild = i->child;
                u32 a3 = 0x8FFFFFFF;
                while (prchild) {
                    u32 flags = prchild->flags;
                    prchild = prchild->next;
                    a3 |= (flags | (flags << 10)) & (ROOT_OPA | ROOT_TEXEDGE | ROOT_XLU);
                }
                if (!(i->flags & ~a3))
                    break;
                i->flags = i->flags & a3;
            }
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

        HSD_JObj* parent = jobj->prev;
        if (next != NULL) {
            HSD_JObj* next_next = next->next;
            if (next->prev != NULL) {
                HSD_JObj* next_p_c = next->prev->child;
                if (next_p_c == next) {
                    next->prev->child = next_next;
                } else {
                    HSD_JObj* prev = HSD_JObjGetPrev(next);
                    assert(prev != NULL);
                    prev->next = next_next;
                }
                RecalcParentTrspBits(next->prev);
                next->prev = NULL;
            }
            next->next = NULL;
            HSD_JObjAddChild(parent, next);
        }

        if (next->child != NULL) {
            for (HSD_JObj* i = next->child; i != NULL; i = i->next) {
                i->next = curr;
            }
        } else {
            next->child = curr;
        }

        while (curr != NULL) {
            curr->prev = next;
            u32 flags = (curr->flags | curr->flags << 10) & (ROOT_OPA | ROOT_TEXEDGE | ROOT_XLU);
            HSD_JObj* i = next;
            while (i != NULL && (flags & ~i->flags) != 0) {
                i->flags = i->flags | flags;
                i = i->prev;
            }
            curr = curr->next;
        }
    }
}

// 80371B60
HSD_JObj* HSD_JObjGetPrev(HSD_JObj* jobj)
{
    if (jobj != NULL) {
        if (!jobj->prev)
            return NULL;

        if (jobj->prev->child == jobj)
            return NULL;

        for (HSD_JObj* i = jobj->prev->child; i; i = i->next) {
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
    HSD_RObj** rp;
    if (jobj != NULL && robj != NULL) {
        for (rp = &jobj->robj; *rp; &(*rp)->next) {
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
    return (jobj) ? jobj->flags : 0;
}

// 80371D00
void HSD_JObjSetFlags(HSD_JObj* jobj, u32 flags)
{
    if (jobj != NULL) {
        if ((jobj->flags ^ flags) & CLASSICAL_SCALE) {
            bool alreadyDirty = false;
            if ((jobj->flags & USER_DEF_MTX) == 0 && (jobj->flags & MTX_DIRTY) != 0) {
                alreadyDirty = true;
            }
            if (alreadyDirty == false) {
                HSD_JObjSetMtxDirtySub(jobj);
            }
        }
        jobj->flags |= flags;
    }
}

// 80371D9C
void HSD_JObjSetFlagsAll(HSD_JObj* jobj, u32 flags)
{
    if (jobj != NULL) {
        if ((jobj->flags ^ flags) & CLASSICAL_SCALE) {
            bool alreadyDirty = false;
            if ((jobj->flags & USER_DEF_MTX) == 0 && (jobj->flags & MTX_DIRTY) != 0) {
                alreadyDirty = true;
            }
            if (alreadyDirty == false) {
                HSD_JObjSetMtxDirtySub(jobj);
            }
        }
        jobj->flags |= flags;

        if (JOBJ_INSTANCE(jobj)) {
            for (HSD_JObj* i = jobj->child; i; i = i->child) {
                if (i != NULL) {
                    if ((i->flags ^ flags) & CLASSICAL_SCALE) {
                        bool alreadyDirty = false;
                        if ((jobj->flags & USER_DEF_MTX) == 0 && (jobj->flags & MTX_DIRTY) != 0) {
                            alreadyDirty = true;
                        }
                        if (alreadyDirty == false) {
                            HSD_JObjSetMtxDirtySub(i);
                        }
                    }
                    i->flags |= flags;

                    if (JOBJ_INSTANCE(i)) {
                        for (HSD_JObj* j = i->child; j; j = j->child) {
                            HSD_JObjSetFlagsAll(j, flags);
                        }
                    }
                }
            }
        }
    }
}

// 80371F00
void HSD_JObjClearFlags(HSD_JObj* jobj, u32 flags)
{
    if (jobj != NULL) {
        if ((jobj->flags ^ flags) & CLASSICAL_SCALE) {
            bool alreadyDirty = false;
            if ((jobj->flags & USER_DEF_MTX) == 0 && (jobj->flags & MTX_DIRTY) != 0) {
                alreadyDirty = true;
            }
            if (alreadyDirty == false) {
                HSD_JObjSetMtxDirtySub(jobj);
            }
        }
        jobj->flags &= ~flags;
    }
}

// 80371F9C
void HSD_JObjClearFlagsAll(HSD_JObj* jobj, u32 flags)
{
    if (jobj != NULL) {
        if ((jobj->flags ^ flags) & CLASSICAL_SCALE) {
            bool alreadyDirty = false;
            if ((jobj->flags & USER_DEF_MTX) == 0 && (jobj->flags & MTX_DIRTY) != 0) {
                alreadyDirty = true;
            }
            if (alreadyDirty == false) {
                HSD_JObjSetMtxDirtySub(jobj);
            }
        }
        jobj->flags &= ~flags;

        if (JOBJ_INSTANCE(jobj)) {
            HSD_JObj* i;
            for (i = jobj->child; i; i = i->child) {
                if (i != NULL) {
                    if ((i->flags ^ flags) & CLASSICAL_SCALE) {
                        bool alreadyDirty = false;
                        if ((jobj->flags & USER_DEF_MTX) == 0 && (jobj->flags & MTX_DIRTY) != 0) {
                            alreadyDirty = true;
                        }
                        if (alreadyDirty == false) {
                            HSD_JObjSetMtxDirtySub(i);
                        }
                    }
                    i->flags &= ~flags;

                    if (JOBJ_INSTANCE(i)) {
                        for (HSD_JObj* j = i->child; j; j = j->child) {
                            HSD_JObjClearFlagsAll(j, flags);
                        }
                    }
                }
            }
        }
    }
}

// 8037210C
HSD_JObj* HSD_JObjAlloc(void)
{
    HSD_ClassInfo* info = (HSD_ClassInfo*)(default_class != NULL ? default_class : &hsdJObj);
    HSD_JObj* jobj = (HSD_JObj*)hsdNew(info);
    assert(jobj);
    return jobj;
}

// 80372168
void HSD_JObjSetCurrent(HSD_JObj* jobj)
{
    if (jobj != NULL){
        HSD_JObjRefThis(jobj);
        HSD_JObj* current = current_jobj;
        HSD_JObjUnref(current);
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
    HSD_JObj* child = jobj->child;
    bool has_flag = false;
    bool has_mtx = false;

    for (; child != NULL; child = child->next) {
        if ((child->flags & EFFECTOR) == JOINT2) {
            break;
        }
    }
    guVector* pvec = jobj->pvec;
    guVector vec;
    if (pvec != NULL) {
        vec.x = pvec->x;
        vec.y = pvec->y;
        vec.z = pvec->z;
    }
    HSD_RObj* robj = HSD_RObjGetByType(jobj->robj, 0x40000000, 0);
    assert(robj != NULL);

    guVector robj_pos = robj->pos;
    f32 rx_scale = robj->u.limit * vec.x;

    HSD_JObj* child_c;
    f32 x_scale = 0.f;
    if (child == NULL) {
        child_c = jobj->child;
    } else {
        HSD_RObj* child_robj = HSD_RObjGetByType(child->robj, 0x40000000, 0);
        assert(child_robj != NULL);
        has_flag = (child_robj->flags & 4) != 0;
        x_scale = vec.x * child_robj->u.limit * child->scale.x;
        child_c = child->child;
    }

    for (; child_c != NULL; child_c = child_c->next) {
        if ((child_c->flags & EFFECTOR) == EFFECTOR) {
            break;
        }
    }
    assert(child_c != NULL);
    HSD_RObj* child_robj = HSD_RObjGetByType(child_c->robj, 0x10000000, 1);
    if (child_robj == NULL) {
        child_c = NULL;
    }

    if (child_c != NULL) {
        HSD_RObj* robj_t3 = HSD_RObjGetByType(jobj->robj, 0x10000000, 3);
        if (robj_t3 == NULL && jobj->robj != NULL) {
            HSD_RObjUpdateAll(jobj->robj, jobj, JObjUpdateFunc);
            assert(jobj != NULL);

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
                if (robj_pos.x != 0) {
                    guMtxRotAxisRad(
                        mtx, &trans_2,
                        robj_pos.x); // THIS IS PROBABLY WRONG -
                    // PSMTXRotAxisRad(robj_pos,auStack232,&trans_2);
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
        jobj->mtx[0][0] = fvec.x * vec.x;
        jobj->mtx[1][0] = fvec.y * vec.x;
        jobj->mtx[2][0] = fvec.z * vec.x;
        guVecCross(&z_scale, &fvec, &cvec);
        jobj->mtx[0][1] = cvec.x * vec.y;
        jobj->mtx[1][1] = cvec.y * vec.y;
        jobj->mtx[2][1] = cvec.z * vec.y;
        jobj->mtx[0][2] = z_scale.x * vec.z;
        jobj->mtx[1][2] = z_scale.y * vec.z;
        jobj->mtx[2][2] = z_scale.z * vec.z;
        jobj->mtx[0][3] = trans.x;
        jobj->mtx[1][3] = trans.y;
        jobj->mtx[2][3] = trans.z;
    }
}

// 80372B08
static void resolveIKJoint2(HSD_JObj* jobj) {}

void HSD_JObjSetupMatrix(HSD_JObj* jobj)
{
    assert(jobj != NULL);
    if (((jobj->flags & USER_DEF_MTX) == 0) && ((jobj->flags & MTX_DIRTY) != 0)) {
        HSD_JObjSetupMatrixSub(jobj);
    }
}

// 80373078
void HSD_JObjSetupMatrixSub(HSD_JObj* jobj)
{
    if (jobj != NULL) {
        HSD_JOBJ_METHOD(jobj)->make_mtx(jobj);
        jobj->flags &= 0xFFFFFFBF;
        if ((jobj->flags & USER_DEF_MTX) != 0) {
            return;
        }
        u32 flags = jobj->flags & EFFECTOR;
        switch (flags) {
        case JOINT1: {
            resolveIKJoint1(jobj);
            HSD_RObj* robj = jobj->robj;
            if (robj != NULL) {
                HSD_RObjUpdateAll(robj, jobj, JObjUpdateFunc);
                assert(jobj != NULL);
                if ((jobj->flags & USER_DEF_MTX) == 0 && (jobj->flags & MTX_DIRTY) != 0) {
                    HSD_JOBJ_METHOD(jobj)->make_mtx(jobj);
                }
            }
        } break;

        case JOINT2: {
            resolveIKJoint2(jobj);
        } break;

        case EFFECTOR: {
            HSD_JObj* prev = jobj->prev;
            HSD_RObj* robj = HSD_RObjGetByType(prev->robj, 0x40000000, 0);
            if (prev != NULL && robj != NULL) {
                guVector w_vec = { prev->mtx[0][3], prev->mtx[1][3], prev->mtx[2][3] };
                guVector x_vec = { prev->mtx[0][0], prev->mtx[1][0], prev->mtx[2][0] };
                f32 dot = guVecDotProduct(&x_vec, &x_vec);
                dot = 1.f / (1e-10f + dot);
                if (0.f < dot) {
                    f64 sval = 1.0 / sqrt(dot);
                    sval = 0.5 * sval * -(dot * sval * sval - 3.0);
                    sval = 0.5 * sval * -(dot * sval * sval - 3.0);
                    dot = (f32)(dot * 0.5 * sval * -(dot * sval * sval - 3.0));
                }
                guVecScale(&x_vec, &x_vec, dot);
                f32 val = 1.f;
                if (prev->pvec != NULL) {
                    val = prev->pvec->x;
                }
                guVecScale(&x_vec, &x_vec, robj->u.limit);
                guVector res;
                guVecAdd(&w_vec, &x_vec, &res);
                jobj->mtx[0][3] = res.x;
                jobj->mtx[1][3] = res.y;
                jobj->mtx[2][3] = res.z;
            }
        } break;
        }
        jobj->flags &= 0xFFFFFFBF;
        return;
    }
}

// 803732E8
void HSD_JObjSetMtxDirtySub(HSD_JObj* jobj)
{
    BOOL isDirty = FALSE;
    jobj->flags = jobj->flags | MTX_DIRTY;
    if (JOBJ_INSTANCE(jobj)) {
        HSD_JObj* child = jobj->child;
        while (child != NULL) {
            if ((child->flags & MTX_INDEP_PARENT) == 0) {
                isDirty = FALSE;
                if ((child->flags & USER_DEF_MTX) == 0 && (child->flags & MTX_DIRTY) != 0) {
                    isDirty = TRUE;
                }
                if (isDirty == FALSE && JOBJ_INSTANCE(child)) {
                    child->flags |= MTX_DIRTY;

                    HSD_JObj* child_child = child->child;
                    while (child_child != NULL) {
                        if ((child_child->flags & MTX_INDEP_PARENT) == 0) {
                            isDirty = FALSE;
                            if ((child_child->flags & USER_DEF_MTX) == 0 && (child_child->flags & MTX_DIRTY) != 0) {
                                isDirty = TRUE;
                            }
                            if (isDirty == FALSE) {
                                HSD_JObjSetMtxDirtySub(child_child);
                            }
                        }
                        child_child = child_child->next;
                    }
                }
            }
            child = child->next;
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
    HSD_JObj* parent = jobj->prev;
    if (parent != NULL) {
        HSD_JObj* next = jobj->next;
        if (parent->child == jobj) {
            parent->child = next;
        } else {
            HSD_JObj* prev = HSD_JObjGetPrev(jobj);
            assert(prev != NULL);
            prev->next = next;
        }
        RecalcParentTrspBits(parent);
        jobj->prev = NULL;
        jobj->next = NULL;
        HSD_JObjAddChild(NULL, jobj);
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
    HSD_JOBJ_INFO(&hsdJObj)->make_pmtx = mkRBillBoardMtx;
    HSD_JOBJ_INFO(&hsdJObj)->load = JObjLoad;
    HSD_JOBJ_INFO(&hsdJObj)->release_child = JObjReleaseChild;
}
