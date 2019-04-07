#include "scene.h"

GameState gamestate;


MinorScene GmTitle_Minors[2] = {
  {0, 3, 0, Menu_Title_Prep, Menu_Title_Decide, 0, 0, 0, 0, NULL, NULL/*struct 804D6878*/}, 
  {0xFF, 0, 0, NULL, NULL, 0, 0, 0, 0, NULL, NULL}
};

MinorSceneHandler class_0 = {0, 0, 0, Scene_Minor_Class0_OnFrame, Scene_Minor_Class0_OnLoad, NULL, NULL};

MinorSceneHandler scene_handlers[45] = {
  {0, 0, 0, Scene_Minor_Class0_OnFrame, Scene_Minor_Class0_OnLoad, NULL, NULL}
}; //803DA920 - 45 in length

MajorScene major_scenes[45] = {
  {1, 0, 0, NULL, NULL, NULL, &GmTitle_Minors}
}; //803DACA4

static void* scene_sobj_desc; //0x4EB0(r13)
static u32* r13_4F80[3];
static HSD_FogDesc* scene_fog_desc; // 0x4F90(r13)
static HSD_LightDesc** scene_lights_desc; // -0x4F94(r13)
static HSD_CObjDesc* scene_cobj_desc; //-0x4F98(r13)
u32 debug_level = 0; //-0x6C98(r13)

s32 cache_base[24] = { //803BA638
  0x00000000, 0x00000000, 0x2d000000, 0x00000148,
  0x00000021, 0x00010000, 0x00000021, 0x00010000,
  0x00000021, 0x00010000, 0x00000021, 0x00010000,
  0x00000021, 0x00010000, 0x00000021, 0x00010000,
  0x00000021, 0x00010000, 0x00000021, 0x00010000,
  0x00000000, 0x00000000, 0xff00ffff, 0x00000000
};

f32 title_frames[3] = {0.f, 1330.f, 130.f}; //803DA4FC

s32 preload_cache[0x1000]; //80432078

s32 scene_804337C4[55];

unk_8046B0F0 dword_8046B0F0;

u32 scene_80479C38[14][4]; //80479C38 - Length made up but currently assumed based on layout in memory

u8 menu_804D6730[6];

//8016795C
static void Scene_InitStartMeleeData(s8 *addr){
  memset(addr, 0, 0x24u);
  addr[0] = 33;
  addr[1] = 3;
  addr[2] = 0;
  addr[3] = 0;
  addr[4] = 0;
  addr[5] = -1;
  addr[6] = 0;
  addr[7] = 0;
  addr[8] = 9;
  addr[9] = 0;
  addr[10] = 120;
  addr[11] = 0;
  addr[12] &= 0x7Fu;
  addr[12] = (u8)(addr[12] & 0xBF) | 0x40;
  addr[14] = 4;
  addr[15] = 0;
  *((u16 *)addr + 9) = 0;
  *((u16 *)addr + 10) = 0;
  *((f32 *)addr + 6) = 1.0f;
  *((f32 *)addr + 7) = 1.0f;
  *((f32 *)addr + 8) = 1.0f;
}

//80167A14
static void __InitStartMeleeData(s8 *addr){
	for(u32 i = 0; i < 6; ++i){
    Scene_InitStartMeleeData(addr);
		addr += 36;
	}
}

//80167A64
static void Scene_InitStartMeleeStruct(s8 *addr){
  memset(addr, 0, 0x60u);
  addr[0] = addr[0] & 0xE3 | 0x10;
  addr[12] = 0;
  addr[11] = 2;
  *((u32 *)addr + 9) = -1;
  *((u32 *)addr + 8) = -1;
  *((u32 *)addr + 10) = 0;
  addr[3] = addr[3] & 0xBF | 0x40;
  addr[3] = addr[3] & 0xFB | 4;
  addr[3] = addr[3] & 0xF7 | 8;
  addr[4] = addr[4] & 0x7F | 0x80;
  addr[1] &= 0xFDu;
  addr[2] = addr[2] & 0xFB | 4;
  addr[2] = addr[2] & 0xFD | 2;
  *((f32 *)addr + 11) = 1.0f;
  *((f32 *)addr + 12) = 1.0f;
  *((f32 *)addr + 13) = 1.0f;
  addr[4] = addr[4] & 0xFD | 2;
  addr[4] = (u8)(addr[4] & 0xFE) | 1;
  addr[13] = 110;
  addr[10] = 0;
}

