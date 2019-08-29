#ifndef _gobj_h_
#define _gobj_h_

#include <assert.h>

#include <gctypes.h>

#include "hsd/hsd_jobj.h"

#define GOBJ_KIND_MENU_COBJ 1
#define GOBJ_KIND_JOBJ 3
#define GOBJ_KIND_FOG 4

#define GOBJ_CLASS_STAGE 0x2
#define GOBJ_CLASS_CAMERA_RUMBLE 0x3
#define GOBJ_CLASS_PLAYER 0x4
#define GOBJ_CLASS_ITEM 0x6
#define GOBJ_CLASS_GFX 0X8
#define GOBJ_CLASS_TEXT 0x9
#define GOBJ_CLASS_HSD_FOG 0xA
#define GOBJ_CLASS_HSD_LOBJ 0xB
#define GOBJ_CLASS_HSD_COBJ_TITLE 0x13

#define GOBJ_NOREF 0xFF

typedef struct _HSD_GObj {
    u16 classifier;
    u8 p_link;
    u8 gx_link;
    u8 p_priority;
    u8 render_priority;
    s8 obj_kind;
    s8 data_kind;
    struct _HSD_GObj* next;
    struct _HSD_GObj* prev;
    struct _HSD_GObj* next_gx;
    struct _HSD_GObj* prev_gx;
    struct _HSD_GObjProc* proc;
    void (*render_cb)(struct _HSD_GObj* gobj, int code);
    u32 x20_unk;
    u32 x24_unk;
    void* data;
    void* hsd_obj; //JObj or CObj known
    void (*user_data_remove_func)(void* data);
    void* x34_unk;
    u32 x38_unk;
    u32 flags;
    void* x40_unk;
    void* x44_unk;
} HSD_GObj;

typedef struct _HSD_GObjProc {
    struct _HSD_GObjProc* child; //0x0
    struct _HSD_GObjProc* next; //0x4
    struct _HSD_GObjProc* prev; //0x8
    u8 s_link; //0xC
    u16 flags; //0xD
    struct _HSD_GObj* gobj; //0x10
    void (*callback)(); //0x14
} HSD_GObjProc;

BOOL GObj_IsPlayer(HSD_GObj*);
void GObj_AnimAll_Callback(HSD_GObj*);
BOOL GObj_IsItem(HSD_GObj*);
void GObj_LinkProc(HSD_GObjProc*);
HSD_GObjProc* GObj_CreateProcWithCallback(HSD_GObj*, void (*)(), u8);
void GObj_PReorder(HSD_GObj*, HSD_GObj*);
HSD_GObj* GObj_Create(u32, u32, u32);
void GObj_Free(HSD_GObj*);
void GObj_GXReorder(HSD_GObj*, HSD_GObj*);
void GObj_SetupGXLink(HSD_GObj*, void*, u32, u32);
void GObj_SetupGXLink_Max(HSD_GObj*, void*, u32);
void GObj_GXLinkDestructor(HSD_GObj*);
void GObj_InitKindObj(HSD_GObj*, s8, void*);
void* GObj_NullObj_ReturnPtr(HSD_GObj*);
void GObj_CallHSDDestructor(HSD_GObj*);
void GObj_InitKindData(HSD_GObj*, s8, void*, void*);
void GObj_CallDestructor(HSD_GObj*);
u32 GObj_GetFlagFromArray(u32);
void GObj_SetTextureCamera(HSD_GObj*, u32);
void GObj_RunGXLinkMaxCallbacks(void);
void GObj_Prep(void**);

#endif
