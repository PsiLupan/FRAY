#include "hsd_texp.h"

#include "hsd_tobj.h"
#include "hsd_state.h"

static u32 num_texgens = 0; //r13_40A4

//80362478
void HSD_StateRegisterTexGen(u32 coord)
{
    u32 num = HSD_TexCoordID2Num(coord);
    if (num_texgens < num) {
        num_texgens = num;
    }
}

//803624A8
void HSD_StateSetNumTexGens()
{
    GX_SetNumTexGens(num_texgens);
    num_texgens = 0;
}

//803629D8
void HSD_SetTevRegAll()
{
}

//80362AA4
u32 HSD_TexCoordID2Num(u32 coord)
{
    switch (coord) {
    case GX_TEXCOORD0:
        return 1;
    case GX_TEXCOORD1:
        return 2;
    case GX_TEXCOORD2:
        return 3;
    case GX_TEXCOORD3:
        return 4;
    case GX_TEXCOORD4:
        return 5;
    case GX_TEXCOORD5:
        return 6;
    case GX_TEXCOORD6:
        return 7;
    case GX_TEXCOORD7:
        return 8;
    case GX_TEXCOORDNULL:
        return 0;
    default:
        assert(0);
    }
    return 0;
}

//80382C00
u32 HSD_TExpGetType(HSD_TExp* texp)
{ //This is dead serious the fucking code, what the fuck
    if ((u32)texp == HSD_TEXP_ZERO) {
        return 0;
    }
    if ((u32)texp == HSD_TEXP_TEX) {
        return 2;
    }
    if ((u32)texp == HSD_TEXP_RAS) {
        return 3;
    }
    return texp->type;
}

//80382C3C
void HSD_TExpRef(HSD_TExp* texp, u8 sel)
{
    u32 type = HSD_TExpGetType(texp);
    if (type == 4) {
        texp->cnst.ref += 1;
        return;
    } else if (type != 1) {
        return;
    }

    if (sel == GX_ENABLE) {
        texp->tev.c_ref += 1;
    } else {
        texp->tev.a_ref += 1;
    }
}

//80382CC4
void HSD_TExpUnref(HSD_TExp* texp, u8 sel)
{
    u32 type = HSD_TExpGetType(texp);
    if (type == 4) {
        if (texp->cnst.ref != 0) {
            texp->cnst.ref -= 1;
            return;
        }
    } else if (type == 1) {
        if (sel == GX_ENABLE) {
            if (texp->tev.c_ref != 0) {
                texp->tev.c_ref -= 1;
            }
        } else if (texp->tev.a_ref != 0) {
            texp->tev.a_ref -= 1;
        }

        if (texp->tev.c_ref == 0 && texp->tev.a_ref == 0) {
            for (u32 i = 0; i < 4; i++) {
                HSD_TExpUnref(texp->tev.c_in[i].exp, texp->tev.c_in[i].sel);
                HSD_TExpUnref(texp->tev.a_in[i].exp, texp->tev.a_in[i].sel);
            }
        }
    }
}

//80382DDC
void HSD_TExpFreeList(HSD_TExp* texp, u32 flags, u8 method)
{
}

//803830FC
HSD_TExp* HSD_TExpTev(HSD_TExp** list)
{
    if (list == NULL) {
        HSD_Halt("HSD_TExpTev: List is NULL");
    }

    HSD_TExp* texp = (HSD_TExp*)hsdAllocMemPiece(sizeof(HSD_TETev));
    if (texp == NULL) {
        HSD_Halt("HSD_TExpTev: No free memory");
    }

    memset(texp, 0xFF, sizeof(HSD_TExp));
    texp->type = 1;
    texp->tev.next = *list;
    *list = texp;
    texp->tev.c_ref = 0;
    texp->tev.a_ref = 0;
    texp->tev.tex = NULL;
    for(u32 i = 0; i < 4; i++){
        texp->tev.c_in[i].exp = NULL;
        texp->tev.a_in[i].exp = NULL;
    }
    return texp;
}

