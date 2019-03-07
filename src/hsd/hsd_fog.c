#include "hsd_fog.h"

//8037DE7C
void HSD_FogAddAnim(HSD_Fog* fog, HSD_AObjDesc* aodesc){
    if(fog != NULL){
        if(fog->aobj != NULL){
            HSD_AObjRemove(fog->aobj);
        }
        fog->aobj = HSD_AObjLoadDesc(aodesc);
    }
}