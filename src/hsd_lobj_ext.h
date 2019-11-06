#ifndef _hsd_lobj_ext_h_
#define _hsd_lobj_ext_h_

#include <gctypes.h>

#include "hsd/hsd_cobj.h"
#include "hsd/hsd_lobj.h"

#include "gobj.h"

#define GOBJ_HSD_LOBJ(s) ((HSD_LObj*)s->data)

typedef struct _AnimList {
    HSD_LightAnim* lightanim;
} AnimList;

typedef struct _LightList {
    HSD_LightDesc* lightdesc;
    AnimList* anims;
} LightList;

typedef struct _HSD_Lights {
    LightList* list;
} HSD_Lights;

HSD_LObj* LObj_LoadLightDescs(HSD_Lights*);
void LObj_Setup_Callback(HSD_GObj*, s32);

#endif
