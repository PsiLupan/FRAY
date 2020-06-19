#include "scene.h"

static void Scene_Minor_Class40_OnFrame();
static void Scene_Minor_Class40_OnLoad();
static void Scene_Minor_Class40_OnLeave();

static void Scene_RunStartupInit();

GameState gamestate;

#define END_MINOR                                      \
    {                                                  \
        0xFF, 0, 0, NULL, NULL, 0, NULL, NULL \
    }

MinorScene GmTitle_Minors[2] = {
    { 0, 3, 0, Menu_Title_Prep, Menu_Title_Decide, 0, NULL, &title_inputs },
    END_MINOR
}; //803DD6A0

MinorScene CSS_Minors[9] = {
    { 0, 3, 0, Menu_CSS_VSMode_Prep, Menu_CSS_VSMode_Decide, 8, NULL, NULL /*0x804807B0, 0x804807B0 */ },
    { 1, 3, 0, stub /*801b1514*/, stub /*801b154c*/, 9, NULL, NULL /*0x80480668, 0x80480668 */ },
    //{ 2, 3, 0, }
    END_MINOR
}; //803DD9A0

MinorScene GmOpening_Progressive_Minors[2] = {
    { 0, 2, 0, Menu_Opening_ProgressiveScan_Prep, Menu_Opening_ProgressiveScan_Decide, 40, NULL, NULL /*0x804D6938, 0x804D693C*/ },
    END_MINOR
}; //803DFDA8

MinorScene GmOpening_Minors[2] = {
    { 0, 1, 0, Menu_Opening_Prep, Menu_Opening_Decide, 42, NULL, NULL /*0x804D6940, 0x804D6948*/ },
    END_MINOR
}; //803DFDD8

MinorSceneHandler scene_handlers[46] = {
    { 0, Menu_Title_OnFrame, Menu_Title_OnLoad, NULL, NULL },
    { 40, Scene_Minor_Class40_OnFrame, Scene_Minor_Class40_OnLoad, Scene_Minor_Class40_OnLeave, NULL },
    { 45, NULL, NULL, NULL, NULL }
}; //803DA920 - 45 in length

MajorScene major_scenes[46] = {
    { 1, 0, 0, NULL, NULL, NULL, &GmTitle_Minors },
    { 0, 2, 0, Menu_CSS_Load, NULL /*Technically a stubbed function*/, Menu_CSS_Init, &CSS_Minors },
    { 0, 39, 0, NULL, NULL, NULL, &GmOpening_Progressive_Minors },
    { 1, 40, 0, NULL, NULL, NULL, &GmOpening_Minors },
    { 0, 45, 0, NULL, NULL, NULL, NULL }
}; //803DACA4

static u8 r13_3D40; //0x3D40(r13)
static HSD_GObj* stub_gobj; //0x4F74(r13)
static u32 r13_4F80[3];

u32 debug_level = 0; //0x6C98(r13)

s32 cache_base[24] = { //803BA638
    0x00000000, 0x00000000, 0x2d000000, 0x00000148,
    0x00000021, 0x00010000, 0x00000021, 0x00010000,
    0x00000021, 0x00010000, 0x00000021, 0x00010000,
    0x00000021, 0x00010000, 0x00000021, 0x00010000,
    0x00000021, 0x00010000, 0x00000021, 0x00010000,
    0x00000000, 0x00000000, 0xff00ffff, 0x00000000
};

s32 preload_cache[0x1000]; //80432078

unk_8046B0F0 dword_8046B0F0;

u32 scene_80479C38[14][4]; //80479C38 - Length made up but currently assumed based on layout in memory

GameState gamestate; //80479D30

void* unk_cb[19]; //80479D48

//8016B3A0
BOOL Scene_IsCurrSceneSuperSuddenDeath()
{
    if (Scene_GetCurrentMajor() == 16)
        return TRUE;
    return FALSE;
}

