#ifndef _hsd_lobj_h_
#define _hsd_lobj_h_

#include "hsd_aobj.h"
#include "hsd_cobj.h"
#include "hsd_jobj.h"

typedef struct _HSD_LObj{
	
} HSD_LObj;

typedef struct _HSD_LObjDesc {
	
} HSD_LObjDesc;

typedef struct _HSD_LObjInfo {
	
} HSD_LObjInfo;

extern HSD_LObjInfo hsdLObj;

#define HSD_LOBJ(o)		((HSD_LObj *)(o))
#define HSD_LOBJ_INFO(i)	((HSD_LObjInfo *)(i))
#define HSD_LOBJ_METHOD(o)	HSD_LOBJ_INFO(HSD_CLASS_METHOD(o))



#endif