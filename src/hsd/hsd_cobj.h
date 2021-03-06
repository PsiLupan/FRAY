#ifndef _hsd_cobj_h_
#define _hsd_cobj_h_

#include <gctypes.h>
#include <ogc/gu.h>

#include "hsd_aobj.h"
#include "hsd_wobj.h"

#define PROJ_PERSPECTIVE 1
#define PROJ_FRUSTRUM 2
#define PROJ_ORTHO 3

typedef struct _HSD_CObj {
    HSD_Obj parent;
    u32 flags; //0x08
    f32 viewport_left; //0x0C
    f32 viewport_right; //0x10
    f32 viewport_top; //0x14
    f32 viewport_bottom; //0x18
    u16 scissor_left; //0x1C
    u16 scissor_right; //0x1E
    u16 scissor_top; //0x20
    u16 scissor_bottom; //0x22
    HSD_WObj* eye_position; //0x24
    HSD_WObj* interest; //0x28
    union {
        f32 roll; //0x2C
        guVector up; //0x2C - 0x34
    } u;
    f32 near; //0x38
    f32 far; //0x3C
    union {
        struct {
            f32 fov;
            f32 aspect;
        } perspective;
        
        struct {
            f32 top;
            f32 bottom;
            f32 left;
            f32 right;
        } frustrum;
        
        struct {
            f32 top;
            f32 bottom;
            f32 left;
            f32 right;
        } ortho;
    } projection_param;
    u8 projection_type; //0x50
    Mtx view_mtx; //0x54
    HSD_AObj* aobj; //0x84
    MtxP proj_mtx; //0x88
} HSD_CObj;

typedef struct _HSD_CObjDesc {
    char* class_name; //0x00
    u16 flags; //0x04
    u16 projection_type; //0x06
    u16 viewport_left; //0x08
    u16 viewport_right; //0x0A
    u16 viewport_top; //0x0C
    u16 viewport_bottom; //0x0E
    u32 scissor_lr; //0x10
    u32 scissor_tb; //0x14
    struct _HSD_WObjDesc* eye_desc; //0x18
    struct _HSD_WObjDesc* interest_desc; //0x1C
    f32 roll; //0x20
    guVector* vector; //0x24
    f32 near; //0x28
    f32 far; //0x2C
    union {
        struct {
            f32 fov;
            f32 aspect;
        } perspective;
        
        struct {
            f32 top;
            f32 bottom;
            f32 left;
            f32 right;
        } frustrum;
        
        struct {
            f32 top;
            f32 bottom;
            f32 left;
            f32 right;
        } ortho;
    } projection_param;
} HSD_CObjDesc;

typedef struct _HSD_CObjInfo {
    HSD_ObjInfo parent;
    int (*load)(HSD_CObj* cobj, HSD_CObjDesc* desc);
} HSD_CObjInfo;

typedef struct _HSD_CameraAnim {
    struct _HSD_AObjDesc* aobjdesc;
    struct _HSD_WObjAnim* eye_anim;
    struct _HSD_WObjAnim* interest_anim;
} HSD_CameraAnim;

#define HSD_COBJ(o) ((HSD_CObj*)(o))
#define HSD_COBJ_INFO(i) ((HSD_CObjInfo*)(i))
#define HSD_COBJ_METHOD(o) HSD_COBJ_INFO(HSD_OBJECT_METHOD(o))

