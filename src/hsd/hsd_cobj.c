#include "hsd_cobj.h"

static HSD_CObjInfo hsdCObj = { CObjInfoInit };

static HSD_CObjInfo *default_class = NULL;

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
        HSD_WObjAddAnim(HSD_CObjGetEyePositionWObj(cobj), aobjdesc->0x04);
        HSD_WObjAddAnim(HSD_CObjGetInterestWObj(cobj), aobjdesc->0x08);
    }
}

//80367948
void CObjUpdateFunc(HSD_CObj* cobj, u32 type, f32* val){
    if(cobj){
        switch(type){
            case 1:
            guVector vec;
            HSD_CObjGetEyePosition(cobj, vec);
            vec.x = *val;
            HSD_CObjSetEyePosition(cobj, vec);
            break;

            case 2:
            guVector vec;
            HSD_CObjGetEyePosition(cobj, vec);
            vec.y = *val;
            HSD_CObjSetEyePosition(cobj, vec);
            break;

            case 3:
            guVector vec;
            HSD_CObjGetEyePosition(cobj, vec);
            vec.z = *val;
            HSD_CObjSetEyePosition(cobj, vec);
            break;

            case 5:
            case 6:
            case 7:
            guVector vec;
            HSD_CObjGetInterest(cobj, vec);
            vec.x = *val;
            HSD_CObjSetInterest(cobj, vec);
            break;

            case 9:
            HSD_CObjSetRoll(cobj, *val);
            break;

            case 10:
            HSD_CObjSetFov(cobj, *val);
            break;

            case 11:
            HSD_CObjSetNear(cobj, *val);
            break;

            case 12:
            HSD_CObjSetFar(cobj, *val);
            break;

            default:
            break;
        }
    }
}

//80367AB8
void HSD_CObjAnim(HSD_CObj* cobj){
    if(cobj != NULL){
        HSD_AObjInterpretAnim(cobj->aobj, cobj, CObjUpdateFunc);
        HSD_WObjInterpretAnim(cobj->eye_position, cobj);
        HSD_WObjInterpretAnim(cobj->interest, cobj);
    }
}

//80367B68
bool makeProjectionMtx(HSD_CObj* cobj, Mtx44 mtx){
    bool isOrtho;
    switch(cobj->projection_type){
        case PROJ_PERSPECTIVE:
        isOrtho = false;
        guPerspective(mtx, cobj->fov_top, cobj->aspect_bottom, cobj->near, cobj->far);
        break;
        
        case PROJ_FRUSTRUM:
        isOrtho = false;
        guFrustum(mtx, cobj->fov_top, cobj->aspect_bottom, cobj->proj_left, cobj->proj_right, cobj->near, cobj->far);
        break;
        
        case PROJ_ORTHO:
        isOrtho = true;
        guOrtho(mtx, cobj->fov_top, cobj->aspect_bottom, cobj->proj_left, cobj->proj_right, cobj->near, cobj->far);
        break;

        default:
        assert(true);
        break;
    }
}

//80368608
void HSD_CObjEndCurrent(){
    _HSD_ZListSort();
    _HSD_ZListDisp();
}

//8036862C
HSD_WObj* HSD_CObjGetInterestWObj(HSD_CObj* cobj){
    assert(cobj);
    return cobj->interest;
}

