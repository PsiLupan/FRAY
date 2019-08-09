#ifndef _hsd_jobj_ext_h_
#define _hsd_jobj_ext_h_

#include <gctypes.h>

#include "hsd/hsd_jobj.h"
#include "hsd/hsd_robj.h"

#include "gobj.h"

#define GOBJ_HSD_JOBJ(s) ((HSD_JObj*)s->data)

void JObj_Spawn(HSD_JObj*, guVector*, guVector*);
void JObj_SetAnimSpeed(HSD_JObj*, f32[3]);
f32 JObj_GetFrame(HSD_JObj*);
void JObj_SetupInstanceMtx(HSD_JObj*, MtxP, u32, u32);
void JObj_SetupInstanceMtx_Callback(HSD_GObj*, u32);

#endif
