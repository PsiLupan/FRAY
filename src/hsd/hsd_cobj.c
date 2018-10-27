#include "hsd_cobj.h"

static HSD_CObj *current_cobj = NULL;

//803676F8
void HSD_CObjEraseScreen(HSD_CObj* cobj, void* unk1, void* unk2, void* unk3){

}

//80367874
void HSD_CObjRemoveAnim(HSD_CObj* cobj){
    if(cobj){
        HSD_AObjRemove(cobj->aobj);
        cobj->aobj = NULL;
        HSD_WObjRemoveAnim(HSD_CObjGetEyePositionWObj(cobj));
        HSD_WObjRemoveAnim(HSD_CObjGetInterestWObj(cobj));
    }
}

//803678CC
void HSD_CObjAddAnim(HSD_CObj* cobj, HSD_AObjDesc* aobjdesc){
    if(cobj && aobjdesc){
        if(cobj->aobj)
            HSD_AObjRemove(cobj->aobj);
        
        cobj->aobj = HSD_AObjLoadDesc(aobjdesc);
        //HSD_WObjAddAnim(HSD_CObjgetEyePositionWObj(cobj), aobjdesc->0x04);
        //HSD_WObjAddAnim(HSD_CObjGetInterestWObj(cobj), aobjdesc->0x08);
    }
}

//80367948
void CObjUpdateFunc(HSD_CObj* cobj, u32 type, guVector vec){ //TODO: This absolutely can't be right. Need to follow in debugger.
    if(cobj){
        switch(type){
            case 1:
            HSD_CObjGetEyePosition(cobj);
            HSD_CObjSetEyePosition(cobj);
            break;

            case 2:
            break;

            case 3:
            break;

            case 5:
            case 6:
            case 7:
            guVector tVec;
            HSD_CObjGetInterest(cobj, tVec);
            tVec.x = vec.x;
            HSD_CObjSetInterest(cobj, vec);
            break;

            case 9:
            HSD_CObjSetRoll(cobj, vec.x);
            break;

            case 10:
            HSD_CObjSetFov(cobj, vec.x);
            break;

            case 11:
            HSD_CObjSetNear(cobj, vec.x);
            break;

            case 12:
            HSD_CObjSetFar(cobj, vec.x);
            break;

            default:
            break;
        }
    }
}

//803686AC
void HSD_CObjGetInterest(HSD_CObj* cobj, guVector interest){
    assert(cobj);
    HSD_WObjGetPosition(cobj->interest, interest);
}

//80368718
void HSD_CObjSetInterest(HSD_CObj* cobj, guVector interest){
    assert(cobj);
    HSD_WObjSetPosition(cobj->interest, interest);
}

//803687F0
void HSD_CObjSetEyePosition(HSD_CObj* cobj, guVector pos){
    assert(cobj);
    HSD_WObjSetPosition(cobj->eye_position, pos);
}

//80369BC8
f32 HSD_CObjGetFov(HSD_CObj* cobj){
    if(cobj){
        if(cobj->projection_type == PROJ_PERSPECTIVE)
            return cobj->fov_top;
    }
    return 0.0f; //r2 - 0x1568, assumed 0.0f currently
}

//80369BEC
void HSD_CObjSetFov(HSD_CObj* cobj, f32 fov){
    if(cobj){
        if(cobj->projection_type == PROJ_PERSPECTIVE)
            cobj->fov_top = fov;
    }
}

//80369C0C
f32 HSD_CObjGetAspect(HSD_CObj* cobj){
    if(cobj){
        if(cobj->projection_type == PROJ_PERSPECTIVE)
            return cobj->aspect_bottom;
    }
    return 0.0f; //r2 - 0x1568, assumed 0.0f currently
}

//80369C30
void HSD_CObjSetAspect(HSD_CObj* cobj, f32 aspect){
    if(cobj){
        if(cobj->projection_type == PROJ_PERSPECTIVE)
            cobj->aspect_bottom = aspect;
    }
}

//80369C50
f32 HSD_CObjGetTop(HSD_CObj* cobj){

}

