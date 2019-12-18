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

#define GOBJ_NOREF -1

typedef struct _HSD_GObj {
    u16 classifier;
    s8 p_link;
    s8 gx_link;
    u8 p_priority;
    u8 render_priority;
    s8 obj_kind;
    s8 data_kind;
    struct _HSD_GObj* next; //0x08
    struct _HSD_GObj* prev; //0x0C
    struct _HSD_GObj* next_gx; //0x10
    struct _HSD_GObj* prev_gx; //0x14
    struct _HSD_GObjProc* proc; //0x18
    void (*render_cb)(struct _HSD_GObj* gobj, int code); //0x1C
    u64 gxlink_prios;
    void* hsd_obj;
    void* data;
    void (*user_data_remove_func)(void* data);
    void* x34_unk;
} HSD_GObj;

typedef struct _HSD_GObjProc {
    struct _HSD_GObjProc* child; //0x0
    struct _HSD_GObjProc* next; //0x4
    struct _HSD_GObjProc* prev; //0x8
    u8 s_link; //0xC
    s8 flags; //0xD
    struct _HSD_GObj* gobj; //0x10
    void (*callback)(struct _HSD_GObj*); //0x14
} HSD_GObjProc;

BOOL GObj_IsPlayer(HSD_GObj*);
void GObj_AnimAll_Callback(HSD_GObj*);
BOOL GObj_IsItem(HSD_GObj*);
void GObj_AnimateJObj(HSD_GObj*);
void GObj_LinkProc(HSD_GObjProc*);
HSD_GObjProc* GObj_CreateProcWithCallback(HSD_GObj*, void (*)(), u8);
void GObj_FreeProc(HSD_GObjProc*);
void GObj_ProcReparent(HSD_GObjProc*);
void GObj_ProcRemoveAll(HSD_GObj*);
void GObj_PReorder(HSD_GObj*, HSD_GObj*);
HSD_GObj* GObj_Create(u32, u32, u32);
void GObj_Free(HSD_GObj*);
void GObj_GXReorder(HSD_GObj*, HSD_GObj*);
void GObj_SetupGXLink(HSD_GObj*, void(*)(HSD_GObj*, s32), u32, u32);
void GObj_SetupCameraGXLink(HSD_GObj*, void(*)(HSD_GObj*, s32), u32);
void GObj_SetupGXLink_HighestPrio_Max(HSD_GObj*, void (*)(HSD_GObj*, s32), u32);
void GObj_GXLinkDestructor(HSD_GObj*);
void GObj_InitKindObj(HSD_GObj*, s8, void*);
void* GObj_NullObj_ReturnPtr(HSD_GObj*);
void GObj_CallHSDDestructor(HSD_GObj*);
void GObj_InitKindData(HSD_GObj*, s8, void(*)(void*), void*);
void GObj_CallDestructor(HSD_GObj*);
void GObj_RunProcs(void);
s32 GObj_GetFlagFromArray(s32);
void GObj_SetTextureCamera(HSD_GObj*, u32);
void GObj_RunGXLinkMaxCallbacks(void);
void GObj_SetCamera(HSD_GObj*);
void GObj_InitializeLibInitData(u32*);
void GObj_Init(u32*);

#endif