void HSD_CObjEraseScreen(HSD_CObj*, s32, s32, s32);
void HSD_CObjRemoveAnim(HSD_CObj*);
void HSD_CObjAddAnim(HSD_CObj*, HSD_CameraAnim*);
void CObjUpdateFunc(HSD_CObj*, u32, FObjData*);
void HSD_CObjAnim(HSD_CObj*);
void HSD_CObjReqAnim(HSD_CObj*, f32);
u8 makeProjectionMtx(HSD_CObj*, Mtx44);
BOOL HSD_CObjSetCurrent(HSD_CObj*);
void HSD_CObjEndCurrent(void);
HSD_WObj* HSD_CObjGetInterestWObj(HSD_CObj*);
HSD_WObj* HSD_CObjGetEyePositionWObj(HSD_CObj*);
void HSD_CObjGetInterest(HSD_CObj*, guVector*);
void HSD_CObjSetInterest(HSD_CObj*, guVector);
void HSD_CObjGetEyePosition(HSD_CObj*, guVector*);
void HSD_CObjSetEyePosition(HSD_CObj*, guVector);
s32 HSD_CObjGetEyeVector(HSD_CObj*, guVector*);
f32 HSD_CObjGetEyeDistance(HSD_CObj*);
s32 HSD_CObjGetUpVector(HSD_CObj*, guVector*);
void HSD_CObjSetUpVector(HSD_CObj*, guVector*);
void HSD_CObjSetMtxDirty(HSD_CObj*);
MtxP HSD_CObjGetViewingMtxPtrDirect(HSD_CObj*);
BOOL HSD_CObjMtxIsDirty(HSD_CObj*);
MtxP HSD_CObjGetInvViewingMtxPtrDirect(HSD_CObj*);

void HSD_CObjSetRoll(HSD_CObj*, f32);

f32 HSD_CObjGetFov(HSD_CObj*);
void HSD_CObjSetFov(HSD_CObj*, f32);
f32 HSD_CObjGetAspect(HSD_CObj*);
void HSD_CObjSetAspect(HSD_CObj*, f32);
f32 HSD_CObjGetTop(HSD_CObj*);
void HSD_CObjSetTop(HSD_CObj*, f32);
f32 HSD_CObjGetBottom(HSD_CObj*);
void HSD_CObjSetBottom(HSD_CObj*, f32);
f32 HSD_CObjGetLeft(HSD_CObj*);
void HSD_CObjSetLeft(HSD_CObj*, f32);
f32 HSD_CObjGetRight(HSD_CObj*);
void HSD_CObjSetRight(HSD_CObj*, f32);
f32 HSD_CObjGetNear(HSD_CObj*);
void HSD_CObjSetNear(HSD_CObj*, f32);
f32 HSD_CObjGetFar(HSD_CObj*);
void HSD_CObjSetFar(HSD_CObj*, f32);
void HSD_CObjGetScissor(HSD_CObj*, u16[4]);
void HSD_CObjSetScissor(HSD_CObj*, u16[4]);
void HSD_CObjSetScissorx4(HSD_CObj*, u16, u16, u16, u16);
void HSD_CObjGetViewportf(HSD_CObj*, f32[4]);
void HSD_CObjSetViewport(HSD_CObj*, u16[4]);
void HSD_CObjSetViewportf(HSD_CObj*, f32[4]);
void HSD_CObjSetViewportfx4(HSD_CObj*, u16, u16, u16, u16);
u8 HSD_CObjGetProjectionType(HSD_CObj*);
void HSD_CObjSetProjectionType(HSD_CObj*, u8);
void HSD_CObjSetPerspective(HSD_CObj*, f32, f32);
void HSD_CObjSetFrustrum(HSD_CObj*, f32, f32, f32, f32);
void HSD_CObjSetOrtho(HSD_CObj*, f32, f32, f32, f32);
void HSD_CObjGetPerspective(HSD_CObj*, f32*, f32*);
void HSD_CObjGetOrtho(HSD_CObj*, f32*, f32*, f32*, f32*);
u32 HSD_CObjGetFlags(HSD_CObj*);
void HSD_CObjSetFlags(HSD_CObj*, u32);
void HSD_CObjClearFlags(HSD_CObj*, u32);
HSD_CObj* HSD_CObjGetCurrent(void);
HSD_CObj* HSD_CObjAlloc(void);
void HSD_CObjSetDefaultClass(HSD_CObjInfo*);
HSD_CObjInfo* HSD_CObjGetDefaultClass(void);
HSD_CObj* HSD_CObjLoadDesc(HSD_CObjDesc*);

#endif
