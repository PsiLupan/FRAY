#ifndef _scene_h_
#define _scene_h_

#include <string.h>

#include <gccore.h>
#include <gctypes.h>

#include "hsd/hsd_fog.h"
#include "hsd/hsd_jobj.h"
#include "hsd/hsd_pad.h"
#include "hsd/hsd_video.h"

#include "hsd_cobj_ext.h"
#include "hsd_fog_ext.h"
#include "hsd_jobj_ext.h"
#include "hsd_lobj_ext.h"
#include "hsd_pad_ext.h"

#include "archive.h"
#include "gobj.h"
#include "match.h"
#include "menu.h"
#include "soundfx.h"
#include "util.h"

#include "ogcext/dvd.h"

//Scenes
#include "scGmRst.h"
#include "scGmTitle.h"
#include "scGmTrain.h"
#include "scOpening.h"

//Length: 0x14(20)
typedef struct _GameState {
    u8 curr_major;
    u8 pending_major;
    u8 prev_major;
    u8 curr_minor;
    u8 unk04;
    s8 unk05;
    u16 unk06;
    u16 unk08;
    u8 unk0A;
    s8 unk0B;
    u8 pending;
    u8 unk0D;
    u8 unk0E;
    u8 unk0F;
    u8 (*unk10)();
    u32* padstatus;
} GameState; //80479D30

extern GameState gamestate;

typedef struct _MinorSceneHandler {
    u8 class_id;
    u32 pad : 3;
    void (*OnFrame)();
    void (*OnLoad)();
    void (*OnLeave)();
    void (*unk_func)();
} MinorSceneHandler; //803DA920

typedef struct _MinorScene {
    u8 idx;
    u8 preload;
    u16 flags;

    void (*Prep)();
    void (*Decide)();

    u8 class_id;
    u8 unk_1;
    u8 unk_2;
    u8 unk_3;

    void* unk_struct_0;
    void* unk_struct_1;
} MinorScene;

typedef struct _MajorScene {
    u8 preload;
    u8 idx;
    u16 flags;
    void (*Load)();
    void (*Unload)();
    void (*Init)();
    void* minor_scenes;
} MajorScene; //803DACA4

typedef struct _unk_8046B0F0 {
    u32 unk00;
    u32 unk04;
    u32 unk08;
    u32 unk0C;
} unk_8046B0F0;

extern u32 debug_level;

extern unk_8046B0F0 dword_8046B0F0;

void Scene_CopyDataToCache();

static void Scene_Minor_Class0_OnFrame();
static void Scene_Minor_Class0_OnLoad();

static void Scene_Minor_Class40_OnFrame();
static void Scene_Minor_Class40_OnLoad();
static void Scene_Minor_Class40_OnLeave();

void Scene_801A36A0(u32, u32*, u32*);

static void Scene_ReqAnimAll_Callback(HSD_GObj*);
static void Scene_RunStartupInit();
void Scene_CompareCacheOnChange(MinorScene*);

u32* Scene_Get10(GameState*);
u32* Scene_GetPadStatus(GameState*);
void Scene_Set05(u8);
u8 Scene_Get04();
u8 Scene_GetCurrentMinor();
void Scene_SetPendingTrue();
void Scene_UpdatePendingMajor(u8);
void Scene_SetPendingMajor(u8);
u8 Scene_GetCurrentMajor();
u8 Scene_LoadPrevMajor();

BOOL Scene_IsSinglePlayer(u8);
u8* Scene_ProcessMajor(u8);
void Scene_StoreClassID(u8);
u32* Scene_Load4F80_idx2();
u32* Scene_Load4F80_idx3();
void Scene_PrepCommon();
MinorSceneHandler* Scene_GetSceneHandlerByClass(u8);
void Scene_PerFrameUpdate(void (*onframefunc)());
MinorSceneHandler* Scene_GetClassHandler();

MajorScene* Scene_GetMajorScenes();

void Scene_ReqAnimAll(HSD_JObj*, f32*);

void Scene_80017700(s32);
void Scene_80018254();

#endif