//80369CE4
void HSD_CObjSetTop(HSD_CObj* cobj, f32 top){
    if(cobj){
        if(cobj->projection_type == PROJ_FRUSTRUM || cobj->projection_type == PROJ_ORTHO)
            cobj->fov_top = top;
    }
}

//80369D18
f32 HSD_CObjGetBottom(HSD_CObj* cobj){

}

//80369DB0
void HSD_CObjSetBottom(HSD_CObj* cobj, f32 bottom){
    if(cobj){
        if(cobj->projection_type == PROJ_FRUSTRUM || cobj->projection_type == PROJ_ORTHO)
            cobj->aspect_bottom = bottom;
    }
}

//80369DE4
f32 HSD_CObjGetLeft(HSD_CObj* cobj){

}

//80369E84
void HSD_CObjSetLeft(HSD_CObj* cobj, f32 left){
    if(cobj){
        if(cobj->projection_type == PROJ_FRUSTRUM || cobj->projection_type == PROJ_ORTHO)
            cobj->left = left;
    }
}

//80369EB8
f32 HSD_CObjGetRight(HSD_CObj* cobj){

}

//80369F54
void HSD_CObjSetRight(HSD_CObj* cobj, f32 right){
    if(cobj){
        if(cobj->projection_type == PROJ_FRUSTRUM || cobj->projection_type == PROJ_ORTHO)
            cobj->right = right;
    }
}

//80369F88
f32 HSD_CObjGetNear(HSD_CObj* cobj){
    if(cobj){
        return cobj->near;
    }
    return 0.0f; //r2 - 0x1568, assumed 0.0f
}

//80369FA0
void HSD_CObjSetNear(HSD_CObj* cobj, f32 near){
    if(cobj)
        cobj->near = near;
}

//80369FB0
f32 HSD_CObjGetFar(HSD_CObj* cobj){
    if(cobj){
        return cobj->far;
    }
    return 0.0f; //r2 - 0x1568, assumed 0.0f
}

//80369FC8
void HSD_CObjSetFar(HSD_CObj* cobj, f32 far){
    if(cobj)
        cobj->far = far;
}

//8036A12C
u8 HSD_CObjGetProjectionType(HSD_CObj* cobj){
    if(cobj)
        return cobj->projection_type;
    return PROJ_PERSPECTIVE;
}

//8036A144
void HSD_CObjSetProjectionType(HSD_CObj* cobj, u8 proj_type){
    if(cobj)
        cobj->projection_type = proj_type;
}

//8036A154
void HSD_CObjSetPerspective(HSD_CObj* cobj, f32 fov, f32 aspect){
    if(cobj){
        cobj->projection_type = PROJ_PERSPECTIVE;
        cobj->fov_top = fov;
        cobj->aspect_bottom = aspect;
    }
}

//8036A170
void HSD_CObjSetFrustrum(HSD_CObj* cobj, f32 top, f32 bottom, f32 left, f32 right){
    if(cobj){
        cobj->projection_type = PROJ_FRUSTRUM;
        cobj->fov_top = top;
        cobj->aspect_bottom = bottom;
        cobj->left = left;
        cobj->right = right;
    }
}

//8036A194
void HSD_CObjSetOrtho(HSD_CObj* cobj, f32 top, f32 bottom, f32 left, f32 right){
    if(cobj){
        cobj->projection_type = PROJ_ORTHO;
        cobj->fov_top = top;
        cobj->aspect_bottom = bottom;
        cobj->left = left;
        cobj->right = right;
    }
}

//8036A250
u32 HSD_CObjGetFlags(HSD_CObj* cobj){
    return cobj->flags;
}

//8036A258
void HSD_CObjSetFlags(HSD_CObj* cobj, u32 flags){
    if(cobj)
        cobj->flags |= flags;
}

//8036A270
void HSD_CObjClearFlags(HSD_CObj* cobj, u32 flags){
    if(cobj)
        cobj->flags &= ~flags;
}

//8036A288
HSD_CObj* HSD_CObjGetCurrent(){
    return current_cobj;
}

//8036A290
HSD_CObj* HSD_CObjAlloc(){

}

//8036A2EC
void HSD_CObjLoad(){

}

//8036A55C
void HSD_CObjInit(r3, r4){
    if(r3 && r4)
        HSD_CObjLoad();
}