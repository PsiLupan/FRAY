#ifndef _hsd_dobj_h_
#define _hsd_dobj_h_

#include <gctypes.h>

#include "hsd_object.h"

//Display Object
typedef struct _HSD_DObj {
	HSD_Class class_parent;
	struct _HSD_DObj* next; //0x04
	struct _HSD_MObj* mobj; //0x08
	struct _HSD_PObj* pobj; //0x0C
	struct _HSD_AObj* aobj; //0x10
	u32 flags; //0x14
	u32 unk;
} HSD_DObj;

typedef struct _HSD_DObjDesc {
	char* class_name;
	struct _HSD_DObjDesc* next;
	struct _HSD_MObjDesc* mobjdesc;
	struct _HSD_PObjDesc* pobjdesc;
} HSD_DObjDesc;

typedef struct _HSD_DObjInfo {
	HSD_ClassInfo parent;
} HSD_DObjInfo;

extern HSD_DObjInfo hsdDObj;

#endif