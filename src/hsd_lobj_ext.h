#ifndef _hsd_lobj_ext_h_
#define _hsd_lobj_ext_h_

#include <gctypes.h>

#include "hsd/hsd_cobj.h"
#include "hsd/hsd_lobj.h"

#include "gobj.h"

#define GOBJ_HSD_LOBJ(s) ((HSD_LObj*)s->data)

HSD_LObj* LObj_LoadLightDescs(HSD_LightDesc**);
void LObj_Setup_Callback(HSD_GObj *);

#endif
