#ifndef _scene_h_
#define _scene_h_

#include <string.h>

#include <gccore.h>
#include <gctypes.h>

#include "unknown.h"

//Length: 0x14(20)
typedef struct _GameState {
	u8 curr_major;
	u8 pending_major;
	u8 prev_major;
	u8 unk03;
	u8 unk04;
	u8 unk05;
	u16 unk06;
	u32 unk08;
	bool pending;
	u8 unk0D;
	u8 unk0E;
	u8 unk0F;
	u32 unk10;
	u32 unk14;
} GameState;

extern GameState gamestate;

typedef struct _SceneHandler {
	u8 idx;
	u16 pad;
	u8 pad2;
	void* OnFrame;
	void* OnLoad;
	void* SomeFunc;
	void* SomeFunc2;
} SceneHandler;

typedef struct _SceneHandlers { //0x2D scenes
	SceneHandler title_screen;
	SceneHandler main_menu;
} SceneHandlers; //803DA920

extern struct SceneHandlers sceneHandlers;

typedef struct _SceneData {
	u8 idx; //starts at 1
	u16 unk01;
	u32* SceneInit;
	u32* UnkFunc;
	void (*StartupInit)();
	u8 scene_subdata[255];
	bool unk_bool;
} SceneData; //803DACA4

extern SceneData sceneInfo[45];

u32* Scene_RunFunc(u8);
void Scene_RunStartupInit();

void Scene_UpdatePendingMajor(u8);
void Scene_SetPendingMajor(u8);
void Scene_SetPendingTrue();
u8 Scene_LoadPrevMajor();
u8 Scene_Get03();
void Scene_Set03And04(u8);
u8 Scene_Get04();
void Scene_Set05(u8);
u32 Scene_StoreTo10(u32);

/**
* Is Scene X
**/
SceneData* Scene_GetSceneDataByIdx(u8 scene_idx);
bool Scene_IsSinglePlayer(u8);
bool Scene_IsCurrSceneSuperSuddenDeath();
bool Scene_IsCurrSceneSinglePlayer();
bool Scene_IsSceneClassicAdvOrAllStar();

struct Scenes* GetScenes();

u32* Scene_Load4F80Relative04();
u32* Scene_Load4F80Relative08();

#endif