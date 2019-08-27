#include "hsd_texp.h"

#include "hsd_state.h"
#include "hsd_tobj.h"

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
{
    if (texp->type == 0) {
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
    if (type == 4) {
        texp->cnst.ref += 1;
        return;
    } else if (type != 1) {
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

//80382DDC
HSD_TExp* HSD_TExpFreeList(HSD_TExp* texp, u32 flags, u8 method)
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
    for (u32 i = 0; i < 4; i++) {
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
static void HSD_TExpColorInSub(HSD_TETev* tev, HSD_TEInput sel, HSD_TExp* exp, u32 idx)
{
    u32 ras;
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
        break;

    case HSD_TE_1:
        tev->c_in[idx].arg = GX_CC_ONE;
        tev->c_in[idx].exp = NULL;
        HSD_TExpUnref(texp, HSD_TE_IMM);
        break;

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
            ras = 7;
            break;
        case HSD_TE_2_8:
            ras = 6;
            break;
        case HSD_TE_3_8:
            ras = 5;
            break;
        case HSD_TE_5_8:
            ras = 3;
            break;
        case HSD_TE_6_8:
            ras = 2;
            break;
        case HSD_TE_7_8:
            ras = 1;
            break;
        default:
            HSD_Halt("Unexpected ras sel");
        }
        if (tev->ras_swap = 0xFF) {
            tev->ras_swap = ras;
        } else {
            HSD_CheckAssert("tev cannot select multiple konst", tev->ras_swap != ras);
        }
        HSD_TExpUnref(texp, HSD_TE_KONST);
        return;
    }

    u8 c_in_type = tev->c_in[idx].type;
    if (c_in_type != 2) {
        switch (c_in_type) {
        case 0:
            break;

        case 3:
            break;
        }
    }
    //TODO

    if (tev->a_range == 0xFF) {
        tev->a_range = c_in_type;
    } else {
        HSD_CheckAssert("swap == HSD_TE_UNDEF || tev->tex_swap == swap", c_in_type == 0xff && tev->a_range == c_in_type);
    }
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
    HSD_CheckAssert("HSD_TExpOrder: texp->type != TEV", HSD_TExpGetType(texp) == 1);
    texp->tev.tex = tex;
    if (chan == 0xff) {
        texp->tev.chan = 0xff;
    } else {
        texp->tev.chan = chan;
    }
}

//803846C0
static u32 TExpAssignReg(HSD_TExp* texp, HSD_TExpRes* res)
{
}

//80384B20
static void TExp2TevDesc(HSD_TExp* texp, HSD_TExpTevDesc* desc, u32* init_cprev, u32* init_aprev)
{
    u32 swap;

    HSD_CheckAssert("TExp2TevDesc: texp cannot be null", texp != NULL);
    HSD_CheckAssert("TExp2TevDesc: desc cannot be null", desc != NULL);
    u32 type = HSD_TExpGetType(texp);
    HSD_CheckAssert("TExp2TevDesc: type != 1", type == 1);
    desc->desc.next = NULL;
    desc->desc.flags = 1;
    desc->tobj = texp->tev.tex;
    if (texp->tev.tex == NULL) {
        desc->desc.coord = GX_TEXCOORDNULL;
        desc->desc.map = GX_TEXMAP_NULL;
    }
    desc->desc.color = texp->tev.chan;
    swap = texp->tev.tex_swap;
    if (texp->tev.tex_swap == 0xFF) {
        swap = 0;
    }
    desc->desc.u.tevconf.ras_swap = swap;
    swap = texp->tev.a_range;
    if (texp->tev.a_range == 0xff) {
        swap = 0;
    }
    desc->desc.u.tevconf.tex_swap = swap;
    swap = texp->tev.ras_swap;
    if (swap == 0xff) {
        swap = 0;
    }
    desc->desc.u.tevconf.kcsel = swap;
    swap = texp->tev.kcsel;
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
    while (num = num + -1, -1 < num) {
        HSD_TExpSimplify2(*t);
        t = t + -1;
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
                for (u32 j = 0; i < 4; ++j) {
                    if (tev->c_in[j].type == 1) {
                        CalcDistance(tevs, dist, &tev->c_in[j].exp->tev, num, d + 1);
                    }
                    if (tev->a_in[j].type == 1) {
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
    HSD_CheckAssert("HSD_TExpMakeDag: type != 1", HSD_TExpGetType(root) == 1);

    HSD_TExp** tevs;
    HSD_TExp** tev_start;
    tevs[0] = root;
    u32 i = 1;
    u32 j;

    for (j = 0; j < i; ++j, ++tev_start) {
        HSD_CheckAssert("HSD_TExpMakeDag: j < HSD_TEXP_MAX_NUM", j < HSD_TEXP_MAX_NUM);
        HSD_TExp* c_tev = *tev_start;
        u32 l = 0;
        HSD_TETev* tev = &c_tev->tev;
        for (u32 k = 0; k < 4; ++k, i = l) {
            l = i;
            if (tev->c_in[k].type == HSD_TE_TEV) {
                u32 num = 0;
                HSD_TExp** tev_list;
                if (i > 0) {
                    for (l = i; l > 0; --l) {
                        if (*tevs == tev->c_in[k].exp) {
                            break;
                        }
                        ++num;
                        ++tev_list;
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
                HSD_TExp** tev_list;
                if (i > 0) {
                    for (l = i; l > 0; --l) {
                        if (*tevs == tev->a_in[k].exp) {
                            break;
                        }
                        ++num;
                        ++tev_list;
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
        s32* v = dist + offset;
        tev_start = tevs + offset;
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
    HSD_TExpDag* dag = &list[idx];
    tev_start = tevs + idx;

    if (idx < 0) {
        return i;
    }
    HSD_TExp* te = *tev_start;

    HSD_TExpDag* cdag = NULL;
    for (u32 o = 0; o < 8; o++) {
        dag->idx = (u8)idx;
        dag->nb_ref = 0;
        dag->nb_dep = 0;
        dag->tev = &te->tev;
        HSD_TETev* tevn = &te->tev;

        if (o < 4) {
            if (tevn->c_in[o].type == HSD_TE_TEV) {
                HSD_TExp** clist = tevs;
                u32 num = idx;

                if (i > idx) {
                    for (u32 cnt = i - idx; cnt > 0; --cnt, ++num, ++clist) {
                        if (tevn->c_in[o].exp == *clist) {
                            cdag = (HSD_TExpDag*)(list + j);
                            num = 0;
                            if (dag->nb_dep == 0) {
                                dag->nb_dep += 1;
                                dag->dag[dag->nb_dep] = cdag;
                                cdag->nb_ref += 1;
                                HSD_CheckAssert("HSD_TExpMakeDag: i <= 0", i > 0);
                            }
                            if (dag->dag[0] == cdag) {
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
                                dag->dag[dag->nb_dep] = cdag;
                                cdag->nb_ref += 1;
                                HSD_CheckAssert("HSD_TExpMakeDag: i <= 0", i > 0);
                            }
                            if (dag->dag[0] == cdag) {
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

//80386234
void HSD_TExpSchedule(u32 num, HSD_TExpDag* list, HSD_TExp** result, HSD_TExpRes* resource)
{
    u32 dep_mtx[32];
    u32 full_dep_mtx[32];

    if (num > 0) {
        for (u32 i = num; i > 0; --i) {
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
                    //TODO
                } else {
                    if (((((sel != 0xff) && (sel == 0)) && (t->tev.c_in[0].sel == 7)) && ((t->tev.c_in[1].sel == 7 && (t->tev.c_bias == 0)))) && (t->tev.c_scale == 0)) {
                        sel = t->tev.c_in[3].type;
                        if (sel == HSD_TE_TEX) {
                            if ((texp->tev.tex == NULL || texp->tev.tex == t->tev.tex) && (texp->tev.a_range == 0xFF || (t->tev.a_range == 0xFF || texp->tev.a_range == t->tev.a_range))) {
                                //TODO
                            }
                        } else if (sel == HSD_TE_TEV && (t->tev.c_in[3].exp->tev.c_range != 0 || t->tev.c_clamp == 0)) {
                            //TODO
                        } else if ((sel == HSD_TE_RAS && (texp->tev.chan == GX_COLORNULL || texp->tev.chan == t->tev.chan)) && (texp->tev.tex_swap == 0xFF || (t->tev.tex_swap == 0xFF || texp->tev.tex_swap == t->tev.a_range))) {
                            //TODO
                        }
                    }
                }
            } else if (t->tev.a_op == 0xFF) {
                HSD_TExpUnref(t, sel);
                res = TRUE;
                //TODO
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
            if (sel == 1) {
                sel = t->tev.a_op;
                if (sel == GX_COLORNULL) {
                    HSD_TExpUnref(t, 1);
                    res = TRUE;
                    //TODO
                } else {
                    if (((((sel != 0xff) && (sel == 0)) && (t->tev.a_in[0].sel == 7)) && ((t->tev.a_in[1].sel == 7 && (t->tev.a_bias == 0)))) && (t->tev.a_scale == 0)) {
                        sel = t->tev.a_in[3].type;
                        if (sel == HSD_TE_TEX) {
                            if (texp->tev.tex == NULL || texp->tev.tex == t->tev.tex) {
                                //TODO
                            }
                        } else if (sel == HSD_TE_TEV) {
                            //TODO
                        } else if (sel == HSD_TE_RAS && (texp->tev.chan == GX_COLORNULL || texp->tev.chan == t->tev.chan)) {
                            //TODO
                        }
                    }
                }
            } else if (t->tev.a_op == 0xFF) {
                HSD_TExpUnref(t, sel);
                res = TRUE;
                //TODO
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
            texp->tev.a_range = 0xFF;
        }
        if ((iVar5 == -1) && (iVar6 == -1)) {
            texp->tev.chan = 0xFF;
            texp->tev.tex_swap = 0xFF;
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
    HSD_TExp* pabVar4;
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
                && (type = HSD_TExpGetType(texp->tev.c_in[0].exp), type != 4)
                && (type = HSD_TExpGetType(texp->tev.c_in[3].exp), type != 4)) {
                if (texp->tev.c_op == 0 && texp->tev.c_in[3].type == 1
                    && (((sel = texp->tev.c_in[3].sel, sel == 1) && texp->tev.c_in[3].exp->tev.c_clamp != 0) || (sel == 5 && texp->tev.c_in[3].exp->tev.a_clamp != 0))
                    && (sel = texp->tev.c_in[0].type, sel < 4)
                    && sel > 1) {
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
                sel = texp->tev.c_in[0].type;
                if (sel == 1) {
                    if (texp->tev.c_in[0].sel == 1) {
                        curr = (HSD_TETev*)texp->tev.c_in[0].exp;
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
                                    if (texp->tev.a_range == 0xff) {
                                        texp->tev.a_range = curr->tev.a_range;
                                    }
                                    if (texp->tev.tex_swap == 0xff) {
                                        texp->tev.tex_swap = curr->tev.tex_swap;
                                    }
                                    HSD_TExpUnref(curr, 1);
                                }
                            }
                        }
                    }
                } else {
                    if (sel == 0 && texp->tev.c_in[3].type == 1 && texp->tev.c_in[3].sel == 1
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
                            if (texp->tev.a_range == 0xFF) {
                                texp->tev.a_range = curr->tev.a_range;
                            }
                            if (texp->tev.tex_swap == 0xFF) {
                                texp->tev.tex_swap = curr->tev.tex_swap;
                            }
                            HSD_TExpUnref(curr, 1);
                        }
                    }
                }
            }
            if ((texp->tev.a_op == 0 || texp->tev.a_op == 1)
                && texp->tev.a_in[1].sel == 7 && texp->tev.a_in[2].sel == 7
                && (type = HSD_TExpGetType(texp->tev.a_in[0].exp), type != 4)
                && (type = HSD_TExpGetType(texp->tev.a_in[3].exp), type != 4)) {
                if (texp->tev.a_op == 0 && texp->tev.a_in[3].type == 1 && texp->tev.a_in[3].exp->tev.a_clamp != 0 && (sel = texp->tev.a_in[0].type, sel < 4 && 1 < sel)) {
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

                sel = texp->tev.a_in[0].type;
                if (sel == 1) {
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
                    if (sel == 0 && texp->tev.a_in[3].type == 1) {
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
    if (HSD_TExpGetType(texp) == 1) {
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
                    if (texp->tev.ras_swap == 0xFF) {
                        t->tev.ras_swap = t->tev.ras_swap;
                    } else {
                        if (texp->tev.ras_swap != t->tev.ras_swap) {
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