//8016B41C
//8016B498
BOOL Scene_IsCurrSceneSinglePlayer()
{
    u8 curr_major = Scene_GetCurrentMajor();
    return Scene_IsSinglePlayer(curr_major);
}

//8016B3D8
BOOL Scene_IsSceneClassicAdvOrAllStar()
{
    u8 curr_major = Scene_GetCurrentMajor();
    if (curr_major < 6 && curr_major >= 3) {
        return TRUE;
    }
    return FALSE;
}

//800174BC
void Scene_PrepCache()
{
    Scene_CopyDataToCache();
    Scene_80018254();
    Scene_80017700(4);
}

BOOL Scene_80017644(s32 val)
{
    BOOL isEqual = FALSE;
    u32 unk = sub_80347364();
    u8* cache = (u8*)&preload_cache[0];

    if (preload_cache[603] == val) {
        isEqual = TRUE;
    } else {
        u32 i = 0;
        do {
            if (((cache[0xAC] == 3 || cache[0xAC] == 4) && (s32)cache[0xAE] == val)
                && ((*(u16*)(cache + 0xB4) < 0 && cache[0xAF] == 2))) {
                sub_800174E8(i);
            }
            i += 1;
            cache = cache + 0x1C;
        } while (i < 80);
    }
    sub_8034738C(unk);
    return isEqual;
}

//80017700
void Scene_80017700(s32 val)
{
    /*BOOL i = TRUE;
  while(i != FALSE){
    i = Scene_80017644(val);
    DVD_CheckDisk();
  }*/
}

//8001822C
s32* Scene_GetPreloadCache_04()
{
    return &preload_cache[1];
}

//80018254
void Scene_80018254()
{
}

//80018C6C
void Scene_CopyDataToCache()
{
    if (preload_cache[0] == 2) {
        preload_cache[1] = 0;
        preload_cache[2] = 0;
        return;
    } else if (preload_cache[0] == 3) {
        preload_cache[1] = 1;
        preload_cache[2] = 1;
        s32* cache = &preload_cache[1];
        s32* base = &cache_base[0];

        u32 i = 9;
        do {
            cache[2] = base[2];
            cache[3] = base[3];
            i -= 1;
            cache = cache + 2;
            base = base + 2;
        } while (i != 0);
        return;
    } else if (preload_cache[0] <= 0) {
        return;
    }
    preload_cache[1] = 1;
}

//80018CF4
void Scene_ResetPreloadCache(BOOL preload)
{
}

//80018F58
void Scene_SetPreloadBool(BOOL preload)
{
    preload_cache[604] = preload;
}

//80019A30
static u32 sub_80019A30(u32 idx)
{
    //DAT_80432A00[idx * 6];
    return 1;
}

//8015FBA4
static void Scene_InitStaticMem()
{
}

//801A36A0
void Scene_801A36A0(u32 offset, u32* res_r3, u32* res_r4)
{
    if (res_r3 != NULL) {
        *res_r3 = scene_80479C38[offset][2];
    }
    if (res_r4 != NULL) {
        *res_r4 = scene_80479C38[offset][3];
    }
}

//801A3EF4
static void Scene_RunStartupInit()
{
    for (u32 i = 0; major_scenes[i].idx != 45; i += 1) {
        if (major_scenes[i].Init) {
            (*major_scenes[i].Init)();
        }
    }
}

//801A3F48
void Scene_CompareCacheOnChange(MinorScene* scene)
{
    /*int v1; // r31@1
  s32 *v3; // r31@9

  v1 = scene;
  Scene_ResetPreloadCache(scene->preload);
  u32 class = scene->class_id;
  if ( class == 8 )
  {
    sub_803A6048(9216);
    goto LABEL_9;
  }
  if ( class < 8 )
  {
    if ( class != 5 )
      goto LABEL_8;
LABEL_6:
    sub_803A6048(49152);
    goto LABEL_9;
  }
  if ( class == 43 )
    goto LABEL_6;
LABEL_8:
  sub_803A6048(18432);
LABEL_9:
  v3 = Scene_GetPreloadCache_04();
  if ( !sub_80015BB8(2) )
    *v3 = 1;
  if ( !sub_80015BB8(3) )
    v3[1] = 1;
  sub_80018254();
  sub_8001C5A4();
  sub_8001D1F4();
  sub_8001E27C();
  sub_803127D4();
  sub_8031C8B8();*/
}

