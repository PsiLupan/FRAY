#include "scGmTitle.h"

MinorScene GmTitle_Minors[2] = {
    {0, 3, 0, Menu_Title_Prep, Menu_Title_Decide, 0, 0, 0, 0, NULL, NULL/*struct 804D6878*/}, 
    {0xFF, 0, 0, NULL, NULL, 0, 0, 0, 0, NULL, NULL}
    };

GmTitle_File title_ptrs; //80479B28

//801A165C
void Menu_Title_SetupLogos(){
	HSD_GObj* gobj = GObj_Create(0xE, 0xF, 0);
	HSD_JObj* jobj = HSD_JObjLoadJoint((HSD_JObjDesc*)title_ptrs.top_joint);
    GObj_InitKindObj(gobj, 3, jobj);
    GObj_SetupGXLink(gobj, , 9, 0);
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
        GObj_CreateProcWithCallback(gobj, /*801A1498*/, 0);
    }else{
        HSD_JObjReqAnimAll(jobj, 400.f);
        GObj_CreateProcWithCallback(gobj, /*801A1630*/, 0);
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
        HSD_JObjSetFlagsAll(jobj, 0x10);
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
    Scene_800174BC();
}

//801B089C
void Menu_Title_Decide(void* unk_struct){

}