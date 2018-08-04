#ifndef __INIT_H__
#define __INIT_H__

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>

#include "scene.h"
#include "unknown.h"

#define DEFAULT_FIFO_SIZE	(256*1024)

void Game_Init();
u8* Game_MainLoop(u8);

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

#endif