//801A4014
void Scene_ProcessMinor(MajorScene* scene)
{
    u32 curr_minor;
    u32 ctr;
    MinorScene* minor_scenes;
    MinorScene* minor_scene = NULL;
    MinorSceneHandler* scene_handler;
    u8 minor_idx = 0;

    curr_minor = gamestate.curr_minor;
    minor_scenes = (MinorScene*)scene->minor_scenes;
    ctr = 255 - gamestate.curr_minor;
    if (gamestate.curr_minor != 255) {
        do {
            MinorScene curr;
            for (u32 i = 0; i < 255 && curr.idx != 255; i++) {
                curr = minor_scenes[i];
                if (curr.idx == curr_minor) {
                    minor_scene = &minor_scenes[i];
                    goto OUT;
                }
            }
            curr_minor += 1;
            --ctr;
        } while (ctr != 0);
    }
OUT:
    if (minor_scene != NULL) {
        gamestate.curr_minor = minor_scene->idx;
    } else {
        return;
    }
    Scene_CompareCacheOnChange(minor_scene);
    if (minor_scene->Prep != NULL) {
        minor_scene->Prep();
    }
    scene_handler = Scene_GetSceneHandlerByClass(minor_scene->class_id);
    Scene_PrepCommon();
    Scene_StoreClassID(minor_scene->class_id);

    if (scene_handler->OnLoad != NULL) {
        scene_handler->OnLoad(minor_scene->unk_struct_0);
    }
    Scene_PerFrameUpdate(scene_handler->OnFrame);

    if (!dword_8046B0F0.unk04 && scene_handler->OnLeave != NULL) {
        scene_handler->OnLeave(minor_scene->unk_struct_1);
    }

    if (!dword_8046B0F0.unk04) {
        if (minor_scene->Decide != NULL) {
            minor_scene->Decide(&gamestate);
        }
        gamestate.unk04 = gamestate.curr_minor;

        if (gamestate.unk05 != 0) {
            gamestate.curr_minor = gamestate.unk05 - 1;
            gamestate.unk05 = 0;
        } else {
            for (u32 i = 0; i < 255; i++) {
                if (minor_scenes[i].idx > gamestate.curr_minor) {
                    minor_idx = minor_scenes[i].idx;
                    break;
                }
            }
            if (minor_idx == 255) {
                minor_idx = 0;
            }
            gamestate.curr_minor = minor_idx;
        }
    }

    //sub_8001CDB4(); Memcard related, likely ignorable for now
    //sub_8001B760(11); More memcard
    //sub_8001F800(); Movie_Unload();

    if (dword_8046B0F0.unk04) {
        //sub_80027DBC();
        HSD_PadReset();
        /*s32 i = 0;
        do {
            i = sub_8001B6F8(); //Save related, so we can ignore for now
        } while ( i == 11 );
        if (DVDCheckDisk() == 0) {
            SYS_ResetSystem(1, 0, 0);
        }
            sub_8001F800(); Movie_Unload();
        
        i = 0;
        do {
            i = HSD_DevComIsBusy(1);
        } while (i != 0);*/
        
        Scene_InitStaticMem();
        memset(&gamestate, 0, 20);
        Scene_RunStartupInit();
        dword_8046B0F0.unk00 = 1;
        Scene_SetPendingMajor(40);
        HSD_VISetBlack(0);
    }
}

//801A427C
u8* Scene_Get10(GameState* state)
{
    return state->unk10;
}

