#ifndef _hsd_texp_h_
#define _hsd_texp_h_

#include <gctypes.h>

#include "hsd_debug.h"
#include "hsd_memory.h"

#define HSD_TEXP_RAS -2
#define HSD_TEXP_TEX -1
#define HSD_TEXP_ZERO 0

#define TEVOP_MODE 0
#define TEVCONF_MODE 1

typedef enum _HSD_TEInput {
    HSD_TE_END = 0,
    HSD_TE_RGB = 1,
    HSD_TE_R = 2,
    HSD_TE_G = 3,
    HSD_TE_B = 4,
    HSD_TE_A = 5,
    HSD_TE_X = 6,
    HSD_TE_0 = 7,
    HSD_TE_1 = 8,
    HSD_TE_1_8 = 9,
    HSD_TE_2_8 = 10,
    HSD_TE_3_8 = 11,
    HSD_TE_4_8 = 12,
    HSD_TE_5_8 = 13,
    HSD_TE_6_8 = 14,
    HSD_TE_7_8 = 15,
    HSD_TE_INPUT_MAX = 16
} HSD_TEInput;

typedef enum _HSD_TEType {
    HSD_TE_U8 = 0,
    HSD_TE_U16 = 1,
    HSD_TE_U32 = 2,
    HSD_TE_F32 = 3,
    HSD_TE_F64 = 4,
    HSD_TE_COMP_TYPE_MAX = 5
} HSD_TEType;

typedef enum _HSD_TECombType {
    HSD_TE_ZERO = 0,
    HSD_TE_TEV = 1,
    HSD_TE_TEX = 2,
    HSD_TE_RAS = 3,
    HSD_TE_CNST = 4,
    HSD_TE_IMM = 5,
    HSD_TE_KONST = 6,
    HSD_TE_ALL = 7,
    HSD_TE_TYPE_MAX = 8
} HSD_TECombType;

typedef struct _HSD_TExpTevDesc {
    struct _HSD_TExpTevDesc* next;
    u32 flags;
    u32 stage;
    u32 coord;
    u32 map;
    u32 color;
    union {
        struct {
            u32 op;
            u32 tevmode;
        } tevop;
        struct {
            u32 clr_op;
            u32 clr_a;
            u32 clr_b;
            u32 clr_c;
            u32 clr_d;
            u32 clr_scale;
            u32 clr_bias;
            u32 clr_clamp;
            u32 clr_reg;
            u32 alpha_op;
        } tevconf;
    } u;
    u32 color_a;
    u32 color_b;
    u32 color_c;
    u32 color_d;
    u32 color_scale;
    u32 color_bias;
    u32 color_clamp;
    u32 color_reg;
    u32 alpha_op;
    u32 alpha_a;
    u32 alpha_b;
    u32 alpha_c;
    u32 alpha_d;
    u32 alpha_scale;
    u32 alpha_bias;
    u32 alpha_clamp;
    u32 alpha_reg;
    u32 x60_unk;
    u32 ras_sel;
    u32 tex_sel;
    u32 color_sel;
    u32 alpha_sel;
    void* x74_unk;
    u32 x78_unk;
} HSD_TExpTevDesc;

typedef struct _HSD_TECommon {
    HSD_TECombType type;
    union _HSD_TExp* next;
} HSD_TECommon;

typedef struct _HSD_TECnst {
    HSD_TECombType type;
    union _HSD_TExp* next;
    void* val;
    u8 comp;
    u8 ctype;
    u8 reg;
    u8 idx;
    u8 ref;
    u8 range;
} HSD_TECnst;

typedef struct _HSD_TExpDag {
    struct _HSD_TETev* tev;
    u8 idx;
    u8 nb_dep;
    u8 nb_ref;
    u8 dist;
    struct _HSD_TExpDag* dag[7];
    u8 unk[4];
    u8 unk_2[4];
} HSD_TExpDag;

typedef struct _HSD_TETev {
    HSD_TECombType type;
    union _HSD_TExp* next;
    u32 c_ref;
    u8 c_dst;
    u8 c_op;
    u8 c_clamp;
    u8 c_bias;
    u8 c_scale;
    u8 c_range;
    u32 a_ref;
    u8 a_dst;
    u8 a_op;
    u8 a_clamp;
    u8 a_bias;
    u8 a_scale;
    u8 a_range;
    u8 tex_swap;
    u8 ras_swap;
    u8 kcsel;
    u8 kasel;
    struct {
        u8 type;
        u8 sel;
        u8 arg;
        union _HSD_TExp* exp;
    } c_in[4];
    struct {
        u8 type;
        u8 sel;
        u8 arg;
        union _HSD_TExp* exp;
    } a_in[4];
    void* tex; //HSD_TObj*
    u8 chan;
    struct _HSD_TExpDag* dag[7];
} HSD_TETev;

typedef union _HSD_TExp {
    HSD_TECombType type;
    struct _HSD_TECommon* comm;
    struct _HSD_TETev tev;
    struct _HSD_TECnst cnst;
} HSD_TExp;

void HSD_StateRegisterTexGen(u32);
void HSD_StateSetNumTexGens();
void HSD_SetTevRegAll();
u32 HSD_TexCoordID2Num(u32);
u32 HSD_TExpGetType(HSD_TExp*);
void HSD_TExpRef(HSD_TExp*, u8);
void HSD_TExpUnref(HSD_TExp*, u8);
void HSD_TExpFreeList(HSD_TExp*, u32, u8);
HSD_TExp* HSD_TExpTev(HSD_TExp**);
HSD_TExp* HSD_TExpCnst(void*, u32, u32, HSD_TExp**);
void HSD_TExpColorOp(HSD_TExp*, u8, u8, u8, u8);
void HSD_TExpAlphaOp(HSD_TExp*, u8, u8, u8, u8);
void HSD_TExpColorIn(HSD_TExp*, HSD_TEInput, HSD_TExp*, HSD_TEInput, HSD_TExp*, HSD_TEInput, HSD_TExp*, HSD_TEInput, HSD_TExp*);
void HSD_TExpAlphaIn(HSD_TExp*, HSD_TEInput, HSD_TExp*, HSD_TEInput, HSD_TExp*, HSD_TEInput, HSD_TExp*, HSD_TEInput, HSD_TExp*);
void HSD_TExpOrder(HSD_TExp*, void*, u8);
void HSD_TExpCompile(HSD_TExp*, HSD_TExpTevDesc**, HSD_TExp**);
void HSD_TExpFreeTevDesc(HSD_TExpTevDesc*);

#endif
