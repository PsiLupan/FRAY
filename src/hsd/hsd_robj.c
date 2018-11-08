#include "hsd_robj.h"

//8037AE90
void HSD_RObjSetFlags(HSD_RObj* robj, u32 flags){
    if(robj){
        robj->flags |= flags;
    }
}