#include "hsd_wobj.h"

#include "hsd_util.h"

#include "hsd_jobj.h"

static void HSD_WObjInfoInit();

static HSD_WObjInfo* default_class = NULL; 

HSD_WObjInfo hsdWObj = { HSD_WObjInfoInit };

//8037D050
void HSD_WObjRemoveAnim(HSD_WObj* wobj){
    if(wobj){
        HSD_AObjRemove(wobj->aobj);
        wobj->aobj = NULL;
        HSD_RObjRemoveAnimAll(wobj->robj);
    }
}

//8037D094
void HSD_WObjReqAnim(HSD_WObj* wobj, f32 frame){
    if(wobj){
        HSD_AObjReqAnim(wobj->aobj, frame);
        HSD_RObjReqAnimAll(wobj->robj, frame);
    }
}

//8037D0E4
void HSD_WObjAddAnim(HSD_WObj* wobj, HSD_WorldAnim* anim){
    if(wobj != NULL && anim != NULL){
        if(wobj->aobj != NULL){
            HSD_AObjRemove(wobj->aobj);
        }
        wobj->aobj = HSD_AObjLoadDesc(anim->aobjdesc);
        HSD_RObjAddAnimAll(wobj->robj, NULL); //Never not NULL and since I can't figure out the struct, fuck it
    }
}

//8037D14C
static void WObjUpdateFunc(HSD_WObj* wobj, u32 type, f32* fval){
    if(wobj != NULL){
        switch(type) {
            case 4: {
                if(*fval < 0.0f){
                    *fval = 0.0f;
                }
                if(*fval > 1.0f){
                    *fval = 1.0f;
                }

                assert(wobj->aobj != NULL);
                HSD_FObj* fobj = wobj->aobj->fobj;
                assert(fobj != NULL);

                guVector pos;
                splArcLengthPoint(&pos, &fobj->startframe, *fval);
                HSD_WObjSetPosition(wobj, &pos);
                wobj->flags |= 1;
            }
            break;

            case 5:
                HSD_WObjSetPositionX(wobj, *fval);
            break;

            case 6:
                HSD_WObjSetPositionY(wobj, *fval);
            break;

            case 7:
                HSD_WObjSetPositionZ(wobj, *fval);
            break;
        }
    }
}

//8037D29C
void HSD_WObjInterpretAnim(HSD_WObj* wobj){
    if(wobj != NULL){
        HSD_AObjInterpretAnim(wobj->aobj, wobj, WObjUpdateFunc);
        HSD_RObjAnimAll(wobj->robj);
    }
}

//8037D2E4
static int WObjLoad(HSD_WObj* wobj, HSD_WObjDesc* desc){
    HSD_WObjSetPosition(wobj, &desc->pos);
    if(wobj->robj != NULL){
        HSD_RObjRemoveAll(wobj->robj);
    }
    HSD_RObj* robj = HSD_RObjLoadDesc(desc->robjdesc);
    wobj->robj = robj;
    HSD_RObjResolveRefsAll(wobj->robj, desc->robjdesc);
    return 0;
}

//8037D34C
void HSD_WObjInit(HSD_WObj* wobj, HSD_WObjDesc* desc){
    if(wobj != NULL & desc != NULL){
        HSD_WObjSetPosition(wobj, &desc->pos);
        if(wobj->robj != NULL){
            HSD_RObjRemoveAll(wobj->robj);
        }
        HSD_RObj* robj = HSD_RObjLoadDesc(desc->robjdesc);
        wobj->robj = robj;
        HSD_RObjResolveRefsAll(wobj->robj, desc->robjdesc);
    }
}

//8037D3C4
HSD_WObj* HSD_WObjLoadDesc(HSD_WObjDesc* desc){
    if(desc != NULL){
        HSD_WObj* wobj;
        HSD_ClassInfo* info;
        if (desc->class_name == NULL || !(info = hsdSearchClassInfo(desc->class_name))){
            wobj = HSD_WObjAlloc();
        }else{
            wobj = (HSD_WObj*)hsdNew(info);
            assert(wobj);
        }
        HSD_WOBJ_METHOD(wobj)->load(wobj, desc);
        return wobj;
    }
    return NULL;
}

//8037D45C
void HSD_WObjSetPosition(HSD_WObj* wobj, guVector* pos){
    if(wobj != NULL && pos != NULL){
        wobj->pos.x = pos->x;
        wobj->pos.y = pos->y;
        wobj->pos.z = pos->z;
        wobj->flags |= 0x2;
        wobj->flags &= 0xFFFFFFFE;
    }
}