//801A4284
u32* Scene_GetPadStatus(GameState* state)
{
    return state->padstatus;
}

//801A428C
void Scene_Set03And04(u8 val)
{
    gamestate.curr_minor = val;
    gamestate.unk04 = val;
}

//801A42A0
void Scene_Set05(u8 val)
{
    u8 v1;
    v1 = val + 1;
    gamestate.unk05 = v1;
}

//801A42B4
u8 Scene_Get04()
{
    return gamestate.unk04;
}

//801A42C4
u8 Scene_GetCurrentMinor()
{
    return gamestate.curr_minor;
}

//801A42D4
void Scene_SetPendingTrue()
{
    gamestate.pending = TRUE;
}

//801A42E8
void Scene_UpdatePendingMajor(u8 val)
{
    gamestate.pending_major = val;
}

//801A42F8
void Scene_SetPendingMajor(u8 val)
{
    gamestate.pending_major = val;
    gamestate.pending = TRUE;
}

//801A4310
u8 Scene_GetCurrentMajor()
{
    return gamestate.curr_major;
}

//801A4320
u8 Scene_LoadPrevMajor()
{
    return gamestate.prev_major;
}

//801A4330
void Scene_StoreTo10(u8 (*func)())
{
    gamestate.unk10 = func;
}

//801A4340
BOOL Scene_IsSinglePlayer(u8 scene)
{
    if (scene != 28) {
        if (scene < 28) {
            if (scene != 15 || (scene > 5 || scene < 3)) {
                return FALSE;
            }
        } else if (scene != 43 && (scene > 38 || scene < 32)) {
            return FALSE;
        }
    }
    return TRUE;
}

//801A43A0
u8* Scene_ProcessMajor(u8 scene)
{
    MajorScene* major_scene_list = Scene_GetMajorScenes();
    MajorScene* scene_ptr = NULL;
    u8* result;

    for (u32 i = 0; major_scene_list[i].idx != 45; i += 1) {
        if (major_scene_list[i].idx == scene) {
            scene_ptr = &major_scene_list[i];
            break;
        }
    }
    HSD_CheckAssert("Scene_ProcessMajor: scene_ptr == NULL", scene_ptr != NULL);
    gamestate.pending = FALSE;
    gamestate.curr_major = 0;
    gamestate.unk04 = 0;
    gamestate.unk05 = 0;
    Scene_SetPreloadBool(scene_ptr->preload);
    if (scene_ptr->Load != NULL) {
        (*scene_ptr->Load)();
    }
    do {
        result = NULL;
        while (true) {
            if (gamestate.pending != FALSE) {
                if (dword_8046B0F0.unk04 == 0 && scene_ptr->Unload != NULL) {
                    (*scene_ptr->Unload)();
                }
            }
            if (gamestate.unk10 != NULL) {
                break;
            }
        JMP_PROCESS:
            Scene_ProcessMinor(scene_ptr);
        }
        u8 major = (*gamestate.unk10)();
        if (major == 45) {
            goto JMP_PROCESS;
        }
        gamestate.unk06 = gamestate.curr_major;
        gamestate.unk0A = gamestate.unk04;
        gamestate.unk0B = gamestate.unk05;
        gamestate.pending = FALSE;
        gamestate.curr_major = 0;
        scene_ptr = NULL;
        for (u32 i = 0; major_scene_list[i].idx != 45; i++) {
            if (major_scene_list[i].idx == major) {
                scene_ptr = &major_scene_list[i];
            }
        }
        Scene_ProcessMinor(scene_ptr);
        if (dword_8046B0F0.unk04 == FALSE) {
            gamestate.curr_major = gamestate.unk06;
            gamestate.unk04 = gamestate.unk0A;
        }
    } while (true);
    return result;
}

//801A46F4
void sub_801A46F4()
{
}

//801A47E4
void sub_801A47E4()
{
}

