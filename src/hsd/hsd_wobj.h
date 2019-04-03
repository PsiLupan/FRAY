#ifndef _hsd_wobj_h_
#define _hsd_wobj_h_

#include <gctypes.h>
#include <ogc/gu.h>

#include "hsd_object.h"

#include "hsd_aobj.h"

typedef struct _HSD_WObj {
    struct _HSD_Class class_parent;
    u32 flags; //0x08
    u32 x; //0x0C
    u32 y; //0x10
    u32 z; //0x14
    struct _HSD_AObj* aobj; //0x18
    struct _HSD_RObj* robj; //0x1C
} HSD_WObj;

typedef struct _HSD_WObjDesc {
    char* class_name;
    struct _HSD_WObjDesc* next;
} HSD_WObjDesc;

typedef struct _HSD_WObjInfo {
    HSD_ClassInfo parent;
} HSD_WObjInfo;

struct _HSD_InterestAnim {
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

#endif