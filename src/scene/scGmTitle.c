#include "scGmTitle.h"
#include "hsd_jobj_ext.h"

GmTitle_File title_ptrs; //80479B28
u32 title_inputs; //804D6878
f32 title_anim_speed[3] = { 0.0f, 1600.f, 400.f }; //803DA4F0
f32 title_frames[3] = { 0.f, 1330.f, 130.f }; //803DA4FC

static void* scene_sobj_desc; //0x4EB0(r13)

static HSD_FogDesc* scene_fog_desc; //0x4F90(r13)
static HSD_Lights* scene_lights_desc; //0x4F94(r13)
static HSD_CObjDesc* scene_cobj_desc; //0x4F98(r13)

//801A146C
void Menu_Title_ReqAnimAll_Callback(HSD_GObj* gobj)
{
    HSD_JObj* jobj = GOBJ_HSD_JOBJ(gobj);
    Scene_ReqAnimAll(jobj, title_frames);
}

//801A1498
static void Menu_Title_801A1498(HSD_GObj* gobj)
{
}

//801A1630
static void Menu_Title_JObjAnimCallback(HSD_GObj* gobj)
{
    JObj_SetAnimSpeed(GOBJ_HSD_JOBJ(gobj), title_anim_speed);
}

//801A165C
void Menu_Title_SetupLogos()
{
    HSD_GObj* gobj = GObj_Create(0xE, 0xF, 0);
    HSD_JObj* jobj = HSD_JObjLoadJoint((HSD_JObjDesc*)title_ptrs.top_joint);
    GObj_InitKindObj(gobj, GOBJ_KIND_JOBJ, jobj);
    GObj_SetupGXLink(gobj, JObj_SetupInstanceMtx_Callback, 9, 0);
    HSD_JObjAddAnimAll(jobj, (HSD_AnimJoint*)title_ptrs.top_animjoint,
        (HSD_MatAnimJoint*)title_ptrs.top_matanim_joint, (HSD_ShapeAnimJoint*)title_ptrs.top_shapeanim_joint);
    u8 major = Scene_GetCurrentMajor();
    u8 minor = Scene_GetCurrentMinor();

    if (major == 0 || (major == 24 && minor == 2)) {
        HSD_JObjReqAnimAll(jobj, 400.f);
        GObj_CreateProcWithCallback(gobj, Menu_Title_JObjAnimCallback, 0);
    } else {
        /*r13 - 0x4F84 = 0;*/
        HSD_JObjReqAnimAll(jobj, 0.f);
        GObj_CreateProcWithCallback(gobj, Menu_Title_801A1498, 0);
    }
    HSD_JObjAnimAll(jobj);
    //BOOL unlock = CheckCharUnlockStatus(EXTERNAL_MARTH);
    //BOOL unlock_2 = CheckCharUnlockStatus(EXTERNAL_ROY);
    BOOL showAddCopyright = TRUE;
    /*if(unlock == FALSE && unlock_2 == FALSE){
        showAddCopyright = FALSE;
    }else{
        showAddCopyright = TRUE;
    }*/
    if (showAddCopyright == FALSE) {
        //TObj_ToggleActiveTexture(jobj, title_ptrs.bg_top_animjoint, 7, -1); //80011E24
        HSD_JObjSetFlagsAll(jobj, HIDDEN);
    }
    /* u32 unk = sub_8000AFBC();
    sub_801692E8(unk, local); //GetTimeAndDate( );
    u32 iter = local_1a;
    while (iter != 0){
        HSD_Rand();
        iter -= 1;
    }
    sub_801BF128();
    */
}

