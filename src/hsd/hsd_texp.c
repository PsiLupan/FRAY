#include "hsd_texp.h"

//803830FC
HSD_TExp* HSD_TExpTev(HSD_TExp** list){
    if(list == NULL){
        HSD_Halt("HSD_TExpTev: List is NULL");
    }
    
    HSD_TExp* texp = (HSD_TExp*)hsdAllocMemPiece(sizeof(HSD_TExp));
    if(texp == NULL){
        HSD_Halt("HSD_TExpTev: No free memory");
    }

    memset(texp, 0xFF, sizeof(HSD_TExp));
    texp->flags = 1;
    texp->next = (HSD_TExp*)list[0];
    list[0] = texp;
    texp->x8_unk = 0;
    texp->x14_unk = 0;
    texp->x28_unk = 0;
    texp->x30_unk = 0;
    texp->x38_unk = 0;
    texp->x40_unk = 0;
    texp->x48_unk = 0;
    texp->x50_unk = 0;
    texp->x58_unk = 0;
    texp->x60_unk = 0;
    texp->x64_unk = 0;
    return texp;
}

