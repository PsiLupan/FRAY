#ifndef _hsd_wobj_h_
#define _hsd_wobj_h_

#include <gctypes.h>
#include <ogc/gu.h>

#include "hsd_object.h"

#include "hsd_aobj.h"
#include "hsd_robj.h"

typedef struct _HSD_WObj {
    struct _HSD_Class class_parent;
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
    HSD_ClassInfo parent;
    int (*load)(HSD_WObj *wobj, HSD_WObjDesc *desc); 
} HSD_WObjInfo;

typedef struct _HSD_InterestAnim {
    struct _HSD_InterestAnim* next;
    struct _HSD_AObjDesc* desc;
    u32 unk;
    u32 unk2;
} HSD_InterestAnim;

typedef struct _HSD_WorldAnim {
    struct _HSD_AObjDesc* aobjdesc;
    struct _HSD_AObjDesc* eye_anim;
    struct _HSD_InterestAnim* interest_anim;
    u32 flags;
} HSD_WorldAnim;

extern HSD_WObjInfo hsdWObj;

#define HSD_WOBJ_INFO(i)	((HSD_WObjInfo *)(i))

#define HSD_WOBJ_METHOD(o)	HSD_WOBJ_INFO(HSD_CLASS_METHOD(o))

#endif