//801A1C18
void Menu_Title_OnFrame(u32 unused, u32 inputs)
{
    u32 res_r4;
    Scene_801A36A0(4, NULL, &res_r4);
    u32* r13_4F8C = Scene_Load4F80_idx3();
    if (*r13_4F8C != 0) {
        *r13_4F8C -= 1;
    } else {
        u32* pVal = Scene_Load4F80_idx2();
        *pVal += 1;
        if (*pVal <= 0x258) {
            if (*pVal & 0x1000) {
                SFX_80026F2C(0x1C);
                SFX_RequestAudioLoad(0xC, 0, 0, 0);
                SFX_AudioCacheUpdate();
                SFX_80027648();
                SFX_Menu_Common(1);
                *pVal = inputs;
                MatchController_ChangeScreen();
            } else if (debug_level >= 3) {
                if (*pVal & 0x100) {
                    SFX_Menu_Common(1);
                    *pVal = inputs;
                    MatchController_ChangeScreen();
                } else if (*pVal & 0x400) {
                    SFX_Menu_Common(1);
                    *pVal = inputs;
                    MatchController_ChangeScreen();
                } else if (*pVal & 0x800) {
                    SFX_Menu_Common(1);
                    *pVal = inputs;
                    MatchController_ChangeScreen();
                }
            }
        } else {
            *pVal = 0;
            MatchController_ChangeScreen();
        }
    }
}

//801A1E20
void Menu_Title_OnLoad(void* unk_struct)
{
    SFX_StopMusic();
    *Scene_Load4F80_idx3() = 0x14;
    *Scene_Load4F80_idx2() = 0;
    char* filename = "GmTtAll.usd";
    /*if(sub_8000ADD4() == TRUE){ //CheckLanguage
        filename = "GmTtAll.usd";
    }else{
        filename = "GmTtAll.dat";
    }*/

    Archive_LoadFileSections(filename, 24, &title_ptrs.top_joint, "TtlMoji_Top_joint",
        &title_ptrs.top_animjoint, "TtlMoji_Top_animjoint", &title_ptrs.top_matanim_joint, "TtlMoji_Top_matanim_joint",
        &title_ptrs.top_shapeanim_joint, "TtlMoji_Top_shapeanim_joint", &scene_cobj_desc, "ScTitle_cam_int1_camera", &scene_lights_desc, "ScTitle_scene_lights",
        &scene_fog_desc, "ScTitle_fog", &title_ptrs.bg_top_joint, "TtlBg_Top_joint", &title_ptrs.bg_top_animjoint, "TtlBg_Top_animjoint",
        &title_ptrs.bg_top_matanim_joint, "TtlBg_Top_matanim_joint", &title_ptrs.bg_top_shapeanim_joint, "TtlBg_Top_shapeanim_joint", &scene_sobj_desc, "TitleMark_sobjdesc");
    SFX_80026F2C(0x12);
    SFX_RequestAudioLoad(2, 0, 0, 4);
    SFX_AudioCacheUpdate();

    HSD_GObj* fog_gobj = GObj_Create(GOBJ_CLASS_HSD_FOG, 3, 0);
    HSD_Fog* fog = HSD_FogLoadDesc(scene_fog_desc);
    GObj_InitKindObj(fog_gobj, GOBJ_KIND_FOG, fog);
    GObj_SetupGXLink(fog_gobj, Fog_Set_Callback, 0, 0);
    GObj_CreateProcWithCallback(fog_gobj, Fog_InterpretAnim_Callback, 0);

    HSD_GObj* lobj_gobj = GObj_Create(GOBJ_CLASS_HSD_LOBJ, 3, 128);
    HSD_LObj* lobj = LObj_LoadLightDescs(scene_lights_desc);
    GObj_InitKindObj(lobj_gobj, 2, lobj);
    GObj_SetupGXLink(lobj_gobj, LObj_Setup_Callback, 0, 0);

    HSD_GObj* menu_gobj = GObj_Create(GOBJ_CLASS_HSD_COBJ_TITLE, 0x14, 0);
    HSD_CObj* menu_cobj = CObj_Create(scene_cobj_desc);
    GObj_InitKindObj(menu_gobj, GOBJ_KIND_MENU_COBJ, menu_cobj);
    GObj_SetupCameraGXLink(menu_gobj, CObj_SetErase_Callback, 0);

    HSD_GObj* menu_gobj_2 = GObj_Create(GOBJ_CLASS_HSD_COBJ_TITLE, 0x14, 0);
    HSD_CObj* menu_cobj_2 = CObj_Create(scene_cobj_desc);
    GObj_InitKindObj(menu_gobj_2, GOBJ_KIND_MENU_COBJ, menu_cobj_2);
    GObj_SetupCameraGXLink(menu_gobj_2, CObj_Texture_Callback, 0xC);

    menu_gobj_2->gxlink_prios = 0x209; //Priority 0, 3, 9

    Menu_Title_SetupLogos();
    SFX_80027648();
    Menu_Title_LoadDemo();

    HSD_GObj* gobj_2 = GObj_Create(0xE, 0xF, 0);
    HSD_JObj* jobj = HSD_JObjLoadJoint((HSD_JObjDesc*)title_ptrs.bg_top_joint);
    GObj_InitKindObj(gobj_2, GOBJ_KIND_JOBJ, jobj);
    GObj_SetupGXLink(gobj_2, JObj_SetupInstanceMtx_Callback, 3, 0);
    HSD_JObjAddAnimAll(jobj, (HSD_AnimJoint*)title_ptrs.bg_top_animjoint,
        (HSD_MatAnimJoint*)title_ptrs.bg_top_matanim_joint, (HSD_ShapeAnimJoint*)title_ptrs.bg_top_shapeanim_joint);
    GObj_CreateProcWithCallback(gobj_2, Menu_Title_ReqAnimAll_Callback, 0);

    u8 major = Scene_GetCurrentMajor();
    u8 minor = Scene_GetCurrentMinor();
    if (major == 0 || (major == 24 && minor == 2)) { //Even forcing this condition, I couldn't see a visible difference
        HSD_JObjReqAnimAll(jobj, 130.0f);
    } else {
        HSD_JObjReqAnimAll(jobj, title_frames[0]);
    }
    HSD_JObjAnimAll(jobj);

    /*if(debug_level >= 1){
    Menu_CreateTextObj(0, NULL, GOBJ_CLASS_TEXT, 13, 0, 14, 0, 19);
    u8* unk_struct = sub_803A6754(0, 0);
    sub_801A1D38("DATE Feb 13 2002  TIME 22:06:27", title_ptrs.debug_text);
    void* unk = sub_803A6B98(unk_struct, "%s", 30.0f, 30.0f); //MenuTextDrawSome
    unk_struct[0x49] = 1;
    sub_803A7548(unk, unk_struct, 0.7f, 0.55f);
  }*/
}

