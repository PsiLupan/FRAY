#include "hsd_texp.h"

#include "hsd_state.h"
#include "hsd_tobj.h"

static u32 num_texgens = 0; //r13_40A4

u8 TevReg[48]; //Technically a struct

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

static u32 HSD_Index2TevRegID(u32 idx)
{
    switch (idx) {
    case 0:
        return GX_TEVREG0;
    case 1:
        return GX_TEVREG1;
    case 2:
        return GX_TEVREG2;
    case 3:
        return 0;
    default:
        return GX_TEVREG0;
    }
}

//803629D8
void HSD_SetTevRegAll(void)
{
    for (u32 i = 0; i < 4; i++) {
        if (*(s32*)(TevReg + i * 0xc + 8) != 0) {
            u32 offset = i * 0xc;
            GXColorS10 color;
            color.r = *(u16*)(TevReg + offset);
            color.g = *(u16*)(TevReg + offset + 2);
            color.b = *(u16*)(TevReg + offset + 4);
            color.a = *(u16*)(TevReg + offset + 6);
            u32 reg = HSD_Index2TevRegID(i);
            GX_SetTevKColorS10(reg, color);
            *(u32*)(TevReg + offset + 8) = 0;
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
        assert(0);
    }
    return 0;
}

//80382C00
u32 HSD_TExpGetType(HSD_TExp* texp)
{
    if (texp == NULL || texp->type == 0) {
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

//80362D04
void _HSD_StateInvalidateTevRegister(void)
{
    TevReg[8] = 0;
    TevReg[9] = 0;
    TevReg[0xA] = 0;
    TevReg[0xB] = 0;

    TevReg[0x14] = 0;
    TevReg[0x15] = 0;
    TevReg[0x16] = 0;
    TevReg[0x17] = 0;

    TevReg[0x20] = 0;
    TevReg[0x21] = 0;
    TevReg[0x22] = 0;
    TevReg[0x23] = 0;

    TevReg[0x2C] = 0;
    TevReg[0x2D] = 0;
    TevReg[0x2E] = 0;
    TevReg[0x2F] = 0;
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

    if (all = 0) {
        if (type == HSD_TE_ALL || type == HSD_TE_TEV) {
            for (HSD_TExp* next = texp_list->tev.next; next != NULL; next = next->tev.next) {
                if (next->type == HSD_TE_TEV && next->tev.c_ref == 0) {
                    HSD_TExpUnref(next, 1);
                    HSD_TExpUnref(next, 5);
                }
            }
        }

        HSD_TExp* ptr = handle[0];
        for (u32 i = 0; ptr != NULL; ++i, ptr = handle[i]) {
            if (type == HSD_TE_ALL || type == ptr->type) {
            }
        }
    } else {
        HSD_TExp* ptr = handle[0];
        for (u32 i = 0; ptr != NULL; ++i, ptr = handle[i]) {
        }
    }
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
    HSD_TExp* result = NULL;

    while (true) {
        if (texp == NULL) {
            if (comp == HSD_TE_0) {
                result = NULL;
            } else {
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
        //BUG: Due to the resulting code, texp could be null potentially and lead to a bad deref
        if (texp->type == 4 && texp->cnst.val == val && texp->cnst.comp == comp) {
            break;
        }
        texp = texp->cnst.next;
    }
    HSD_CheckAssert("HSD_TExpCnst: ctype != type", texp->cnst.ctype == type);
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
    }
    texp->tev.c_bias = 0;
    texp->tev.c_scale = 0;
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
    }
    texp->tev.a_bias = 0;
    texp->tev.a_scale = 0;
}

//80383488
static void HSD_TExpColorInSub(HSD_TETev* tev, HSD_TEInput sel, HSD_TExp* exp, u32 idx)
{
    u32 swap;
    HSD_TExpType o_type = tev->c_in[idx].type;
    HSD_TExp* texp = tev->c_in[idx].exp;

    u32 type = HSD_TExpGetType(exp);
    tev->c_in[idx].type = type;
    tev->c_in[idx].sel = sel;
    tev->c_in[idx].exp = exp;
    tev->c_in[idx].arg = 0xFF;
    switch (sel) {
    case HSD_TE_0:
        tev->c_in[idx].arg = GX_CC_ZERO;
        tev->c_in[idx].exp = NULL;
        HSD_TExpUnref(texp, HSD_TE_ZERO);
        return;

    case HSD_TE_1:
        tev->c_in[idx].arg = GX_CC_ONE;
        tev->c_in[idx].exp = NULL;
        HSD_TExpUnref(texp, HSD_TE_IMM);
        return;

    case HSD_TE_4_8:
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
            HSD_Halt("Unexpected kcsel");
        }

        if (tev->kcsel == HSD_TE_UNDEF) {
            tev->kcsel = swap;
        } else {
            HSD_CheckAssert("tev cannot select multiple konst", tev->kcsel != swap);
        }

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
            HSD_CheckAssert("HSD_TExpColorInSub: ", idx == 3 || sel != HSD_TE_RGB || texp->tev.c_clamp != 0);
            HSD_CheckAssert("HSD_TExpColorInSub: ", idx == 3 || sel != HSD_TE_A || texp->tev.a_clamp != 0);
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
        HSD_CheckAssert("swap == HSD_TE_UNDEF || tev->tex_swap == swap", swap == HSD_TE_UNDEF || tev->tex_swap == swap);
    }
END:
    HSD_TExpUnref(texp, tev->c_in[idx].sel);
    return;
}

//80383A64
void HSD_TExpColorIn(HSD_TExp* texp, HSD_TEInput sel_a, HSD_TExp* exp_a, HSD_TEInput sel_b, HSD_TExp* exp_b,
    HSD_TEInput sel_c, HSD_TExp* exp_c, HSD_TEInput sel_d, HSD_TExp* exp_d)
{
    assert(HSD_TExpGetType(texp) == 1);
    HSD_TExpColorInSub(&texp->tev, sel_a, exp_a, 0);
    HSD_TExpColorInSub(&texp->tev, sel_b, exp_b, 1);
    HSD_TExpColorInSub(&texp->tev, sel_c, exp_c, 2);
    HSD_TExpColorInSub(&texp->tev, sel_d, exp_d, 3);
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
    HSD_TECnst* cnst = &tev->c_in[idx].exp->cnst;
    if (cnst->reg == HSD_TE_UNDEF) {
        if (cnst->comp == HSD_TE_X) {
            u32 i, j;
            for (i = 4, j = 4; i > 0; --i, ++j) {
                if (res->reg[j].alpha == 0) {
                    res->reg[j].alpha = 1;
                    cnst->reg = j;
                    cnst->idx = 3;
                    tev->c_in[idx].type = HSD_TE_IMM;
                    tev->c_in[idx].arg = 0; //WRONG
                    return 0;
                }
            }
        } else {
            u32 i, j;
            for (i = 4, j = 4; i > 0; --i, ++j) {
                if (res->reg[j].color == 0) {
                    res->reg[j].color = 3;
                    cnst->reg = j;
                    cnst->idx = 0;
                    tev->c_in[idx].type = HSD_TE_IMM;
                    tev->c_in[idx].arg = 0; //WRONG
                    return 0;
                }
            }
        }
        return -1;
    }
    if (cnst->reg > 3) {
        tev->c_in[idx].type = HSD_TE_IMM;
        if (cnst->comp == HSD_TE_X) {
            tev->c_in[idx].arg = 0; //WRONG
        } else {
            tev->c_in[idx].arg = 0; //WRONG
        }
        return 0;
    }
    return -1;
}

//80384274
static s32 AssignAlphaReg(HSD_TETev* tev, u32 idx, HSD_TExpRes* res)
{
    HSD_TECnst* cnst = &tev->a_in[idx].exp->cnst;
    if (cnst->reg == HSD_TE_UNDEF) {
        u32 i, j;
        for (i = 4, j = 4; i > 0; --i, ++j) {
            if (res->reg[j].alpha == 0) {
                res->reg[j].alpha = 1;
                cnst->reg = j;
                cnst->idx = 3;
                tev->a_in[idx].type = HSD_TE_IMM;
                tev->a_in[idx].arg = 0; //WRONG
                return 0;
            }
        }
    }
    if (cnst->reg < 4) {
        return -1;
    }
    tev->a_in[idx].type = HSD_TE_IMM;
    tev->a_in[idx].arg = 0; //WRONG
    return 0;
}

//80384340
static s32 AssignColorKonst(HSD_TETev* tev, u32 idx, HSD_TExpRes* res)
{
    HSD_TECnst* cnst = &tev->c_in[idx].exp->cnst;
    if (cnst->reg == HSD_TE_UNDEF) {
        if (cnst->comp == HSD_TE_X) {
            //TODO
        } else {
            for (u32 i = 0; i < 4; ++i) {
                if (res->reg[i].color == 0) {
                    res->reg[i].color = 3;
                    cnst->reg = i;
                    cnst->idx = 0;
                    tev->kcsel = cnst->reg; //WRONG
                    tev->c_in[idx].type = HSD_TE_KONST;
                    tev->c_in[idx].arg = 0xE;
                }
            }
        }
        return -1;
    }
    if (cnst->reg < 4) {
        if (cnst->comp == HSD_TE_X) {
            tev->kcsel = cnst->idx; //WRONG
            tev->c_in[idx].type = HSD_TE_KONST;
            tev->c_in[idx].arg = 0xE;
        } else {
            tev->kcsel = cnst->reg;
            tev->c_in[idx].type = HSD_TE_KONST;
            tev->c_in[idx].arg = 0xE;
        }
        return 0;
    }
    return -1;
}

//80384560
static s32 AssignAlphaKonst(HSD_TETev* tev, u32 idx, HSD_TExpRes* res)
{
    HSD_TECnst* cnst = &tev->a_in[idx].exp->cnst;
    if (cnst->reg == HSD_TE_UNDEF) {
        //TODO
        return -1;
    }
    if (cnst->reg > 3) {
        return -1;
    }
    tev->kasel = cnst->idx; //WRONG
    tev->a_in[idx].type = HSD_TE_KONST;
    tev->a_in[idx].arg = 6;
}

//803846C0
static u32 TExpAssignReg(HSD_TExp* texp, HSD_TExpRes* res)
{
    s32 val;
    u32 i;

    if (texp->tev.c_ref > 0) {
        if (texp->tev.kcsel == HSD_TE_UNDEF) {
            if (texp->tev.c_op == 0 && texp->tev.c_in[0].sel == 7) {
                if (texp->tev.c_in[1].sel == 7) {
                    if (texp->tev.c_bias == 0) {
                        if (texp->tev.c_scale == 0) {
                            if (texp->tev.c_in[3].type == HSD_TE_CNST) {
                                if (AssignColorKonst(&texp->tev, 3, res) == -1) {
                                    val = AssignColorReg(&texp->tev, 3, res);
                                    HSD_CheckAssert("TExpAssignReg: val == -1", val >= 0);
                                    return val;
                                }
                            }
                        } else {
                            goto ELSE_COLOR;
                        }
                    } else {
                        goto ELSE_COLOR;
                    }
                } else {
                    goto ELSE_COLOR;
                }
            } else {
            ELSE_COLOR:
                for (i = 0; i < 4; ++i) {
                    if (texp->tev.c_in[i].type == HSD_TE_CNST) {
                        if (AssignColorKonst(&texp->tev, i, res) == -1) {
                            val = AssignColorReg(&texp->tev, i, res);
                            HSD_CheckAssert("TExpAssignReg: val == -1", val >= 0);
                            return val;
                        }
                    }
                }

                for (; i < 4; ++i) {
                    if (texp->tev.c_in[i].type == HSD_TE_CNST) {
                        val = AssignColorReg(&texp->tev, i, res);
                        HSD_CheckAssert("TExpAssignReg: val == -1", val >= 0);
                        return val;
                    }
                }
            }
        } else {
            for (i = 0; i < 4; ++i) {
                if (texp->tev.c_in[i].type == HSD_TE_CNST) {
                    val = AssignColorReg(&texp->tev, i, res);
                    HSD_CheckAssert("TExpAssignReg: val == -1", val >= 0);
                    return val;
                }
            }
        }
    }

    if (texp->tev.a_ref > 0) {
        if (texp->tev.kasel == HSD_TE_UNDEF) {
            if (texp->tev.a_op == 0 && texp->tev.a_in[0].sel == 7) {
                if (texp->tev.a_in[1].sel == 7) {
                    if (texp->tev.a_bias == 0) {
                        if (texp->tev.a_scale == 0) {
                            if (texp->tev.a_in[3].type == HSD_TE_CNST) {
                                if (AssignAlphaReg(&texp->tev, 3, res) == -1) {
                                    val = AssignAlphaKonst(&texp->tev, 3, res);
                                    HSD_CheckAssert("TExpAssignReg: val == -1", val >= 0);
                                    return val;
                                }
                            }
                        } else {
                            goto ELSE_ALPHA;
                        }
                    } else {
                        goto ELSE_ALPHA;
                    }
                } else {
                    goto ELSE_ALPHA;
                }
            } else {
            ELSE_ALPHA:
                for (i = 0; i < 4; ++i) {
                    if (texp->tev.a_in[i].type == HSD_TE_CNST) {
                        if (AssignAlphaKonst(&texp->tev, i, res) == -1) {
                            val = AssignAlphaReg(&texp->tev, i, res);
                            HSD_CheckAssert("TExpAssignReg: val == -1", val >= 0);
                            return val;
                        }
                    }
                }

                for (; i < 4; ++i) {
                    if (texp->tev.a_in[i].type == HSD_TE_CNST) {
                        val = AssignAlphaReg(&texp->tev, i, res);
                        HSD_CheckAssert("TExpAssignReg: val == -1", val >= 0);
                        return val;
                    }
                }
            }
        } else {
            for (i = 0; i < 4; ++i) {
                if (texp->tev.a_in[i].type == HSD_TE_CNST) {
                    val = AssignAlphaReg(&texp->tev, i, res);
                    HSD_CheckAssert("TExpAssignReg: val == -1", val >= 0);
                    return val;
                }
            }
        }
    }
}

//80384B20
static void TExp2TevDesc(HSD_TExp* texp, HSD_TExpTevDesc* desc, u32* init_cprev, u32* init_aprev)
{
    u32 swap;

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
    if (texp->tev.tex_swap == 0xff) {
        swap = 0;
    }
    desc->desc.u.tevconf.tex_swap = swap;

    swap = texp->tev.ras_swap;
    if (swap == 0xff) {
        swap = 0;
    }
    desc->desc.u.tevconf.ras_swap = swap;

    swap = texp->tev.kcsel;
    if (swap == 0xff) {
        swap = 0;
    }
    desc->desc.u.tevconf.kcsel = swap;

    swap = texp->tev.kasel;
    if (swap == 0xff) {
        swap = 0;
    }
    desc->desc.u.tevconf.kasel = swap;

    u8 val;
    if (texp->tev.c_op == GX_COLORNULL || (((((texp->tev.c_ref == 0 && (val = texp->tev.a_op, val != 8)) && (val != 9)) && ((val != 10 && (val != 11)))) && ((val != 12 && (val != 13)))))) {
        desc->desc.u.tevconf.clr_op = 0;
        desc->desc.u.tevconf.clr_a = 0xF;
        desc->desc.u.tevconf.clr_b = 0xF;
        desc->desc.u.tevconf.clr_c = 0xF;
        if (*init_cprev == 0) {
            desc->desc.u.tevconf.clr_d = 0;
        } else {
            *init_cprev = 0;
            desc->desc.u.tevconf.clr_d = 0xF;
        }
        desc->desc.u.tevconf.clr_scale = 0;
        desc->desc.u.tevconf.clr_bias = 0;
        desc->desc.u.tevconf.clr_clamp = 0;
        desc->desc.u.tevconf.clr_out_reg = 0;
    } else {
        desc->desc.u.tevconf.clr_op = texp->tev.c_op;

        u8 arg = texp->tev.c_in[0].arg;
        swap = arg;
        if (arg == 0xFF) {
            swap = 0xF;
        }
        desc->desc.u.tevconf.clr_a = swap;

        arg = texp->tev.c_in[1].arg;
        swap = arg;
        if (arg == 0xFF) {
            swap = 0xF;
        }
        desc->desc.u.tevconf.clr_b = swap;

        arg = texp->tev.c_in[2].arg;
        swap = arg;
        if (arg == 0xFF) {
            swap = 0xF;
        }
        desc->desc.u.tevconf.clr_c = swap;

        arg = texp->tev.c_in[3].arg;
        swap = arg;
        if (arg == 0xFF) {
            swap = 0xF;
        }
        desc->desc.u.tevconf.clr_d = swap;

        swap = texp->tev.c_scale;
        if (swap == 0xFF) {
            swap = 0;
        }
        desc->desc.u.tevconf.clr_scale = swap;

        swap = texp->tev.c_bias;
        if (swap == 0xFF) {
            swap = 0;
        }
        desc->desc.u.tevconf.clr_bias = swap;

        desc->desc.u.tevconf.clr_clamp = texp->tev.c_clamp != 0;

        HSD_CheckAssert("c_dst is undefined", texp->tev.c_dst != 0xFF);

        desc->desc.u.tevconf.clr_out_reg = texp->tev.c_dst;
        if (desc->desc.u.tevconf.clr_out_reg == 0) {
            *init_cprev = 0;
        }
    }

    if ((texp->tev.a_op == 0xff) || (texp->tev.a_ref == 0)) {
        desc->desc.u.tevconf.alpha_op = 0;
        desc->desc.u.tevconf.alpha_a = 7;
        desc->desc.u.tevconf.alpha_b = 7;
        desc->desc.u.tevconf.alpha_c = 7;
        if (*init_aprev == 0) {
            desc->desc.u.tevconf.alpha_d = 0;
        } else {
            *init_aprev = 0;
            desc->desc.u.tevconf.alpha_d = 7;
        }
        desc->desc.u.tevconf.alpha_scale = 0;
        desc->desc.u.tevconf.alpha_bias = 0;
        desc->desc.u.tevconf.alpha_clamp = 0;
        desc->desc.u.tevconf.alpha_out_reg = 0;
    } else {
        desc->desc.u.tevconf.alpha_op = texp->tev.a_op;

        u8 arg = texp->tev.a_in[0].arg;
        swap = arg;
        if (arg == 0xFF) {
            swap = 7;
        }
        desc->desc.u.tevconf.alpha_a = swap;

        arg = texp->tev.a_in[1].arg;
        swap = arg;
        if (arg == 0xFF) {
            swap = 7;
        }
        desc->desc.u.tevconf.alpha_b = swap;

        arg = texp->tev.a_in[2].arg;
        swap = arg;
        if (arg == 0xFF) {
            swap = 7;
        }
        desc->desc.u.tevconf.alpha_c = swap;

        arg = texp->tev.a_in[3].arg;
        swap = arg;
        if (arg == 0xFF) {
            swap = 7;
        }
        desc->desc.u.tevconf.alpha_d = swap;

        swap = texp->tev.a_scale;
        if (swap == 0xFF) {
            swap = 0;
        }
        desc->desc.u.tevconf.alpha_scale = swap;

        swap = texp->tev.a_bias;
        if (swap == 0xFF) {
            swap = 0;
        }
        desc->desc.u.tevconf.alpha_bias = swap;

        desc->desc.u.tevconf.alpha_clamp = texp->tev.a_clamp != 0;

        HSD_CheckAssert("a_dst is undefined", texp->tev.a_dst != 0xFF);

        desc->desc.u.tevconf.alpha_out_reg = texp->tev.a_dst;
        if (desc->desc.u.tevconf.alpha_out_reg == 0) {
            *init_aprev = 0;
        }
    }
    desc->desc.u.tevconf.mode = 0;
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
            tevdesc->desc.map = ((HSD_TObj*)tevdesc->tobj)->id;
            tevdesc->desc.coord = ((HSD_TObj*)tevdesc->tobj)->coord;
        }
        HSD_StateAssignTev();
        HSD_SetupTevStage(&tevdesc->desc);
        tevdesc = (HSD_TExpTevDesc*)tevdesc->desc.next;
    }
}

