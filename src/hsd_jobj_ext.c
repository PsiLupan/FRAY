#include "hsd_jobj_ext.h"

#include "hsd/hsd_display.h"

//8000C1C0
void JObj_AttachJoint_CopyPos(HSD_JObj* jobj_attach, HSD_JObj* jobj_bone)
{
    HSD_RObj* robj = HSD_RObjAlloc();
    HSD_RObjSetFlags(robj, 0x90000001);
    HSD_RObjSetConstraintObj(robj, jobj_bone);
    HSD_JObjPrependRObj(jobj_attach, robj);
}

//8000C228
void JObj_AttachJoint_CopyUnk(HSD_JObj* jobj_attach, HSD_JObj* jobj_bone)
{
    HSD_RObj* robj = HSD_RObjAlloc();
    HSD_RObjSetFlags(robj, 0x90000002);
    HSD_RObjSetConstraintObj(robj, jobj_bone);
    HSD_JObjPrependRObj(jobj_attach, robj);
}

//8000C290
void JObj_AttachJoint_CopyRot(HSD_JObj* jobj_attach, HSD_JObj* jobj_bone)
{
    HSD_RObj* robj = HSD_RObjAlloc();
    HSD_RObjSetFlags(robj, 0x90000004);
    HSD_RObjSetConstraintObj(robj, jobj_bone);
    HSD_JObjPrependRObj(jobj_attach, robj);
}

//8000C2F8
void JObj_AttachJoint_CopyPosRot(HSD_JObj* jobj_attach, HSD_JObj* jobj_bone)
{
    HSD_RObj* robj = HSD_RObjAlloc();
    HSD_RObjSetFlags(robj, 0x90000001);
    HSD_RObjSetConstraintObj(robj, jobj_bone);
    HSD_JObjPrependRObj(jobj_attach, robj);
    HSD_RObj* robj_2 = HSD_RObjAlloc();
    HSD_RObjSetFlags(robj_2, 0x90000004);
    HSD_RObjSetConstraintObj(robj_2, jobj_bone);
    HSD_JObjPrependRObj(jobj_attach, robj_2);
}

//800B1CC
void JObj_Spawn(HSD_JObj* jobj, guVector* unk, guVector* pos)
{
    if (jobj != NULL) {
    }
}

//8016895C
void JObj_AddAnims(HSD_JObj* jobj, void* unk_struct, u32 idx){
    
}

//8022ED6C
void JObj_SetAnimSpeed(HSD_JObj* jobj, f32 anim_speed[3])
{
    f32 frame = JObj_GetFrame(jobj);
    if (frame < anim_speed[0] || anim_speed[1] < frame) {
        HSD_JObjReqAnimAll(jobj, anim_speed[0]);
    }
    if (-0.1f == anim_speed[2]) {
        if (JObj_GetFrame(jobj) < anim_speed[1]) {
            HSD_JObjAnimAll(jobj);
            if (anim_speed[1] < JObj_GetFrame(jobj)) {
                HSD_JObjReqAnimAll(jobj, anim_speed[1]);
                HSD_JObjAnimAll(jobj);
            }
        }
    } else {
        HSD_JObjAnimAll(jobj);
        f32 curr_frame = JObj_GetFrame(jobj);
        if (anim_speed[1] < curr_frame) {
            curr_frame = anim_speed[2] + (curr_frame - anim_speed[1]);
            HSD_JObjReqAnimAll(jobj, curr_frame);
            HSD_JObjAnimAll(jobj);
        }
    }
}

//8022F298
f32 JObj_GetFrame(HSD_JObj* jobj)
{
    if (jobj->aobj != NULL) {
        return jobj->aobj->curr_frame;
    }
    HSD_DObj* dobj = jobj->u.dobj;
    if (dobj != NULL) {
        HSD_MObj* mobj = dobj->mobj;
        if (mobj != NULL) {
            if (mobj->aobj != NULL) {
                return mobj->aobj->curr_frame;
            }
            if (mobj->tobj != NULL && mobj->tobj->aobj != NULL) {
                return mobj->tobj->aobj->curr_frame;
            }
        }
    }
    if ((jobj->flags & 0x1000) == 0) {
        HSD_JObj* child = jobj->child;
        while (child != NULL) {
            f32 frame = JObj_GetFrame(child);
            if (-1.f != frame) {
                return frame;
            }
            child = child->next;
        }
    }
    return -1.f;
}

//80391070
void JObj_SetupInstanceMtx_Callback(HSD_GObj* gobj, s32 offset)
{
    HSD_JObj* jobj = GOBJ_HSD_JOBJ(gobj);
    u32 flags = GObj_GetFlagFromArray(offset);
    HSD_JObjDispAll(jobj, 0, flags, 0);
}
