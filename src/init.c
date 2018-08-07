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
	memset(&GameState, 0, 20);
	for (u32 i = 0; ; i += 1 )
	{
		if ( SceneData[i].idx == 45 )
		{
			/*if ( VI_GetDTVStatus(result) && (*(_WORD *)(v2 - 19312) & 0x200 || OS_GetProgressiveMode() == 1) )
				byte_80479D30 = 39;
			else
				byte_80479D30 = 40;*/
			GameState.curr_major = 40;
			GameState.prev_major = 45;
			while ( 1 )
			{
				curr_major = *Game_MainLoop(GameState.curr_major);
				if ( dword_8046B0F0.unk04 )
					dword_8046B0F0.unk04 = 0;
				GameState.prev_major = GameState.curr_major;
				GameState.curr_major = curr_major;
			}
		}
		if ( SceneData[i].StartupInit ){ //For things such as VS, this points to a function that allocates the memory for StartMelee, etc..
			SceneData[i].StartupInit();
			break;
		}
	}
}

//801A43A0
u8* Game_MainLoop(u8 scene){
	struct SceneData *scene_ptr; // r30@3
	u32 *result; // r3@7

	for (u32 i = 0; ; i += 1 )
	{
		if ( SceneData[i].idx == 45 )
			break;
		if ( SceneData[i].idx == scene )
		{
			scene_ptr = &SceneData[i];
			goto LABEL_7;
		}
	}
	scene_ptr = 0;
	LABEL_7:
	GameState.pending = false;
	GameState.unk03 = 0;
	GameState.unk04 = 0;
	GameState.unk05 = 0;
	//result = (u32 *)sub_80018F58(*scene_ptr);
	if ( !scene_ptr->idx )
	{
		result = 0;
		while ( !GameState.pending )
		{
			if ( GameState.unk10 )
				return (u8*)result;
			//result = Scene_RunFunc(scene_ptr);
		}
		result = &dword_8046B0F0;
		if ( dword_8046B0F0.unk04 || !scene_ptr->unk01 )
			result = &GameState.pending_major;
	}
	return (u8*)result;
}

/*
void Game_Loop(){
	PAD_ScanPads();
	
	GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
	
	//Draw Functions
	
	GX_DrawDone();
	
	
	fb ^= 1;		// flip framebuffer
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	GX_SetColorUpdate(GX_TRUE);
	GX_CopyDisp(frameBuffer[fb],GX_TRUE);

	VIDEO_SetNextFramebuffer(frameBuffer[fb]);
	VIDEO_Flush();
	VIDEO_WaitVSync();
}*/