//801A48A4
u64 sub_801A48A4(u32 uParm1)
{
    /*u32 uVar1;
  u32 *puVar2;
  u32 uVar3;
  s32 iVar4;
  
  iVar4 = 2;
  puVar2 = &DAT_803da888;
  uVar1 = 0;
  uVar3 = 0;
  do {
    if ((uParm1 & 1) != 0) {
      uVar3 = uVar3 | *puVar2;
      uVar1 = uVar1 | puVar2[1];
    }
    uParm1 = (s32)(uParm1 & 0xff) >> 1;
    if ((uParm1 & 1) != 0) {
      uVar3 = uVar3 | puVar2[2];
      uVar1 = uVar1 | puVar2[3];
    }
    uParm1 = (s32)uParm1 >> 1;
    if ((uParm1 & 1) != 0) {
      uVar3 = uVar3 | puVar2[4];
      uVar1 = uVar1 | puVar2[5];
    }
    uParm1 = (s32)uParm1 >> 1;
    if ((uParm1 & 1) != 0) {
      uVar3 = uVar3 | puVar2[6];
      uVar1 = uVar1 | puVar2[7];
    }
    uParm1 = (s32)uParm1 >> 1;
    puVar2 = puVar2 + 8;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  return CONCAT44(uVar3,uVar1);*/
    return 1;
}

//801A4B88
void Scene_StoreClassID(u8 class_id)
{
    r13_4F80[0] = class_id;
}

//801A4B90
u32* Scene_Load4F80_idx2()
{
    return (u32*)&r13_4F80[1];
}

//801A4B9C
u32* Scene_Load4F80_idx3()
{
    return (u32*)&r13_4F80[2];
}

//801A4BD4
void Scene_PrepCommon()
{
    unk_cb[10] = sub_801A46F4;
    unk_cb[11] = sub_801A47E4;
    unk_cb[17] = NULL;
    unk_cb[18] = NULL;
    //sub_80322E54(0.0166667 * - 4.5036e15)
    //sub_80019880();
    GObj_InitializeLibInitData((u32*)unk_cb);
    //DAT_80479d4a = 0x18;
    //u32 res = GObj_803912A8(&unk_cb, &DAT_8040C3A4);
    //r13_3D40 = res;
    //sub_803A44A4();
    unk_cb[3] = &unk_cb[14];
    GObj_Init((u32*)unk_cb);

    HSD_GObj* gobj = GObj_Create(0xE, 0, 0);
    stub_gobj = gobj;
    if (stub_gobj != NULL) {
        GObj_CreateProcWithCallback(stub_gobj, stub, 0);
    }
}

//801A4CE0
MinorSceneHandler* Scene_GetSceneHandlerByClass(u8 class_id)
{
    MinorSceneHandler* sh = Scene_GetClassHandler();
    for (u32 i = 0; sh[i].class_id != 45; i++) {
        if (sh[i].class_id == class_id) {
            return &sh[i];
        }
    }
    return NULL;
}

