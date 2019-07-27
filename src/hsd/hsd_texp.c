#include "hsd_texp.h"

#include "hsd_tobj.h"

//80382C00
u32 HSD_TExpGetType(HSD_TExp* texp){ //This is dead serious the fucking code, what the fuck
    if (texp == NULL) {
        return 0;
    }
    if ((u32)texp == 0xFFFF) {
        return 2;
    }
    if ((u32)texp == 0xFFFE) {
        return 3;
    }
    return texp->flags;
}

//80382C3C
void HSD_TExpRef(HSD_TExp* texp, u8 opt){
    u32 type = HSD_TExpGetType(texp);
    if(type == 4){
        texp->t4_ref_count += 1;
        return;
    }else if(type != 1){
        return;
    }

    if(opt == TRUE){
        texp->opt_ref_count += 1;
    }else{
        texp->nopt_ref_count += 1;
    }
}

//80382CC4
void HSD_TExpUnref(HSD_TExp* texp, u8 opt){
    u32 type = HSD_TExpGetType(texp);
    if(type == 4){
        if(texp->t4_ref_count != 0){
            texp->t4_ref_count -= 1;
            return;
        }
    }else if (type == 1){
        if(opt == TRUE){
            if(texp->opt_ref_count != 0){
                texp->opt_ref_count -= 1;
            }
        }else if(texp->nopt_ref_count != 0){
            texp->nopt_ref_count -= 1;
        }

        if(texp->opt_ref_count == 0 && texp->nopt_ref_count == 0){
            for(u32 i = 0; i < 4; i++){
                HSD_TExpUnref(texp->texp_2, texp->texp2_opt);
                HSD_TExpUnref(texp->texp_3, texp->texp3_opt);
                texp = (HSD_TExp*)texp->opt_ref_count;
            }
        }
    }
}

//80382DDC
void HSD_TExpFreeList(HSD_TExp* texp, u32 flags, u8 method){

}

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
    texp->next = *list;
    *list = texp;
    texp->opt_ref_count = 0;
    texp->nopt_ref_count = 0;
    texp->texp_2 = NULL;
    texp->x30_unk = 0;
    texp->x38_unk = 0;
    texp->x40_unk = 0;
    texp->texp_3 = NULL;
    texp->x50_unk = 0;
    texp->x58_unk = 0;
    texp->x60_unk = 0;
    texp->x64_unk = 0;
    return texp;
}

//803831BC
HSD_TExp* HSD_TExpCnst(void* color, u32 value, u32 type, HSD_TExp** list){

}

//80384050
void HSD_TExpOrder(HSD_TExp* texp, void* tobj, u8 chan){

}

//803854B4
void HSD_TExpCompile(HSD_TExp* texp, HSD_TExpTevDesc** tdesc, HSD_TExp** list){

}

//80385758
void HSD_TExpFreeTevDesc(HSD_TExpTevDesc* tdesc){
    HSD_TExpTevDesc* next = tdesc;
    while(next != NULL){
        hsdFreeMemPiece(next, sizeof(HSD_TExpTevDesc));
        next = next->next;
    }
}
