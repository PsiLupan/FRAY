#include "scGmTitle.h"
#include "gobj.h"
#include "hsd_jobj_ext.h"
#include "scene.h"

GmTitle_File title_ptrs; //80479B28
u32 title_inputs; //804D6878
f32 title_anim_speed[3] = { 0.0f, 1600.f, 400.f }; //803DA4F0
f32 title_frames[3] = { 0.f, 1330.f, 130.f }; //803DA4FC

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
    GObj_SetupGXLink(gobj, JObj_SetupInstanceMtx_Callback, 0/*9*/, 0);
    HSD_JObjAddAnimAll(jobj, (HSD_AnimJoint*)title_ptrs.top_animjoint,
        (HSD_MatAnimJoint*)title_ptrs.top_matanim_joint, (HSD_ShapeAnimJoint*)title_ptrs.top_shapeanim_joint);
    u8 major = Scene_GetCurrentMajor();
    u8 minor = Scene_GetCurrentMinor();

    if ((major & 0xFF) == 0 || (major & 0xFF) == 0x18 || (minor & 0xFF) == 2) {
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

//801B087C
void Menu_Title_Prep()
{
    Scene_PrepCache();
}

//801B089C
void Menu_Title_Decide(void* state)
{
    u32* status = Scene_GetPadStatus((GameState*)state); //804D6878 from the Minors
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