//801A4D34
void Scene_PerFrameUpdate(void (*onframefunc)())
{
    MatchController_InitData(&match_controller);
    match_controller.timer = 0;
    match_controller.unk04 = 0;
    match_controller.timer2 = 0;
    match_controller.screen_ctrl = 0;
    HSD_PadFlushQueue(HSD_PAD_FLUSH_QUEUE_LEAVE1);
    //MemCard_DisplaySaveBanner(); 8001CF18
    do {
        if (match_controller.screen_ctrl != 0) {
            HSD_VISetXFBDrawDone();
            return;
        }
        //For clarity sake, this the game does the following with the PAD Alarm.
        //Doing it here is technically the fix for poll drift.
        u32 intr = IRQ_Disable();
        HSD_PadRenewRawStatus();
        IRQ_Restore(intr);

        //sub_80392E80(); Something memory card related
        u8 pad_queue_count;
        while (true) {
            pad_queue_count = Pad_CheckQueue();
            if (pad_queue_count != 0) {
                break;
            }
            DVD_CheckDisk();
        }
        DVD_CheckDisk();
        BOOL reset = HSD_PadGetResetSwitch();
        if (reset == TRUE) {
            dword_8046B0F0.unk04 = 1;
            HSD_VISetXFBDrawDone();
            return;
        }
        for (u32 i = 0; i < pad_queue_count; ++i) {
            //HSD_PerfSetStartTime();
            HSD_PadRenewMasterStatus(); //Normally Pad_Renew() - Don't see much reason to use a wrapper function when not debugging
            /*if(debug_level >= 3){
                DevelopMode_CPUStats(&match_controller.unk14);
            }*/

            if ((match_controller.pause & 1) == 0 && (match_controller.frozen & 1) != 0) {
                match_controller.flags &= 0x7F;
            } else {
                match_controller.flags = (match_controller.flags & 0x7F) | 0x80;
            }
            if (match_controller.flags >> 7 != 0) {
                if (sub_80019A30(0) != 0) {
                    Pad_CheckInputs();
                    if (sub_80019A30(0) != 0 && onframefunc != NULL) {
                        (*onframefunc)();
                    }
                }
            }

            s64 res = 0;
            if (match_controller.frozen != match_controller.unk11 || match_controller.pause != match_controller.unk13) {
                res = sub_801A48A4(match_controller.frozen);
                if (match_controller.flags >> 7 == 0) {
                    res = -1;
                }
                match_controller.unk11 = match_controller.frozen;
                match_controller.unk13 = match_controller.pause;
                match_controller.pause = 0;
            }
            match_controller.unk20 = res;
            match_controller.unk24 = res;

            if (sub_80019A30(0) == 0) {
                match_controller.unk24 = match_controller.unk24 | match_controller.unk28;
            }

            if (sub_80019A30(1) == 0) {
                match_controller.unk24 = match_controller.unk24 | match_controller.unk28;
            }
            /*if(debug_level >= 3){
                DevelopMode_USBScreenshot();
            }*/

            SFX_ProcessVolume();
            if (match_controller.unk2C != NULL) {
                (*match_controller.unk2C)();
            }
            GObj_RunProcs();
            if (match_controller.timer != -2) {
                match_controller.timer += 1;
            }

            if (match_controller.flags >> 7 != 0) {
                if (sub_80019A30(0) != 0 && match_controller.timer2 != -2) {
                    match_controller.timer2 += 1;
                }
            }

            /*HSD_PerfSetCPUTime();
            if(debug_level >= 3){
                OSCheckActiveThreads();
            }*/
            dword_8046B0F0.unk0C = 0;
            if (match_controller.screen_ctrl != 0) {
                break;
            }
        }
        if (match_controller.screen_ctrl == 2) {
            HSD_VISetXFBDrawDone();
            return;
        }
        DVD_CheckDisk();
        HSD_StartRender(HSD_RP_SCREEN);
        GObj_RunGXLinkMaxCallbacks();
        //HSD_IsScreenRenderPass(); - This function just stores the result of rp == 0 in the cmp register
        //HSD_PerfSetDrawTime();
        HSD_VICopyXFBASync(HSD_RP_SCREEN);
        if (match_controller.unk04 != -2) {
            match_controller.unk04 += 1;
        }
        //DevelopMode_CheckForUSBScreenshot();
        //HSD_PerfSetTotalTime();
        //HSD_PerfInitStat();
    } while (true);
}

//801A50A0
MinorSceneHandler* Scene_GetClassHandler()
{
    return scene_handlers;
}

MajorScene* Scene_GetMajorScenes()
{
    return major_scenes;
}

