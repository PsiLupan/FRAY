#ifndef _hsd_aobj_h_
#define _hsd_aobj_h_

#include "hsd_object.h"

#include "hsd_fobj.h"
#include "hsd_jobj.h"

//Animation Object
typedef struct _HSD_AObj {
	u32 flags;
	f32 curr_frame;
	f32 rewind_frame;
	f32 end_frame;
	f32 framerate;
	struct _HSD_FObj* fobj;
	void* hsd_obj;
} HSD_AObj;

typedef struct _HSD_AObjDesc {
	u32 flags;
	f32 end_frame;
	struct _HSD_FObjDesc* fobjdesc;
	u32 obj_id;
} HSD_AObjDesc;

#define HSD_AOBJ(o)		((HSD_AObj *)(o))

#endif