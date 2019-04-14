#include "init.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <gccore.h>

#include "hsd/hsd_init.h"
#include "hsd/hsd_pad.h"
#include "hsd/hsd_video.h"

#include "ogcext/dvd.h"

#include "scene.h"

static u32 arena_size; //-0x5110(r13)
static u64 sys_time;

GXRModeObj* rmode = &TVNtsc480IntDf;

u8* pad_raw_queue[PAD_QUEUE_SIZE * 0x44]; //8046B108

//80019AAC
void Init_PadReader(void (*init_func)(void)){
	(*init_func)();
	//TODO
}

//8015FD24
void Pad_InitCallback(){
	PAD_SetSpec(5);
	HSD_PadInit(PAD_QUEUE_SIZE, pad_raw_queue, 0xc, NULL); //Technically the last param isn't NULL, but K7 does it, so it's probably nbd
	pad_raw_queue[0x1C] = 0;
	pad_raw_queue[0x1D] = 1;
	pad_raw_queue[0x1E] = 0x50;
	pad_raw_queue[0x1F] = 0;
	pad_raw_queue[0x20] = 1;
	pad_raw_queue[0x21] = 0x8C;
	pad_raw_queue[0x22] = 0;
	pad_raw_queue[0x26] = 0x50;
	pad_raw_queue[0x27] = 0x8C;
}

//8015FDA0
static void stub(){
}

//8015FEB4
int main(void){

	SYS_Init();
	VIDEO_Init();
	DVDInit(); //Calls FRAY's custom OGCExt DVDInit
	PAD_Init();

	arena_size = (u32)SYS_GetArena1Hi() - (u32)SYS_GetArena1Lo();

	HSD_SetInitParameter(1, 2); //Set XFB Max Num
	HSD_SetInitParameter(4, rmode); //Set RModeObj
	HSD_SetInitParameter(0, 0x40000); //Set FifoSize
	HSD_SetInitParameter(2, 4); //Set Heap Size
	//sub_80228C4C - Checks DebuggerIsPresent, so may be used to init the leak checker or something of that nature

	HSD_AllocateXFB(2, rmode);
	void* fifo = HSD_AllocateFIFO(0x40000);
	HSD_GXSetFifoObj(GX_Init(fifo, 0x40000));

	HSD_InitComponent();

	GX_SetMisc(1, 8);

	sys_time = SYS_Time();

	//sub_8002838C(); Inits AR, ARQ, and AI - "Audio_Init"

	Init_PadReader(Pad_InitCallback);

	HSD_VISetUserPostRetraceCallback(stub);
	HSD_VISetUserGXDrawDoneCallback(HSD_VIDrawDoneXFB);
	HSD_VISetBlack(0);
	//sub_8001564C(); Allocs memory in AR
	//sub_80015F3C();
	//sub_80018F68();
	//sub_80014D2C();
	//sub_8001C5BC();
	//sub_8001D21C();
	//sub_8001E290();
	//sub_8015FCC0();
	//sub_8001F87C();

	//sub_803A6048(); FirstHeapAlloc
	//InitializeStaticMemRegions();
	
	Init_Game();	
	return 0;
}

//801A4510
void Init_Game(){
	u8 curr_major;	
	memset(&gamestate, 0, 20);
	MajorScene* major_scenes = Scene_GetMajorScenes();
	for (u32 i = 0; i < 45; i += 1 )
	{
		if( major_scenes[i].Init ){ //For things such as VS, this points to a function that allocates the memory for StartMelee, etc..
			major_scenes[i].Init();
		}
	}
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
}