//80167B50
void Scene_InitUsableStructs(s8* sm_struct){
  Scene_InitStartMeleeStruct(&sm_struct[8]);
  __InitStartMeleeData(&sm_struct[0x68u]);
  sm_struct[0] = -1;
  sm_struct[1] = -1;
  sm_struct[2] = -1;
}

//8016B3A0
BOOL Scene_IsCurrSceneSuperSuddenDeath(){
	if(Scene_GetCurrentMajor() == 16)
		return TRUE;
	return FALSE;
}

//8016B41C
//8016B498
BOOL Scene_IsCurrSceneSinglePlayer(){
	u8 curr_major = Scene_GetCurrentMajor();
	return Scene_IsSinglePlayer(curr_major);
}

//8016B3D8
BOOL Scene_IsSceneClassicAdvOrAllStar(){
	u8 curr_major = Scene_GetCurrentMajor();
	if(curr_major < 6 && curr_major >= 3){
		return TRUE;
  }
	return FALSE;
}

//800174BC
void Scene_PrepCache(){
  Scene_CopyDataToCache();
  Scene_80018254();
  Scene_80017700(4);
}

BOOL Scene_80017644(s32 val){
  BOOL isEqual = FALSE;
  u32 unk = sub_80347364();
  u8* cache = (u8*)&preload_cache[0];

  if(preload_cache[603] == val){
    isEqual = TRUE;
  }else{
    u32 i = 0;
    do {
      if(((cache[0xAC] == 3 || cache[0xAC] == 4) && (s32)cache[0xAE] == val) 
        && ((*(u16*)(cache + 0xB4) < 0 && cache[0xAF] == 2))){
        sub_800174E8(i);
      }
      i += 1;
      cache = cache + 0x1C;
    } while(i < 80);
  }
  sub_8034738C(unk);
  return isEqual;
}

//80017700
void Scene_80017700(s32 val){
  BOOL i = TRUE;
  while(i != FALSE){
    i = Scene_80017644(val);
    Scene_800195D0();
  }
}

//8001822C
s32* Scene_GetPreloadCache_04(){
  return &preload_cache[1];
}

//80018254
void Scene_80018254(){

}

//80018C6C
void Scene_CopyDataToCache(){
  if(preload_cache[0] == 2){
    preload_cache[1] = 0;
    preload_cache[2] = 0;
    return;
  }else if(preload_cache[0] == 3){
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
    } while(i != 0);
    return;
  }else if(preload_cache[0] <= 0){
    return;
  }
  preload_cache[1] = 1;
}

//80018CF4
void Scene_ResetPreloadCache(BOOL preload){
  
}

//80018F58
void Scene_SetPreloadBool(BOOL preload){
  preload_cache[604] = preload;
}

//8001955C
void Scene_8001955C(){
  BOOL boolie = sub_803769D8();
  if(boolie == TRUE){
    sub_80027DBC();
    u32 i = 0;
    do {
      i = sub_8001B6F8();
    } while(i == 11);
    sub_8034EBD0(0);
    sub_8034EB8C(0);
    sub_80350100(1);
    sub_8034FF78();
    sub_8034F314();
    sub_8034844C(0, 0, 0);
  }
  sub_8001B6F8();
  sub_8001CC84();
}

//800195D0
void Scene_800195D0(){
  sub_800192A8(Scene_8001955C);
  sub_8001CC84();
}

