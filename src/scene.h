#ifndef _scene_h_
#define _scene_h_

#include <string.h>

#include <gccore.h>
#include <gctypes.h>

#include "unknown.h"

//Length: 0x14(20)
struct GameState {
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
} game_state;

struct MenuInfo {
	
} menu_info;

struct SceneInfo {
	
} scene_info;


u32* Scene_RunFunc(u8);

u8 Scene_UpdatePendingMajor(u8);
u8 Scene_SetPendingMajor(u8);
struct GameState* Scene_SetPendingTrue();
u8 Scene_LoadPrevMajor();
u8 Scene_Get03();
u8 Scene_Set03And04(u8);
u8 Scene_Get04();
struct GameState* Scene_Set05(u8);
u32 Scene_StoreTo10(u32);
bool Scene_IsSinglePlayer(u8);

#endif