#include "init.h"

static u32 arena_size; //-0x5110(r13)
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

	arena_size = (u32)SYS_GetArena1Hi() - (u32)SYS_GetArena1Lo();

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

	//sub_8002838C(); ZeroAudioCache

	//sub_80019AAC(&8015FDA0);

	HSD_VISetUserPostRetraceCallback(stub);
	HSD_VISetUserGXDrawDoneCallback(HSD_VIDrawDoneXFB);
	HSD_VISetBlack(0);
	//sub_8001564C(); ARAM_Initialize
	//sub_80018F68();
	//sub_80014D2C();
	//sub_8001C5BC();
	//sub_8001D21C();
	//sub_8001E290();
	//sub_8015FCC0();
	//sub_8001F87C();

	//sub_803A6048(); FirstHeapAlloc
	//InitializeStaticMemRegions();

	Game_Init();
	
	return 0;
}

//801A4510
void Game_Init(){
	u8 curr_major;	
	memset(&gamestate, 0, 20);
	MajorScene* major_scenes = Scene_GetMajorScenes();
	for (u32 i = 0; i < 45; i += 1 )
	{
		if ( major_scenes[i].idx == 45 )
		{
			if ( (VIDEO_HaveComponentCable() && (rmode->viTVMode & 0x02)) /*|| OS_GetProgressiveMode() == 1)*/ ){
				gamestate.curr_major = 39;
			}else {
				gamestate.curr_major = 40;
			}
			gamestate.prev_major = 45;
			while ( 1 )
			{
				curr_major = *Scene_ProcessMajor(gamestate.curr_major);
				if ( dword_8046B0F0.unk04 )
					dword_8046B0F0.unk04 = 0;
				gamestate.prev_major = gamestate.curr_major;
				gamestate.curr_major = curr_major;
			}
		}else if ( major_scenes[i].Init ){ //For things such as VS, this points to a function that allocates the memory for StartMelee, etc..
			major_scenes[i].Init();
		}
	}
}