#ifndef _hsd_dobj_h_
#define _hsd_dobj_h_

#include "hsd_jobj.h"
#include "hsd_mobj.h"
#include "hsd_pobj.h"

//Display Object
typedef struct _HSD_DObj {
	HSD_Class parent;
	struct DObj* next;
	struct MObj* mobj;
	struct PObj* pobj;
	s32 unk;
	s32 flags;
} HSD_DObj;

typedef struct _HSD_DObjDesc {
	char* unused_class;
	HSD_DObjDesc* next;
	struct MObjDesc* mobjdesc;
	struct PObjDesc* pobjdesc;
} HSD_DObjDesc;

typedef struct _HSD_DObjInfo {
	
} HSD_DObjInfo;

extern HSD_DObjInfo hsdDObj;

#endif