#include "hsd_texp.h"

#include "hsd_state.h"
#include "hsd_tobj.h"

static u32 num_texgens = 0; //r13_40A4

//80362478
void HSD_StateRegisterTexGen(u32 coord){
    u32 num = HSD_TexCoordID2Num(coord);
    if(num_texgens < num){
        num_texgens = num;
    }
}

//803624A8
void HSD_StateSetNumTexGens(){
    GX_SetNumTexGens(num_texgens);
    num_texgens = 0;
}

//803629D8
void HSD_SetTevRegAll(){

}

//80362AA4
u32 HSD_TexCoordID2Num(u32 coord){
    switch(coord){
        case GX_TEXCOORD0: return 1;
        case GX_TEXCOORD1: return 2;
        case GX_TEXCOORD2: return 3;
        case GX_TEXCOORD3: return 4;
        case GX_TEXCOORD4: return 5;
        case GX_TEXCOORD5: return 6;
        case GX_TEXCOORD6: return 7;
        case GX_TEXCOORD7: return 8;
        case GX_TEXCOORDNULL: return 0;
        default: assert(0);
    }
    return 0;
}

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

//803832D0
void HSD_TExpColorOp(HSD_TExp* texp, u8 op, u8 bias, u8 scale, u8 enable){
    assert(HSD_TExpGetType(texp) == 1);
    texp->color_op = op;
    texp->color_enable = (enable ? GX_ENABLE : GX_DISABLE);
    if(op < 2){
        texp->color_bias = bias;
        texp->color_scale = scale;
    }
    texp->color_bias = 0;
    texp->color_scale = 0;
}

//803833AC
void HSD_TExpAlphaOp(HSD_TExp* texp, u8 op, u8 bias, u8 scale, u8 enable){
    assert(HSD_TExpGetType(texp) == 1);
    texp->alpha_op = op;
    texp->alpha_enable = (enable ? GX_ENABLE : GX_DISABLE);
    if(op < 2){
        texp->alpha_bias = bias;
        texp->alpha_scale = scale;
    }
    texp->alpha_bias = 0;
    texp->alpha_scale = 0;
}

//80383488
static void HSD_TExpColorInSub(HSD_TExp* texp, u8 te_num, u8 texp_num, u32 sel){

}

//80383A64
void HSD_TExpColorIn(HSD_TExp* texp, u8 te_num, u8 texp_num, u8 te_num2, u8 texp_num2, 
    u8 te_num3, u8 texp_num3, u8 type, HSD_TExp* texp_rgb){
    assert(HSD_TExpGetType(texp) == 1);
    HSD_TExpColorInSub(texp, te_num, texp_num, 0);
    HSD_TExpColorInSub(texp, te_num2, texp_num2, 1);
    HSD_TExpColorInSub(texp, te_num3, texp_num3, 2);
    HSD_TExpColorInSub(texp, type, texp_rgb, 3);
}

//80383F50
void HSD_TExpAlphaIn(HSD_TExp* texp, u8 te_num, u8 texp_num, u8 te_num2, u8 texp_num2, 
    u8 te_num3, u8 texp_num3, u8 type, HSD_TExp* texp_alpha){
    assert(HSD_TExpGetType(texp) == 1);
}

//80384050
void HSD_TExpOrder(HSD_TExp* texp, void* tobj, u8 chan){

}

//80384F28
void HSD_TExpSetReg(HSD_TExp* texp){

}

//80385448
void HSD_TExpSetupTev(HSD_TExpTevDesc* tevdesc, HSD_TExp* texp){
    HSD_TExpSetReg(texp);
    while(tevdesc != NULL){
        if(tevdesc->x74_unk != NULL){
            //tevdesc->map = (tevdesc->x74_unk + 0xc);
            //tevdesc->coord = (tevdesc->x74_unk + 0xa4);
        }
        HSD_StateAssignTev();
        HSD_SetupTevStage(tevdesc);
        tevdesc = tevdesc ->next;
    }
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
