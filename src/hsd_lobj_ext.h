#ifndef _hsd_lobj_ext_h_
#define _hsd_lobj_ext_h_

#include <gctypes.h>

#include "hsd/hsd_cobj.h"
#include "hsd/hsd_lobj.h"

#include "gobj.h"

#define GOBJ_HSD_LOBJ(s) ((HSD_LObj*)s->data)

typedef struct _HSD_Light {
    HSD_LightDesc* desc;
    struct _HSD_Light* next;
    HSD_LightAnim* anim;
} HSD_Light;

HSD_LObj* LObj_LoadLightDescs(HSD_Light*);
void LObj_Setup_Callback(HSD_GObj *);

#endif
