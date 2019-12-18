#ifndef _hsd_cobj_ext_h_
#define _hsd_cobj_ext_h_

#include <gctypes.h>

#include "hsd/hsd_cobj.h"
#include "hsd/hsd_display.h"

#include "gobj.h"

#define GOBJ_HSD_COBJ(s) ((HSD_CObj*)s->hsd_obj)

HSD_CObj* CObj_Create(HSD_CObjDesc*);
void CObj_Texture_Callback(HSD_GObj*, s32);
void CObj_SetErase_Callback(HSD_GObj*, s32);

#endif