//801AD088
static void Scene_Minor_Class40_LoadDAT()
{
    /*void* sections[2];
    FUN_80016dbc("NtProge.dat", 4, sections[0], "", sections[1], "");
    HSD_GObj* gobj = GObj_Create(GOBJ_CLASS_HSD_COBJ_TITLE, 0x14, 0);
    HSD_CObj* cobj = HSD_CObjLoadDesc(local_14[0][1]->class_name);
    GObj_InitKindObj(gobj, GOBJ_KIND_MENU_COBJ, cobj);
    GObj_SetupGXLink_Max(gobj, GObj_SetCamera, 0);
    gobj->x24_unk = 0x4801;
    gobj->x20_unk = 0;
    DAT_80480d7c = Menu_CreateTextObj(0, (int*)0x0, 0xe, 0xf, 0, 0xe, 8, 0);
    FUN_803a62a0(0, "SdProge.usd", "SIS_ProgeData");
    iVar2 = FUN_803a5acc(0.f, 0.f, 0.f, 640.f, 480.f, 0, DAT_80480d7c);
    DAT_80480d78 = iVar2;
    *(undefined4*)(iVar2 + 0x24) = 1.0f;
    *(undefined4*)(iVar2 + 0x28) = 1.0f;
    HSD_GObj* gobj_2 = GObj_Create(0XE, 0xF, 0);
    HSD_JObj* jobj = HSD_JObjLoadJoint((HSD_JObjDesc*)((HSD_JObjDesc*)(*local_14[0])->class_name)->class_name);
    GObj_InitKindObj(gobj_2, GOBJ_KIND_JOBJ, jobj);
    GObj_SetupGXLink(gobj_2, JObj_SetupInstanceMtx_Callback, 0xB, 0);
    JObj_AddAnims(jobj, (*local_14[0])->class_name, 0);
    HSD_JObjReqAnimAll(jobj, );
    HSD_JObjAnimAll(jobj);
    DAT_80480d74 = jobj;*/
}

//801AD254
static void Scene_Minor_Class40_JObjUpdate(u32 sel)
{
    /*HSD_JObj* jobj;

    jobj = PTR_80480d74;
    switch (sel) {
    case 0:
        *(undefined*)(DAT_80480d78 + 0x4d) = 0;
        HSD_JObjClearFlagsAll(jobj, HIDDEN);
        FUN_803a6368(DAT_80480d78, 6);
        HSD_JObjReqAnimAll(jobj, 0.f);
        jobj->position.y = -15.f;
        if (((jobj->flags & MTX_INDEP_SRT) == 0) && jobj != NULL) {
            if (((jobj->flags & USER_DEF_MTX) == 0) && ((jobj->flags & 0x40) != 0)) {
                break;
            }
            HSD_JObjSetMtxDirtySub(jobj);
        }
        break;

    case 1:
        *(undefined*)(DAT_80480d78 + 0x4d) = 0;
        HSD_JObjClearFlagsAll(jobj, HIDDEN);
        FUN_803a6368(DAT_80480d78, 2);
        HSD_JObjReqAnimAll(jobj, 1.0f);
        jobj->position.y = 0.f;
        if ((jobj->flags & MTX_INDEP_SRT) == 0 && jobj != NULL) {
            if ((jobj->flags & USER_DEF_MTX) == 0 && ((jobj->flags & MTX_DIRTY) != 0)) {
                break;
            }
            HSD_JObjSetMtxDirtySub(jobj);
        }
        break;

    case 2:
        *(undefined*)(DAT_80480d78 + 0x4d) = 0;
        HSD_JObjClearFlagsAll(jobj, HIDDEN);
        FUN_803a6368(DAT_80480d78, 3);
        HSD_JObjReqAnimAll(jobj, 2.0f);
        jobj->position.y = 0.f;
        if ((jobj->flags & MTX_INDEP_SRT) == 0 && jobj != NULL) {
            if ((jobj->flags & USER_DEF_MTX) == 0 && (jobj->flags & MTX_DIRTY) != 0) {
                break;
            }
            HSD_JObjSetMtxDirtySub(jobj);
        }
        break;

    case 4:
        *(undefined*)(DAT_80480d78 + 0x4d) = 0;
        HSD_JObjClearFlagsAll(jobj, HIDDEN);
        FUN_803a6368(DAT_80480d78, 5);
        HSD_JObjReqAnimAll(jobj, 0.f);
        jobj->position.y = 0.f;
        if ((jobj->flags & MTX_INDEP_SRT) == 0 && jobj != NULL) {
            if (((jobj->flags & USER_DEF_MTX) == 0) && ((jobj->flags & MTX_DIRTY) != 0)) {
                break;
            }
            HSD_JObjSetMtxDirtySub(jobj);
        }
        break;

    case 5:
        *(undefined*)(DAT_80480d78 + 0x4d) = 0;
        HSD_JObjClearFlagsAll(jobj, HIDDEN);
        FUN_803a6368(DAT_80480d78, 4);
        HSD_JObjReqAnimAll(jobj,0.f);
        jobj->position.y = 0.f;
        if ((jobj->flags & MTX_INDEP_SRT) == 0 && jobj != NULL) {
            if (((jobj->flags & USER_DEF_MTX) == 0) && ((jobj->flags & MTX_DIRTY) != 0)) {
                break;
            }
            HSD_JObjSetMtxDirtySub(jobj);
        }
        break;
    }

    HSD_JObjAnimAll(jobj);*/
}

