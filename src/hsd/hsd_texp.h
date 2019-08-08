#ifndef _hsd_texp_h_
#define _hsd_texp_h_

#include <gctypes.h>

#include "hsd_debug.h"
#include "hsd_memory.h"

#define HSD_TE_TEV 1
#define HSD_TE_RGB 1
#define HSD_TE_R 2
#define HSD_TE_G 3
#define HSD_TE_B 4
#define HSD_TE_A 5
#define HSD_TE_X 6
#define HSD_TE_0 7
#define HSD_TE_1 8
#define HSD_TE_4_8 12

#define HSD_TEXP_RAS -2
#define HSD_TEXP_TEX -1
#define HSD_TEXP_ZERO 0

#define HSD_TE_U8 0
#define HSD_TE_U16 1
#define HSD_TE_U32 2
#define HSD_TE_F32 3
#define HSD_TE_ALL 7

#define TEVOP_MODE   0
#define TEVCONF_MODE 1

typedef u32 HSD_TEInput;

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

typedef struct _HSD_TExp {
    u32 flags;
    struct _HSD_TExp* next;
    u32 opt_ref_count;
    u8 xC_unk;
    u8 color_op;
    u8 color_enable;
    u8 color_bias;
    u32 color_scale;
    u8 nopt_ref_count;
    u8 x15_unk;
    u8 t4_ref_count;
    u8 x17_unk;
    u8 x18_unk;
    u8 alpha_op;
    u8 alpha_enable;
    u8 alpha_bias;
    u32 alpha_scale;
    u32 x20_unk;
    u8 x24_unk;
    u8 texp2_opt;
    u8 x26_unk;
    u8 x27_unk;
    struct _HSD_TExp* texp_2;
    u32 x2C_unk;
    u32 x30_unk;
    u32 x34_unk;
    u32 x38_unk;
    u32 x3C_unk;
    u32 x40_unk;
    u8 x44_unk;
    u8 texp3_opt;
    u8 x46_unk;
    u8 x47_unk;
    struct _HSD_TExp* texp_3;
    u32 x4C_unk;
    u32 x50_unk;
    u32 x54_unk;
    u32 x58_unk;
    u32 x5C_unk;
    u32 x60_unk;
    u32 x64_unk;
    u32 x68_unk;
    u32 x6C_unk;
} HSD_TExp;

void HSD_StateRegisterTexGen(u32);
void HSD_StateSetNumTexGens();
void HSD_SetTevRegAll();
u32 HSD_TexCoordID2Num(u32);
u32 HSD_TExpGetType(HSD_TExp *);
void HSD_TExpRef(HSD_TExp *, u8);
void HSD_TExpUnref(HSD_TExp *, u8);
void HSD_TExpFreeList(HSD_TExp *, u32, u8);
HSD_TExp* HSD_TExpTev(HSD_TExp **);
HSD_TExp* HSD_TExpCnst(void*, u32, u32, HSD_TExp**);
void HSD_TExpColorOp(HSD_TExp *, u8, u8, u8, u8);
void HSD_TExpAlphaOp(HSD_TExp *, u8, u8, u8, u8);
void HSD_TExpColorIn(HSD_TExp *, u32, HSD_TExp *, u32, HSD_TExp *, u32, HSD_TExp *, u32, HSD_TExp *);
void HSD_TExpAlphaIn(HSD_TExp *, u32, HSD_TExp *, u32, HSD_TExp *, u32, HSD_TExp *, u32, HSD_TExp *);
void HSD_TExpOrder(HSD_TExp *, void *, u8);
void HSD_TExpCompile(HSD_TExp *, HSD_TExpTevDesc **, HSD_TExp **);
void HSD_TExpFreeTevDesc(HSD_TExpTevDesc *);

#endif
