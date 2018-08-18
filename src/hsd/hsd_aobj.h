#ifndef _hsd_aobj_h_
#define _hsd_aobj_h_

#include "hsd_fobj.h"

//Animation Object
typedef struct _HSD_AObj {
	s32 unused_class;
	s32 flags;
	s32 curr_frame;
	s32 rewind_frame;
	s32 end_frame;
	s32 framerate;
	struct FObj* fobj;
} HSD_AObj;

typedef struct _HSD_AObjDesc {
	s32 unused_class;
	s32 end_frame;
	struct _HSD_FObjDesc* fobjdesc;
	s32 unk;
} HSD_AObjDesc;

#endif