#include "jobj_ext.h"

//8000C228
void JObj_AttachJoint_CopyPos(HSD_JObj* jobj_attach, HSD_JObj* jobj_bone){
    HSD_RObj* robj = HSD_RObjAlloc();
    HSD_RObjSetFlags(robj, 0x90000004);
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