//803854B4
void HSD_TExpCompile(HSD_TExp* texp, HSD_TExpTevDesc** tevdesc, HSD_TExp** texp_list)
{
    HSD_TExpRes res;
    u32 num;
    u32 init_cprev = 1;
    u32 init_aprev = 1;
    HSD_TExp* order[32];
    HSD_TExpDag list[32];

    assert(tevdesc != NULL);
    assert(list != NULL);
    HSD_TExpRef(texp, 1);
    HSD_TExpRef(texp, 5);
    HSD_TExpSimplify(texp);

    memset(&res, 0, sizeof(HSD_TExpRes));
    num = HSD_TExpMakeDag(texp, list);
    HSD_TExpSchedule(num, list, order, &res);
    for (u32 i = 0; i < num; ++i) {
        u32 val = TExpAssignReg(order[i], &res);
        HSD_CheckAssert("val >= 0", val >= 0);
    }

    HSD_TExp** t = (HSD_TExp**)(order + num);
    while (num += -1, num >= 0) {
        HSD_TExpSimplify2(*t);
        t += -1;
    }

    num = HSD_TExpMakeDag(texp, list);
    HSD_TExpSchedule(num, list, order, &res);
    *tevdesc = NULL;
    for (u32 i = 0; i < num; ++i) {
        HSD_TExpTevDesc* tdesc = hsdAllocMemPiece(sizeof(HSD_TExpTevDesc));
        u32 stage = HSD_Index2TevStage(i);
        tdesc->desc.stage = stage;
        TExp2TevDesc(order[num - i], tdesc, &init_cprev, &init_aprev);
        tdesc->desc.next = (HSD_TevDesc*)(*tevdesc);
        *tevdesc = tdesc;
    }

    HSD_TExp* free = HSD_TExpFreeList(*texp_list, 1, 1);
    *texp_list = free;
    free = HSD_TExpFreeList(*texp_list, 4, 0);
    *texp_list = free;
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
}

