#include "hsd_robj.h"

//8037AE90
void HSD_RObjSetFlags(HSD_RObj* robj, u32 flags){
    if(robj){
        robj->flags |= flags;
    }
}

//8037AEA8
HSD_RObj* HSD_RObjGetByType(HSD_RObj* robj, u32 flags1, u32 flags2){
    if(robj){
        for(HSD_RObj* curr = robj; curr != NULL; curr = curr->next){
            u32 flags = curr->flags;
            if( (flags & 0x80000000) != 0 && (flags & 0x70000000) == flags1 && (!flags2 || flags2 == (flags & 0xFFFFFFF)) ){
                return curr;
            }
        }
    }
}