//803831BC
HSD_TExp* HSD_TExpCnst(void* val, HSD_TEInput comp, HSD_TEType type, HSD_TExp** texp_list)
{
}

//803832D0
void HSD_TExpColorOp(HSD_TExp* texp, u8 op, u8 bias, u8 scale, u8 clamp)
{
    assert(HSD_TExpGetType(texp) == 1);
    texp->tev.c_op = op;
    texp->tev.c_clamp = (clamp ? GX_ENABLE : GX_DISABLE);
    if (op < 2) {
        texp->tev.c_bias = bias;
        texp->tev.c_scale = scale;
    }
    texp->tev.c_bias = 0;
    texp->tev.c_scale = 0;
}

//803833AC
void HSD_TExpAlphaOp(HSD_TExp* texp, u8 op, u8 bias, u8 scale, u8 clamp)
{
    assert(HSD_TExpGetType(texp) == 1);
    texp->tev.a_op = op;
    texp->tev.a_clamp = (clamp ? GX_ENABLE : GX_DISABLE);
    if (op < 2) {
        texp->tev.a_bias = bias;
        texp->tev.a_scale = scale;
    }
    texp->tev.a_bias = 0;
    texp->tev.a_scale = 0;
}

//80383488
static void HSD_TExpColorInSub(HSD_TExp* tev, HSD_TEInput sel, HSD_TExp* exp, u32 idx)
{
}

//80383A64
void HSD_TExpColorIn(HSD_TExp* texp, HSD_TEInput sel_a, HSD_TExp* exp_a, HSD_TEInput sel_b, HSD_TExp* exp_b,
    HSD_TEInput sel_c, HSD_TExp* exp_c, HSD_TEInput sel_d, HSD_TExp* exp_d)
{
    assert(HSD_TExpGetType(texp) == 1);
    HSD_TExpColorInSub(texp, sel_a, exp_a, 0);
    HSD_TExpColorInSub(texp, sel_b, exp_b, 1);
    HSD_TExpColorInSub(texp, sel_c, exp_c, 2);
    HSD_TExpColorInSub(texp, sel_d, exp_d, 3);
}

//80383F50
void HSD_TExpAlphaIn(HSD_TExp* texp, HSD_TEInput sel_a, HSD_TExp* exp_a, HSD_TEInput sel_b, HSD_TExp* exp_b,
    HSD_TEInput sel_c, HSD_TExp* exp_c, HSD_TEInput sel_d, HSD_TExp* exp_d)
{
    assert(HSD_TExpGetType(texp) == 1);
}

//80384050
void HSD_TExpOrder(HSD_TExp* texp, void* tex, u8 chan)
{
}

//80384F28
void HSD_TExpSetReg(HSD_TExp* texp)
{
}

//80385448
void HSD_TExpSetupTev(HSD_TExpTevDesc* tevdesc, HSD_TExp* texp)
{
    HSD_TExpSetReg(texp);
    while (tevdesc != NULL) {
        if (tevdesc->tobj != NULL) {
            tevdesc->map = ((HSD_TObj*)tevdesc->tobj)->id;
            tevdesc->coord = ((HSD_TObj*)tevdesc->tobj)->coord;
        }
        HSD_StateAssignTev();
        HSD_SetupTevStage(tevdesc);
        tevdesc = tevdesc->next;
    }
}

//803854B4
void HSD_TExpCompile(HSD_TExp* texp, HSD_TExpTevDesc** tdesc, HSD_TExp** list)
{
}

//80385758
void HSD_TExpFreeTevDesc(HSD_TExpTevDesc* tdesc)
{
    HSD_TExpTevDesc* next = tdesc;
    while (next != NULL) {
        hsdFreeMemPiece(next, sizeof(HSD_TExpTevDesc));
        next = next->next;
    }
}
