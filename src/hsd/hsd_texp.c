#include "hsd_texp.h"

#include "hsd_tobj.h"

//80382C00
u32 HSD_TExpGetType(u32* texp){
    if (texp == NULL) {
        return 0;
    }
    //Having debugged this section, this is what the code actually does - Definitely seems suspect
    u32* p = (u32*)(texp + 0x10000);
    if (p == 0xFFFF) {
        return 2;
    }
    if (p == 0xFFFE) {
        return 3;
    }
    return *texp;
}

//80382C3C
void HSD_TExpRef(u8* texp, u8 inc){
    u32 type = HSD_TExpGetType((u32*)texp);
    if(type == 4){
        texp[0x16] += 1;
        return;
    }else if(type != 1){
        return;
    }

    if(inc == TRUE){
        texp[0x8] += 1;
        return;
    }
    texp[0x14] += 1;
}

//80382CC4
void HSD_TExpUnref(u8* texp, u8 inc){
    u32 type = HSD_TExpGetType((u32*)texp);
    if(type == 4){
        if(texp[0x16] != 0){
            texp[0x16] -= 1;
            return;
        }
    }else if (type == 1){
        u32* texp_32 = (u32*)texp;
        if(inc == TRUE){
            if(texp_32[2] != 0){
                texp_32[2] -= 1;
            }
        }else if(texp_32[5] != 0){
            texp_32[5] -= 1;
        }

        if(texp_32[2] == 0 && texp_32[5] == 0){
            for(u32 i = 0; i < 4; i++){
                HSD_TExpUnref((u8*)texp_32[10], texp[0x25]);
                HSD_TExpUnref((u8*)texp_32[18], texp[0x45]);
                texp += 2;
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

//803831BC
HSD_TExp* HSD_TExpCnst(void* color, u32 value, u32 type, HSD_TExp** list){

}

//80384050
void HSD_TExpOrder(HSD_TExp* texp, void* tobj, u8 chan){

}

//803854B4
void HSD_TExpCompile(HSD_TExp* texp, HSD_TExpTevDesc* tdesc, HSD_TExp** list){

}

//80385758
void HSD_TExpFreeTevDesc(HSD_TExpTevDesc* tdesc){
    HSD_TExpTevDesc* next = tdesc;
    while(next != NULL){
        hsdFreeMemPiece(next, sizeof(HSD_TExpTevDesc));
        next = next->next;
    }
}