//80385B8C
void CalcDistance(HSD_TETev** tevs, s32* dist, HSD_TETev* tev, s32 num, s32 d)
{
    HSD_TETev** list = tevs;
    u32 n = 0;

    if (num > 0) {
        for (u32 i = num; i > 0; --i) {
            if (*list == tev) {
                if (d <= dist[n]) {
                    return;
                }
                dist[n] = d;
                for (u32 j = 0; j < 4; ++j) {
                    if (tev->c_in[j].type == HSD_TE_TEV) {
                        CalcDistance(tevs, dist, &tev->c_in[j].exp->tev, num, d + 1);
                    }
                    if (tev->a_in[j].type == HSD_TE_TEV) {
                        CalcDistance(tevs, dist, &tev->c_in[j].exp->tev, num, d + 1);
                    }
                }
            }
            ++list;
            ++n;
        }
    }
}

//80385C60
u32 HSD_TExpMakeDag(HSD_TExp* root, HSD_TExpDag* list)
{
    HSD_CheckAssert("HSD_TExpMakeDag: type != 1", HSD_TExpGetType(root) == HSD_TE_TEV);

    HSD_TExp* tevs[32];
    tevs[0] = root;
    u32 i = 1;
    u32 j;

    for (j = 0; j < i; ++j) {
        HSD_CheckAssert("HSD_TExpMakeDag: j < HSD_TEXP_MAX_NUM", j < HSD_TEXP_MAX_NUM);
        HSD_TExp* c_tev = tevs[j];
        u32 l = 0;
        HSD_TETev* tev = &c_tev->tev;
        for (u32 k = 0; k < 4; ++k, i = l) {
            l = i;
            if (tev->c_in[k].type == HSD_TE_TEV) {
                u32 num = 0;
                HSD_TExp** tev_list = tevs;
                if (i > 0) {
                    for (l = i; l > 0; --l, ++num, ++tev_list) {
                        if (*tev_list == tev->c_in[k].exp) {
                            break;
                        }
                    }
                    if (i <= num) {
                        l = i + 1;
                        tevs[i] = tev->c_in[k].exp;
                    }
                }
            }
        }
        for (u32 k = 0; k < 4; ++k, i = l) {
            l = i;
            if (tev->a_in[k].type == HSD_TE_TEV) {
                u32 num = 0;
                HSD_TExp** tev_list = tevs;
                if (i > 0) {
                    for (l = i; l > 0; --l, ++num, ++tev_list) {
                        if (*tev_list == tev->a_in[k].exp) {
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
    }

    j = 0;
    s32 dist[32];
    if (i > 0) {
        if (i > 8) {
            u32 cnt = i - 1 >> 3;
            if ((i - 8) > 0) {
                do {
                    dist[0 + j] = -1;
                    dist[1 + j] = -1;
                    dist[2 + j] = -1;
                    dist[3 + j] = -1;
                    dist[4 + j] = -1;
                    dist[5 + j] = -1;
                    dist[6 + j] = -1;
                    dist[7 + j] = -1;
                    j += 8;
                    --cnt;
                } while (cnt > 0);
            }
        }

        u32 cnt = i - j;
        if (i > j) {
            for (u32 k = 0; cnt > 0; ++k, --cnt) {
                dist[k] = -1;
            }
        }
    }
    CalcDistance((HSD_TETev**)tevs, dist, &tevs[0]->tev, i, 0);

    for (j = 0; j < i; ++j) {
        u32 offset = j + 1;
        s32* v = &dist[offset];
        HSD_TExp** tev_start = &tevs[offset];
        if (i > offset) {
            for (u32 n = i - offset; n > 0; --n, ++v, ++tev_start) {
                if (*v < v[-1]) {
                    HSD_TExp* t = tev_start[-1];
                    tev_start[-1] = *tev_start;
                    *tev_start = t;
                    s32 temp = v[-1];
                    v[-1] = *v;
                    *v = temp;
                }
            }
        }
    }

    u32 idx = i - 1;
    j = idx;

    if (idx < 0) {
        return i;
    }

    HSD_TExpDag* cdag = NULL;
    for (u32 o = 0; o < 8; o++, --idx, --j) {
        HSD_TExp* te = tevs[idx];
        HSD_TExpDag* dag = &list[idx];
        HSD_TETev* tevn = &te->tev;

        dag->idx = (u8)idx;
        dag->nb_ref = 0;
        dag->nb_dep = 0;
        dag->tev = tevn;

        if (o < 4) {
            if (tevn->c_in[o].type == HSD_TE_TEV) {
                HSD_TExp** clist = tevs;
                u32 num = idx;

                if (i > idx) {
                    for (u32 cnt = i - idx; cnt > 0; --cnt, ++num, ++clist) {
                        if (tevn->c_in[o].exp == *clist) {
                            cdag = &list[j];
                            num = 0;
                            if (dag->nb_dep == 0) {
                                dag->nb_dep += 1;
                                dag->depend[dag->nb_dep] = cdag;
                                cdag->nb_ref += 1;
                                HSD_CheckAssert("HSD_TExpMakeDag: i <= 0", i > 0);
                            }
                            if (dag->depend[0] == cdag) {
                                break;
                            }
                        }
                    }
                }
            }
        } else {
            if (tevn->a_in[o].type == HSD_TE_TEV) {
                HSD_TExp** clist = tevs;
                u32 num = idx;

                if (i > idx) {
                    for (u32 cnt = i - idx; cnt > 0; --cnt, ++num, ++clist) {
                        if (tevn->a_in[o].exp == *clist) {
                            cdag = (HSD_TExpDag*)(list + j);
                            num = 0;
                            if (dag->nb_dep == 0) {
                                dag->nb_dep += 1;
                                dag->depend[dag->nb_dep] = cdag;
                                cdag->nb_ref += 1;
                                HSD_CheckAssert("HSD_TExpMakeDag: i <= 0", i > 0);
                            }
                            if (dag->depend[0] == cdag) {
                                break;
                            }
                        }
                    }
                }
            }
        }
        if (idx < 0) {
            return i;
        }
    }
}

//80385944
static void order_dag(s32 num, u32* dep_mtx, u32* full_dep_mtx, HSD_TExpDag* list, s32 depth, s32 idx, u32 done_set, u32 ready_set, s32* order, s32* min, s32* min_order)
{
    s32* piVar1;
    s32 idx_00;
    s32* piVar2;
    u32* puVar3;
    s32* piVar4;
    s32 iVar5;
    u32 uVar6;
    s32 depth_00;
    u32 ready_set_00;

    depth_00 = depth + 1;
    done_set = done_set | 1 << idx;
    order[depth] = idx & 0xff;
    if (depth_00 == num) {
        depth_00 = assign_reg(num, dep_mtx, list, order);
        if (depth_00 < *min) {
            *min = depth_00;
            depth_00 = 0;
            if (0 < num) {
                if ((8 < num) && (ready_set_00 = num - 1U >> 3, piVar2 = order, piVar4 = min_order, 0 < num + -8)) {
                    do {
                        depth_00 = depth_00 + 8;
                        *piVar4 = *piVar2;
                        piVar4[1] = piVar2[1];
                        piVar4[2] = piVar2[2];
                        piVar4[3] = piVar2[3];
                        piVar4[4] = piVar2[4];
                        piVar4[5] = piVar2[5];
                        piVar4[6] = piVar2[6];
                        piVar1 = piVar2 + 7;
                        piVar2 = piVar2 + 8;
                        piVar4[7] = *piVar1;
                        piVar4 = piVar4 + 8;
                        ready_set_00 = ready_set_00 - 1;
                    } while (ready_set_00 != 0);
                }
                order = order + depth_00;
                min_order = min_order + depth_00;
                idx_00 = num - depth_00;
                if (depth_00 < num) {
                    do {
                        depth_00 = *order;
                        order = order + 1;
                        *min_order = depth_00;
                        min_order = min_order + 1;
                        idx_00 = idx_00 + -1;
                    } while (idx_00 != 0);
                }
            }
        }
    } else {
        ready_set_00 = ready_set & ~(1 << idx) | dep_mtx[idx];
        uVar6 = 0;
        iVar5 = 0;
        puVar3 = full_dep_mtx;
        idx_00 = num;
        if (0 < num) {
            do {
                if ((ready_set_00 & 1 << iVar5) != 0) {
                    uVar6 = uVar6 | *puVar3;
                }
                puVar3 = puVar3 + 1;
                iVar5 = iVar5 + 1;
                idx_00 = idx_00 + -1;
            } while (idx_00 != 0);
        }
        ready_set_00 = ready_set_00 & ~uVar6;
        if ((list[idx].nb_dep == 1) && ((ready_set_00 & dep_mtx[idx]) != 0)) {
            order_dag(num, dep_mtx, full_dep_mtx, list, depth_00, list[idx].depend[0]->idx, done_set, ready_set_00, order, min, min_order);
        } else {
            idx_00 = 0;
            while (idx_00 < num) {
                if ((ready_set_00 & 1 << idx_00) != 0) {
                    order_dag(num, dep_mtx, full_dep_mtx, list, depth_00, idx_00, done_set, ready_set_00, order, min,
                        min_order);
                }
                idx_00 = idx_00 + 1;
            }
        }
    }
}

//80386100
static void make_full_dependency_mtx(s32 num, u32* dep, u32* full)
{
    BOOL bVar2;
    s32 iVar3;
    u32* puVar4;
    u32* puVar5;
    s32 iVar6;
    u32 uVar7;
    u32 uVar8;

    if (num > 0) {
        iVar6 = 0;
        if ((8 < num) && (puVar4 = dep, puVar5 = full, 0 < num + -8)) {
            for (uVar8 = num - 1 >> 3; uVar8 > 0; --uVar8) {
                puVar5[iVar6] = puVar4[iVar6];
                puVar5[iVar6 + 1] = puVar4[iVar6 + 1];
                puVar5[iVar6 + 2] = puVar4[iVar6 + 2];
                puVar5[iVar6 + 3] = puVar4[iVar6 + 3];
                puVar5[iVar6 + 4] = puVar4[iVar6 + 4];
                puVar5[iVar6 + 5] = puVar4[iVar6 + 5];
                puVar5[iVar6 + 6] = puVar4[iVar6 + 6];
                puVar5[iVar6 + 7] = puVar4[iVar6 + 7];
                iVar6 = iVar6 + 8;
            }
        }
        dep = dep + iVar6;
        puVar4 = full + iVar6;
        iVar3 = num - iVar6;
        if (iVar6 < num) {
            do {
                uVar8 = *dep;
                dep = dep + 1;
                *puVar4 = uVar8;
                puVar4 = puVar4 + 1;
                iVar3 = iVar3 + -1;
            } while (iVar3 != 0);
        }
    }
    do {
        bVar2 = FALSE;
        puVar4 = full;
        for (iVar6 = 0; iVar6 < num; ++iVar6) {
            uVar8 = *puVar4;
            puVar5 = full;
            if (num > 0) {
                for (iVar3 = num; iVar3 > 0; --iVar3) {
                    uVar7 = *puVar5;
                    if (((1 << iVar6 & uVar7) != 0) && (*puVar5 = uVar7 | uVar8, uVar7 != *puVar5)) {
                        bVar2 = TRUE;
                    }
                    puVar5 = puVar5 + 1;
                    iVar3 = iVar3 + -1;
                }
            }
            puVar4 = puVar4 + 1;
        }
    } while (bVar2);
}

//80386234
void HSD_TExpSchedule(u32 num, HSD_TExpDag* list, HSD_TExp** result, HSD_TExpRes* resource)
{
    u32 dep_mtx[32];
    u32 full_dep_mtx[32];
    s32 order[32];
    s32 min_order[32];
    s32 min = 5;
    u32 i, j, k;

    memset(min_order, 0, 32 * 4);
    if (num > 0) {
        for (i = num, j = 0; i > 0; --i, ++j) {
            HSD_TExpDag* list_cur = &list[j];
            dep_mtx[j] = 0;
            for (k = 0; k < list->nb_dep; k++) {
                HSD_TExpDag* dag = list_cur->depend[k];
                full_dep_mtx[j] |= 1 << dag->idx;
            }
        }
    }
    make_full_dependency_mtx(num, dep_mtx, full_dep_mtx);
    order_dag(num, dep_mtx, full_dep_mtx, list, 0, 0, 0, 0, order, &min, min_order);

    for (i = 0; i < num; ++i) {
        result[i]->tev = *list[min_order[i]].tev;

        if (result[i]->tev.c_dst != HSD_TE_UNDEF) {
            resource->reg[result[i]->tev.c_dst].color = 3;
            for (j = 0; j < 4; j++) {
                if (HSD_TExpGetType(result[i]->tev.c_in[j].exp) == HSD_TE_TEV) {
                    if (result[i]->tev.c_in[j].sel == 1) {
                        result[i]->tev.c_in[j].arg = 0; //WRONG
                    } else {
                        result[i]->tev.c_in[j].arg = 0; //WRONG
                    }
                }
            }
        }

        if (result[i]->tev.a_dst != HSD_TE_UNDEF) {
            resource->reg[result[i]->tev.a_dst].alpha = 1;
            for (j = 0; j < 4; j++) {
                if (HSD_TExpGetType(result[i]->tev.a_in[j].exp) == HSD_TE_TEV) {
                    if (result[i]->tev.a_in[j].sel == 1) {
                        result[i]->tev.a_in[j].arg = 0; //WRONG
                    }
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
                sel = t->tev.c_op;
                if (sel == GX_COLORNULL) {
                    HSD_TExpUnref(t, 1);
                    res = TRUE;
                    tev->c_in[i].type = 0;
                    tev->c_in[i].sel = 7;
                    tev->c_in[i].arg = 0xFF;
                    tev->c_in[i].exp = NULL;
                } else {
                    if (sel == 0 && t->tev.c_in[0].sel == 7 && t->tev.c_in[1].sel == 7 && t->tev.c_bias == 0 && t->tev.c_scale == 0) {
                        sel = t->tev.c_in[3].type;
                        if (sel == HSD_TE_TEX) {
                            if ((texp->tev.tex == NULL || texp->tev.tex == t->tev.tex) && (texp->tev.tex_swap == HSD_TE_UNDEF || (t->tev.tex_swap == HSD_TE_UNDEF || texp->tev.tex_swap == t->tev.tex_swap))) {
                                tev->c_in[i].type = t->tev.c_in[3].type;
                                tev->c_in[i].sel = t->tev.c_in[3].sel;
                                tev->c_in[i].arg = t->tev.c_in[3].arg;
                                tev->c_in[i].exp = t->tev.c_in[3].exp;
                                tev->tex = t->tev.tex;
                                if (tev->tex_swap == HSD_TE_UNDEF) {
                                    tev->tex_swap = t->tev.tex_swap;
                                }
                                HSD_TExpUnref(t, 1);
                                res = TRUE;
                            }
                        } else if (sel == HSD_TE_TEV && (t->tev.c_in[3].exp->tev.c_range != 0 || t->tev.c_clamp == 0)) {
                            tev->c_in[i].type = t->tev.c_in[3].type;
                            tev->c_in[i].sel = t->tev.c_in[3].sel;
                            tev->c_in[i].arg = t->tev.c_in[3].arg;
                            tev->c_in[i].exp = t->tev.c_in[3].exp;
                            HSD_TExpRef(tev->c_in[i].exp, tev->c_in[i].sel);
                            HSD_TExpUnref(t, 1);
                            res = TRUE;
                        } else if (sel == HSD_TE_RAS && (texp->tev.chan == GX_COLORNULL || texp->tev.chan == t->tev.chan) && (texp->tev.ras_swap == HSD_TE_UNDEF || (t->tev.ras_swap == HSD_TE_UNDEF || texp->tev.ras_swap == t->tev.tex_swap))) {
                            tev->c_in[i].type = t->tev.c_in[3].type;
                            tev->c_in[i].sel = t->tev.c_in[3].sel;
                            tev->c_in[i].arg = t->tev.c_in[3].arg;
                            tev->c_in[i].exp = t->tev.c_in[3].exp;
                            tev->chan = t->tev.chan;
                            if (tev->ras_swap == HSD_TE_UNDEF) {
                                tev->ras_swap = t->tev.ras_swap;
                            }
                            HSD_TExpUnref(t, 1);
                            res = TRUE;
                        }
                    }
                }
            } else if (t->tev.a_op == 0xFF) {
                HSD_TExpUnref(t, sel);
                res = TRUE;
                tev->c_in[i].type = 0;
                tev->c_in[i].sel = 7;
                tev->c_in[i].arg = 0xFF;
                tev->c_in[i].exp = NULL;
            }
        }
    }

    for (u32 i = 0; i < 4; i++) {
        if (tev->a_in[i].type == HSD_TE_TEV) {
            HSD_TExp* t = tev->a_in[i].exp;
            u8 sel = tev->a_in[i].sel;
            if (HSD_TExpSimplify(t) != 0) {
                res = TRUE;
            }
            u8 op = t->tev.a_op;
            if (op == GX_COLORNULL) {
                HSD_TExpUnref(t, 1);
                res = TRUE;
                tev->a_in[i].type = 0;
                tev->a_in[i].sel = 7;
                tev->a_in[i].arg = 0xFF;
                tev->a_in[i].exp = NULL;
            } else {
                if (op == 0 && t->tev.a_in[0].sel == 7 && t->tev.a_in[1].sel == 7 && t->tev.a_bias == 0 && t->tev.a_scale == 0) {
                    op = t->tev.a_in[3].type;
                    if (op == HSD_TE_TEX) {
                        if (texp->tev.tex == NULL || texp->tev.tex == t->tev.tex) {
                            tev->a_in[i].type = t->tev.a_in[3].type;
                            tev->a_in[i].sel = t->tev.a_in[3].sel;
                            tev->a_in[i].arg = t->tev.a_in[3].arg;
                            tev->a_in[i].exp = t->tev.a_in[3].exp;
                            tev->tex = t->tev.tex;
                            HSD_TExpUnref(t, sel);
                            res = TRUE;
                        }
                    } else if (op == HSD_TE_TEV) {
                        tev->a_in[i].type = t->tev.a_in[3].type;
                        tev->a_in[i].sel = t->tev.a_in[3].sel;
                        tev->a_in[i].arg = t->tev.a_in[3].arg;
                        tev->a_in[i].exp = t->tev.a_in[3].exp;
                        HSD_TExpRef(tev->a_in[i].exp, tev->a_in[i].sel);
                        HSD_TExpUnref(t, sel);
                        res = TRUE;
                    } else if (op == HSD_TE_RAS && (texp->tev.chan == GX_COLORNULL || texp->tev.chan == t->tev.chan)) {
                        tev->a_in[i].type = t->tev.a_in[3].type;
                        tev->a_in[i].sel = t->tev.a_in[3].sel;
                        tev->a_in[i].arg = t->tev.a_in[3].arg;
                        tev->a_in[i].exp = t->tev.a_in[3].exp;
                        tev->chan = t->tev.chan;
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
    u8 sel;
    BOOL bVar1;
    s32 iVar3;
    s32 iVar4;
    s32 iVar5;
    s32 iVar6;
    u32 res;

    res = 0;
    while (true) {
        sel = texp->tev.c_in[0].type;
        iVar3 = -1;
        iVar4 = -1;
        iVar5 = -1;
        iVar6 = -1;
        if (sel == 3) {
            iVar5 = 0;
        } else {
            if ((sel < 3) && (1 < sel)) {
                iVar3 = 0;
            }
        }
        sel = texp->tev.a_in[0].type;
        if (sel == 3) {
            iVar6 = 0;
        } else {
            if ((sel < 3) && (1 < sel)) {
                iVar4 = 0;
            }
        }
        sel = texp->tev.c_in[1].type;
        if (sel == 3) {
            iVar5 = 1;
        } else {
            if ((sel < 3) && (1 < sel)) {
                iVar3 = 1;
            }
        }
        sel = texp->tev.a_in[1].type;
        if (sel == 3) {
            iVar6 = 1;
        } else {
            if ((sel < 3) && (1 < sel)) {
                iVar4 = 1;
            }
        }
        sel = texp->tev.c_in[2].type;
        if (sel == 3) {
            iVar5 = 2;
        } else {
            if ((sel < 3) && (1 < sel)) {
                iVar3 = 2;
            }
        }
        sel = texp->tev.a_in[2].type;
        if (sel == 3) {
            iVar6 = 2;
        } else {
            if ((sel < 3) && (1 < sel)) {
                iVar4 = 2;
            }
        }
        sel = texp->tev.c_in[3].type;
        if (sel == 3) {
            iVar5 = 3;
        } else {
            if ((sel < 3) && (1 < sel)) {
                iVar3 = 3;
            }
        }
        sel = texp->tev.a_in[3].type;
        if (sel == 3) {
            iVar6 = 3;
        } else {
            if ((sel < 3) && (1 < sel)) {
                iVar4 = 3;
            }
        }
        if ((iVar3 == -1) && (iVar4 == -1)) {
            texp->tev.tex = NULL;
            texp->tev.tex_swap = HSD_TE_UNDEF;
        }
        if ((iVar5 == -1) && (iVar6 == -1)) {
            texp->tev.chan = 0xFF;
            texp->tev.ras_swap = HSD_TE_UNDEF;
        }

        sel = texp->tev.a_op;
        bVar1 = FALSE;
        if (sel == 0xff || ((u8)(sel - 0xe) < 2) || sel < 2) {
            if ((texp->tev.c_op != 0xFF) && (texp->tev.c_ref == 0)) {
                texp->tev.c_op = 0xFF;
                for (u32 i = 0; i < 4; i++) {
                    HSD_TExpUnref(texp->tev.c_in[i].exp, texp->tev.c_in[i].sel);
                    texp->tev.c_in[i].type = 0;
                    texp->tev.c_in[i].sel = 7;
                    texp->tev.c_in[i].arg = 0xFF;
                    texp->tev.c_in[i].exp = NULL;
                }
                bVar1 = TRUE;
            }
            switch (texp->tev.c_op) {
            case 0:
            case 1:
                if (texp->tev.c_in[2].sel == 7) {
                    if (texp->tev.c_in[1].sel != 7) {
                        HSD_TExpUnref(texp->tev.c_in[1].exp, sel);
                        texp->tev.c_in[1].type = 0;
                        texp->tev.c_in[1].sel = 7;
                        texp->tev.c_in[1].arg = 0xFF;
                        texp->tev.c_in[1].exp = NULL;

                        bVar1 = TRUE;
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

                        bVar1 = TRUE;
                    }
                }
                if (texp->tev.c_in[2].sel == 8) {
                    if (texp->tev.c_in[0].sel != 7) {
                        HSD_TExpUnref(texp->tev.c_in[0].exp, sel);
                        texp->tev.c_in[0].type = 0;
                        texp->tev.c_in[0].sel = 7;
                        texp->tev.c_in[0].arg = 0xFF;
                        texp->tev.c_in[0].exp = NULL;

                        bVar1 = TRUE;
                    }
                    if ((texp->tev.c_op == 0) && (texp->tev.c_in[3].sel == 7)) {
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

                        bVar1 = TRUE;
                    }
                }
                if (texp->tev.c_in[0].sel == 7 && texp->tev.c_in[1].sel == 7 && texp->tev.c_in[3].sel == 7 && texp->tev.c_bias == 0) {
                    texp->tev.c_op = 0xff;

                    HSD_TExpUnref(texp->tev.c_in[2].exp, texp->tev.c_in[2].sel);
                    texp->tev.c_in[2].type = 0;
                    texp->tev.c_in[2].sel = 7;
                    texp->tev.c_in[2].arg = 0xFF;
                    texp->tev.c_in[2].exp = NULL;

                    bVar1 = TRUE;
                }
                break;
            case 8:
            case 10:
            case 0xc:
            case 0xe:
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

                    bVar1 = TRUE;
                } else {
                    if (texp->tev.c_in[0].sel == 7) {
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

                        bVar1 = TRUE;
                    }
                }
                break;
            case 9:
            case 0xb:
            case 0xd:
            case 0xf:
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

                    bVar1 = TRUE;
                } else {
                    if ((texp->tev.c_in[0].sel == 7) && (texp->tev.c_in[1].sel == 7)) {
                        texp->tev.c_op = 0;
                        texp->tev.c_in[0].type = texp->tev.c_in[2].type;
                        texp->tev.c_in[0].sel = texp->tev.c_in[2].sel;
                        texp->tev.c_in[0].arg = texp->tev.c_in[2].arg;
                        texp->tev.c_in[0].exp = texp->tev.c_in[2].exp;

                        texp->tev.c_in[2].type = 0;
                        texp->tev.c_in[2].sel = 7;
                        texp->tev.c_in[2].arg = 0xFF;
                        texp->tev.c_in[2].exp = NULL;

                        bVar1 = TRUE;
                    }
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
            bVar1 = TRUE;
        }
        sel = texp->tev.a_op;
        if (sel == 0xE) {
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

                bVar1 = TRUE;
            } else {
                if (texp->tev.a_in[0].sel == 7) {
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

                    bVar1 = TRUE;
                }
            }
        } else {
            if (sel < 0xe) {
                if (sel < 2) {
                    if (true) {
                        if (texp->tev.a_in[2].sel == 7) {
                            sel = texp->tev.a_in[1].sel;
                            if (sel != 7) {
                                HSD_TExpUnref(texp->tev.a_in[1].exp, sel);
                                texp->tev.a_in[1].type = 0;
                                texp->tev.a_in[1].sel = 7;
                                texp->tev.a_in[1].arg = 0xFF;
                                texp->tev.a_in[1].exp = NULL;

                                bVar1 = TRUE;
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

                                bVar1 = TRUE;
                            }
                        }
                        if (texp->tev.a_in[2].sel == 8) {
                            sel = texp->tev.a_in[0].sel;
                            if (sel != 7) {
                                HSD_TExpUnref(texp->tev.a_in[0].exp, sel);
                                texp->tev.a_in[0].type = 0;
                                texp->tev.a_in[0].sel = 7;
                                texp->tev.a_in[0].arg = 0xFF;
                                texp->tev.a_in[0].exp = NULL;

                                bVar1 = TRUE;
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

                                bVar1 = TRUE;
                            }
                        }
                        if (((texp->tev.a_in[0].sel == 7) && (texp->tev.a_in[1].sel == 7)) && texp->tev.a_in[3].sel == 7) {
                            texp->tev.a_op = 0xFF;
                            bVar1 = TRUE;
                        }
                    }
                } else {
                    if ((7 < sel) && (texp->tev.a_in[2].sel == 7)) {
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

                        bVar1 = TRUE;
                    }
                }
            } else {
                if (sel < 0x10) {
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

                        bVar1 = TRUE;
                    } else {
                        if ((texp->tev.a_in[0].sel == 7) && (texp->tev.a_in[1].sel == 7)) {
                            texp->tev.a_op = 0;

                            texp->tev.a_in[0].type = texp->tev.a_in[2].type;
                            texp->tev.a_in[0].sel = texp->tev.a_in[2].sel;
                            texp->tev.a_in[0].arg = texp->tev.a_in[2].arg;
                            texp->tev.a_in[0].exp = texp->tev.a_in[2].exp;

                            texp->tev.a_in[2].type = 0;
                            texp->tev.a_in[2].sel = 7;
                            texp->tev.a_in[2].arg = 0xFF;
                            texp->tev.a_in[2].exp = NULL;

                            bVar1 = TRUE;
                        }
                    }
                }
            }
        }
        if (!bVar1)
            break;
        res = 1;
    }
    return res;
}

//803870E4
static u32 SimplifyByMerge(HSD_TExp* texp)
{
    u8 sel;
    u8 bVar1;
    BOOL bVar2;
    BOOL bVar3;
    u32 type;
    HSD_TExp* uVar7;
    s32 iVar8;
    u32 uVar11;

    HSD_TExp* curr;
    uVar11 = 0;
    do {
        sel = texp->tev.a_op;
        bVar3 = false;
        if (sel == 0xFF || ((u8)(sel - 0xe) < 2) || sel < 2) {
            if ((texp->tev.c_op == 0 || texp->tev.c_op == 1)
                && texp->tev.c_in[1].sel == 7 && texp->tev.c_in[2].sel == 7
                && (type = HSD_TExpGetType(texp->tev.c_in[0].exp), type != HSD_TE_CNST)
                && (type = HSD_TExpGetType(texp->tev.c_in[3].exp), type != HSD_TE_CNST)) {

                if (texp->tev.c_op == 0 && texp->tev.c_in[3].type == HSD_TE_TEV
                    && (((sel = texp->tev.c_in[3].sel, sel == 1) && texp->tev.c_in[3].exp->tev.c_clamp != 0) || (sel == 5 && texp->tev.c_in[3].exp->tev.a_clamp != 0))
                    && (sel = texp->tev.c_in[0].type, sel < 4 && sel > 1)) {
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

                            if (texp->tev.tex == NULL || curr->tev.tex == NULL || texp->tev.tex == curr->tev.tex) {
                                if (texp->tev.chan == 0xFF || curr->tev.chan == 0xFF || texp->tev.chan == curr->tev.chan) {
                                    bVar2 = false;
                                } else {
                                    bVar2 = true;
                                }
                            } else {
                                bVar2 = true;
                            }
                            if (!bVar2) {
                                sel = curr->tev.c_bias;
                                if (sel == 2) {
                                    iVar8 = -1;
                                } else {
                                    if ((sel < 2) && (sel != 0)) {
                                        iVar8 = 1;
                                    } else {
                                        iVar8 = 0;
                                    }
                                }
                                if (curr->tev.c_op == 1) {
                                    iVar8 = -iVar8;
                                }
                                sel = texp->tev.c_bias;
                                if (sel == 2) {
                                    iVar8 = iVar8 + -1;
                                } else {
                                    if ((sel < 2) && (sel != 0)) {
                                        iVar8 = iVar8 + 1;
                                    }
                                }
                                if (iVar8 == 0) {
                                    texp->tev.c_bias = 0;
                                    bVar3 = true;
                                } else {
                                    if (iVar8 < 0) {
                                        if (iVar8 < -1) {
                                        LAB_8038737c:
                                            bVar3 = false;
                                        } else {
                                            texp->tev.c_bias = 2;
                                            bVar3 = true;
                                        }
                                    } else {
                                        if (1 < iVar8)
                                            goto LAB_8038737c;
                                        texp->tev.c_bias = 1;
                                        bVar3 = true;
                                    }
                                }
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
                                    if (texp->tev.tex == NULL) {
                                        texp->tev.tex = curr->tev.tex;
                                    }
                                    if (texp->tev.chan == 0xff) {
                                        texp->tev.chan = curr->tev.chan;
                                    }
                                    if (texp->tev.tex_swap == HSD_TE_UNDEF) {
                                        texp->tev.tex_swap = curr->tev.tex_swap;
                                    }
                                    if (texp->tev.ras_swap == HSD_TE_UNDEF) {
                                        texp->tev.ras_swap = curr->tev.ras_swap;
                                    }
                                    HSD_TExpUnref(curr, 1);
                                }
                            }
                        }
                    }
                } else {
                    if (sel == HSD_TE_ZERO && texp->tev.c_in[3].type == HSD_TE_TEV && texp->tev.c_in[3].sel == 1
                        && (curr = texp->tev.c_in[3].exp, curr->tev.c_scale == 0)
                        && (texp->tev.c_bias == 0 || texp->tev.c_bias != curr->tev.c_bias)) {

                        if (texp->tev.tex == NULL || curr->tev.tex == NULL || texp->tev.tex == curr->tev.tex) {
                            if (texp->tev.chan == 0xFF || curr->tev.chan == 0xFF || texp->tev.chan == curr->tev.chan) {
                                bVar2 = false;
                            } else {
                                bVar2 = true;
                            }
                        } else {
                            bVar2 = true;
                        }
                        if (!bVar2) {
                            bVar3 = true;
                            for (u32 i = 0; i < 4; ++i) {
                                texp->tev.c_in[i].type = curr->tev.c_in[i].type;
                                texp->tev.c_in[i].sel = curr->tev.c_in[i].sel;
                                texp->tev.c_in[i].arg = curr->tev.c_in[i].arg;
                                texp->tev.c_in[i].exp = curr->tev.c_in[i].exp;
                                HSD_TExpRef(texp->tev.c_in[i].exp, texp->tev.c_in[i].sel);
                            }
                            texp->tev.c_op = curr->tev.c_op;
                            sel = curr->tev.c_bias;
                            if (sel == 2) {
                                iVar8 = -1;
                            } else {
                                if ((sel < 2) && (sel != 0)) {
                                    iVar8 = 1;
                                } else {
                                    iVar8 = 0;
                                }
                            }
                            if (curr->tev.c_op == 1) {
                                iVar8 = -iVar8;
                            }
                            sel = texp->tev.c_bias;
                            if (sel == 2) {
                                iVar8 = iVar8 + -1;
                            } else {
                                if ((sel < 2) && (sel != 0)) {
                                    iVar8 = iVar8 + 1;
                                }
                            }
                            if (iVar8 == 0) {
                            LAB_803875d0:
                                texp->tev.c_bias = 0;
                            } else {
                                if (iVar8 < 0) {
                                    if (iVar8 < -1)
                                        goto LAB_803875d0;
                                    texp->tev.c_bias = 2;
                                } else {
                                    if (1 < iVar8)
                                        goto LAB_803875d0;
                                    texp->tev.c_bias = 1;
                                }
                            }
                            if ((texp->tev.c_clamp == 0xFF) || (texp->tev.c_clamp == 0)) {
                                texp->tev.c_clamp = curr->tev.c_clamp;
                            }
                            if (texp->tev.tex == NULL) {
                                texp->tev.tex = curr->tev.tex;
                            }
                            if (texp->tev.chan == 0xFF) {
                                texp->tev.chan = curr->tev.chan;
                            }
                            if (texp->tev.tex_swap == HSD_TE_UNDEF) {
                                texp->tev.tex_swap = curr->tev.tex_swap;
                            }
                            if (texp->tev.ras_swap == HSD_TE_UNDEF) {
                                texp->tev.ras_swap = curr->tev.ras_swap;
                            }
                            HSD_TExpUnref(curr, 1);
                        }
                    }
                }
            }
            if ((texp->tev.a_op == 0 || texp->tev.a_op == 1)
                && texp->tev.a_in[1].sel == 7 && texp->tev.a_in[2].sel == 7
                && (type = HSD_TExpGetType(texp->tev.a_in[0].exp), type != HSD_TE_CNST)
                && (type = HSD_TExpGetType(texp->tev.a_in[3].exp), type != HSD_TE_CNST)) {

                if (texp->tev.a_op == 0 && texp->tev.a_in[3].type == HSD_TE_TEV && texp->tev.a_in[3].exp->tev.a_clamp != 0
                    && (sel = texp->tev.a_in[0].type, sel < 4 && sel > 1)) {
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
                        if (texp->tev.tex == NULL || curr->tev.tex == NULL || texp->tev.tex == curr->tev.tex) {
                            if (texp->tev.chan == 0xFF || curr->tev.chan == 0xFF || texp->tev.chan == curr->tev.chan) {
                                bVar2 = false;
                            } else {
                                bVar2 = true;
                            }
                        } else {
                            bVar2 = true;
                        }
                        if (!bVar2) {
                            bVar1 = curr->tev.a_bias;
                            if (bVar1 == 2) {
                                iVar8 = -1;
                            } else {
                                if ((bVar1 < 2) && (bVar1 != 0)) {
                                    iVar8 = 1;
                                } else {
                                    iVar8 = 0;
                                }
                            }
                            if (curr->tev.a_op == 1) {
                                iVar8 = -iVar8;
                            }
                            bVar1 = texp->tev.a_bias;
                            if (bVar1 == 2) {
                                iVar8 = iVar8 + -1;
                            } else {
                                if ((bVar1 < 2) && (bVar1 != 0)) {
                                    iVar8 = iVar8 + 1;
                                }
                            }
                            if (iVar8 == 0) {
                                texp->tev.a_bias = 0;
                                bVar3 = true;
                            } else {
                                if (iVar8 < 0) {
                                    if (iVar8 < -1) {
                                    LAB_80387874:
                                        bVar3 = false;
                                    } else {
                                        texp->tev.a_bias = 2;
                                        bVar3 = true;
                                    }
                                } else {
                                    if (1 < iVar8)
                                        goto LAB_80387874;
                                    texp->tev.a_bias = 1;
                                    bVar3 = true;
                                }
                            }
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
                                if (texp->tev.chan == 0xff) {
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
                            if (texp->tev.tex == NULL || curr->tev.tex == NULL || texp->tev.tex == curr->tev.tex) {
                                if (texp->tev.chan == 0xff || curr->tev.chan == 0xff || texp->tev.chan == curr->tev.chan) {
                                    bVar2 = false;
                                } else {
                                    bVar2 = true;
                                }
                            } else {
                                bVar2 = true;
                            }
                            if (!bVar2) {
                                bVar3 = true;
                                for (u32 i = 0; i < 4; ++i) {
                                    texp->tev.a_in[i].type = curr->tev.a_in[i].type;
                                    texp->tev.a_in[i].sel = curr->tev.a_in[i].sel;
                                    texp->tev.a_in[i].arg = curr->tev.a_in[i].arg;
                                    texp->tev.a_in[i].exp = curr->tev.a_in[i].exp;
                                    HSD_TExpRef(texp->tev.a_in[i].exp, texp->tev.a_in[i].sel);
                                }
                                texp->tev.a_op = curr->tev.a_op;
                                bVar1 = curr->tev.a_bias;
                                if (bVar1 == 2) {
                                    iVar8 = -1;
                                } else {
                                    if ((bVar1 < 2) && (bVar1 != 0)) {
                                        iVar8 = 1;
                                    } else {
                                        iVar8 = 0;
                                    }
                                }
                                if (curr->tev.a_op == 1) {
                                    iVar8 = -iVar8;
                                }
                                bVar1 = texp->tev.a_bias;
                                if (bVar1 == 2) {
                                    iVar8 = iVar8 + -1;
                                } else {
                                    if ((bVar1 < 2) && (bVar1 != 0)) {
                                        iVar8 = iVar8 + 1;
                                    }
                                }
                                if (iVar8 == 0) {
                                LAB_80387a98:
                                    texp->tev.a_bias = 0;
                                } else {
                                    if (iVar8 < 0) {
                                        if (iVar8 < -1)
                                            goto LAB_80387a98;
                                        texp->tev.a_bias = 2;
                                    } else {
                                        if (1 < iVar8)
                                            goto LAB_80387a98;
                                        texp->tev.a_bias = 1;
                                    }
                                }
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
            return uVar11;
        }
        uVar11 = 1;
    } while (true);
}

//80387B1C
u32 HSD_TExpSimplify(HSD_TExp* texp)
{
    u32 res = 0;
    if (HSD_TExpGetType(texp) == HSD_TE_TEV) {
        res = SimplifySrc(texp) != 0 ? 1 : 0;
        res = SimplifyThis(texp) != 0 ? 1 : 0;
        res = SimplifyByMerge(texp) != 0 ? 1 : 0;
    }
    return res;
}

//80387BA4
u32 HSD_TExpSimplify2(HSD_TExp* texp)
{
    for (u32 i = 0; i < 4; i++) {
        HSD_TExp* t = texp->tev.c_in[i].exp;
        if (texp->tev.c_in[i].type == HSD_TE_TEV && texp->tev.c_in[i].sel == 1) {
            if (t->tev.c_op == 0 && t->tev.c_in[0].sel == 7 && t->tev.c_in[1].sel == 7 && t->tev.c_bias == 0 && t->tev.c_scale == 0) {
                if (t->tev.c_in[3].type == HSD_TE_KONST) {
                    if (texp->tev.tex_swap == 0xFF) {
                        t->tev.tex_swap = t->tev.tex_swap;
                    } else {
                        if (texp->tev.tex_swap != t->tev.tex_swap) {
                            continue;
                        }
                    }
                } else {
                    continue;
                }
                texp->tev.c_in[i].type = t->tev.c_in[3].type;
                texp->tev.c_in[i].exp = t->tev.c_in[3].exp;
                HSD_TExpRef(texp->tev.c_in[i].exp, texp->tev.c_in[i].sel);
                HSD_TExpUnref(t, 1);
            }
        }
    }

    for (u32 i = 0; i < 4; i++) {
        HSD_TExp* t = texp->tev.a_in[i].exp;
        if (texp->tev.a_in[i].type == HSD_TE_TEV) {
            if (t->tev.a_op == 0 && t->tev.a_in[0].sel == 7 && t->tev.a_in[1].sel == 7 && t->tev.a_bias == 0 && t->tev.a_scale == 0) {
                if (t->tev.a_in[3].type == HSD_TE_KONST) {
                    if (texp->tev.kcsel == 0xFF) {
                        t->tev.kcsel = t->tev.kcsel;
                    } else {
                        if (texp->tev.kcsel != t->tev.kcsel) {
                            continue;
                        }
                    }
                } else {
                    continue;
                }
                texp->tev.a_in[i].type = t->tev.a_in[3].type;
                texp->tev.a_in[i].exp = t->tev.a_in[3].exp;
                HSD_TExpRef(texp->tev.a_in[i].exp, texp->tev.a_in[i].sel);
                HSD_TExpUnref(t, texp->tev.a_in[i].sel);
            }
        }
    }

    return 0;
}
