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

#define HSD_TEXP_TEX -1
#define HSD_TEXP_ZERO 0

#define HSD_TE_U8 0
#define HSD_TE_U16 1
#define HSD_TE_U32 2
#define HSD_TE_F32 3
#define HSD_TE_ALL 7

typedef struct _HSD_TExpTevDesc {
    struct _HSD_TExpTevDesc* next;
    u32 x4_unk;
    u32 x8_unk;
    u32 xC_unk;
    u32 x10_unk;
    u32 x14_unk;
    u32 x18_unk;
    u32 x1C_unk;
    u32 x20_unk;
    u32 x24_unk;
    u32 x28_unk;
    u32 x2C_unk;
    u32 x30_unk;
    u32 x34_unk;
    u32 x38_unk;
    u32 x3C_unk;
    u32 x40_unk;
    u32 x44_unk;
    u32 x48_unk;
    u32 x4C_unk;
    u32 x50_unk;
    u32 x54_unk;
    u32 x58_unk;
    u32 x5C_unk;
    u32 x60_unk;
    u32 x64_unk;
    u32 x68_unk;
    u32 x6C_unk;
    u32 x70_unk;
    u32 x74_unk;
    u32 x78_unk;
} HSD_TExpTevDesc;

typedef struct _HSD_TExp {
    u32 flags;
    struct _HSD_TExp* next;
    u32 x8_unk;
    u32 xC_unk;
    u32 x10_unk;
    u8 x14_unk;
    u8 x15_unk;
    u8 x16_unk;
    u8 x17_unk;
    u32 x18_unk;
    u32 x1C_unk;
    u32 x20_unk;
    u32 x24_unk;
    u32 x28_unk;
    u32 x2C_unk;
    u32 x30_unk;
    u32 x34_unk;
    u32 x38_unk;
    u32 x3C_unk;
    u32 x40_unk;
    u32 x44_unk;
    u32 x48_unk;
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

u32 HSD_TExpGetType(u32 *);
void HSD_TExpRef(u8 *, u8);
void HSD_TExpUnref(u8 *, u8);
void HSD_TExpFreeList(HSD_TExp *, u32, u8);
HSD_TExp* HSD_TExpTev(HSD_TExp **);
HSD_TExp* HSD_TExpCnst(void*, u32, u32, HSD_TExp**);
void HSD_TExpOrder(HSD_TExp *, void *, u8);
void HSD_TExpCompile(HSD_TExp *, HSD_TExpTevDesc **, HSD_TExp **);
void HSD_TExpFreeTevDesc(HSD_TExpTevDesc *);

#endif
