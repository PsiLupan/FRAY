#include "hsd_jobj_ext.h"

//8000C1C0
void JObj_AttachJoint_CopyPos(HSD_JObj* jobj_attach, HSD_JObj* jobj_bone){
    HSD_RObj* robj = HSD_RObjAlloc();
    HSD_RObjSetFlags(robj, 0x90000001);
    HSD_RObjSetConstraintObj(robj, jobj_bone);
    HSD_JObjPrependRObj(jobj_attach, robj);
}

//8000C228
void JObj_AttachJoint_CopyUnk(HSD_JObj* jobj_attach, HSD_JObj* jobj_bone){
    HSD_RObj* robj = HSD_RObjAlloc();
    HSD_RObjSetFlags(robj, 0x90000002);
    HSD_RObjSetConstraintObj(robj, jobj_bone);
    HSD_JObjPrependRObj(jobj_attach, robj);
}

//8000C290
void JObj_AttachJoint_CopyRot(HSD_JObj* jobj_attach, HSD_JObj* jobj_bone){
    HSD_RObj* robj = HSD_RObjAlloc();
    HSD_RObjSetFlags(robj, 0x90000004);
    HSD_RObjSetConstraintObj(robj, jobj_bone);
    HSD_JObjPrependRObj(jobj_attach, robj);
}

//8000C2F8
void JObj_AttachJoint_CopyPosRot(HSD_JObj* jobj_attach, HSD_JObj* jobj_bone){
    HSD_RObj* robj = HSD_RObjAlloc();
    HSD_RObjSetFlags(robj, 0x90000001);
    HSD_RObjSetConstraintObj(robj, jobj_bone);
    HSD_JObjPrependRObj(jobj_attach, robj);
    HSD_RObj* robj_2 = HSD_RObjAlloc();
    HSD_RObjSetFlags(robj_2, 0x90000004);
    HSD_RObjSetConstraintObj(robj_2, jobj_bone);
    HSD_JObjPrependRObj(jobj_attach, robj_2);
}

//8022F298
f32 JObj_GetFrame(HSD_JObj* jobj){
    if(jobj->aobj != NULL){
        return jobj->aobj->curr_frame;
    }
    HSD_DObj* dobj = jobj->u.dobj;
    if(dobj != NULL){
        HSD_MObj* mobj = dobj->mobj;
        if(mobj != NULL){
            if(mobj->aobj != NULL){ 
                return mobj->aobj->curr_frame;
            }
            if(mobj->tobj != NULL && mobj->tobj->aobj != NULL){
                return mobj->tobj->aobj->curr_frame;
            }
        }
    }
    if((jobj->flags & 0x1000) == 0){
        HSD_JObj* child = jobj->child;
        f32 val = -1;
        while(child != NULL){
            f32 frame = JObj_GetFrame(child);
            if(-1.f != frame){
                return frame;
            }
            child = child->next;
        }
    }
    return -1.f;
}

//80391070
void JObj_SetupInstanceMtx_Callback(HSD_GObj* gobj, u32 offset){
    HSD_JObj* jobj = GOBJ_HSD_JOBJ(gobj);
    u32 flags = GObj_GetArrayFlag(offset);
    setupInstanceMtx(jobj, 0, flags, 0);
}