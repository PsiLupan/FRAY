#include "init.h"

static u64 sys_time;

//8015FDA0
static void stub(){
}

//8015FEB4
int main(void){

	SYS_Init();
	VIDEO_Init();
	DVD_Init();
	PAD_Init();

	HSD_SetInitParameter(1, 2); //Set XFB Max Num
	HSD_SetInitParameter(4, &TVNtsc480IntDf); //Set RModeObj
	HSD_SetInitParameter(0, 0x40000); //Set FifoSize
	HSD_SetInitParameter(2, 4); //Set Heap Size
	//sub_80228C4C - Checks DebuggerIsPresent, so may be used to init the leak checker or something of that nature

	HSD_AllocateXFB(2, &TVNtsc480IntDf);
	void* fifo = HSD_AllocateFIFO(0x40000);
	GXFifoObj* fifoobj = GX_Init(fifo, 0x40000);
	HSD_GXSetFifoObj(fifoobj);

	HSD_InitComponent();

	GX_SetMisc(1, 8);

	sys_time = SYS_Time();

	//sub_8002838C();

	//sub_80019AAC(&8015FDA0);

	HSD_VISetUserPostRetraceCallback(&stub);

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
		if ( major_scenes[i].idx == 45 )
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
		if ( major_scenes[i].Init ){ //For things such as VS, this points to a function that allocates the memory for StartMelee, etc..
			major_scenes[i].Init();
			break;
		}
	}
}

//801A43A0
u8* Game_MainLoop(u8 scene){
	MajorScene *scene_ptr; // r30@3
	u32 *result; // r3@7

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
			Scene_RunMajor(scene_ptr);
		}
		result = &dword_8046B0F0;
		if ( dword_8046B0F0.unk04 || !scene_ptr->flags )
			result = &gamestate.pending_major;
	}
	return result;
}