//801AD620
static void Scene_Minor_Class40_OnFrame()
{
}

//801AD874
static void Scene_Minor_Class40_OnLoad(void* unk_struct)
{
    Scene_Minor_Class40_LoadDAT();
    Scene_Minor_Class40_JObjUpdate(*(u32*)unk_struct);
    /* if(*unk_struct == 1){
        80480d84 = 0;
    }else{
        80480d84 = 2;
    }
    80480d80 = *unk_struct;
    80480d88 = 0;
    80480d8C = 0;
    80480d90 = 0;
    */
}

//801AD8EC
static void Scene_Minor_Class40_OnLeave()
{
}

//8022BFBC
u32 sub_8022BFBC(u32 result)
{
    if (result == 2)
        return 0x0ED241FF;
    if (result < 2) {
        if (result) {
            if (result >= 0)
                result = 0xFF5A41FF;
        } else {
            result = 0x5A73FFFF;
        }
    } else if (result == 4) {
        result = 0x9B41FFFF;
    } else if (result < 4) {
        result = 0xF0C85AFF;
    }
    return result;
}

//8022C010
s32 sub_8022C010(s32 result, s32 a2)
{
    if (!result)
        return a2;
    switch (result) {
    case 0:
        return result;
    case 1:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 33:
        result = 0;
        break;
    case 2:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
        result = 1;
        break;
    case 3:
        result = 2;
        break;
    case 4:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
        result = 3;
        break;
    case 5:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
    case 32:
        result = 4;
        break;
    }
    return result;
}

//8022ED6C
f32 Scene_ReqAnimAll(HSD_JObj* jobj, f32* frames)
{
    f32 anim_speed;
    f32 frame = JObj_GetFrame(jobj);

    if ((frame < frames[0]) || (frames[1] < frame)) {
        HSD_JObjReqAnimAll(jobj, frames[0]);
    }
    if (-0.1f == frames[2]) {
        frame = JObj_GetFrame(jobj);
        if (frame < frames[1]) {
            HSD_JObjAnimAll(jobj);
            frame = JObj_GetFrame(jobj);
            anim_speed = frames[1];
            if (anim_speed < frame) {
                HSD_JObjReqAnimAll(jobj, anim_speed);
                HSD_JObjAnimAll(jobj);
                frame = anim_speed;
            }
        }
    } else {
        HSD_JObjAnimAll(jobj);
        frame = JObj_GetFrame(jobj);
        if (frames[1] <= frame) {
            frame = (frames[2] + (frame - frames[1]));
            HSD_JObjReqAnimAll(jobj, frame);
            HSD_JObjAnimAll(jobj);
        }
    }
    return frame;
}