//8036866C
HSD_WObj* HSD_CObjGetEyePositionWObj(HSD_CObj* cobj){
    assert(cobj);
    return cobj->eye_position;
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

//80368784
void HSD_CObjGetEyePosition(HSD_CObj* cobj, guVector pos){
    assert(cobj);
    HSD_WObjGetPosition(cobj->eye_position, pos);
}

//803687F0
void HSD_CObjSetEyePosition(HSD_CObj* cobj, guVector pos){
    assert(cobj);
    HSD_WObjSetPosition(cobj->eye_position, pos);
}

//8036885C
bool HSD_CObjGetEyeVector(HSD_CObj* cobj, guVector* vec){
    if(cobj){
        if(cobj->eye_position && cobj->interest && vec != NULL){
            guVector eye_pos;
            guVector interest_pos;
            HSD_WObjGetPosition(cobj->eye_position, eye_pos);
            HSD_WObjGetPosition(cobj->interest, interest_pos);

            guVecSub(&interest_pos, &eye_pos, vec);

            bool denormalized = TRUE;
            if((u32)vec->x & 0x7FFFFFFF > 1.17549435E-38f 
                    && (u32)vec->y & 0x7FFFFFFF > 1.17549435E-38f
                    && (u32)vec->z & 0x7FFFFFFF > 1.17549435E-38f){
                guVecNormalize(vec);
                denormalized = FALSE;
            }else{
                denormalized = TRUE;
            }

            if(denormalized == TRUE){
                vec->x = 0.0f;
                vec->y = 0.0f;
                vec->z = -1.0f;
            }
            return denormalized;
        }
    }
}

//80368A08
void HSD_CObjGetEyeDistance(HSD_CObj* cobj){

}

//80369564
void HSD_CObjSetMtxDirty(HSD_CObj* cobj){
    cobj->flags |= 0xC0000000;
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
    if(cobj){
        if(cobj->projection_type == PROJ_FRUSTRUM || cobj->projection_type == PROJ_ORTHO){
            return cobj->fov_top;
        }
        if(cobj->projection_type == PROJ_PERSPECTIVE){
          return cobj->near * tan(0.5f * (cobj->fov_top * 0.017453292f));
        }
    }
    return 0.0f;
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
            cobj->proj_left = left;
    }
}

//80369EB8
f32 HSD_CObjGetRight(HSD_CObj* cobj){

}

