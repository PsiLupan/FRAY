#include "init.h"

//8015FEB4
int main(void){

	
	Game_Init();
	
	return 0;
}


//801A4510
void Game_Init(){
	u8 curr_major; // r3@11
	
	//sub_801A50AC();
	memset(&gamestate, 0, 20);
	for (u32 i = 0; ; i += 1 )
	{
		if ( sceneInfo[i].idx == 45 )
		{
			/*if ( VI_GetDTVStatus(result) && (*(_WORD *)(v2 - 19312) & 0x200 || OS_GetProgressiveMode() == 1) )
				gamestate.curr_major = 39;
			else
				gamestate.curr_major = 40;*/
			gamestate.curr_major = 40;
			gamestate.prev_major = 45;
			while ( 1 )
			{
				curr_major = *Game_MainLoop(gamestate.curr_major);
				if ( dword_8046B0F0.unk04 )
					dword_8046B0F0.unk04 = 0;
				gamestate.prev_major = gamestate.curr_major;
				gamestate.curr_major = curr_major;
			}
		}
		if ( sceneInfo[i].StartupInit ){ //For things such as VS, this points to a function that allocates the memory for StartMelee, etc..
			sceneInfo[i].StartupInit();
			break;
		}
	}
}

//801A43A0
u8* Game_MainLoop(u8 scene){
	SceneData *scene_ptr; // r30@3
	u32 *result; // r3@7

	for (u32 i = 0; ; i += 1 )
	{
		if ( sceneInfo[i].idx == 45 )
			break;
		if ( sceneInfo[i].idx == scene )
		{
			scene_ptr = &sceneInfo[i];
			goto LABEL_7;
		}
	}
	scene_ptr = NULL;
LABEL_7:
	gamestate.pending = false;
	gamestate.unk03 = 0;
	gamestate.unk04 = 0;
	gamestate.unk05 = 0;
	unkstatestruct_set970(scene_ptr);
	if ( !scene_ptr->idx )
	{
		result = NULL;
		while ( !gamestate.pending )
		{
			if ( gamestate.unk10 )
				return (u8*)result;
			result = Scene_RunFunc(scene_ptr);
		}
		result = &dword_8046B0F0;
		if ( dword_8046B0F0.unk04 || !scene_ptr->unk01 )
			result = &gamestate.pending_major;
	}
	return result;
}