//8037D4A4
void HSD_WObjSetPositionX(HSD_WObj* wobj, f32 x){
    if(wobj != NULL){
        if(wobj->flags & 1){
            if(wobj->aobj != NULL){
                HSD_JObj* jobj = (HSD_JObj*)wobj->aobj->hsd_obj;
                if(jobj != NULL){
                    if ((jobj->flags & 0x800000) == 0 && (jobj->flags & 0x40) != 0){
                        HSD_JObjSetupMatrixSub(jobj);
                    }
                    guVecMultiply(jobj->mtx, &wobj->pos, &wobj->pos);
                }
            }
            wobj->flags &= 0xFFFFFFFE;
        }
        wobj->pos.x = x;
        wobj->flags |= 0x2;
    }
}

//8037D578
void HSD_WObjSetPositionY(HSD_WObj* wobj, f32 y){
    if(wobj != NULL){
        if(wobj->flags & 1){
            if(wobj->aobj != NULL){
                HSD_JObj* jobj = (HSD_JObj*)wobj->aobj->hsd_obj;
                if(jobj != NULL){
                    if ((jobj->flags & 0x800000) == 0 && (jobj->flags & 0x40) != 0){
                        HSD_JObjSetupMatrixSub(jobj);
                    }
                    guVecMultiply(jobj->mtx, &wobj->pos, &wobj->pos);
                }
            }
            wobj->flags &= 0xFFFFFFFE;
        }
        wobj->pos.y = y;
        wobj->flags |= 0x2;
    }
}

//8037D64C
void HSD_WObjSetPositionZ(HSD_WObj* wobj, f32 z){
    if(wobj != NULL){
        if(wobj->flags & 1){
            if(wobj->aobj != NULL){
                HSD_JObj* jobj = (HSD_JObj*)wobj->aobj->hsd_obj;
                if(jobj != NULL){
                    if ((jobj->flags & 0x800000) == 0 && (jobj->flags & 0x40) != 0){
                        HSD_JObjSetupMatrixSub(jobj);
                    }
                    guVecMultiply(jobj->mtx, &wobj->pos, &wobj->pos);
                }
            }
            wobj->flags &= 0xFFFFFFFE;
        }
        wobj->pos.z = z;
        wobj->flags |= 0x2;
    }
}

//8037D720
void HSD_WObjGetPosition(HSD_WObj* wobj, guVector* pos){
    if(wobj != NULL && pos != NULL){
        if(wobj->flags & 1){
            if(wobj->aobj != NULL){
                HSD_JObj* jobj = (HSD_JObj*)wobj->aobj->hsd_obj;
                if(jobj != NULL){
                    if ((jobj->flags & 0x800000) == 0 && (jobj->flags & 0x40) != 0){
                        HSD_JObjSetupMatrixSub(jobj);
                    }
                    guVecMultiply(jobj->mtx, &wobj->pos, &wobj->pos);
                }
            }
            wobj->flags &= 0xFFFFFFFE;
        }
        pos->x = wobj->pos.x;
        pos->y = wobj->pos.y;
        pos->z = wobj->pos.z;
    }
}

//8037D808
HSD_WObj* HSD_WObjAlloc(){
    HSD_ClassInfo* info = (HSD_ClassInfo*)(default_class != NULL ? (HSD_ClassInfo*)default_class : (HSD_ClassInfo*)&hsdWObj);
    HSD_WObj* wobj = (HSD_WObj*)hsdNew(info);
	assert(wobj != NULL);
    return wobj;
}

//8037D864
static void WObjRelease(HSD_Class* o){
    HSD_WObj* wobj = (HSD_WObj*)o;
    HSD_RObjRemoveAll(wobj->robj);
    HSD_AObjRemove(wobj->aobj);
    HSD_PARENT_INFO(&hsdWObj)->release(o);
}

//8037D8B8
static void WObjAmnesia(HSD_ClassInfo* info){
    if(info == default_class){
        default_class = NULL;
    }
    HSD_PARENT_INFO(&hsdWObj)->amnesia(info);
}

//8037D900
static void HSD_WObjInfoInit(){
    hsdInitClassInfo(HSD_CLASS_INFO(&hsdWObj), HSD_CLASS_INFO(&hsdClass), HSD_BASE_CLASS_LIBRARY, "hsd_wobj", sizeof(HSD_WObjInfo), sizeof(HSD_WObj));
    HSD_CLASS_INFO(&hsdWObj)->release = WObjRelease;
	HSD_CLASS_INFO(&hsdWObj)->amnesia = WObjAmnesia;
    HSD_WOBJ_INFO(&hsdWObj)->load = WObjLoad;
}

void HSD_WObjUnref(HSD_WObj* wobj){
    if(wobj != NULL){
        u16 ref_count = wobj->class_parent.ref_count;
		u32 lz = __builtin_clz(0xFFFF - ref_count);
		lz = lz >> 5;
		if(lz == 0){
			wobj->class_parent.ref_count -= 1;
            lz = __builtin_clz(-ref_count);
			lz = lz >> 5;
		}
        if(lz != 0){
            HSD_INFO_METHOD(wobj)->release((HSD_Class*)wobj);
        }
    }
}