//80369F54
void HSD_CObjSetRight(HSD_CObj* cobj, f32 right){
    if(cobj){
        if(cobj->projection_type == PROJ_FRUSTRUM || cobj->projection_type == PROJ_ORTHO)
            cobj->proj_right = right;
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

//80369FD8
void HSD_CObjGetScissor(HSD_CObj* cobj, u16 scissors[4]){
    if(cobj){
        scissors[0] = cobj->scissor_left;
        scissors[1] = cobj->scissor_right;
        scissors[2] = cobj->scissor_top;
        scissors[3] = cobj->scissor_bottom;
    }
}

//80369FF4
void HSD_CObjSetScissor(HSD_CObj* cobj, u16 scissors[4]){
    if(cobj){
        cobj->scissor_left = scissors[0];
        cobj->scissor_right = scissors[1];
        cobj->scissor_top = scissors[2];
        cobj->scissor_bottom = scissors[3];
    }
}

//8036A010
void HSD_CObjSetScissorx4(HSD_CObj* cobj, u16 left, u16 right, u16 top, u16 bottom){
    if(cobj){
        cobj->scissor_left = left;
        cobj->scissor_right = right;
        cobj->scissor_top = top;
        cobj->scissor_bottom = bottom;
    }
}

//8036A02C
void HSD_CObjGetViewportf(HSD_CObj* cobj, f32 viewports[4]){
    if(cobj){
        viewports[0] = cobj->viewport_left;
        viewports[1] = cobj->viewport_right;
        viewports[2] = cobj->viewport_top;
        viewports[3] = cobj->viewport_bottom;
    }
}

//8036A0E4
void HSD_CObjSetViewportf(HSD_CObj* cobj, f32 viewports[4]){
    if(cobj){
        cobj->viewport_left = viewports[0];
        cobj->viewport_right = viewports[1];
        cobj->viewport_top = viewports[2];
        cobj->viewport_bottom = viewports[3];
    }
}

//8036A110
void HSD_CObjSetViewportfx4(HSD_CObj* cobj, u16 left, u16 right, u16 top, u16 bottom){
    if(cobj){
        cobj->viewport_left = left;
        cobj->viewport_right = right;
        cobj->viewport_top = top;
        cobj->viewport_bottom = bottom;
    }
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
        cobj->proj_left = left;
        cobj->proj_right = right;
    }
}

//8036A194
void HSD_CObjSetOrtho(HSD_CObj* cobj, f32 top, f32 bottom, f32 left, f32 right){
    if(cobj){
        cobj->projection_type = PROJ_ORTHO;
        cobj->fov_top = top;
        cobj->aspect_bottom = bottom;
        cobj->proj_left = left;
        cobj->proj_right = right;
    }
}

//8036A1B8
void HSD_CObjGetPerspective(HSD_CObj* cobj, f32* top, f32* bottom){
    if(cobj){
        if(cobj->projection_type == PROJ_PERSPECTIVE){
            if(top != NULL)
                *top = cobj->fov_top;
            if(bottom != NULL)
                *bottom = cobj->aspect_bottom;
        }
    }
}

//8036A1F4
void HSD_CObjGetOrtho(HSD_CObj* cobj, f32* top, f32* bottom, f32* left, f32* right){
    if(cobj){
        if(cobj->projection_type == PROJ_ORTHO){
            if(top != NULL)
                *top = cobj->fov_top;
            if(bottom != NULL)
                *bottom = cobj->aspect_bottom;
            if(left != NULL)
                *left = cobj->proj_left;
            if(right != NULL)
                *right = cobj->proj_right;
            
        }
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
    HSD_CObj* cobj = hsdNew(HSD_CObjGetDefaultClass());
    assert(cobj != NULL);
    return cobj;
}

//8036A2EC
static int HSD_CObjLoad(){
    //TODO
}

void HSD_CObjSetDefaultClass(HSD_CObjInfo *info){
	default_class = info;
}

HSD_CObjInfo* HSD_CObjGetDefaultClass(){
	return default_class ? default_class : &hsdCObj;
}

//8036A55C
static void CObjInit(HSD_Class *o){
    HSD_PARENT_INFO(&hsdCObj)->init(o);
    
    if(o != NULL){
        HSD_CObj* cobj = HSD_COBJ(o);
        cobj->flags |= 0xC000;

        cobj->eye_position = HSD_WObjAlloc();
        cobj->interest = HSD_WObjAlloc();
    }
}

//8036A6C8
static void CObjRelease(HSD_Class* o){
    HSD_CObj *cobj = HSD_COBJ(o);

    HSD_AObjRemove(cobj->aobj);

    HSD_WObj* wobj = cobj->eye_position;
    if(wobj != NULL){
        wobj->class_parent.ref_count -= 1;
        if(wobj->class_parent.ref_count == 0){
            wobj->class_parent.class_init->release(wobj);
            wobj->class_parent.class_init->destroy(wobj);
        }
    }

    wobj = cobj->interest;
    if(wobj != NULL){
        wobj->class_parent.ref_count -= 1;
        if(wobj->class_parent.ref_count == 0){
            wobj->class_parent.class_init->release(wobj);
            wobj->class_parent.class_init->destroy(wobj);
        }
    }

    if(cobj->proj_mtx != NULL){
        HSD_MtxFree(cobj->proj_mtx);
    }

    HSD_PARENT_INFO(&hsdCObj)->release(o);
}

//8036A85C
static void CObjAmnesia(HSD_ClassInfo *info){
	if (info == HSD_CLASS_INFO(default_class)) {
		default_class = NULL;
	}
	if (info == HSD_CLASS_INFO(&hsdCObj)) {
		current_cobj = NULL;
	}
	HSD_PARENT_INFO(&hsdCObj)->amnesia(info);
}

//8036A8BC
static void CObjInfoInit(){
    hsdInitClassInfo(HSD_CLASS_INFO(&hsdCObj), HSD_CLASS_INFO(&hsdClass), HSD_BASE_CLASS_LIBRARY, "hsd_cobj", sizeof(HSD_CObjInfo), sizeof(HSD_CObj));

    HSD_CLASS_INFO(&hsdCObj)->init = CObjInit;
    HSD_CLASS_INFO(&hsdCObj)->release = CObjRelease;
    HSD_CLASS_INFO(&hsdCObj)->amnesia = CObjAmnesia;
    HSD_COBJ_INFO(&hsdCObj)->load = HSD_CObjLoad;
}