#ifndef _hsd_dobj_h_
#define _hsd_dobj_h_

#include <gctypes.h>

#include "hsd_jobj.h"
#include "hsd_mobj.h"
#include "hsd_pobj.h"

//Display Object
typedef struct _HSD_DObj {
	struct _HSD_Obj class_parent;
	struct _HSD_DObj* next;
	struct _HSD_MObj* mobj;
	struct _HSD_PObj* pobj;
	u32 unk;
	u32 flags;
} HSD_DObj;

typedef struct _HSD_DObjDesc {
	char* class_name;
	struct _HSD_DObjDesc* next;
	struct _HSD_MObjDesc* mobjdesc;
	struct _HSD_PObjDesc* pobjdesc;
} HSD_DObjDesc;

typedef struct _HSD_DObjInfo {
	
} HSD_DObjInfo;

extern HSD_DObjInfo hsdDObj;

#endif