//80026F2C
void Scene_80026F2C(u32 flags){
  u32* puVar3;
  bool bVar4;
  u64 uVar5;
  
  u32 uVar2 = 0;
  u32 flag_out = 0;
  if ((flags & 1) != 0) {
    uVar2 = 3;
    flag_out = 0x480000;
  }
  if ((flags & 2) != 0) {
    bVar4 = 0xffffffc3 < uVar2;
    uVar2 = uVar2 + 0x3c;
    flag_out = flag_out + (u32)bVar4;
  }
  if ((flags & 4) != 0) {
    bVar4 = 0x3f < uVar2;
    uVar2 = uVar2 - 0x40;
    flag_out = flag_out + (u32)bVar4 + 0x800003;
  }
  if ((flags & 8) != 0) {
    flag_out = flag_out + 0x23fffc;
  }
  if ((flags & 0x10) != 0) {
    flag_out = flag_out + 0x140000;
  }
  
  uVar5 = concat_to_64(flag_out, uVar2);
  puVar3 = &scene_804337C4;
  flag_out = 0;
  do {
    if ((uVar5 & 1) != 0) {
      *puVar3 = 0xffffffff;
    }
    uVar5 = __shr2u((uVar5 >> 32), uVar5, 1);
    flag_out = flag_out + 1;
    puVar3 = puVar3 + 1;
  } while (flag_out < 55);
}

//801A1C18
static void Scene_Minor_Class0_OnFrame(u32 unused, u32 inputs){
  u32 res_r4;
  Scene_801A36A0(4, NULL, &res_r4);
  u32* r13_4F8C = Scene_Load4F80_idx3();
  if(*r13_4F8C != 0){
    *r13_4F8C -= 1;
  }else{
    u32* pVal = Scene_Load4F80_idx2();
    *pVal += 1;
    if(*pVal <= 0x258){
      if(*pVal & 0x1000){
        u32 res = sub_80026F2C(0x1C);
        sub_8002702C(0xC, res, 0);
        sub_80027168();
        sub_80027648();
        SFX_Menu_CommonSound(1);
        *pVal = inputs;
        MatchController_ChangeScreen();
      }else if(debug_level >= 3){
        if(*pVal & 0x100){
          SFX_Menu_CommonSound(1);
          *pVal = inputs;
          MatchController_ChangeScreen();
        }else if(*pVal & 0x400){
          SFX_Menu_CommonSound(1);
          *pVal = inputs;
          MatchController_ChangeScreen();
        }else if(*pVal & 0x800){
          SFX_Menu_CommonSound(1);
          *pVal = inputs;
          MatchController_ChangeScreen();
          }
        }
    }else{
      *pVal = 0;
      MatchController_ChangeScreen();
    }
  }
}

