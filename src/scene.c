#include "scene.h"
#include "unknown.h"

GameState gamestate;
SceneHandlers sceneHandlers;

SceneData sceneInfo[45]; //803DACA4

u32* unk4F80[3];

//801A4014
u32* Scene_RunFunc(SceneData* scene)
{
  u32 v2; // r4@1
  u8 *v3; // r5@1
  u32 v4; // ctr@1
  u8 *v5; // r6@2
  int i; // r3@2
  int v7; // r27@4
  int v8; // r0@6
  u32 *result; // r3@10
  u32 *v10; // r26@11
  u8 *v11; // r7@21
  int v12; // r4@21
  u8 *v13; // r6@21
  u8 *v14; // r3@21
  u8 v15; // r3@23
  signed int v16; // r3@31

  v2 = gamestate.unk03;
  v3 = scene->scene_subdata;
  v4 = 255 - gamestate.unk03;
  if ( gamestate.unk03 < 255 )
  {
    do
    {
      v5 = v3;
      for ( i = 0; ; ++i )
      {
        v8 = *v5;
        if ( v8 == 255 )
          break;
        if ( v2 == v8 )
        {
          v7 = (int)&v3[24 * i];
          goto LABEL_9;
        }
        v5 += 24;
      }
      ++v2;
      --v4;
    }
    while ( v4 );
  }
  v7 = 0;
LABEL_9:
  gamestate.unk03 = *(u8 *)v7;
  sub_801A3F48(v7);
  if ( *(u32 *)(v7 + 4) )
    return (u32 *)v7;
  v10 = (int *)sub_801A4CE0(*(u8 *)(v7 + 12));
  sub_801A4BD4();
  sub_801A4B88(v7 + 12);
  if ( v10[2] )
    return *(u32 **)(v7 + 16);
  sub_801A4D34(v10[1]);
  if ( !dword_8046B0F0.unk04 && v10[3] )
    return *(u32 **)(v7 + 20);
  if ( !dword_8046B0F0.unk04 )
  {
    if ( *(u32 *)(v7 + 8) )
      return (u32 *)v7;
    gamestate.unk04 = gamestate.unk03;
    
    if (gamestate.unk04)
    {
      gamestate.unk03 = gamestate.unk04 - 1;
      gamestate.unk05 = 0;
    }else
    {
      v11 = scene->scene_subdata;
      v12 = 0;
      v13 = scene->scene_subdata;
      v14 = scene->scene_subdata;
      while ( *v13 != 255 ){
        if ( *v14 > (unsigned int)(u8)gamestate.unk03 ){
          v15 = v11[24 * v12];
          goto LABEL_28;
        }
        v14 += 24;
        v13 += 24;
        ++v12;
      }
      v15 = *v11;
      if ( v15 == 255 )
        v15 = NULL;
LABEL_28:
      gamestate.unk03 = v15;
    }
  }
  sub_8001CDB4();
  sub_8001B760(11);
  result = sub_8001F800();
  if ( dword_8046B0F0.unk04 ){
    sub_80027DBC((unsigned int)result);
    sub_80377D18();
    do {
      v16 = sub_8001B6F8();
    } while ( v16 == 11 );
    if ( !DVD_CheckDisk(v16) )
      SYS_ResetSystem(1u, 0, 0);
    sub_8001F800();
    while (sub_8038EA50(1));
    CheckLanguage();
    memset(&gamestate, 0, 20);
    Scene_RunStartupInit();
    dword_8046B0F0.unk00 = 1;
    Scene_SetPendingMajor(40);
    VIDEO_SetBlack(FALSE);
    result = NULL;
  }
  return result;
}

//801A3EF4
void Scene_RunStartupInit(){  
  for (u32 i = 0; sceneInfo[i].idx != 45; i += 1 ){
    if(sceneInfo[i].StartupInit){
      (*sceneInfo[i].StartupInit)();
    }
  }
}

//801A4310
u8 Scene_LoadCurrentMajor(){
	return gamestate.curr_major;
}

//801A42E8
void Scene_UpdatePendingMajor(u8 val){
	gamestate.pending_major = val;
}

//801A42F8
void Scene_SetPendingMajor(u8 val){
	gamestate.pending_major = val;
	gamestate.pending = true;
}

//801A42D4
void Scene_SetPendingTrue(){
	gamestate.pending = true;
}

//801A4320
u8 Scene_LoadPrevMajor(){
	return gamestate.prev_major;
}

//801A42C4
u8 Scene_Get03(){
	return gamestate.unk03;
}

//801A428C
void Scene_Set03And04(u8 val){
	gamestate.unk03 = val;
	gamestate.unk04 = val;
}

//801A42B4
u8 Scene_Get04(){
  return gamestate.unk04;
}

//801A42A0
void Scene_Set05(u8 val){
	u8 v1;
	v1 = val + 1;
	gamestate.unk05 = v1;
}

//801A4330
u32 Scene_StoreTo10(u32 val){
	gamestate.unk10 = val;
	return val;
}

/**
* Is Scene X
**/

//801A4CE0
SceneData* Scene_GetSceneDataByIdx(u8 scene_idx){
  for ( int i = 0; sceneInfo[i].idx >= 45 ; i++ ){
    if ( sceneInfo[i].idx == scene_idx )
      return &sceneInfo[i];
  }
  return NULL;
}

//801A4340
bool Scene_IsSinglePlayer(u8 scene){
	if ( scene == 28 )
		return true;
	if ( scene >= 28 )
	{
		if ( scene != 43 && (scene >= 43 || scene >= 39 || scene < 32) )
			return false;
		return true;
	}
	if ( scene == 15 || (scene < 15 && scene < 6 && scene >= 3) )
		return true;
	return false;
}

//8016B3A0
bool Scene_IsCurrSceneSuperSuddenDeath(){
	if(Scene_LoadCurrentMajor() == 16)
		return true;
	return false;
}

//8016B41C
//8016B498
bool Scene_IsCurrSceneSinglePlayer(){
	u8 curr_major = Scene_LoadCurrentMajor();
	return Scene_IsSinglePlayer(curr_major);
}

//8016B3D8
bool Scene_IsSceneClassicAdvOrAllStar(){
	u8 curr_major = Scene_LoadCurrentMajor();
	if(curr_major < 6 && curr_major >= 3)
		return true;
	return false;
}

//801A50A0
struct SceneHandlers* GetSceneHandlers(){
  return &sceneHandlers;
}

//801A4B88
/*u32* Scene_StorePtr4F80(u32* addr){
	unk4F80 = addr;
	return addr;
}*/

//801A4B90
u32* Scene_Load4F80Relative04(){
	return unk4F80[1];
}

//801A4B9C
u32* Scene_Load4F80Relative08(){
	return unk4F80[2];
}

