#include "hsd_texp.h"

#include "hsd_state.h"
#include "hsd_tobj.h"

static u32 num_texgens = 0; //r13_40A4

HSD_TevReg TevReg[4];

//80362478
void HSD_StateRegisterTexGen(u32 coord)
{
    u32 num = HSD_TexCoordID2Num(coord);
    if (num_texgens < num) {
        num_texgens = num;
    }
}

//803624A8
void HSD_StateSetNumTexGens(void)
{
    GX_SetNumTexGens(num_texgens);
    num_texgens = 0;
}

static u8 HSD_Index2TevRegID(u32 idx)
{
    switch (idx) {
    case 0:
        return GX_TEVREG0;
    case 1:
        return GX_TEVREG1;
    case 2:
        return GX_TEVREG2;
    case 3:
        return GX_TEVPREV;
    default:
        return GX_TEVREG0;
    }
}

//803629D8
void HSD_SetTevRegAll(void)
{
    for (u32 i = 0; i < 4; i++) {
        if (TevReg[i].color.r != 0 || TevReg[i].color.g != 0 || TevReg[i].color.b != 0 || TevReg[i].color.a != 0) {
            u32 reg = HSD_Index2TevRegID(i);
            GX_SetTevColorS10(reg, TevReg[i].color);
            TevReg[i].color.r = 0;
            TevReg[i].color.g = 0;
            TevReg[i].color.b = 0;
            TevReg[i].color.a = 0;
        }
    }
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
        HSD_Halt("Unexpected texcoord id");
    }
    return 0;
}

//80382C00
u32 HSD_TExpGetType(HSD_TExp* texp)
{
    if (texp == NULL) {
        return HSD_TE_ZERO;
    }
    if (texp->type == -1) {
        return HSD_TE_TEX;
    }
    if (texp->type == -2) {
        return HSD_TE_RAS;
    }
    return texp->type;
}

//80382C3C
void HSD_TExpRef(HSD_TExp* texp, u8 sel)
{
    u32 type = HSD_TExpGetType(texp);
    if (type == HSD_TE_CNST) {
        texp->cnst.ref += 1;
        return;
    } else if (type != HSD_TE_TEV) {
        return;
    }

    if (sel == TRUE) {
        texp->tev.c_ref += 1;
    } else {
        texp->tev.a_ref += 1;
    }
}