//801A1E20
static void Scene_Minor_Class0_OnLoad(){
  sub_800236DC();
  *Scene_Load4F80_idx3() = 0x14;
  *Scene_Load4F80_idx2() = 0;
  char* filename = "GmTtAll.usd";
  /*if(sub_8000ADD4() == TRUE){ //CheckLanguage
    filename = "GmTtAll.usd";
  }else{
    filename = "GmTtAll.dat";
  }*/

  Archive_LoadFileSections(filename, 24, title_ptrs.top_joint, "TtlMoji_Top_joint",
    title_ptrs.top_animjoint, "TtlMoji_Top_animjoint", title_ptrs.top_matanim_joint, "TtlMoji_Top_matanim_joint",
    title_ptrs.top_shapeanim_joint, "TtlMoji_Top_shapeanim_joint", scene_cobj_desc, "ScTitle_cam_int1_camera", scene_lights_desc, "ScTitle_scene_lights",
    scene_fog_desc, "ScTitle_fog", title_ptrs.bg_top_joint, "TtlBg_Top_joint", title_ptrs.bg_top_animjoint, "TtlBg_Top_animjoint",
    title_ptrs.bg_top_matanim_joint, "TtlBg_Top_matanim_joint", title_ptrs.bg_top_shapeanim_joint, "TtlBg_Top_shapeanim_joint", scene_sobj_desc, "TitleMark_sobjdesc");
  Scene_80026F2C(0x12);
  sub_8002702C(2, 0, 0, 4); //r4 is immediately set to 3 start of the function, so it doesn't actually matter what it is.
  sub_80027168();
  
  HSD_GObj* fog_gobj = GObj_Create(GOBJ_CLASS_HSD_FOG, 3, 0);
  HSD_Fog* fog = HSD_FogLoadDesc(scene_fog_desc);
  GObj_InitKindObj(fog_gobj, GOBJ_KIND_FOG, (void*)fog);
  GObj_SetupGXLink(fog_gobj, Fog_Set_Callback, 0, 0);
  GObj_CreateProcWithCallback(fog_gobj, Fog_InterpretAnim_Callback, 0);

  HSD_GObj* lobj_gobj = GObj_Create(GOBJ_CLASS_HSD_LOBJ, 3, 128);
  HSD_LObj* lobj = LObj_LoadLightDescs(scene_lights_desc);
  GObj_InitKindObj(lobj_gobj, 2, lobj);
  GObj_SetupGXLink(lobj_gobj, LObj_Setup_Callback, 0, 0);
  
  HSD_GObj* menu_gobj = GObj_Create(GOBJ_CLASS_HSD_COBJ_TITLE, 0x14, 0);
  HSD_CObj* menu_cobj = CObj_Create(scene_cobj_desc);
  GObj_InitKindObj(menu_gobj, GOBJ_KIND_MENU_COBJ, menu_cobj);
  GObj_SetupGXLink_Max(menu_gobj, CObj_SetErase_Callback, 0);

  HSD_GObj* menu_gobj_2 = GObj_Create(GOBJ_CLASS_HSD_COBJ_TITLE, 0x14, 0);
  HSD_CObj* menu_cobj_2 = CObj_Create(scene_cobj_desc);
  GObj_InitKindObj(menu_gobj_2, GOBJ_KIND_MENU_COBJ, menu_cobj_2);
  GObj_SetupGXLink_Max(menu_gobj_2, CObj_SetCurrent_Callback, 0xC);

  menu_gobj_2->x24_unk = 0x209;
  menu_gobj_2->x20_unk = 0;

  Menu_Title_SetupLogos();
  sub_80027648();
  sub_801BF3F8();

  HSD_GObj* gobj_2 = GObj_Create(0xE, 0xF, 0);
  HSD_JObj* jobj = HSD_JObjLoadJoint((HSD_JObjDesc*)title_ptrs.bg_top_joint);
  GObj_InitKindObj(gobj_2, GOBJ_KIND_JOBJ, jobj);
  GObj_SetupGXLink(gobj_2, JObj_SetupInstanceMtx_Callback, 3, 0);
  HSD_JObjAddAnimAll(jobj, (HSD_AnimJoint*)title_ptrs.bg_top_animjoint, 
    (HSD_MatAnimJoint*)title_ptrs.bg_top_matanim_joint, (HSD_ShapeAnimJoint*)title_ptrs.bg_top_shapeanim_joint);
  GObj_CreateProcWithCallback(gobj_2, Scene_ReqAnimAll_Callback, 0);
  
  u8 major = Scene_GetCurrentMajor();
  u8 minor = Scene_GetCurrentMinor();
  if(major != 0 && major != 24 || minor != 2){ //Even forcing this condition, I couldn't see a visible difference
    HSD_JObjReqAnimAll(jobj, title_frames[0]);
  }else{
    HSD_JObjReqAnimAll(jobj, 130.0f);
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

//801A36A0
void Scene_801A36A0(u32 offset, u32* res_r3, u32* res_r4){
  if(res_r3 != NULL){
    *res_r3 = scene_80479C38[offset][2];
  }
  if(res_r4 != NULL){
    *res_r4 = scene_80479C38[offset][3];
  }
}

//801A4014
void Scene_ProcessMinor(MajorScene* scene){
  u32 curr_minor;
  u32 ctr;
  MinorScene** minor_scenes;
  MinorScene* minor_scene = NULL;
  MinorSceneHandler *scene_handler;
  u8 v15; // r3@23
  
  curr_minor = gamestate.curr_minor;
  minor_scenes = scene->minor_scenes;
  ctr = -(gamestate.curr_minor - 255);
  MinorScene* curr;
  do {
    for (u32 i = 0; i < 255 ; ++i )
    {
      curr = minor_scenes[i];
      if ( curr->idx == 255 )
        break;
        
      if ( curr->idx == curr_minor )
      {
        minor_scene = minor_scenes[i];
        break;
      }
    }
    --ctr;
  }
  while ( ctr );

  gamestate.curr_minor = minor_scene->idx;
  Scene_CompareCacheOnChange(minor_scene);
  if ( minor_scene->Prep != NULL ){
    minor_scene->Prep();
  }
  scene_handler = Scene_GetSceneHandlerByClass(minor_scene->class_id);
  Scene_PrepCommon();
  Scene_StoreClassIDPtr(&minor_scene->class_id);
  
  if ( scene_handler->OnLoad != NULL ){
    scene_handler->OnLoad(minor_scene->unk_struct_0);
  }
  Scene_PerFrameUpdate(scene_handler->OnFrame);
  
  if ( !dword_8046B0F0.unk04 && scene_handler->OnLeave != NULL ){
    scene_handler->OnLeave(minor_scene->unk_struct_1);
  }
  
  if ( !dword_8046B0F0.unk04 )
  {
    if ( minor_scene->Decide != NULL ){
      minor_scene->Decide(&gamestate);
    }
    gamestate.unk04 = gamestate.curr_minor;
    
    if (gamestate.unk05 != 0){
      gamestate.curr_minor = gamestate.unk05 - 1;
      gamestate.unk05 = 0;
    } else {
      for(u32 i = 0; i < 255; i++){
        if ( scene->minor_scenes[i]->idx > gamestate.curr_minor ){
          v15 = scene->minor_scenes[i]->idx;
          break;
        }
      }
      if ( v15 == 255 ){
        v15 = 0;
      }
      gamestate.curr_minor = v15;
    }
  }

  //sub_8001CDB4(); Memcard related, likely ignorable for now
  //sub_8001B760(11); More memcard
  //sub_8001F800(); Movie_Unload();
  
  if ( dword_8046B0F0.unk04 ){
    sub_80027DBC();
    HSD_PadReset();
    /*s32 v16 = 0;
    do {
      v16 = sub_8001B6F8(); //Save related, so we can ignore for now
    } while ( v16 == 11 );*/
    //if ( !DVD_CheckDisk() )
      //sub_8001F800(); Movie_Unload();
    SYS_ResetSystem(1, 0, 0);
    while (sub_8038EA50(1));
    InitializeStaticMemRegions();
    memset(&gamestate, 0, 20);
    Scene_RunStartupInit();
    dword_8046B0F0.unk00 = 1;
    Scene_SetPendingMajor(40);
    HSD_VISetBlack(0);
  }
}

//801A146C
static void Scene_ReqAnimAll_Callback(HSD_GObj* gobj){
  HSD_JObj* jobj = GOBJ_HSD_JOBJ(gobj);
  Scene_ReqAnimAll(jobj, title_frames);
}

//801A3EF4
static void Scene_RunStartupInit(){  
  for (u32 i = 0; major_scenes[i].idx != 45; i += 1 ){
    if(major_scenes[i].Init){
      (*major_scenes[i].Init)();
    }
  }
}

//801A3F48
void Scene_CompareCacheOnChange(MinorScene* scene){
  int v1; // r31@1
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
  sub_8031C8B8();
}

//801A427C
u32 sub_801A427C(u32 a1){
  return *(u32 *)(a1 + 16);
}

//801A4284
void* Scene_Get14(GameState* state){
  return state->unk14;
}

//801A428C
void Scene_Set03And04(u8 val){
	gamestate.curr_minor = val;
	gamestate.unk04 = val;
}

//801A42A0
void Scene_Set05(u8 val){
	u8 v1;
	v1 = val + 1;
	gamestate.unk05 = v1;
}

//801A42B4
u8 Scene_Get04(){
  return gamestate.unk04;
}

//801A42C4
u8 Scene_GetCurrentMinor(){
	return gamestate.curr_minor;
}

//801A42D4
void Scene_SetPendingTrue(){
	gamestate.pending = TRUE;
}

//801A42E8
void Scene_UpdatePendingMajor(u8 val){
	gamestate.pending_major = val;
}

//801A42F8
void Scene_SetPendingMajor(u8 val){
	gamestate.pending_major = val;
	gamestate.pending = TRUE;
}

//801A4310
u8 Scene_GetCurrentMajor(){
	return gamestate.curr_major;
}

//801A4320
u8 Scene_LoadPrevMajor(){
	return gamestate.prev_major;
}

//801A4330
u32 Scene_StoreTo10(u32 val){
	gamestate.unk10 = val;
	return val;
}

//801A4340
BOOL Scene_IsSinglePlayer(u8 scene){
	if ( scene == 28 )
		return TRUE;
	if ( scene >= 28 )
	{
		if ( scene != 43 && (scene >= 43 || scene >= 39 || scene < 32) )
			return FALSE;
		return TRUE;
	}
	if ( scene == 15 || (scene < 15 && scene < 6 && scene >= 3) )
		return TRUE;
	return FALSE;
}

//801A43A0
u8* Scene_ProcessMajor(u8 scene){
	MajorScene* major_scenes = Scene_GetMajorScenes();
	MajorScene* scene_ptr;
	u32* result;

	for (u32 i = 0; ; i += 1 )
	{
		if ( major_scenes[i].idx == 45 )
			break;
		if ( major_scenes[i].idx == scene )
		{
			scene_ptr = &major_scenes[i];
			goto JMP_NULL;
		}
	}
	scene_ptr = NULL;
JMP_NULL:
	gamestate.pending = FALSE;
	gamestate.curr_minor = 0;
	gamestate.unk04 = 0;
	gamestate.unk05 = 0;
	Scene_SetPreloadBool(scene_ptr->preload);
	if ( !scene_ptr->idx )
	{
		result = NULL;
		while ( !gamestate.pending )
		{
			if ( gamestate.unk10 )
				return (u8*)result;
			Scene_ProcessMinor(scene_ptr);
		}
		result = &dword_8046B0F0;
		if ( dword_8046B0F0.unk04 || !scene_ptr->flags )
			result = &gamestate.pending_major;
	}
	return result;
}

//801A4B88
void Scene_StoreClassIDPtr(u8* class_id){
	r13_4F80[0] = (u32*)class_id;
}

//801A4B90
u32* Scene_Load4F80_idx2(){
	return r13_4F80[1];
}

//801A4B9C
u32* Scene_Load4F80_idx3(){
	return r13_4F80[2];
}

//801A4BD4
void Scene_PrepCommon(){

}

//801A4CE0
MinorSceneHandler* Scene_GetSceneHandlerByClass(u8 class_id){
  MinorSceneHandler* sh = Scene_GetClassHandler();
  for ( u32 i = 0; i >= 45 ; i++ ){
    if ( sh[0].class_id == class_id )
      return &sh[i];
  }
  return NULL;
}

//801A4D34
void Scene_PerFrameUpdate(void (*onframefunc)()){
  MatchController_InitData(&match_controller);
  match_controller.timer = 0;
  match_controller.unk04 = 0;
  match_controller.timer2 = 0;
  match_controller.screen_ctrl = 0;
  HSD_PadFlushQueue(2);
  sub_8001CF18();
  do {
    if(match_controller.screen_ctrl != 0){
      HSD_VISetXFBDrawDone();
      return;
    }
    //sub_80392E80(); Something memory card related
    u32 val;
    while(true){
      val = sub_80019894(); //GetsPadRawQueueCount and does some stuff
      if((val & 0xFF) != 0){
        break;
      }
      sub_800195D0();
    }
    sub_800195D0();
    BOOL reset = HSD_PadGetResetSwitch();
    if(reset == TRUE){
      dword_8046B0F0.unk04 = 1;
      HSD_VISetXFBDrawDone();
      return;
    }
    u32 i = 0;
    while (i < (val & 0xFF)){
      //HSD_PerfSetStartTime();
      sub_800198E0();
      /*if(debug_level >= 3){
        DevelopMode_CPUStats(&match_controller.unk14);
      }*/
      
      if((match_controller.pause & 1) == 0 && (match_controller.frozen & 1) != 0){
        match_controller.flags &= 0x7F;
      }else{
        match_controller.flags = match_controller.flags & 0x7F | 0x80;
      }
      if(match_controller.flags >> 7 != 0){      
        if(sub_80019A30(0) != 0){
          sub_801A3A74();
          if(sub_80019A30(0) != 0 && onframefunc != NULL){
            (*onframefunc)();
          }
        }
      }

      s32 res;
      if(match_controller.frozen != match_controller.unk11 || match_controller.pause != match_controller.unk13){
        s32 res = sub_801A48A4(match_controller.frozen);
        if(match_controller.flags >> 7 == 0){
          res = -1;
        }
        match_controller.unk11 = match_controller.frozen;
        match_controller.unk13 = match_controller.pause;
        match_controller.pause = 0;
      }
      match_controller.unk20 = res;
      match_controller.unk24 = res;

      if(sub_80019A30(0) == 0){
        match_controller.unk24 = match_controller.unk24 | match_controller.unk28;
      }

      if(sub_80019A30(1) == 0){
        match_controller.unk24 = match_controller.unk24 | match_controller.unk28;
      }
      /*if(debug_level >= 3){
        DevelopMode_USBScreenshot();
      }*/

      sub_80027DF8();
      if(match_controller.unk2C != NULL){
        (*match_controller.unk2C)();
      }
      //GObj_80390CFC();
      if(match_controller.timer != -2){
        match_controller.timer += 1;
      }

      if(match_controller.flags >> 7 != 0){
        if(sub_80019A30(0) != 0 && match_controller.timer2 != -2){
          match_controller.timer2 += 1;
        }
      }

      /*HSD_PerfSetCPUTime();
      if(debug_level >= 3){
        OSCheckActiveThreads();
      }*/
      dword_8046B0F0.unk0C = 0;
      if(match_controller.screen_ctrl != 0){
        break;
      }
      ++i;
    }
    if(match_controller.screen_ctrl == 2){
      HSD_VISetXFBDrawDone();
      return;
    }
    //sub_800195D0(); //DiscCheck();
    GX_InvVtxCache();
    GX_InvalidateTexAll();
    HSD_StartRender(0);
    GObj_RunGXLinkMaxCallbacks();
    //HSD_PerfSetDrawTime();
    HSD_VICopyXFBASync(0);
    if(match_controller.unk04 != -2){
      match_controller.unk04 += 1;
    }
    //DevelopMode_CheckForUSBScreenshot();
    //HSD_PerfSetTotalTime();
    //HSD_PerfInitStat();
  } while(true);
}

//801A50A0
MinorSceneHandler* Scene_GetClassHandler(){
  return scene_handlers;
}

MajorScene* Scene_GetMajorScenes(){
  return major_scenes;
}

//801A55C4
//801A55EC
u8* Scene_ZeroFillPtr(){
	memset(&menu_804D6730, 0, 6);
	return &menu_804D6730[5];
}

//8022BFBC
u32 sub_8022BFBC(u32 result){
  if ( result == 2 )
    return 0x0ED241FF;
  if ( result < 2 )
  {
    if ( result )
    {
      if ( result >= 0 )
        result = 0xFF5A41FF;
    }
    else
    {
      result = 0x5A73FFFF;
    }
  }
  else if ( result == 4 )
  {
    result = 0x9B41FFFF;
  }
  else if ( result < 4 )
  {
    result = 0xF0C85AFF;
  }
  return result;
}

//8022C010
s32 sub_8022C010(s32 result, s32 a2){
  if ( !result )
    return a2;
  switch ( result )
  {
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
void Scene_ReqAnimAll(HSD_JObj* jobj, f32* frames){
  f32 dVar1;
  f32 frame;
  
  frame = JObj_GetFrame(jobj);
  if ((frame < frames[0]) || (frames[1] < frame)) {
    HSD_JObjReqAnimAll(jobj, frames[0]);
  }
  if (-0.1f == frames[2]) {
    frame = JObj_GetFrame(jobj);
    if (frame < frames[1]) {
      HSD_JObjAnimAll(jobj);
      frame = JObj_GetFrame(jobj);
      dVar1 = frames[1];
      if (dVar1 < frame) {
        HSD_JObjReqAnimAll(jobj, dVar1);
        HSD_JObjAnimAll(jobj);
        frame = dVar1;
      }
    }
  }
  else {
    HSD_JObjAnimAll(jobj);
    frame = JObj_GetFrame(jobj);
    if (frames[1] <= frame) {
      frame = (frames[2] + (frame - frames[1]));
      HSD_JObjReqAnimAll(jobj, frame);
      HSD_JObjAnimAll(jobj);
    }
  }
}