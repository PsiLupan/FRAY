#ifndef _hsd_wobj_h_
#define _hsd_wobj_h_

#include <gctypes.h>
#include <ogc/gu.h>

#include "hsd_object.h"

#include "hsd_aobj.h"
#include "hsd_robj.h"

typedef struct _HSD_WObj {
    HSD_Obj parent;
    u32 flags; //0x08
    guVector pos; //0xC
    struct _HSD_AObj* aobj; //0x18
    struct _HSD_RObj* robj; //0x1C
} HSD_WObj;

typedef struct _HSD_WObjDesc {
    char* class_name;
    guVector pos; //0x04
    struct _HSD_RObjDesc* robjdesc; //0x10
    struct _HSD_WObjDesc* next;
} HSD_WObjDesc;

typedef struct _HSD_WObjInfo {
    HSD_ObjInfo parent;
    int (*load)(HSD_WObj* wobj, HSD_WObjDesc* desc);
} HSD_WObjInfo;

typedef struct _HSD_WObjAnim {
    struct _HSD_AObjDesc* aobjdesc;
    struct _HSD_RObjAnim* robjanim;
} HSD_WObjAnim;

extern HSD_WObjInfo hsdWObj;

#define HSD_WOBJ_INFO(i) ((HSD_WObjInfo*)(i))

#define HSD_WOBJ_METHOD(o) HSD_WOBJ_INFO(HSD_OBJECT_METHOD(o))

void HSD_WObjRemoveAnim(HSD_WObj*);
void HSD_WObjReqAnim(HSD_WObj*, f32);
void HSD_WObjAddAnim(HSD_WObj*, HSD_WObjAnim*);
void HSD_WObjInterpretAnim(HSD_WObj*);
void HSD_WObjInit(HSD_WObj*, HSD_WObjDesc*);
HSD_WObj* HSD_WObjLoadDesc(HSD_WObjDesc*);
void HSD_WObjSetPosition(HSD_WObj*, guVector*);
void HSD_WObjSetPositionX(HSD_WObj*, f32);
void HSD_WObjSetPositionY(HSD_WObj*, f32);
void HSD_WObjSetPositionZ(HSD_WObj*, f32);
void HSD_WObjGetPosition(HSD_WObj*, guVector*);
HSD_WObj* HSD_WObjAlloc();
void HSD_WObjUnref(HSD_WObj* wobj);

#endif