//80382CC4
void HSD_TExpUnref(HSD_TExp* texp, u8 sel)
{
    u32 type = HSD_TExpGetType(texp);
    if (type == HSD_TE_CNST) {
        if (texp->cnst.ref != 0) {
            texp->cnst.ref -= 1;
            return;
        }
    } else if (type == HSD_TE_TEV) {
        if (sel == TRUE) {
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

//80362D04
void _HSD_StateInvalidateTevRegister(void)
{
    for (u32 i = 0; i < 4; i++) {
        TevReg[i].color.r = 0;
        TevReg[i].color.g = 0;
        TevReg[i].color.b = 0;
        TevReg[i].color.a = 0;
    }
}

//80362D24
void _HSD_StateInvalidateTexCoordGen(void)
{
    num_texgens = 0;
}

//80382DDC
HSD_TExp* HSD_TExpFreeList(HSD_TExp* texp_list, HSD_TExpType type, s32 all)
{
    HSD_TExp** handle;
    HSD_TExp* next;
    HSD_TExp* n;

    n = texp_list;
    handle = &n;

    if (all == 0) {
        if (type == HSD_TE_ALL || type == HSD_TE_TEV) {
            for (; texp_list != NULL; texp_list = texp_list->tev.next) {
                if (texp_list->type == HSD_TE_TEV && texp_list->tev.c_ref == 0) {
                    HSD_TExpUnref(texp_list, 1);
                    HSD_TExpUnref(texp_list, 5);
                }
            }
        }
    }

    while (*handle != NULL) {
        HSD_TExpType curr_type = (*handle)->type;
        if (type == HSD_TE_ALL || type == curr_type) {
            if (curr_type == HSD_TE_CNST) {
                if (all == 0 && (*handle)->cnst.ref != 0) {
                    handle = &(*handle)->cnst.next;
                    continue;
                }
                next = (*handle)->cnst.next;
                hsdFreeMemPiece((*handle), sizeof(HSD_TECnst));
                *handle = next;
            } else {
                HSD_CheckAssert("TExpFreeList: type != HSD_TE_TEV", curr_type == HSD_TE_TEV);
                if (all == 0 && ((*handle)->tev.c_ref != 0 || (*handle)->tev.a_ref != 0)) {
                    handle = &(*handle)->tev.next;
                    continue;
                }
                next = (*handle)->tev.next;
                hsdFreeMemPiece((*handle), sizeof(HSD_TETev));
                *handle = next;
            }
        } else {
            handle = &(*handle)->cnst.next;
        }
    }
    return n;
}

//803830FC
HSD_TExp* HSD_TExpTev(HSD_TExp** list)
{
    HSD_CheckAssert("HSD_TExpTev: List is NULL", list != NULL);

    HSD_TExp* texp = (HSD_TExp*)hsdAllocMemPiece(sizeof(HSD_TETev));
    HSD_CheckAssert("HSD_TExpTev: No free memory", texp != NULL);

    memset(texp, 0xFF, sizeof(HSD_TETev));
    texp->type = HSD_TE_TEV;
    texp->tev.next = *list;
    *list = texp;
    texp->tev.c_ref = 0;
    texp->tev.a_ref = 0;
    texp->tev.tex = NULL;
    for (u32 i = 0; i < 4; i++) {
        texp->tev.c_in[i].exp = NULL;
        texp->tev.a_in[i].exp = NULL;
    }
    return texp;
}

//803831BC
HSD_TExp* HSD_TExpCnst(void* val, HSD_TEInput comp, HSD_TEType type, HSD_TExp** texp_list)
{
    HSD_CheckAssert("HSD_TExpCnst: texp_list == NULL", texp_list != NULL);
    HSD_TExp* texp = *texp_list;

    while (true) {
        if (texp == NULL) {
            HSD_TExp* result = NULL;

            if (comp != HSD_TE_0) {
                result = (HSD_TExp*)hsdAllocMemPiece(sizeof(HSD_TECnst));
                HSD_CheckAssert("HSD_TExpCnst: Could not alloc cnst", result != NULL);
                result->cnst.type = HSD_TE_CNST;
                result->cnst.next = *texp_list;
                result->cnst.val = val;
                result->cnst.comp = comp;
                result->cnst.ctype = type;
                result->cnst.reg = 0xFF;
                result->cnst.idx = 0xFF;
                result->cnst.ref = 0;

                *texp_list = result;
            }
            return result;
        }
        if (texp->type == HSD_TE_CNST && texp->cnst.val == val && texp->cnst.comp == comp) {
            HSD_CheckAssert("HSD_TExpCnst: ctype != type", texp->cnst.ctype == type);
            break;
        }
        texp = texp->cnst.next;
    }
    return texp;
}

//803832D0
void HSD_TExpColorOp(HSD_TExp* texp, u8 op, u8 bias, u8 scale, u8 clamp)
{
    assert(HSD_TExpGetType(texp) == HSD_TE_TEV);
    texp->tev.c_op = op;
    texp->tev.c_clamp = (clamp ? GX_ENABLE : GX_DISABLE);
    if (op < 2) {
        texp->tev.c_bias = bias;
        texp->tev.c_scale = scale;
    } else {
        texp->tev.c_bias = 0;
        texp->tev.c_scale = 0;
    }
}

//803833AC
void HSD_TExpAlphaOp(HSD_TExp* texp, u8 op, u8 bias, u8 scale, u8 clamp)
{
    assert(HSD_TExpGetType(texp) == HSD_TE_TEV);
    texp->tev.a_op = op;
    texp->tev.a_clamp = (clamp ? GX_ENABLE : GX_DISABLE);
    if (op < 2) {
        texp->tev.a_bias = bias;
        texp->tev.a_scale = scale;
    } else {
        texp->tev.a_bias = 0;
        texp->tev.a_scale = 0;
    }
}

//80383488
static void HSD_TExpColorInSub(HSD_TETev* tev, HSD_TEInput sel, HSD_TExp* exp, u32 idx)
{
    u32 swap;
    HSD_TExp* texp = tev->c_in[idx].exp;

    u32 type = HSD_TExpGetType(exp);
    tev->c_in[idx].type = type;
    tev->c_in[idx].sel = sel;
    tev->c_in[idx].exp = exp;
    tev->c_in[idx].arg = 0xFF;
    switch (sel) {
    case HSD_TE_0:
        tev->c_in[idx].type = HSD_TE_ZERO;
        tev->c_in[idx].arg = GX_CC_ZERO;
        tev->c_in[idx].exp = NULL;
        HSD_TExpUnref(texp, HSD_TE_ZERO);
        return;

    case HSD_TE_1:
        tev->c_in[idx].type = HSD_TE_IMM;
        tev->c_in[idx].arg = GX_CC_ONE;
        tev->c_in[idx].exp = NULL;
        HSD_TExpUnref(texp, HSD_TE_IMM);
        return;

    case HSD_TE_4_8:
        tev->c_in[idx].type = HSD_TE_IMM;
        tev->c_in[idx].arg = GX_CC_HALF;
        tev->c_in[idx].exp = NULL;
        HSD_TExpUnref(texp, HSD_TE_IMM);
        return;

    case HSD_TE_1_8:
    case HSD_TE_2_8:
    case HSD_TE_3_8:
    case HSD_TE_5_8:
    case HSD_TE_6_8:
    case HSD_TE_7_8:
        tev->c_in[idx].arg = GX_CC_KONST;
        switch (sel) {
        case HSD_TE_1_8:
            swap = 7;
            break;
        case HSD_TE_2_8:
            swap = 6;
            break;
        case HSD_TE_3_8:
            swap = 5;
            break;
        case HSD_TE_5_8:
            swap = 3;
            break;
        case HSD_TE_6_8:
            swap = 2;
            break;
        case HSD_TE_7_8:
            swap = 1;
            break;
        default:
            HSD_Halt("HSD_TExpColorInSub: Unexpected kcsel");
        }

        if (tev->kcsel == HSD_TE_UNDEF) {
            tev->kcsel = swap;
        } else {
            HSD_CheckAssert("HSD_TExpColorInSub: tev cannot select multiple konst", tev->kcsel != swap);
        }

        tev->c_in[idx].type = HSD_TE_KONST;
        goto END;
    }

    u8 c_in_type = tev->c_in[idx].type;
    if (c_in_type != HSD_TE_TEX) {
        switch (c_in_type) {
        case HSD_TE_ZERO:
            tev->c_in[idx].type = HSD_TE_ZERO;
            tev->c_in[idx].sel = HSD_TE_0;
            tev->c_in[idx].arg = GX_CC_ZERO;
            goto END;

        case HSD_TE_TEV:
            HSD_CheckAssert("HSD_TExpColorInSub: sel != HSD_TE_RGB or HSD_TE_A", sel == HSD_TE_RGB || sel == HSD_TE_A);
            HSD_CheckAssert("HSD_TExpColorInSub: idx == 3 || sel != HSD_TE_RGB || exp->tev.c_clamp != 0", idx == 3 || sel != HSD_TE_RGB || exp->tev.c_clamp != 0);
            HSD_CheckAssert("HSD_TExpColorInSub: idx == 3 || sel != HSD_TE_A || exp->tev.a_clamp != 0", idx == 3 || sel != HSD_TE_A || exp->tev.a_clamp != 0);
            HSD_TExpRef(tev->c_in[idx].exp, tev->c_in[idx].sel);
            goto END;

        case HSD_TE_RAS:
            swap = HSD_TE_UNDEF;

            switch (sel) {
            case HSD_TE_RGB:
                tev->c_in[idx].arg = GX_CC_RASC;
                swap = 0;
                break;

            case HSD_TE_R:
                tev->c_in[idx].arg = GX_CC_RASC;
                swap = 1;
                break;

            case HSD_TE_G:
                tev->c_in[idx].arg = GX_CC_RASC;
                swap = 2;
                break;

            case HSD_TE_B:
                tev->c_in[idx].arg = GX_CC_RASC;
                swap = 3;
                break;

            case HSD_TE_A:
                tev->c_in[idx].arg = GX_CC_RASA;
                break;
            }

            if (tev->ras_swap == HSD_TE_UNDEF) {
                tev->ras_swap = swap;
            }
            goto END;
            break;

        case HSD_TE_CNST:
            tev->c_in[idx].sel = exp->cnst.comp;
            HSD_TExpRef(tev->c_in[idx].exp, tev->c_in[idx].sel);
            goto END;
            break;

        default:
            HSD_Halt("HSD_TExpColorInSub: Unexpected c_in_type");
        }
    }

    swap = HSD_TE_UNDEF;
    switch (sel) {
    case HSD_TE_RGB:
        tev->c_in[idx].arg = GX_CC_TEXC;
        swap = 0;
        break;

    case HSD_TE_R:
        tev->c_in[idx].arg = GX_CC_TEXC;
        swap = 1;
        break;

    case HSD_TE_G:
        tev->c_in[idx].arg = GX_CC_TEXC;
        swap = 2;
        break;

    case HSD_TE_B:
        tev->c_in[idx].arg = GX_CC_TEXC;
        swap = 3;
        break;

    case HSD_TE_A:
        tev->c_in[idx].arg = GX_CC_TEXA;
        break;
    }

    if (tev->tex_swap == HSD_TE_UNDEF) {
        tev->tex_swap = swap;
    } else {
        HSD_CheckAssert("HSD_TExpColorInSub: swap == HSD_TE_UNDEF || tev->tex_swap == swap", swap == HSD_TE_UNDEF || tev->tex_swap == swap);
    }
END:
    HSD_TExpUnref(texp, tev->c_in[idx].sel);
    return;
}

//80383A64
void HSD_TExpColorIn(HSD_TExp* texp, HSD_TEInput sel_a, HSD_TExp* exp_a, HSD_TEInput sel_b, HSD_TExp* exp_b,
    HSD_TEInput sel_c, HSD_TExp* exp_c, HSD_TEInput sel_d, HSD_TExp* exp_d)
{
    assert(HSD_TExpGetType(texp) == HSD_TE_TEV);
    HSD_TExpColorInSub(&texp->tev, sel_a, exp_a, 0);
    HSD_TExpColorInSub(&texp->tev, sel_b, exp_b, 1);
    HSD_TExpColorInSub(&texp->tev, sel_c, exp_c, 2);
    HSD_TExpColorInSub(&texp->tev, sel_d, exp_d, 3);
}

//80383B64
static void HSD_TExpAlphaInSub(HSD_TETev* tev, HSD_TEInput sel, HSD_TExp* exp, u32 idx)
{
    u32 swap;
    HSD_TExp* texp = tev->a_in[idx].exp;

    u32 type = HSD_TExpGetType(exp);
    tev->a_in[idx].type = type;
    tev->a_in[idx].sel = sel;
    tev->a_in[idx].exp = exp;
    tev->a_in[idx].arg = 0xFF;

    if (sel == HSD_TE_0) {
        tev->a_in[idx].type = HSD_TE_ZERO;
        tev->a_in[idx].arg = GX_CA_ZERO;
        tev->a_in[idx].exp = NULL;
    } else {
        if (sel < HSD_TE_0 || HSD_TE_7_8 < sel) {
            u8 a_in_type = tev->a_in[idx].type;
            switch (a_in_type) {
            case HSD_TE_ZERO:
                tev->a_in[idx].type = HSD_TE_ZERO;
                tev->a_in[idx].sel = HSD_TE_0;
                tev->a_in[idx].arg = GX_CA_ZERO;
                tev->a_in[idx].exp = NULL;
                break;

            case HSD_TE_TEV:
                HSD_CheckAssert("HSD_TExpAlphaInSub: sel != HSD_TE_A", sel == HSD_TE_A);
                HSD_CheckAssert("HSD_TExpAlphaInSub: idx != 3 || exp->tev.a_clamp != 0", idx == 3 || exp->tev.a_clamp != 0);
                HSD_TExpRef(tev->a_in[idx].exp, tev->a_in[idx].sel);
                break;

            case HSD_TE_TEX:
                HSD_CheckAssert("HSD_TExpAlphaInSub: sel != HSD_TE_A", sel == HSD_TE_A);
                tev->a_in[idx].arg = GX_CA_TEXA;
                break;

            case HSD_TE_RAS:
                HSD_CheckAssert("HSD_TExpAlphaInSub: sel != HSD_TE_A", sel == HSD_TE_A);
                tev->a_in[idx].arg = GX_CA_RASA;
                break;

            case HSD_TE_CNST:
                HSD_CheckAssert("HSD_TExpAlphaInSub: sel != HSD_TE_A || sel != HSD_TE_X", sel == HSD_TE_A || sel == HSD_TE_X);
                HSD_CheckAssert("HSD_TExpAlphaInSub: exp->cnst.comp != HSD_TE_X", exp->cnst.comp == HSD_TE_X);
                tev->a_in[idx].sel = HSD_TE_X;
                HSD_TExpRef(tev->a_in[idx].exp, tev->a_in[idx].sel);
                break;

            default:
                HSD_Halt("HSD_TExpAlphaInSub: Unexpected a_in_type");
            }
        } else {
            tev->a_in[idx].exp = NULL;
            tev->a_in[idx].arg = GX_CA_KONST;
            switch (sel) {
            case HSD_TE_1_8:
                swap = 7;
                break;
            case HSD_TE_2_8:
                swap = 6;
                break;
            case HSD_TE_3_8:
                swap = 5;
                break;
            case HSD_TE_4_8:
                swap = 4;
                break;
            case HSD_TE_5_8:
                swap = 3;
                break;
            case HSD_TE_6_8:
                swap = 2;
                break;
            case HSD_TE_7_8:
                swap = 1;
                break;
            default:
                swap = 0;
            }

            if (tev->kasel == HSD_TE_UNDEF) {
                tev->kasel = swap;
            } else {
                HSD_CheckAssert("HSD_TExpAlphaInSub: tev->kasel != swap", tev->kcsel != swap);
            }
            tev->a_in[idx].type = HSD_TE_KONST;
        }
    }
    HSD_TExpUnref(texp, tev->a_in[idx].sel);
    return;
}

//80383F50
void HSD_TExpAlphaIn(HSD_TExp* texp, HSD_TEInput sel_a, HSD_TExp* exp_a, HSD_TEInput sel_b, HSD_TExp* exp_b,
    HSD_TEInput sel_c, HSD_TExp* exp_c, HSD_TEInput sel_d, HSD_TExp* exp_d)
{
    assert(HSD_TExpGetType(texp) == HSD_TE_TEV);
    HSD_TExpAlphaInSub(&texp->tev, sel_a, exp_a, 0);
    HSD_TExpAlphaInSub(&texp->tev, sel_b, exp_b, 1);
    HSD_TExpAlphaInSub(&texp->tev, sel_c, exp_c, 2);
    HSD_TExpAlphaInSub(&texp->tev, sel_d, exp_d, 3);
}

//80384050
void HSD_TExpOrder(HSD_TExp* texp, HSD_TObj* tex, u8 chan)
{
    HSD_CheckAssert("HSD_TExpOrder: texp == NULL", texp != NULL);
    HSD_CheckAssert("HSD_TExpOrder: texp->type != HSD_TE_TEV", HSD_TExpGetType(texp) == HSD_TE_TEV);
    texp->tev.tex = tex;
    if (chan == 0xff) {
        texp->tev.chan = 0xff;
    } else {
        texp->tev.chan = chan;
    }
}

//80384114
static s32 AssignColorReg(HSD_TETev* tev, u32 idx, HSD_TExpRes* res)
{
    u32 a_in[4] = { GX_CC_A0, GX_CC_A1, GX_CC_A2, GX_CC_APREV };
    u32 c_in[4] = { GX_CC_C0, GX_CC_C1, GX_CC_C2, GX_CC_CPREV };

    HSD_TECnst* cnst = &tev->c_in[idx].exp->cnst;
    if (cnst->reg == HSD_TE_UNDEF) {
        if (cnst->comp == HSD_TE_X) {
            for (u32 i = 4; i < 8; ++i) {
                if (res->reg[i].alpha == 0) {
                    res->reg[i].alpha = 1;
                    cnst->reg = i;
                    cnst->idx = 3;
                    tev->c_in[idx].type = HSD_TE_IMM;
                    tev->c_in[idx].arg = a_in[i - 4];
                    return 0;
                }
            }
        } else {
            for (u32 i = 4; i < 8; ++i) {
                if (res->reg[i].color == 0) {
                    res->reg[i].color = 3;
                    cnst->reg = i;
                    cnst->idx = 0;
                    tev->c_in[idx].type = HSD_TE_IMM;
                    tev->c_in[idx].arg = c_in[i - 4];
                    return 0;
                }
            }
        }
        return -1;
    }
    if (cnst->reg > 3) {
        tev->c_in[idx].type = HSD_TE_IMM;
        if (cnst->comp == HSD_TE_X) {
            tev->c_in[idx].arg = a_in[cnst->reg - 4];
        } else {
            tev->c_in[idx].arg = c_in[cnst->reg - 4];
        }
        return 0;
    }
    return -1;
}

//80384274
static s32 AssignAlphaReg(HSD_TETev* tev, u32 idx, HSD_TExpRes* res)
{
    u32 args[4] = { GX_CA_A0, GX_CA_A1, GX_CA_A2, GX_CA_APREV };

    HSD_TECnst* cnst = &tev->a_in[idx].exp->cnst;
    if (cnst->reg == HSD_TE_UNDEF) {
        for (u32 i = 4; i < 8; ++i) {
            if (res->reg[i].alpha == 0) {
                res->reg[i].alpha = 1;
                cnst->reg = i;
                cnst->idx = 3;
                tev->a_in[idx].type = HSD_TE_IMM;
                tev->a_in[idx].arg = args[i - 4];
                return 0;
            }
        }
        return -1;
    }
    if (cnst->reg > 3) {
        tev->a_in[idx].type = HSD_TE_IMM;
        tev->a_in[idx].arg = args[cnst->reg - 4];
        return 0;
    }
    return -1;
}

//80384340
static s32 AssignColorKonst(HSD_TETev* tev, u32 idx, HSD_TExpRes* res)
{
    u32 xsel[4][4] = {
        { GX_TEV_KCSEL_K0_R, GX_TEV_KCSEL_K0_G, GX_TEV_KCSEL_K0_B, GX_TEV_KCSEL_K0_A },
        { GX_TEV_KCSEL_K1_R, GX_TEV_KCSEL_K1_G, GX_TEV_KCSEL_K1_B, GX_TEV_KCSEL_K1_A },
        { GX_TEV_KCSEL_K2_R, GX_TEV_KCSEL_K2_G, GX_TEV_KCSEL_K2_B, GX_TEV_KCSEL_K2_A },
        { GX_TEV_KCSEL_K3_R, GX_TEV_KCSEL_K3_G, GX_TEV_KCSEL_K3_B, GX_TEV_KCSEL_K3_A }
    };

    u32 csel[4] = { GX_TEV_KCSEL_K0, GX_TEV_KCSEL_K1, GX_TEV_KCSEL_K2, GX_TEV_KCSEL_K3 };

    HSD_TECnst* cnst = &tev->c_in[idx].exp->cnst;
    if (cnst->reg == HSD_TE_UNDEF) {
        if (cnst->comp == HSD_TE_X) {
            for (u32 i = 1; i < 4; i++) {
                if (res->reg[i].alpha == 0) {
                    res->reg[i].alpha = 1;
                    cnst->reg = i;
                    cnst->idx = 3;
                    tev->kcsel = xsel[cnst->reg][cnst->idx];
                    tev->c_in[idx].type = HSD_TE_KONST;
                    tev->c_in[idx].arg = GX_CC_KONST;
                    return 0;
                }
            }

            for (u32 i = 0; i < 4; ++i) {
                if (res->reg[i].color < 3) {
                    cnst->reg = i;
                    cnst->idx = res->reg[i].color;
                    res->reg[i].color += 1;
                    tev->kcsel = xsel[cnst->reg][cnst->idx];
                    tev->c_in[idx].type = HSD_TE_KONST;
                    tev->c_in[idx].arg = GX_CC_KONST;
                    return 0;
                }
            }
        } else {
            for (u32 i = 0; i < 4; ++i) {
                if (res->reg[i].color == 0) {
                    res->reg[i].color = 3;
                    cnst->reg = i;
                    cnst->idx = 0;
                    tev->kcsel = csel[cnst->reg];
                    tev->c_in[idx].type = HSD_TE_KONST;
                    tev->c_in[idx].arg = GX_CC_KONST;
                    return 0;
                }
            }
        }
        return -1;
    }
    if (cnst->reg < 4) {
        tev->c_in[idx].type = HSD_TE_KONST;
        tev->c_in[idx].arg = GX_CC_KONST;
        if (cnst->comp == HSD_TE_X) {
            tev->kcsel = xsel[cnst->reg][cnst->idx];
        } else {
            tev->kcsel = csel[cnst->reg];
        }
        return 0;
    }
    return -1;
}

//80384560
static s32 AssignAlphaKonst(HSD_TETev* tev, u32 idx, HSD_TExpRes* res)
{
    u32 regs[4][4] = {
        { GX_TEV_KASEL_K0_R, GX_TEV_KASEL_K0_G, GX_TEV_KASEL_K0_B, GX_TEV_KASEL_K0_A },
        { GX_TEV_KASEL_K1_R, GX_TEV_KASEL_K1_G, GX_TEV_KASEL_K1_B, GX_TEV_KASEL_K1_A },
        { GX_TEV_KASEL_K2_R, GX_TEV_KASEL_K2_G, GX_TEV_KASEL_K2_B, GX_TEV_KASEL_K2_A },
        { GX_TEV_KASEL_K3_R, GX_TEV_KASEL_K3_G, GX_TEV_KASEL_K3_B, GX_TEV_KASEL_K3_A }
    };

    HSD_TECnst* cnst = &tev->a_in[idx].exp->cnst;
    if (cnst->reg == HSD_TE_UNDEF) {
        for (u32 i = 1; i < 4; i++) {
            if (res->reg[i].alpha == 0) {
                res->reg[i].alpha = 1;
                cnst->reg = i;
                cnst->idx = 3;
                tev->kasel = regs[cnst->reg][cnst->idx];
                tev->a_in[idx].type = HSD_TE_KONST;
                tev->a_in[idx].arg = GX_CA_KONST;
                return 0;
            }
        }
        for (u32 i = 0; i < 4; i++) {
            if (res->reg[i].color < 3) {
                cnst->reg = i;
                cnst->idx = res->reg[i].color;
                res->reg[i].color += 1;
                tev->kasel = regs[cnst->reg][cnst->idx];
                tev->a_in[idx].type = HSD_TE_KONST;
                tev->a_in[idx].arg = GX_CA_KONST;
                return 0;
            }
        }
        return -1;
    }
    if (cnst->reg < 4) {
        tev->kasel = regs[cnst->reg][cnst->idx];
        tev->a_in[idx].type = HSD_TE_KONST;
        tev->a_in[idx].arg = GX_CA_KONST;
        return 0;
    }
    return -1;
}

static BOOL IsThroughColor(HSD_TExp* texp)
{
    if (texp->tev.c_op == 0 && texp->tev.c_in[0].sel == 7 && texp->tev.c_in[1].sel == 7
        && texp->tev.c_bias == 0 && texp->tev.c_scale == 0) {
        return TRUE;
    }
    return FALSE;
}

static BOOL IsThroughAlpha(HSD_TExp* texp)
{
    if (texp->tev.a_op == 0 && texp->tev.a_in[0].sel == 7 && texp->tev.a_in[1].sel == 7
        && texp->tev.a_bias == 0 && texp->tev.a_scale == 0) {
        return TRUE;
    }
    return FALSE;
}

//803846C0
static s32 TExpAssignReg(HSD_TExp* texp, HSD_TExpRes* res)
{
    s32 val = 0;
    u32 i;

    if (texp->tev.c_ref > 0) {
        if (texp->tev.kcsel == HSD_TE_UNDEF) {
            if (IsThroughColor(texp) == 0 || texp->tev.c_in[3].type != HSD_TE_CNST) {
                for (i = 0; i < 4; ++i) {
                    if (texp->tev.c_in[i].type == HSD_TE_CNST) {
                        if (AssignColorKonst(&texp->tev, i, res) == -1) {
                            val = AssignColorReg(&texp->tev, i, res);
                            return val;
                        }
                    }
                }
            } else {
                if (AssignColorKonst(&texp->tev, 3, res) == -1) {
                    val = AssignColorReg(&texp->tev, 3, res);
                    return val;
                }
            }
        } else {
            for (i = 0; i < 4; ++i) {
                if (texp->tev.c_in[i].type == HSD_TE_CNST) {
                    val = AssignColorReg(&texp->tev, i, res);
                    return val;
                }
            }
        }
    }

    if (texp->tev.a_ref > 0) {
        if (texp->tev.kasel == HSD_TE_UNDEF) {
            if (IsThroughAlpha(texp) == 0 || texp->tev.a_in[3].type != HSD_TE_CNST) {
                for (i = 0; i < 4; ++i) {
                    if (texp->tev.a_in[i].type == HSD_TE_CNST) {
                        if (AssignAlphaKonst(&texp->tev, i, res) == -1) {
                            val = AssignAlphaReg(&texp->tev, i, res);
                            return val;
                        }
                    }
                }
            } else {
                if (AssignAlphaReg(&texp->tev, 3, res) == -1) {
                    val = AssignAlphaKonst(&texp->tev, 3, res);
                    return val;
                }
            }
        } else {
            for (i = 0; i < 4; ++i) {
                if (texp->tev.a_in[i].type == HSD_TE_CNST) {
                    val = AssignAlphaReg(&texp->tev, i, res);
                    return val;
                }
            }
        }
    }
    return val;
}

//80384B20
static void TExp2TevDesc(HSD_TExp* texp, HSD_TExpTevDesc* desc, u32* init_cprev, u32* init_aprev)
{
    u32 args[4] = { GX_TEVREG0, GX_TEVREG1, GX_TEVREG2, GX_TEVPREV };

    u32 swap;
    u8 arg;

    HSD_CheckAssert("TExp2TevDesc: texp cannot be null", texp != NULL);
    HSD_CheckAssert("TExp2TevDesc: desc cannot be null", desc != NULL);
    u32 type = HSD_TExpGetType(texp);
    HSD_CheckAssert("TExp2TevDesc: type != HSD_TE_TEV", type == HSD_TE_TEV);
    desc->desc.next = NULL;
    desc->desc.flags = 1;
    desc->tobj = texp->tev.tex;

    if (texp->tev.tex == NULL) {
        desc->desc.coord = GX_TEXCOORDNULL;
        desc->desc.map = GX_TEXMAP_NULL;
    }

    swap = texp->tev.chan;
    if (texp->tev.chan == 0xFF) {
        swap = 0xFF;
    }
    desc->desc.color = swap;

    swap = texp->tev.tex_swap;
    if (texp->tev.tex_swap == 0xFF) {
        swap = 0;
    }
    desc->desc.u.tevconf.tex_swap = swap;

    swap = texp->tev.ras_swap;
    if (swap == 0xFF) {
        swap = 0;
    }
    desc->desc.u.tevconf.ras_swap = swap;

    swap = texp->tev.kcsel;
    if (swap == 0xFF) {
        swap = 0;
    }
    desc->desc.u.tevconf.kcsel = swap;

    swap = texp->tev.kasel;
    if (swap == 0xFF) {
        swap = 0;
    }
    desc->desc.u.tevconf.kasel = swap;

    u8 val = texp->tev.a_op;
    if (texp->tev.c_op == GX_COLORNULL || (texp->tev.c_ref == 0 && val != 8 && val != 9 && val != 10 && val != 11 && val != 12 && val != 13)) {
        desc->desc.u.tevconf.clr_op = 0;
        desc->desc.u.tevconf.clr_a = GX_CC_ZERO;
        desc->desc.u.tevconf.clr_b = GX_CC_ZERO;
        desc->desc.u.tevconf.clr_c = GX_CC_ZERO;
        if (*init_cprev == 0) {
            desc->desc.u.tevconf.clr_d = GX_CC_CPREV;
        } else {
            *init_cprev = 0;
            desc->desc.u.tevconf.clr_d = GX_CC_ZERO;
        }
        desc->desc.u.tevconf.clr_scale = 0;
        desc->desc.u.tevconf.clr_bias = 0;
        desc->desc.u.tevconf.clr_clamp = 0;
        desc->desc.u.tevconf.clr_out_reg = 0;
    } else {
        desc->desc.u.tevconf.clr_op = texp->tev.c_op;

        for (u32 i = 0; i < 4; i++) {
            arg = texp->tev.c_in[i].arg;
            swap = arg;
            if (arg == 0xFF) {
                swap = GX_CC_ZERO;
            }
            switch (i) {
            case 0:
                desc->desc.u.tevconf.clr_a = swap;
                break;

            case 1:
                desc->desc.u.tevconf.clr_b = swap;
                break;

            case 2:
                desc->desc.u.tevconf.clr_c = swap;
                break;

            case 3:
                desc->desc.u.tevconf.clr_d = swap;
                break;
            }
        }

        arg = texp->tev.c_scale;
        swap = arg;
        if (arg == 0xFF) {
            swap = 0;
        }
        desc->desc.u.tevconf.clr_scale = swap;

        arg = texp->tev.c_bias;
        swap = arg;
        if (arg == 0xFF) {
            swap = 0;
        }
        desc->desc.u.tevconf.clr_bias = swap;

        desc->desc.u.tevconf.clr_clamp = texp->tev.c_clamp != 0;

        HSD_CheckAssert("TExp2TevDesc: c_dst is undefined", texp->tev.c_dst != 0xFF);

        desc->desc.u.tevconf.clr_out_reg = args[texp->tev.c_dst];
        if (desc->desc.u.tevconf.clr_out_reg == 0) {
            *init_cprev = 0;
        }
    }

    if (texp->tev.a_op == GX_COLORNULL || texp->tev.a_ref == 0) {
        desc->desc.u.tevconf.alpha_op = 0;
        desc->desc.u.tevconf.alpha_a = GX_CA_ZERO;
        desc->desc.u.tevconf.alpha_b = GX_CA_ZERO;
        desc->desc.u.tevconf.alpha_c = GX_CA_ZERO;
        if (*init_aprev == 0) {
            desc->desc.u.tevconf.alpha_d = GX_CA_APREV;
        } else {
            *init_aprev = 0;
            desc->desc.u.tevconf.alpha_d = GX_CA_ZERO;
        }
        desc->desc.u.tevconf.alpha_scale = 0;
        desc->desc.u.tevconf.alpha_bias = 0;
        desc->desc.u.tevconf.alpha_clamp = 0;
        desc->desc.u.tevconf.alpha_out_reg = 0;
    } else {
        desc->desc.u.tevconf.alpha_op = texp->tev.a_op;

        for (u32 i = 0; i < 4; i++) {
            arg = texp->tev.a_in[i].arg;
            swap = arg;
            if (arg == 0xFF) {
                swap = GX_CA_ZERO;
            }
            switch (i) {
            case 0:
                desc->desc.u.tevconf.alpha_a = swap;
                break;

            case 1:
                desc->desc.u.tevconf.alpha_b = swap;
                break;

            case 2:
                desc->desc.u.tevconf.alpha_c = swap;
                break;

            case 3:
                desc->desc.u.tevconf.alpha_d = swap;
                break;
            }
        }

        arg = texp->tev.a_scale;
        swap = arg;
        if (arg == 0xFF) {
            swap = 0;
        }
        desc->desc.u.tevconf.alpha_scale = swap;

        arg = texp->tev.a_bias;
        swap = arg;
        if (arg == 0xFF) {
            swap = 0;
        }
        desc->desc.u.tevconf.alpha_bias = swap;

        desc->desc.u.tevconf.alpha_clamp = texp->tev.a_clamp != 0;

        HSD_CheckAssert("TExp2TevDesc: a_dst is undefined", texp->tev.a_dst != 0xFF);

        desc->desc.u.tevconf.alpha_out_reg = args[texp->tev.a_dst];
        if (desc->desc.u.tevconf.alpha_out_reg == 0) {
            *init_aprev = 0;
        }
    }
    desc->desc.u.tevconf.mode = 0;
}

//80384F28
void HSD_TExpSetReg(HSD_TExp* texp)
{
    u32 changed, i;
    changed = 0;

    u8 tev_regid[4] = { 0, 1, 2, 3 };

    GXColor reg[8];

    for (; texp != NULL; texp = texp->cnst.next) {
        HSD_CheckAssert("HSD_TExpSetReg: texp->type != HSD_TE_CNST", texp->type == HSD_TE_CNST);

        if (texp->cnst.reg > 7) {
            continue;
        }

        s32 x;
        changed |= (1 << (u32)texp->cnst.reg);
        if (texp->cnst.comp != HSD_TE_RGB) {
            switch (texp->cnst.ctype) {
            case HSD_TE_U8:
                x = *(u8*)texp->cnst.val;
                break;

            case HSD_TE_U16:
                x = *(u16*)texp->cnst.val;
                break;

            case HSD_TE_U32:
                x = *(s32*)texp->cnst.val;
                break;

            case HSD_TE_F32:
                x = (s32)(255.0f * *(f32*)texp->cnst.val);
                break;

            case HSD_TE_F64:
                x = (s32)(255.0 * *(f64*)texp->cnst.val);
                break;
            }

            if (x > 255) {
                x = 255;
            }
            if (x < 0) {
                x = 0;
            }

            if (texp->cnst.reg < 4) {
                switch (texp->cnst.idx) {
                case 0:
                    reg[texp->cnst.reg].r = (u8)x;
                    break;

                case 1:
                    reg[texp->cnst.reg].g = (u8)x;
                    break;

                case 2:
                    reg[texp->cnst.reg].b = (u8)x;
                    break;

                case 3:
                    reg[texp->cnst.reg].a = (u8)x;
                    break;
                default:
                    reg[texp->cnst.reg].a = (u8)x;
                    break;
                }
            } else {
                if (texp->cnst.idx == 3) {
                    reg[texp->cnst.reg].a = (u8)x;
                } else {
                    reg[texp->cnst.reg].r = (u8)x;
                    reg[texp->cnst.reg].g = (u8)x;
                    reg[texp->cnst.reg].b = (u8)x;
                }
            }
        } else {
            HSD_TEType ctype = texp->cnst.ctype;
            s32 r;
            s32 g;
            s32 b;
            f64* ptr_64;
            if (ctype == 1) {
            LAB_80385060:
                if (ctype == 2) {
                LAB_803850dc:
                    ptr_64 = (f64*)(texp->cnst.val);
                    r = (u32)(255.0 * ptr_64[0]);
                    g = (u32)(255.0 * ptr_64[1]);
                    b = (u32)(255.0 * ptr_64[2]);
                } else {
                    if (ctype < 2) {
                        if (ctype < 1)
                            goto LAB_803850dc;
                        u16* ptr = (u16*)(texp->cnst.val);
                        r = ptr[0];
                        g = ptr[1];
                        b = ptr[2];
                    } else {
                        if (3 < ctype)
                            goto LAB_803850dc;
                        f32* ptr = (f32*)(texp->cnst.val);
                        r = (u32)(255.0f * ptr[0]);
                        g = (u32)(255.0f * ptr[1]);
                        b = (u32)(255.0f * ptr[2]);
                    }
                }

                if (r < 0) {
                    r = 0;
                }
                if (r > 255) {
                    r = 255;
                }
                reg[texp->cnst.reg].r = (u8)r;

                if (g < 0) {
                    g = 0;
                }
                if (g > 255) {
                    g = 255;
                }
                reg[texp->cnst.reg].g = (u8)g;

                if (b < 0) {
                    b = 0;
                }
                if (b > 255) {
                    b = 255;
                }
                reg[texp->cnst.reg].b = (u8)b;
            } else {
                if (ctype == 0) {
                    GXColor* c = (GXColor*)texp->cnst.val;
                    reg[texp->cnst.reg].r = c->r;
                    reg[texp->cnst.reg].g = c->g;
                    reg[texp->cnst.reg].b = c->b;
                    reg[texp->cnst.reg].a = reg[texp->cnst.reg].a;
                } else {
                    if (2 < ctype)
                        goto LAB_80385060;
                    u32* ptr = (u32*)(texp->cnst.val);
                    r = ptr[0];
                    g = ptr[1];
                    b = ptr[2];

                    if (r > 255) {
                        r = 255;
                    }
                    reg[texp->cnst.reg].r = (u8)r;

                    if (g > 255) {
                        g = 255;
                    }
                    reg[texp->cnst.reg].g = (u8)g;

                    if (b > 255) {
                        b = 255;
                    }
                    reg[texp->cnst.reg].b = (u8)b;
                }
            }
        }
    }

    if (changed != 0) {
        GX_PixModeSync();
        for (i = 0; i < 4; ++i) {
            if ((changed & (1 << i)) != 0) {
                GX_SetTevKColor(tev_regid[i], reg[i]);
            }
        }

        for (i = 4; i < 7; ++i) {
            if ((changed & (1 << i)) != 0) {
                GX_SetTevColor(tev_regid[i - 3], reg[i]);
            }
        }
        HSD_StateInvalidate(HSD_STATE_TEV_REGISTER);
        GX_PixModeSync();
    }
}

//80385448
void HSD_TExpSetupTev(HSD_TExpTevDesc* tevdesc, HSD_TExp* texp)
{
    HSD_TExpSetReg(texp);
    while (tevdesc != NULL) {
        if (tevdesc->tobj != NULL) {
            tevdesc->desc.map = ((HSD_TObj*)tevdesc->tobj)->id;
            tevdesc->desc.coord = ((HSD_TObj*)tevdesc->tobj)->coord;
        }
        HSD_StateAssignTev();
        HSD_SetupTevStage(&tevdesc->desc);
        tevdesc = (HSD_TExpTevDesc*)tevdesc->desc.next;
    }
}

//803854B4
s32 HSD_TExpCompile(HSD_TExp* texp, HSD_TExpTevDesc** tevdesc, HSD_TExp** texp_list)
{
    HSD_TExpRes res;
    s32 num;
    u32 init_cprev = 1;
    u32 init_aprev = 1;
    HSD_TExp* order[32];
    HSD_TExpDag list[32];

    assert(tevdesc != NULL);
    assert(texp_list != NULL);

    memset(&res, 0, sizeof(HSD_TExpRes));

    HSD_TExpRef(texp, 1);
    HSD_TExpRef(texp, 5);
    HSD_TExpSimplify(texp);

    num = HSD_TExpMakeDag(texp, list);
    HSD_TExpSchedule(num, list, order, &res);
    for (u32 i = 0; i < num; ++i) {
        s32 val = TExpAssignReg(order[i], &res);
        HSD_CheckAssert("HSD_TExpCompile: val < 0", val >= 0);
    }

    for (num = num - 1; num > -1; --num) {
        HSD_TExpSimplify2(order[num]);
    }

    num = HSD_TExpMakeDag(texp, list);
    HSD_TExpSchedule(num, list, order, &res);
    *tevdesc = NULL;
    for (u32 i = 0; i < num; ++i) {
        HSD_TExpTevDesc* tdesc = hsdAllocMemPiece(sizeof(HSD_TExpTevDesc));
        tdesc->desc.stage = HSD_Index2TevStage(i);
        TExp2TevDesc(order[(num - i) - 1], tdesc, &init_cprev, &init_aprev);
        tdesc->desc.next = &(*tevdesc)->desc;
        *tevdesc = tdesc;
    }

    *texp_list = HSD_TExpFreeList(*texp_list, HSD_TE_TEV, 1);
    *texp_list = HSD_TExpFreeList(*texp_list, HSD_TE_CNST, 0);
    return num;
}

//80385758
void HSD_TExpFreeTevDesc(HSD_TExpTevDesc* tdesc)
{
    HSD_TExpTevDesc* next = tdesc;
    while (next != NULL) {
        HSD_TExpTevDesc* temp = (HSD_TExpTevDesc*)next->desc.next;
        hsdFreeMemPiece(next, sizeof(HSD_TExpTevDesc));
        next = temp;
    }
}

//80385798
static s32 assign_reg(s32 num, u32* unused, HSD_TExpDag* list, s32* order)
{
    u8 dst;
    s32 c_use = 4;
    s32 a_use = 4;

    u8 a_reg[4] = { 0, 0, 0, 0 }; //r2_11F4
    u8 c_reg[4] = { 0, 0, 0, 0 }; //r2_11F8

    for (num = num - 1; num > -1; --num) {
        HSD_TETev* tev = list[order[num]].tev;
        for (s32 i = 0; i < 4; ++i) {
            u32 type = HSD_TExpGetType(tev->c_in[i].exp);
            if (type == HSD_TE_TEV) {
                if (tev->c_in[i].sel == GX_ENABLE) {
                    dst = tev->c_in[i].exp->tev.c_dst;
                    c_reg[dst] -= 1;
                } else {
                    dst = tev->c_in[i].exp->tev.a_dst;
                    a_reg[dst] -= 1;
                }
            }
            type = HSD_TExpGetType(tev->a_in[i].exp);
            if (type == HSD_TE_TEV) {
                dst = tev->a_in[i].exp->tev.a_dst;
                a_reg[dst] -= 1;
            }
        }

        tev = list[order[num]].tev;
        if (tev->c_ref > 0) {
            for (s32 i = 3; i > -1; --i) {
                if (c_reg[i] == 0) {
                    c_reg[i] = tev->c_ref;
                    tev->c_dst = i;
                    if (i < c_use) {
                        c_use = i;
                    }
                    break;
                }
            }
        }

        if (tev->a_ref > 0) {
            for (s32 i = 3; i > -1; --i) {
                if (a_reg[i] == 0) {
                    a_reg[i] = tev->a_ref;
                    tev->a_dst = i;
                    if (i < a_use) {
                        a_use = i;
                    }
                    break;
                }
            }
        }
    }
    return (8 - a_use) - c_use;
}

//80385B8C
void CalcDistance(HSD_TETev** tevs, s32* dist, HSD_TETev* tev, s32 num, s32 d)
{
    HSD_TETev** list = tevs;

    if (num > 0) {
        for (u32 i = 0; num <= i; ++i) {
            if (list[i] == tev) {
                if (dist[i] < d) {
                    dist[i] = d;
                    for (u32 j = 0; j < 4; ++j) {
                        if (tev->c_in[j].type == HSD_TE_TEV) {
                            CalcDistance(tevs, dist, &tev->c_in[j].exp->tev, num, d + 1);
                        }
                        if (tev->a_in[j].type == HSD_TE_TEV) {
                            CalcDistance(tevs, dist, &tev->a_in[j].exp->tev, num, d + 1);
                        }
                    }
                } else {
                    return;
                }
            }
        }
    }
}

//80385C60
s32 HSD_TExpMakeDag(HSD_TExp* root, HSD_TExpDag* list)
{
    s32 oidx;
    s32 iVar3;
    HSD_TExpDag* pHVar5;
    HSD_TExp** clist;
    HSD_TExp** ppabVar7;
    s32 iVar8;
    s32 k;
    HSD_TExpDag* cdag;
    HSD_TExpDag* pHVar9;
    HSD_TExp* te;
    s32 i;
    s32 j;
    s32 l;
    s32 idx;
    HSD_TExpDag* dag;
    HSD_TETev* tevn;
    u32 uVar10;
    HSD_TExp** tev_start;
    HSD_TExp* tevs[33];
    s32 dist[32];

    HSD_CheckAssert("HSD_TExpMakeDag: type != 1", HSD_TExpGetType(root) == HSD_TE_TEV);

    tevs[0] = root;
    i = 1;
    for (j = 0; j < i; ++j) {
        HSD_CheckAssert("HSD_TExpMakeDag: j > HSD_TEXP_MAX_NUM", j <= HSD_TEXP_MAX_NUM);

        HSD_TETev* tev = &tevs[j]->tev;
        for (k = 0; k < 4; ++k, i = l) {
            l = i;
            if (tev->c_in[k].type == HSD_TE_TEV) {
                u32 num = 0;
                for (num = 0; num < i; ++num) {
                    if (tevs[num] == tev->c_in[k].exp) {
                        break;
                    }
                }
                if (i <= num) {
                    l = i + 1;
                    tevs[i] = tev->c_in[k].exp;
                }
            }
        }

        for (k = 0; k < 4; ++k, i = l) {
            l = i;
            if (tev->a_in[k].type == HSD_TE_TEV) {
                u32 num = 0;
                for (num = 0; num < i; ++num) {
                    if (tevs[num] == tev->a_in[k].exp) {
                        break;
                    }
                }
                if (i <= num) {
                    l = i + 1;
                    tevs[i] = tev->a_in[k].exp;
                }
            }
        }
    }

    for (j = 0; j < i; j++) {
        dist[j] = -1;
    }
    CalcDistance((HSD_TETev**)tevs, dist, (HSD_TETev*)tevs[0], i, 0);

    j = 0;
    while (k = j, l = i, j < i) {
        while (l = k, k = l + 1, k < i) {
            if (dist[k] < dist[l]) {
                HSD_TExp* t = tevs[l];
                tevs[l] = tevs[k];
                tevs[k] = t;

                s32 temp = dist[l];
                dist[l] = dist[k];
                dist[k] = temp;
            }
        }
        ++j;
    }

#if 0
    for (; l > 0; --l){
        HSD_TExpDag* dag = &list[l];
        HSD_TETev* tev = &tevs[l]->tev;
        dag->idx = l;
        dag->nb_dep = 0;
        dag->nb_ref = 0;
        dag->tev = tev;

        for (j = 0; j < 4; ++j) {
            k = l;
            if (tev->c_in[j].type == HSD_TE_TEV) {
            JUMP_1:
                if (k < i) {
                    if (tev->c_in[j].exp != tevs[k]) {
                        ++k;
                        goto JUMP_1;
                    }
                    
                    HSD_TExpDag* t_dag = &list[k];
                    for (k = 0; k < dag->nb_dep && dag->depend[k] != t_dag; ++k) {
                        if (dag->depend[k] == t_dag) {
                            break;
                        }
                    }
                    if (dag->nb_dep <= k) {
                        u32 offset = dag->nb_dep;
                        dag->nb_dep += 1;
                        dag->depend[offset] = t_dag;
                        t_dag->nb_ref += 1;
                    }
                }
                assert(k < i);
            }
        }

        for (j = 0; j < 4; ++j) {
            k = l;
            if (tev->a_in[j].type == HSD_TE_TEV) {
            JUMP_2:
                if (k < i) {
                    if (tev->a_in[j].exp != tevs[k]) {
                        ++k;
                        goto JUMP_2;
                    }
                    
                    HSD_TExpDag* t_dag = &list[k];
                    for (k = 0; k < dag->nb_dep && dag->depend[k] != t_dag; ++k) {
                        if (dag->depend[k] == t_dag) {
                            break;
                        }
                    }
                    if (dag->nb_dep <= k) {
                        u32 offset = dag->nb_dep;
                        dag->nb_dep += 1;
                        dag->depend[offset] = t_dag;
                        t_dag->nb_ref += 1;
                    }
                }
                assert(k < i);
            }
        }
    }
#else
    for (idx = i - 1, dag = list + idx, tev_start = tevs + idx; idx >= 0; --idx, --dag, --tev_start) {
        te = *tev_start;
        dag->idx = idx;
        dag->nb_ref = 0;
        dag->nb_dep = 0;
        dag->tev = &te->tev;
        tevn = &te->tev;

        for (j = 0; j < 4; ++j) {
            if (tevn->c_in[j].type == 1) {
                oidx = i - idx;
                clist = &tevs[idx];
                iVar8 = idx;
                if (idx < i) {
                    do {
                        if (tevn->c_in[j].exp == *clist) {
                            cdag = list + iVar8;
                            iVar8 = 0;
                            pHVar5 = dag;
                            uVar10 = dag->nb_dep;
                            if (dag->nb_dep == 0)
                                goto LAB_80385f7c;
                            goto LAB_80385f64;
                        }
                        clist = clist + 1;
                        iVar8 = iVar8 + 1;
                        oidx = oidx + -1;
                    } while (oidx != 0);
                }
                goto LAB_80385fb8;
            }
            continue;
            while (true) {
                pHVar5 = (HSD_TExpDag*)&pHVar5->idx;
                iVar8 = iVar8 + 1;
                uVar10 = uVar10 - 1;
                if (uVar10 == 0)
                    break;
            LAB_80385f64:
                if (pHVar5->depend[0] == cdag)
                    break;
            }
        LAB_80385f7c:
            if (dag->nb_dep <= iVar8) {
                dag->nb_dep += 1;
                dag->depend[dag->nb_dep - 1] = cdag;
                cdag->nb_ref = cdag->nb_ref + 1;
            }
        LAB_80385fb8:
            HSD_CheckAssert("HSD_TExpMakeDag: i <= iVar8", i > iVar8);
        }

        for (j = 0; j < 4; ++j) {
            if (te->tev.a_in[j].type == 1) {
                iVar3 = i - idx;
                ppabVar7 = &tevs[idx];
                iVar8 = idx;
                if (idx < i) {
                    do {
                        if (te->tev.a_in[j].exp == *ppabVar7) {
                            pHVar9 = list + iVar8;
                            iVar8 = 0;
                            pHVar5 = dag;
                            uVar10 = dag->nb_dep;
                            if (dag->nb_dep == 0)
                                goto LAB_80386058;
                            goto LAB_80386040;
                        }
                        ppabVar7 = ppabVar7 + 1;
                        iVar8 = iVar8 + 1;
                        iVar3 = iVar3 + -1;
                    } while (iVar3 != 0);
                }
                goto LAB_80386094;
            }
            continue;
            while (true) {
                pHVar5 = (HSD_TExpDag*)&pHVar5->idx;
                iVar8 = iVar8 + 1;
                uVar10 = uVar10 - 1;
                if (uVar10 == 0)
                    break;
            LAB_80386040:
                if (pHVar5->depend[0] == pHVar9)
                    break;
            }
        LAB_80386058:
            if (dag->nb_dep <= iVar8) {
                dag->nb_dep += 1;
                dag->depend[dag->nb_dep - 1] = pHVar9;
                pHVar9->nb_ref = pHVar9->nb_ref + 1;
            }
        LAB_80386094:
            HSD_CheckAssert("HSD_TExpMakeDag: i <= iVar8", i > iVar8);
        }
    }
#endif
    return i;
}

//80385944
static void order_dag(s32 num, u32* dep_mtx, u32* full_dep_mtx, HSD_TExpDag* list, s32 depth, s32 idx, u32 done_set, u32 ready_set, s32* order, s32* min, s32* min_order)
{
    s32 depth_00;
    u32 total_dep;

    depth_00 = depth + 1;
    done_set = done_set | 1 << idx;
    order[depth] = idx & 0xff;
    if (depth_00 == num) {
        depth_00 = assign_reg(num, dep_mtx, list, order);
        if (depth_00 < *min) {
            *min = depth_00;
            for (u32 i = 0; i < num; i++) {
                min_order[i] = order[i];
            }
        }
    } else {
        total_dep = (ready_set & ~(1 << idx)) | dep_mtx[idx];
        u32 bits = 0;
        for (u32 i = 0; i < num; i++) {
            if ((total_dep & (1 << i)) != 0) {
                bits |= full_dep_mtx[i];
            }
        }
        total_dep &= ~bits;
        if ((list[idx].nb_dep == 1) && ((total_dep & dep_mtx[idx]) != 0)) {
            order_dag(num, dep_mtx, full_dep_mtx, list, depth_00, list[idx].depend[0]->idx, done_set, total_dep, order, min, min_order);
        } else {
            for (u32 i = 0; i < num; i++) {
                if ((total_dep & (1 << i)) != 0) {
                    order_dag(num, dep_mtx, full_dep_mtx, list, depth_00, i, done_set, total_dep, order, min, min_order);
                }
            }
        }
    }
}

static void make_dependency_mtx(s32 num, HSD_TExpDag* list, u32* dep_mtx)
{
    for (u32 i = 0; i < num; ++i) {
        dep_mtx[i] = 0;
        for (u32 j = 0; j < list[i].nb_dep; ++j) {
            dep_mtx[i] |= (1 << list[i].depend[j]->idx);
        }
    }
}

//80386100
static void make_full_dependency_mtx(s32 num, u32* dep, u32* full)
{
    BOOL changed;
    u32 old;

    for (u32 i = 0; i < num; ++i) {
        full[i] = dep[i];
    }
    do {
        changed = FALSE;
        for (u32 i = 0; i < num; ++i) {
            for (u32 j = 0; j < num; ++j) {
                if ((1 << i & full[j]) != 0) {
                    old = full[j];
                    full[j] = old | full[i];
                    if (old != full[j]) {
                        changed = TRUE;
                    }
                }
            }
        }
    } while (changed);
}

//80386234
void HSD_TExpSchedule(u32 num, HSD_TExpDag* list, HSD_TExp** result, HSD_TExpRes* resource)
{
    u32 args[4] = { GX_CA_A0, GX_CA_A1, GX_CA_A2, GX_CA_APREV };
    u32 a_in[4] = { GX_CC_A0, GX_CC_A1, GX_CC_A2, GX_CC_APREV };
    u32 c_in[4] = { GX_CC_C0, GX_CC_C1, GX_CC_C2, GX_CC_CPREV };

    u32 dep_mtx[32];
    u32 full_dep_mtx[32];
    s32 order[32];
    s32 min_order[32];
    s32 min = 5;
    u32 i, j;

    memset(min_order, 0, 32 * sizeof(s32));
    make_dependency_mtx(num, list, dep_mtx);
    make_full_dependency_mtx(num, dep_mtx, full_dep_mtx);
    order_dag(num, dep_mtx, full_dep_mtx, list, 0, 0, 0, 0, order, &min, min_order);

    for (i = 0; i < num; ++i) {
        result[i] = (HSD_TExp*)list[min_order[i]].tev;

        if (result[i]->tev.c_dst != HSD_TE_UNDEF) {
            resource->reg[result[i]->tev.c_dst + 4].color = 3;
            for (j = 0; j < 4; j++) {
                if (HSD_TExpGetType(result[i]->tev.c_in[j].exp) == HSD_TE_TEV) {
                    if (result[i]->tev.c_in[j].sel == 1) {
                        result[i]->tev.c_in[j].arg = c_in[result[i]->tev.c_in[j].exp->tev.c_dst];
                    } else {
                        result[i]->tev.c_in[j].arg = a_in[result[i]->tev.c_in[j].exp->tev.a_dst];
                    }
                }
            }
        }

        if (result[i]->tev.a_dst != HSD_TE_UNDEF) {
            resource->reg[result[i]->tev.a_dst + 4].alpha = 1;
            for (j = 0; j < 4; j++) {
                if (HSD_TExpGetType(result[i]->tev.a_in[j].exp) == HSD_TE_TEV) {
                    result[i]->tev.a_in[j].arg = args[result[i]->tev.a_in[j].exp->tev.a_dst];
                }
            }
        }
    }
}

//80386470
static u32 SimplifySrc(HSD_TExp* texp)
{
    HSD_TETev* tev = &texp->tev;
    BOOL res = FALSE;

    for (u32 i = 0; i < 4; i++) {
        if (tev->c_in[i].type == HSD_TE_TEV) {
            HSD_TExp* t = tev->c_in[i].exp;
            u8 sel = tev->c_in[i].sel;
            if (HSD_TExpSimplify(t) != 0) {
                res = TRUE;
            }
            if (sel == 1) {
                u8 c_op = t->tev.c_op;
                if (c_op == GX_COLORNULL) {
                    HSD_TExpUnref(t, tev->c_in[i].sel);
                    res = TRUE;
                    tev->c_in[i].type = 0;
                    tev->c_in[i].sel = 7;
                    tev->c_in[i].arg = 0xFF;
                    tev->c_in[i].exp = NULL;
                } else {
                    if (c_op == 0 && t->tev.c_in[0].sel == 7 && t->tev.c_in[1].sel == 7 && t->tev.c_bias == 0 && t->tev.c_scale == 0) {
                        u8 type = t->tev.c_in[3].type;

                        if (type == HSD_TE_TEX) {
                            if ((tev->tex == NULL || tev->tex == t->tev.tex)
                                && (tev->tex_swap == HSD_TE_UNDEF || (t->tev.tex_swap == HSD_TE_UNDEF || tev->tex_swap == t->tev.tex_swap))) {
                                tev->c_in[i].type = t->tev.c_in[3].type;
                                tev->c_in[i].sel = t->tev.c_in[3].sel;
                                tev->c_in[i].arg = t->tev.c_in[3].arg;
                                tev->c_in[i].exp = t->tev.c_in[3].exp;
                                tev->tex = t->tev.tex;
                                if (tev->tex_swap == HSD_TE_UNDEF) {
                                    tev->tex_swap = t->tev.tex_swap;
                                }
                                HSD_TExpUnref(t, sel);
                                res = TRUE;
                            }
                        } else if (type == HSD_TE_TEV && (t->tev.c_in[3].exp->tev.c_clamp != 0 || t->tev.c_clamp == 0)) {
                            tev->c_in[i].type = t->tev.c_in[3].type;
                            tev->c_in[i].sel = t->tev.c_in[3].sel;
                            tev->c_in[i].arg = t->tev.c_in[3].arg;
                            tev->c_in[i].exp = t->tev.c_in[3].exp;
                            HSD_TExpRef(tev->c_in[i].exp, tev->c_in[i].sel);
                            HSD_TExpUnref(t, sel);
                            res = TRUE;
                        } else if (type == HSD_TE_RAS && (tev->chan == GX_COLORNULL || tev->chan == t->tev.chan) && (tev->ras_swap == HSD_TE_UNDEF || t->tev.ras_swap == HSD_TE_UNDEF || tev->ras_swap == t->tev.ras_swap)) {
                            tev->c_in[i].type = t->tev.c_in[3].type;
                            tev->c_in[i].sel = t->tev.c_in[3].sel;
                            tev->c_in[i].arg = t->tev.c_in[3].arg;
                            tev->c_in[i].exp = t->tev.c_in[3].exp;
                            tev->chan = t->tev.chan;
                            if (tev->ras_swap == HSD_TE_UNDEF) {
                                tev->ras_swap = t->tev.ras_swap;
                            }
                            HSD_TExpUnref(t, sel);
                            res = TRUE;
                        } else if (type == HSD_TE_IMM) {
                            tev->c_in[i].type = t->tev.c_in[3].type;
                            tev->c_in[i].sel = t->tev.c_in[3].sel;
                            tev->c_in[i].arg = t->tev.c_in[3].arg;
                            tev->c_in[i].exp = t->tev.c_in[3].exp;
                            HSD_TExpRef(tev->c_in[i].exp, tev->c_in[i].sel);
                            HSD_TExpUnref(t, sel);
                            res = TRUE;
                        }
                    }
                }
            } else if (t->tev.a_op == 0xFF) {
                HSD_TExpUnref(t, sel);
                tev->c_in[i].type = 0;
                tev->c_in[i].sel = 7;
                tev->c_in[i].arg = 0xFF;
                tev->c_in[i].exp = NULL;
                res = TRUE;
            }
        }
    }

    for (u32 i = 0; i < 4; i++) {
        if (tev->a_in[i].type == HSD_TE_TEV) {
            HSD_TExp* t = tev->a_in[i].exp;
            u8 sel = tev->a_in[i].sel;
            HSD_TExpSimplify(t);
            u8 op = t->tev.a_op;
            if (op == GX_COLORNULL) {
                HSD_TExpUnref(t, sel);
                res = TRUE;
                tev->a_in[i].type = 0;
                tev->a_in[i].sel = 7;
                tev->a_in[i].arg = 0xFF;
                tev->a_in[i].exp = NULL;
            } else {
                if (op == 0 && t->tev.a_in[0].sel == 7 && t->tev.a_in[1].sel == 7 && t->tev.a_bias == 0 && t->tev.a_scale == 0) {
                    u8 type = t->tev.a_in[3].type;
                    if (type == HSD_TE_TEX) {
                        if (tev->tex == NULL || tev->tex == t->tev.tex) {
                            tev->a_in[i].type = t->tev.a_in[3].type;
                            tev->a_in[i].sel = t->tev.a_in[3].sel;
                            tev->a_in[i].arg = t->tev.a_in[3].arg;
                            tev->a_in[i].exp = t->tev.a_in[3].exp;
                            tev->tex = t->tev.tex;
                            HSD_TExpUnref(t, sel);
                            res = TRUE;
                        }
                    } else if (type == HSD_TE_TEV) {
                        tev->a_in[i].type = t->tev.a_in[3].type;
                        tev->a_in[i].sel = t->tev.a_in[3].sel;
                        tev->a_in[i].arg = t->tev.a_in[3].arg;
                        tev->a_in[i].exp = t->tev.a_in[3].exp;
                        HSD_TExpRef(tev->a_in[i].exp, tev->a_in[i].sel);
                        HSD_TExpUnref(t, sel);
                        res = TRUE;
                    } else if (type == HSD_TE_RAS && (tev->chan == GX_COLORNULL || tev->chan == t->tev.chan)) {
                        tev->a_in[i].type = t->tev.a_in[3].type;
                        tev->a_in[i].sel = t->tev.a_in[3].sel;
                        tev->a_in[i].arg = t->tev.a_in[3].arg;
                        tev->a_in[i].exp = t->tev.a_in[3].exp;
                        tev->chan = t->tev.chan;
                        HSD_TExpUnref(t, sel);
                        res = TRUE;
                    } else if (type == HSD_TE_IMM) {
                        tev->a_in[i].type = t->tev.a_in[3].type;
                        tev->a_in[i].sel = t->tev.a_in[3].sel;
                        tev->a_in[i].arg = t->tev.a_in[3].arg;
                        tev->a_in[i].exp = t->tev.a_in[3].exp;
                        HSD_TExpRef(tev->a_in[i].exp, tev->a_in[i].sel);
                        HSD_TExpUnref(t, sel);
                        res = TRUE;
                    }
                }
            }
        }
    }
    return res;
}

//8038687C
static u32 SimplifyThis(HSD_TExp* texp)
{
    u8 op;
    u8 type;
    BOOL match;
    s32 tex_c;
    s32 tex_a;
    s32 ras_c;
    s32 ras_a;
    BOOL simplified = FALSE;

    while (true) {
        tex_c = -1;
        tex_a = -1;
        ras_c = -1;
        ras_a = -1;

        for (u32 i = 0; i < 4; i++) {
            type = texp->tev.c_in[i].type;
            if (type == HSD_TE_RAS) {
                ras_c = i;
            } else if (type == HSD_TE_TEX) {
                tex_c = i;
            }

            type = texp->tev.a_in[i].type;
            if (type == HSD_TE_RAS) {
                ras_a = i;
            } else if (type == HSD_TE_TEX) {
                tex_a = i;
            }
        }

        if (tex_c == -1 && tex_a == -1) {
            texp->tev.tex = NULL;
            texp->tev.tex_swap = HSD_TE_UNDEF;
        }
        if (ras_c == -1 && ras_a == -1) {
            texp->tev.chan = 0xFF;
            texp->tev.ras_swap = HSD_TE_UNDEF;
        }

        match = FALSE;
        if (texp->tev.a_op == 0xFF || texp->tev.a_op == 0xE || texp->tev.a_op == 0xF) {
            if ((texp->tev.c_op != 0xFF) && (texp->tev.c_ref == 0)) {
                texp->tev.c_op = 0xFF;
                for (u32 i = 0; i < 4; i++) {
                    HSD_TExpUnref(texp->tev.c_in[i].exp, texp->tev.c_in[i].sel);
                    texp->tev.c_in[i].type = 0;
                    texp->tev.c_in[i].sel = 7;
                    texp->tev.c_in[i].arg = 0xFF;
                    texp->tev.c_in[i].exp = NULL;
                }
                match = TRUE;
            }
            switch (texp->tev.c_op) {
            case 0:
            case 1:
                if (texp->tev.c_in[2].sel == 7) {
                    if (texp->tev.c_in[1].sel != 7) {
                        HSD_TExpUnref(texp->tev.c_in[1].exp, texp->tev.c_in[1].sel);
                        texp->tev.c_in[1].type = 0;
                        texp->tev.c_in[1].sel = 7;
                        texp->tev.c_in[1].arg = 0xFF;
                        texp->tev.c_in[1].exp = NULL;

                        match = TRUE;
                    }
                    if ((texp->tev.c_op == 0) && (texp->tev.c_in[3].sel == 7)) {
                        texp->tev.c_in[3].type = texp->tev.c_in[0].type;
                        texp->tev.c_in[3].sel = texp->tev.c_in[0].sel;
                        texp->tev.c_in[3].arg = texp->tev.c_in[0].arg;
                        texp->tev.c_in[3].exp = texp->tev.c_in[0].exp;

                        texp->tev.c_in[0].type = 0;
                        texp->tev.c_in[0].sel = 7;
                        texp->tev.c_in[0].arg = 0xFF;
                        texp->tev.c_in[0].exp = NULL;

                        texp->tev.c_clamp = 1;

                        match = TRUE;
                    }
                }

                if (texp->tev.c_in[2].sel == 8) {
                    if (texp->tev.c_in[0].sel != 7) {
                        HSD_TExpUnref(texp->tev.c_in[0].exp, texp->tev.c_in[0].sel);
                        texp->tev.c_in[0].type = 0;
                        texp->tev.c_in[0].sel = 7;
                        texp->tev.c_in[0].arg = 0xFF;
                        texp->tev.c_in[0].exp = NULL;

                        match = TRUE;
                    }
                    if (texp->tev.c_op == 0 && texp->tev.c_in[3].sel == 7) {
                        texp->tev.c_in[3].type = texp->tev.c_in[1].type;
                        texp->tev.c_in[3].sel = texp->tev.c_in[1].sel;
                        texp->tev.c_in[3].arg = texp->tev.c_in[1].arg;
                        texp->tev.c_in[3].exp = texp->tev.c_in[1].exp;

                        texp->tev.c_in[1].type = 0;
                        texp->tev.c_in[1].sel = 7;
                        texp->tev.c_in[1].arg = 0xFF;
                        texp->tev.c_in[1].exp = NULL;

                        texp->tev.c_in[2].type = 0;
                        texp->tev.c_in[2].sel = 7;
                        texp->tev.c_in[2].arg = 0xFF;
                        texp->tev.c_in[2].exp = NULL;

                        match = TRUE;
                    }
                }

                if (texp->tev.c_in[0].sel == 7 && texp->tev.c_in[1].sel == 8) {
                    texp->tev.c_in[0].type = texp->tev.c_in[2].type;
                    texp->tev.c_in[0].sel = texp->tev.c_in[2].sel;
                    texp->tev.c_in[0].arg = texp->tev.c_in[2].arg;
                    texp->tev.c_in[0].exp = texp->tev.c_in[2].exp;

                    texp->tev.c_in[1].type = 0;
                    texp->tev.c_in[1].sel = 7;
                    texp->tev.c_in[1].arg = 0xFF;
                    texp->tev.c_in[1].exp = NULL;

                    texp->tev.c_in[2].type = 0;
                    texp->tev.c_in[2].sel = 7;
                    texp->tev.c_in[2].arg = 0xFF;
                    texp->tev.c_in[2].exp = NULL;

                    match = true;
                }

                if (texp->tev.c_in[0].sel == 7 && texp->tev.c_in[1].sel == 7 && texp->tev.c_in[3].sel == 7 && texp->tev.c_bias == 0) {
                    texp->tev.c_op = 0xff;

                    HSD_TExpUnref(texp->tev.c_in[2].exp, texp->tev.c_in[2].sel);
                    texp->tev.c_in[2].type = 0;
                    texp->tev.c_in[2].sel = 7;
                    texp->tev.c_in[2].arg = 0xFF;
                    texp->tev.c_in[2].exp = NULL;

                    match = TRUE;
                }
                break;
            case 8:
            case 10:
            case 12:
            case 14:
                if (texp->tev.c_in[2].sel == 7) {
                    texp->tev.c_op = 0;

                    HSD_TExpUnref(texp->tev.c_in[0].exp, texp->tev.c_in[0].sel);
                    texp->tev.c_in[0].type = 0;
                    texp->tev.c_in[0].sel = 7;
                    texp->tev.c_in[0].arg = 0xFF;
                    texp->tev.c_in[0].exp = NULL;

                    HSD_TExpUnref(texp->tev.c_in[1].exp, texp->tev.c_in[1].sel);
                    texp->tev.c_in[1].type = 0;
                    texp->tev.c_in[1].sel = 7;
                    texp->tev.c_in[1].arg = 0xFF;
                    texp->tev.c_in[1].exp = NULL;

                    match = TRUE;
                } else if (texp->tev.c_in[0].sel == 7) {
                    texp->tev.c_op = 0;

                    HSD_TExpUnref(texp->tev.c_in[1].exp, texp->tev.c_in[1].sel);
                    texp->tev.c_in[1].type = 0;
                    texp->tev.c_in[1].sel = 7;
                    texp->tev.c_in[1].arg = 0xFF;
                    texp->tev.c_in[1].exp = NULL;

                    HSD_TExpUnref(texp->tev.c_in[2].exp, texp->tev.c_in[2].sel);
                    texp->tev.c_in[2].type = 0;
                    texp->tev.c_in[2].sel = 7;
                    texp->tev.c_in[2].arg = 0xFF;
                    texp->tev.c_in[2].exp = NULL;

                    match = TRUE;
                }
                break;
            case 9:
            case 11:
            case 13:
            case 15:
                if (texp->tev.c_in[2].sel == 7) {
                    texp->tev.c_op = 0;

                    HSD_TExpUnref(texp->tev.c_in[0].exp, texp->tev.c_in[0].sel);
                    texp->tev.c_in[0].type = 0;
                    texp->tev.c_in[0].sel = 7;
                    texp->tev.c_in[0].arg = 0xFF;
                    texp->tev.c_in[0].exp = NULL;

                    HSD_TExpUnref(texp->tev.c_in[1].exp, texp->tev.c_in[1].sel);
                    texp->tev.c_in[1].type = 0;
                    texp->tev.c_in[1].sel = 7;
                    texp->tev.c_in[1].arg = 0xFF;
                    texp->tev.c_in[1].exp = NULL;

                    match = TRUE;
                } else if ((texp->tev.c_in[0].sel == 7) && (texp->tev.c_in[1].sel == 7)) {
                    texp->tev.c_op = 0;

                    texp->tev.c_in[0].type = texp->tev.c_in[2].type;
                    texp->tev.c_in[0].sel = texp->tev.c_in[2].sel;
                    texp->tev.c_in[0].arg = texp->tev.c_in[2].arg;
                    texp->tev.c_in[0].exp = texp->tev.c_in[2].exp;

                    texp->tev.c_in[2].type = 0;
                    texp->tev.c_in[2].sel = 7;
                    texp->tev.c_in[2].arg = 0xFF;
                    texp->tev.c_in[2].exp = NULL;

                    match = TRUE;
                }
            }
        }

        if ((texp->tev.a_op != 0xFF) && (texp->tev.a_ref == 0)) {
            texp->tev.a_op = 0xFF;
            for (u32 i = 0; i < 4; ++i) {
                HSD_TExpUnref(texp->tev.a_in[i].exp, texp->tev.a_in[i].sel);
                texp->tev.a_in[i].type = 0;
                texp->tev.a_in[i].sel = 7;
                texp->tev.a_in[i].arg = 0xFF;
                texp->tev.a_in[i].exp = NULL;
            }
            match = TRUE;
        }

        op = texp->tev.a_op;
        if (op == 0xE) {
            if (texp->tev.a_in[2].sel == 7) {
                texp->tev.a_op = 0;

                HSD_TExpUnref(texp->tev.a_in[0].exp, texp->tev.a_in[0].sel);
                texp->tev.a_in[0].type = 0;
                texp->tev.a_in[0].sel = 7;
                texp->tev.a_in[0].arg = 0xFF;
                texp->tev.a_in[0].exp = NULL;

                HSD_TExpUnref(texp->tev.a_in[1].exp, texp->tev.a_in[1].sel);
                texp->tev.a_in[1].type = 0;
                texp->tev.a_in[1].sel = 7;
                texp->tev.a_in[1].arg = 0xFF;
                texp->tev.a_in[1].exp = NULL;

                match = TRUE;
            } else if (texp->tev.a_in[0].sel == 7) {
                texp->tev.a_op = 0;

                HSD_TExpUnref(texp->tev.a_in[1].exp, texp->tev.a_in[1].sel);
                texp->tev.a_in[1].type = 0;
                texp->tev.a_in[1].sel = 7;
                texp->tev.a_in[1].arg = 0xFF;
                texp->tev.a_in[1].exp = NULL;

                HSD_TExpUnref(texp->tev.a_in[2].exp, texp->tev.a_in[2].sel);
                texp->tev.a_in[2].type = 0;
                texp->tev.a_in[2].sel = 7;
                texp->tev.a_in[2].arg = 0xFF;
                texp->tev.a_in[2].exp = NULL;

                match = TRUE;
            }
        } else if (op < 0xe) {
            if (op < 2) {
                if (texp->tev.a_in[2].sel == 7) {
                    if (texp->tev.a_in[1].sel != 7) {
                        HSD_TExpUnref(texp->tev.a_in[1].exp, texp->tev.a_in[1].sel);
                        texp->tev.a_in[1].type = 0;
                        texp->tev.a_in[1].sel = 7;
                        texp->tev.a_in[1].arg = 0xFF;
                        texp->tev.a_in[1].exp = NULL;

                        match = TRUE;
                    }
                    if (texp->tev.a_op == 0 && texp->tev.a_in[3].sel == 7) {
                        texp->tev.a_in[3].type = texp->tev.a_in[0].type;
                        texp->tev.a_in[3].sel = texp->tev.a_in[0].sel;
                        texp->tev.a_in[3].arg = texp->tev.a_in[0].arg;
                        texp->tev.a_in[3].exp = texp->tev.a_in[0].exp;

                        texp->tev.a_in[0].type = 0;
                        texp->tev.a_in[0].sel = 7;
                        texp->tev.a_in[0].arg = 0xFF;
                        texp->tev.a_in[0].exp = NULL;

                        match = TRUE;
                    }
                }

                if (texp->tev.a_in[2].sel == 8) {
                    if (texp->tev.a_in[0].sel != 7) {
                        HSD_TExpUnref(texp->tev.a_in[0].exp, texp->tev.a_in[0].sel);
                        texp->tev.a_in[0].type = 0;
                        texp->tev.a_in[0].sel = 7;
                        texp->tev.a_in[0].arg = 0xFF;
                        texp->tev.a_in[0].exp = NULL;

                        match = TRUE;
                    }
                    if (texp->tev.a_op == 0 && texp->tev.a_in[3].sel == 7) {
                        texp->tev.a_in[3].type = texp->tev.a_in[1].type;
                        texp->tev.a_in[3].sel = texp->tev.a_in[1].sel;
                        texp->tev.a_in[3].arg = texp->tev.a_in[1].arg;
                        texp->tev.a_in[3].exp = texp->tev.a_in[1].exp;

                        texp->tev.a_in[1].type = 0;
                        texp->tev.a_in[1].sel = 7;
                        texp->tev.a_in[1].arg = 0xFF;
                        texp->tev.a_in[1].exp = NULL;

                        texp->tev.a_in[2].type = 0;
                        texp->tev.a_in[2].sel = 7;
                        texp->tev.a_in[2].arg = 0xFF;
                        texp->tev.a_in[2].exp = NULL;

                        match = TRUE;
                    }
                }

                if (texp->tev.a_in[0].sel == 7 && texp->tev.a_in[1].sel == 7 && texp->tev.a_in[3].sel == 7) {
                    texp->tev.a_op = 0xFF;
                    match = TRUE;
                }
            } else if (7 < op && texp->tev.a_in[2].sel == 7) {
                texp->tev.a_op = 0;

                HSD_TExpUnref(texp->tev.a_in[0].exp, texp->tev.a_in[0].sel);
                texp->tev.a_in[0].type = 0;
                texp->tev.a_in[0].sel = 7;
                texp->tev.a_in[0].arg = 0xFF;
                texp->tev.a_in[0].exp = NULL;

                HSD_TExpUnref(texp->tev.a_in[1].exp, texp->tev.a_in[1].sel);
                texp->tev.a_in[1].type = 0;
                texp->tev.a_in[1].sel = 7;
                texp->tev.a_in[1].arg = 0xFF;
                texp->tev.a_in[1].exp = NULL;

                match = TRUE;
            }
        } else if (op < 0x10) {
            if (texp->tev.a_in[2].sel == 7) {
                texp->tev.a_op = 0;

                HSD_TExpUnref(texp->tev.a_in[0].exp, texp->tev.a_in[0].sel);
                texp->tev.a_in[0].type = 0;
                texp->tev.a_in[0].sel = 7;
                texp->tev.a_in[0].arg = 0xFF;
                texp->tev.a_in[0].exp = NULL;

                HSD_TExpUnref(texp->tev.a_in[1].exp, texp->tev.a_in[1].sel);
                texp->tev.a_in[1].type = 0;
                texp->tev.a_in[1].sel = 7;
                texp->tev.a_in[1].arg = 0xFF;
                texp->tev.a_in[1].exp = NULL;

                match = TRUE;
            } else if (texp->tev.a_in[0].sel == 7 && texp->tev.a_in[1].sel == 7) {
                texp->tev.a_op = 0;

                texp->tev.a_in[0].type = texp->tev.a_in[2].type;
                texp->tev.a_in[0].sel = texp->tev.a_in[2].sel;
                texp->tev.a_in[0].arg = texp->tev.a_in[2].arg;
                texp->tev.a_in[0].exp = texp->tev.a_in[2].exp;

                texp->tev.a_in[2].type = 0;
                texp->tev.a_in[2].sel = 7;
                texp->tev.a_in[2].arg = 0xFF;
                texp->tev.a_in[2].exp = NULL;

                match = TRUE;
            }
        }
        if (match == FALSE)
            break;
        simplified = 1;
    }
    return simplified;
}

static u32 CalcBias(u8 op, u8 b0, u8 b1, u8* res)
{
    s32 bias = 0;
    if (b1 == 2) {
        bias = -1;
    } else if ((b1 < 2) && (b1 != 0)) {
        bias = 1;
    } else {
        bias = 0;
    }

    if (op == 1) {
        bias = -bias;
    }

    if (b0 == 2) {
        bias -= 1;
    } else if ((b0 < 2) && (0 < b0)) {
        bias += 1;
    }

    if (bias == 0) {
        *res = 0;
        return TRUE;
    } else {
        if (bias < 0) {
            if (-2 < bias) {
                *res = 2;
                return TRUE;
            }
        } else if (bias < 2) {
            *res = 1;
            return TRUE;
        }
    }
    return FALSE;
}

static void MergeResources(HSD_TETev* dst, HSD_TETev* src)
{
    if (dst->tex == NULL) {
        dst->tex = src->tex;
    }
    if (dst->chan == HSD_TE_UNDEF) {
        dst->chan = src->chan;
    }
    if (dst->tex_swap == HSD_TE_UNDEF) {
        dst->tex_swap = src->tex_swap;
    }
    if (dst->ras_swap == HSD_TE_UNDEF) {
        dst->ras_swap = src->ras_swap;
    }
}

static u32 ResConflict(HSD_TExp* t1, HSD_TExp* t2)
{
    if (t1->tev.tex == NULL || t2->tev.tex == NULL || t1->tev.tex == t2->tev.tex) {
        if (t1->tev.chan == 0xFF || t2->tev.chan == 0xFF || t1->tev.chan == t2->tev.chan) {
            return 0;
        } else {
            return 1;
        }
    }
    return 1;
}

static u32 IsUsingColorCnst(HSD_TExp* texp)
{
    if ((texp->tev.c_in[0].type != HSD_TE_CNST) && (texp->tev.c_in[1].type != HSD_TE_CNST)) {
        if (texp->tev.c_in[2].type != HSD_TE_CNST) {
            if (texp->tev.c_in[3].type != HSD_TE_CNST) {
                return 0;
            }
        }
    }
    return 1;
}

//803870E4
static u32 SimplifyByMerge(HSD_TExp* texp)
{
#if 0
    u8 sel;
    BOOL bVar2;
    BOOL bVar3;
    u32 result;

    HSD_TExp* curr;
    result = FALSE;
    do {
        sel = texp->tev.c_op;
        bVar3 = false;
        if ((texp->tev.c_op == 0 || texp->tev.c_op == 1) && texp->tev.c_in[2].sel == 7) {
            curr = texp->tev.c_in[0].exp;
            sel = texp->tev.c_in[0].sel;
            if (texp->tev.c_in[0].type != HSD_TE_TEV || texp->tev.c_in[0].sel != 1 || curr->tev.c_range != 0 || texp->tev.c_in[3].sel != 7 || ResConflict(texp, curr) != 0 || (IsUsingColorCnst(texp) != 0 && IsUsingColorCnst(curr) != 0)) {

                curr = texp->tev.c_in[3].exp;
                sel = texp->tev.c_in[3].sel;

                if (texp->tev.c_op != 0 || texp->tev.c_in[3].type != 1 || texp->tev.c_in[3].sel != 1 || curr->tev.c_range != 0 || curr->tev.c_in[3].sel != 7 || ResConflict(texp, curr) != 0 || (IsUsingColorCnst(texp) != 0 && IsUsingColorCnst(curr) != 0)) {
                    goto ALPHA;
                }
            }
        }

    ALPHA:
    } while (true);
#else
    u8 sel;
    BOOL bVar3;
    u32 result;

    HSD_TExp* curr;
    result = FALSE;
    do {
        sel = texp->tev.a_op;
        bVar3 = false;
        if (sel == 0xFF || ((u8)(sel - 0xe) < 2) || sel < 2) {
            if ((texp->tev.c_op == 0 || texp->tev.c_op == 1)
                && texp->tev.c_in[1].sel == 7 && texp->tev.c_in[2].sel == 7
                && (HSD_TExpGetType(texp->tev.c_in[0].exp) != HSD_TE_CNST)
                && (HSD_TExpGetType(texp->tev.c_in[3].exp) != HSD_TE_CNST)) {

                if (texp->tev.c_op == 0 && texp->tev.c_in[3].type == HSD_TE_TEV
                    && ((texp->tev.c_in[3].sel == 1 && texp->tev.c_in[3].exp->tev.c_clamp != 0) || (texp->tev.c_in[3].sel == 5 && texp->tev.c_in[3].exp->tev.a_clamp != 0))
                    && (texp->tev.c_in[0].type < 4 && texp->tev.c_in[0].type > 1)) {
                    u8 type = texp->tev.c_in[0].type;
                    u8 select = texp->tev.c_in[0].sel;
                    u8 arg = texp->tev.c_in[0].arg;
                    HSD_TExp* t = texp->tev.c_in[0].exp;

                    texp->tev.c_in[0].type = texp->tev.c_in[3].type;
                    texp->tev.c_in[0].sel = texp->tev.c_in[3].sel;
                    texp->tev.c_in[0].arg = texp->tev.c_in[3].arg;
                    texp->tev.c_in[0].exp = texp->tev.c_in[3].exp;

                    texp->tev.c_in[3].type = type;
                    texp->tev.c_in[3].sel = select;
                    texp->tev.c_in[3].arg = arg;
                    texp->tev.c_in[3].exp = t;
                }

                if (texp->tev.c_in[0].type == HSD_TE_TEV) {
                    if (texp->tev.c_in[0].sel == 1) {
                        curr = texp->tev.c_in[0].exp;
                        if ((curr->tev.c_op == 0 || curr->tev.c_op == 1)
                            && curr->tev.c_in[3].sel == 7 && curr->tev.c_scale == 0) {

                            if (ResConflict(texp, curr) == 0) {
                                bVar3 = CalcBias(curr->tev.c_op, texp->tev.c_bias, curr->tev.c_bias, &texp->tev.c_bias);
                                if (bVar3) {
                                    if (curr->tev.c_op == 1) {
                                        texp->tev.c_op = texp->tev.c_op == 0;
                                    }
                                    for (u32 i = 0; i < 3; ++i) {
                                        texp->tev.c_in[i].type = curr->tev.c_in[i].type;
                                        texp->tev.c_in[i].sel = curr->tev.c_in[i].sel;
                                        texp->tev.c_in[i].arg = curr->tev.c_in[i].arg;
                                        texp->tev.c_in[i].exp = curr->tev.c_in[i].exp;
                                        HSD_TExpRef(texp->tev.c_in[i].exp, texp->tev.c_in[i].sel);
                                    }
                                    HSD_TExpUnref(curr, 1);
                                    MergeResources(&texp->tev, &curr->tev);
                                }
                            }
                        }
                    }
                } else {
                    if (sel == HSD_TE_ZERO && texp->tev.c_in[3].type == HSD_TE_TEV && texp->tev.c_in[3].sel == 1
                        && (curr = texp->tev.c_in[3].exp, curr->tev.c_scale == 0)
                        && (texp->tev.c_bias == 0 || texp->tev.c_bias != curr->tev.c_bias)) {

                        if (ResConflict(texp, curr) == 0) {
                            bVar3 = true;
                            for (u32 i = 0; i < 4; ++i) {
                                texp->tev.c_in[i].type = curr->tev.c_in[i].type;
                                texp->tev.c_in[i].sel = curr->tev.c_in[i].sel;
                                texp->tev.c_in[i].arg = curr->tev.c_in[i].arg;
                                texp->tev.c_in[i].exp = curr->tev.c_in[i].exp;
                                HSD_TExpRef(texp->tev.c_in[i].exp, texp->tev.c_in[i].sel);
                            }
                            texp->tev.c_op = curr->tev.c_op;
                            CalcBias(curr->tev.c_op, texp->tev.c_bias, curr->tev.c_bias, &texp->tev.c_bias);

                            if (texp->tev.c_clamp == 0xFF || texp->tev.c_clamp == 0) {
                                texp->tev.c_clamp = curr->tev.c_clamp;
                            }
                            MergeResources(&texp->tev, &curr->tev);
                            HSD_TExpUnref(curr, 1);
                        }
                    }
                }
            }

            if ((texp->tev.a_op == 0 || texp->tev.a_op == 1)
                && texp->tev.a_in[1].sel == 7 && texp->tev.a_in[2].sel == 7
                && (HSD_TExpGetType(texp->tev.a_in[0].exp) != HSD_TE_CNST)
                && (HSD_TExpGetType(texp->tev.a_in[3].exp) != HSD_TE_CNST)) {

                if (texp->tev.a_op == 0 && texp->tev.a_in[3].type == HSD_TE_TEV && texp->tev.a_in[3].exp->tev.a_clamp != 0
                    && (texp->tev.a_in[0].type < 4 && texp->tev.a_in[0].type > 1)) {
                    u8 type = texp->tev.a_in[0].type;
                    u8 select = texp->tev.a_in[0].sel;
                    u8 arg = texp->tev.a_in[0].arg;
                    HSD_TExp* t = texp->tev.a_in[0].exp;

                    texp->tev.a_in[0].type = texp->tev.a_in[3].type;
                    texp->tev.a_in[0].sel = texp->tev.a_in[3].sel;
                    texp->tev.a_in[0].arg = texp->tev.a_in[3].arg;
                    texp->tev.a_in[0].exp = texp->tev.a_in[3].exp;

                    texp->tev.a_in[3].type = type;
                    texp->tev.a_in[3].sel = select;
                    texp->tev.a_in[3].arg = arg;
                    texp->tev.a_in[3].exp = t;
                }

                if (texp->tev.a_in[0].type == HSD_TE_TEV) {
                    curr = texp->tev.a_in[0].exp;
                    sel = texp->tev.a_in[0].sel;
                    if ((curr->tev.a_op == 0 || curr->tev.a_op == 1)
                        && curr->tev.a_in[3].sel == 7 && curr->tev.a_scale == 0) {

                        if (ResConflict(texp, curr) == 0) {
                            bVar3 = CalcBias(curr->tev.a_op, texp->tev.a_bias, curr->tev.a_bias, &texp->tev.a_bias);
                            if (bVar3) {
                                if (curr->tev.a_op == 1) {
                                    texp->tev.a_op = texp->tev.a_op == 0;
                                }
                                for (u32 i = 0; i < 3; ++i) {
                                    texp->tev.a_in[i].type = curr->tev.a_in[i].type;
                                    texp->tev.a_in[i].sel = curr->tev.a_in[i].sel;
                                    texp->tev.a_in[i].arg = curr->tev.a_in[i].arg;
                                    texp->tev.a_in[i].exp = curr->tev.a_in[i].exp;
                                    HSD_TExpRef(texp->tev.a_in[i].exp, texp->tev.a_in[i].sel);
                                }

                                if (texp->tev.tex == NULL) {
                                    texp->tev.tex = curr->tev.tex;
                                }
                                if (texp->tev.chan == 0xFF) {
                                    texp->tev.chan = curr->tev.chan;
                                }
                                HSD_TExpUnref(curr, sel);
                            }
                        }
                    }
                } else {
                    if (texp->tev.a_in[0].type == HSD_TE_ZERO && texp->tev.a_in[3].type == HSD_TE_TEV) {
                        curr = texp->tev.a_in[3].exp;
                        sel = texp->tev.a_in[3].sel;
                        if (curr->tev.a_scale == 0 && (texp->tev.a_bias == 0 || texp->tev.a_bias != curr->tev.a_bias)) {

                            if (ResConflict(texp, curr) == 0) {
                                bVar3 = true;
                                for (u32 i = 0; i < 4; ++i) {
                                    texp->tev.a_in[i].type = curr->tev.a_in[i].type;
                                    texp->tev.a_in[i].sel = curr->tev.a_in[i].sel;
                                    texp->tev.a_in[i].arg = curr->tev.a_in[i].arg;
                                    texp->tev.a_in[i].exp = curr->tev.a_in[i].exp;
                                    HSD_TExpRef(texp->tev.a_in[i].exp, texp->tev.a_in[i].sel);
                                }
                                texp->tev.a_op = curr->tev.a_op;
                                CalcBias(curr->tev.a_op, texp->tev.a_bias, curr->tev.a_bias, &texp->tev.a_bias);

                                if (texp->tev.a_clamp == 0xFF || texp->tev.a_clamp == 0) {
                                    texp->tev.a_clamp = curr->tev.a_clamp;
                                }
                                if (texp->tev.tex == NULL) {
                                    texp->tev.tex = curr->tev.tex;
                                }
                                if (texp->tev.chan == 0xFF) {
                                    texp->tev.chan = curr->tev.chan;
                                }
                                HSD_TExpUnref(curr, sel);
                            }
                        }
                    }
                }
            }
        }
        if (!bVar3) {
            return result;
        }
        result = TRUE;
    } while (true);
#endif
}

#ifdef NEW_LIB
static void CalcTevRange(HSD_TExp* texp)
{
    u32 upper = 0;
    if (texp->tev.c_in[3].sel == HSD_TE_0) {
    COLOR_LOOP:
        for (u32 i = 0; i < 3; i++) {
            if (texp->tev.c_in[i].sel != HSD_TE_0) {
                if (texp->tev.c_op == 1) {
                    texp->tev.c_range = 1;
                    goto ALPHA;
                }
                upper += 256;
                break;
            }
        }

        switch (texp->tev.c_bias) {
        case 0:
            switch (texp->tev.c_scale) {
            case 1:
                upper = upper << 1;
                break;
            case 2:
                upper = upper << 2;
                break;
            case 3:
                upper = upper >> 1;
                break;
            }
            texp->tev.c_range = 256 < upper;
            break;
        case 1:
            upper += 128;
            switch (texp->tev.c_scale) {
            case 1:
                upper = upper << 1;
                break;
            case 2:
                upper = upper << 2;
                break;
            case 3:
                upper = upper >> 1;
                break;
            }
            texp->tev.c_range = 256 < upper;
            break;
        case 2:
            texp->tev.c_range = 1;
            break;
        }
    } else {
        if (HSD_TExpGetType(texp->tev.c_in[3].exp) != HSD_TE_TEV) {
            upper = 256;
            goto COLOR_LOOP;
        }
        if (texp->tev.c_in[3].sel == HSD_TE_RGB) {
            if (texp->tev.c_in[3].exp->tev.c_clamp == 1) {
                if (texp->tev.c_in[3].exp->tev.c_range != 1) {
                    upper = 256;
                    goto COLOR_LOOP;
                }
            }
            texp->tev.c_range = 1;
        } else {
            if (texp->tev.c_in[3].exp->tev.a_clamp == 1) {
                if (texp->tev.c_in[3].exp->tev.a_range != 1) {
                    upper = 256;
                    goto COLOR_LOOP;
                }
            }
            texp->tev.c_range = 1;
        }
    }
ALPHA:
    upper = 0;
    if (texp->tev.a_in[3].sel == HSD_TE_0) {
        if (HSD_TExpGetType(texp->tev.c_in[3].exp) == HSD_TE_TEV) {
            if (texp->tev.c_in[3].exp->tev.a_clamp != 1) {
                if (texp->tev.c_in[3].exp->tev.a_range == 1) {
                    return;
                }
            }
            upper = 256;
        } else {
            upper = 256;
        }

        for (u32 i = 0; i < 3; i++) {
            if (texp->tev.a_in[i].sel != HSD_TE_0) {
                if (texp->tev.a_op == 1) {
                    texp->tev.a_range = 1;
                    return;
                }
                upper += 256;
                break;
            }
        }

        switch (texp->tev.a_bias) {
        case 0:
            switch (texp->tev.a_scale) {
            case 1:
                upper = upper << 1;
                break;
            case 2:
                upper = upper << 2;
                break;
            case 3:
                upper = upper >> 1;
                break;
            }
            texp->tev.a_range = 256 < upper;
            break;
        case 1:
            upper += 128;
            switch (texp->tev.a_scale) {
            case 1:
                upper = upper << 1;
                break;
            case 2:
                upper = upper << 2;
                break;
            case 3:
                upper = upper >> 1;
                break;
            }
            texp->tev.a_range = 256 < upper;
            break;
        case 2:
            texp->tev.a_range = 1;
            break;
        }
    }
}
#endif

//80387B1C
u32 HSD_TExpSimplify(HSD_TExp* texp)
{
    u32 res = 0;
    if (HSD_TExpGetType(texp) == HSD_TE_TEV) {
        res = SimplifySrc(texp) != 0 ? 1 : 0;
        res = SimplifyThis(texp) != 0 ? 1 : res;
        res = SimplifyByMerge(texp) != 0 ? 1 : res;
#ifdef NEW_LIB
        CalcTevRange(texp);
#endif
    }
    return res;
}

//80387BA4
u32 HSD_TExpSimplify2(HSD_TExp* texp)
{
    HSD_TExp* t;
    u8 sel;

    for (u32 i = 0; i < 4; i++) {
        t = texp->tev.c_in[i].exp;
        sel = texp->tev.c_in[i].sel;
        if (texp->tev.c_in[i].type == HSD_TE_TEV && texp->tev.c_in[i].sel == 1 && IsThroughColor(t)) {
            if (t->tev.c_in[3].type == HSD_TE_KONST) {
                if (texp->tev.kcsel == 0xFF) {
                    texp->tev.kcsel = t->tev.kcsel;
                } else if (texp->tev.kcsel != t->tev.kcsel) {
                    continue;
                }
            } else if (t->tev.c_in[3].type != HSD_TE_IMM) {
                continue;
            }
            texp->tev.c_in[i].type = t->tev.c_in[3].type;
            texp->tev.c_in[i].sel = t->tev.c_in[3].sel;
            texp->tev.c_in[i].arg = t->tev.c_in[3].arg;
            texp->tev.c_in[i].exp = t->tev.c_in[3].exp;
            HSD_TExpRef(texp->tev.c_in[i].exp, texp->tev.c_in[i].sel);
            HSD_TExpUnref(t, sel);
        }
    }

    for (u32 i = 0; i < 4; i++) {
        t = texp->tev.a_in[i].exp;
        sel = texp->tev.a_in[i].sel;
        if (texp->tev.a_in[i].type == HSD_TE_TEV && IsThroughAlpha(t)) {
            if (t->tev.a_in[3].type == HSD_TE_KONST) {
                if (texp->tev.kasel == 0xFF) {
                    texp->tev.kasel = t->tev.kasel;
                } else if (texp->tev.kasel != t->tev.kasel) {
                    continue;
                }
            } else if (t->tev.a_in[3].type != HSD_TE_IMM) {
                continue;
            }
            texp->tev.a_in[i].type = t->tev.a_in[3].type;
            texp->tev.a_in[i].sel = t->tev.a_in[3].sel;
            texp->tev.a_in[i].arg = t->tev.a_in[3].arg;
            texp->tev.a_in[i].exp = t->tev.a_in[3].exp;
            HSD_TExpRef(texp->tev.a_in[i].exp, texp->tev.a_in[i].sel);
            HSD_TExpUnref(t, sel);
        }
    }

    return 0;
}