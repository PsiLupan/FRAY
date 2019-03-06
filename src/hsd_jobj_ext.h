#ifndef _hsd_jobj_ext_h_
#define _hsd_jobj_ext_h_

#include <gctypes.h>

#include "hsd/hsd_jobj.h"
#include "hsd/hsd_robj.h"

#include "gobj.h"

#define GOBJ_HSD_JOBJ(s) ((HSD_JObj*)s->data)

void JObj_SetupInstanceMtx_Callback(HSD_GObj*, u32);

#endif