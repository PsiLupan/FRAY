#include "scGmTitle.h"
#include "scene.h"
#include "gobj.h"
#include "hsd_jobj_ext.h"

GmTitle_File title_ptrs; //80479B28
f32 title_anim_speed = 0.0f; //803DA4F0

//801A1498
static void Menu_Title_801A1498(HSD_GObj* gobj){

}


//801A1630
static void Menu_Title_JObjAnimCallback(HSD_GObj* gobj){
    sub_8022ED6C(gobj->data, &(title_anim_speed));
}

//801A165C
void Menu_Title_SetupLogos(){
	HSD_GObj* gobj = GObj_Create(0xE, 0xF, 0);
	HSD_JObj* jobj = HSD_JObjLoadJoint((HSD_JObjDesc*)title_ptrs.top_joint);
    GObj_InitKindObj(gobj, GOBJ_KIND_JOBJ, jobj);
    GObj_SetupGXLink(gobj, JObj_SetupInstanceMtx_Callback, 9, 0);
    HSD_JObjAddAnimAll(jobj, (HSD_AnimJoint*)title_ptrs.top_animjoint, 
        (HSD_MatAnimJoint*)title_ptrs.top_matanim_joint, (HSD_ShapeAnimJoint*)title_ptrs.top_shapeanim_joint);
    u8 major = Scene_GetCurrentMajor();
    u8 minor = Scene_GetCurrentMinor();
    BOOL notScene = FALSE;
    if((major & 0xFF) == 0 || (major & 0xFF) == 0x18 || (minor & 0xFF) == 2){
        notScene = FALSE;
    }else{
        notScene = TRUE;
    }
    if(notScene == TRUE){
        /*r13 - 0x4F84 = 0;*/
        HSD_JObjReqAnimAll(jobj, 0.f);
        GObj_CreateProcWithCallback(gobj, Menu_Title_801A1498, 0);
    }else{
        HSD_JObjReqAnimAll(jobj, 400.f);
        GObj_CreateProcWithCallback(gobj, Menu_Title_JObjAnimCallback, 0);
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
    if(showAddCopyright == FALSE){
        //TObj_ToggleActiveTexture(jobj, title_ptrs.bg_top_animjoint, 7, -1); //80011E24
        HSD_JObjSetFlagsAll(jobj, 0x10); //JOBJ_HIDDEN
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
void Menu_Title_Prep(){
    Scene_PrepCache();
}

//801B089C
void Menu_Title_Decide(void* state){
    u32* scene_ptr = (u32*)Scene_Get14((GameState*)state); //scene_ptr = 804D6878 from the Minors, so something there
    u32 inputs = scene_ptr[0];
    if(debug_level < 3){
        if((inputs & 0x1000) == 0){
            sub_801BF708(1);
            Scene_UpdatePendingMajor(0x18);
        }else{
            sub_80173EEC();
            sub_80172898(0x100);
            u32 res = sub_80173754(1, 0);
            if(res == 0){
                Scene_UpdatePendingMajor(0x1);
            }
        }
    }else{
        if((inputs & 0x100) == 0){
            if((inputs & 0x1000) == 0){
                if((inputs & 0x400) == 0){
                    if((inputs & 0x800) == 0){
                        sub_801BF708(1);
                        Scene_UpdatePendingMajor(0x18);
                    }else{
                        Scene_UpdatePendingMajor(0x6);
                    }
                }else{
                    Scene_UpdatePendingMajor(0x7);
                }
            }else{
                sub_80173EEC();
                sub_80172898(0x100);
                u32 res = sub_80173754(1, 0);
                if(res == 0){
                    Scene_UpdatePendingMajor(0x1);
                }
            }
        }else{
            Scene_UpdatePendingMajor(0xE);
        }
    }
    Scene_SetPendingTrue();
}