//801B087C
void Menu_Title_Prep(GameState* state)
{
    Scene_PrepCache();
}

//801B089C
void Menu_Title_Decide(GameState* state)
{
    u32* status = (u32*)Scene_GetData2(state); //804D6878 from the Minors
    if(status == NULL){
        return;
    }
    u32 inputs = *status;
    if (debug_level < 3) {
        if ((inputs & PAD_BUTTON_START) == 0) {
            //sub_801BF708(1);
            Scene_UpdatePendingMajor(0x18);
        } else {
            //sub_80173EEC();
            //sub_80172898(0x100);
            u32 res = 1; //sub_80173754(1, 0);
            if (res == 0) {
                Scene_UpdatePendingMajor(0x1);
            }
        }
    } else {
        if ((inputs & PAD_BUTTON_A) == 0) {
            if ((inputs & PAD_BUTTON_START) == 0) {
                if ((inputs & PAD_BUTTON_X) == 0) {
                    if ((inputs & PAD_BUTTON_Y) == 0) {
                        //sub_801BF708(1);
                        Scene_UpdatePendingMajor(0x18);
                    } else {
                        Scene_UpdatePendingMajor(0x6);
                    }
                } else {
                    Scene_UpdatePendingMajor(0x7);
                }
            } else {
                //sub_80173EEC();
                //sub_80172898(0x100);
                u32 res = 1; //sub_80173754(1, 0);
                if (res == 0) {
                    Scene_UpdatePendingMajor(0x1);
                }
            }
        } else {
            Scene_UpdatePendingMajor(0xE);
        }
    }
    Scene_SetPendingTrue();
}

//801BF3F8
void Menu_Title_LoadDemo()
{
    /*s32* cache_ptr = Scene_GetPreloadCache_04();
    Scene_CopyDataToCache();
    s32* ptr = cache_ptr;
    for(u32 i = 0; i < 4; ++i, ptr += 2){

    }*/
}
