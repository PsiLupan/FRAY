#include "hsd_fog_ext.h"

//801A1A18
void Fog_InterpretAnim_Callback(HSD_GObj* gobj){
    HSD_Fog* fog = GOBJ_HSD_FOG(gobj);
    HSD_FogInterpretAnim(fog);
}

//803910B4
void Fog_Set_Callback(HSD_GObj* gobj){
    HSD_Fog* fog = GOBJ_HSD_FOG(gobj);
    HSD_FogSet(fog);
}