#ifndef __INIT_H__
#define __INIT_H__

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>

#include "hsd/hsd_init.h"
#include "hsd/hsd_video.h"

#include "scene.h"

#define DEFAULT_FIFO_SIZE	(256*1024)

void Game_Init();
u8* Game_MainLoop(u8);

#endif