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
    if(c_in_type != 2){
        switch (c_in_type){
            case 0:
            break;

            case 3:
            break;
        }
    }
    //TODO

    if(tev->a_range == 0xFF){
        tev->a_range = c_in_type;
    }else{
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
    }
    else {
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
    if (texp->tev.c_op == GX_COLORNULL || 
        (((((texp->tev.c_ref == 0 && (val = texp->tev.a_op, val != 8)) && (val != 9)) &&
        ((val != 10 && (val != 11)))) && ((val != 12 && (val != 13)))))) {
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

    if(num > 0){
        for(u32 i = num; i > 0; --i){
            if(*list == tev){
                if(d <= dist[n]){
                    return;
                }
                dist[n] = d;
                for(u32 j = 0; i < 4; ++j){
                    if(tev->c_in[j].type == 1){
                        CalcDistance(tevs, dist, &tev->c_in[j].exp->tev, num, d + 1);
                    }
                    if(tev->a_in[j].type == 1){
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
    u32 i = 1;

    for(u32 j = 0; j < i; ++i){
        HSD_CheckAssert("HSD_TExpMakeDag: j < HSD_TEXP_MAX_NUM", j < HSD_TEXP_MAX_NUM);

        u32 l = 0;
        HSD_TExp* texp;
        for(u32 k = 0; k < 4; ++k, i = l){
            if(texp->tev.c_in[k].type == HSD_TE_TEV){
                u32 num = 0;
                if(i > 0){
                    for(l = i; l > 0; --l){
                        if(*tevs == texp->tev.c_in[k].exp){

                            break;
                        }

                    }
                    if(i <= num){
                        l = i + 1;
                        local_ac[i] = (HSD_TETev *)tev->c_in[0].exp;
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

    if(num > 0){
        for(u32 i = num; i > 0; --i){

        }
    }
}

//80386470
static u32 SimplifySrc(HSD_TExp* texp)
{
}

//8038687C
static u32 SimplifyThis(HSD_TExp* texp)
{
}

//803870E4
static u32 SimplifyByMerge(HSD_TExp* texp)
{
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
}
