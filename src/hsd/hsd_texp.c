#include "hsd_texp.h"

//803830FC
HSD_TExp* HSD_TExpTev(HSD_TExp** list){
    if(list == NULL){
        HSD_Halt("HSD_TExpTev: List is NULL");
    }
    
    u32* piece = (u32*)hsdAllocMemPiece(0x6C);
    if(piece == NULL){
        HSD_Halt("HSD_TExpTev: No free memory");
    }

    memset(piece, 0xFF, 0x6C);
    piece[0] = 1;
    piece[1] = (u32*)list[0];
    list[0] = (HSD_TExp*)piece;
    piece[2] = 0;
    piece[5] = 0;
    piece[10] = 0;
    piece[12] = 0;
    piece[14] = 0;
    piece[16] = 0;
    piece[18] = 0;
    piece[20] = 0;
    piece[22] = 0;
    piece[24] = 0;
    piece[25] = 0;
    return (HSD_TExp*)piece;
}