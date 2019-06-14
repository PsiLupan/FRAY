#ifndef _hsd_fog_ext_h_
#define _hsd_fog_ext_h_

#include <gctypes.h>

#include "hsd/hsd_fog.h"

#include "gobj.h"

#define GOBJ_HSD_FOG(s) ((HSD_Fog*)s->data)

void Fog_InterpretAnim_Callback(HSD_GObj *);
void Fog_Set_Callback(HSD_GObj *);

#endif
