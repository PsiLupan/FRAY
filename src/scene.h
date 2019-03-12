#ifndef _scene_h_
#define _scene_h_

#include <string.h>

#include <gccore.h>
#include <gctypes.h>

#include "hsd/hsd_fog.h"
#include "hsd/hsd_jobj.h"
#include "hsd/hsd_video.h"

#include "hsd_cobj_ext.h"
#include "hsd_fog_ext.h"
#include "hsd_jobj_ext.h"
#include "hsd_lobj_ext.h"

#include "archive.h"
#include "gobj.h"
#include "match.h"
#include "menu.h"

//Scenes
#include "scGmRst.h"
#include "scGmTitle.h"
#include "scGmTrain.h"

//Length: 0x14(20)
typedef struct _GameState {
	u8 curr_major;
	u8 pending_major;
	u8 prev_major;
	u8 curr_minor;
	u8 unk04;
	s8 unk05;
	u16 unk06;
	u32 unk08;
	BOOL pending;
	u8 unk0D;
	u8 unk0E;
	u8 unk0F;
	u32 unk10;
	void* unk14;
} GameState;

extern GameState gamestate;

typedef struct _MinorSceneHandler {
	u8 class_id;
	u16 pad;
	u8 pad2;
	void (*OnFrame)();
	void (*OnLoad)();
	void (*OnLeave)();
	void (*unk_func)();
} MinorSceneHandler; //803DA920

extern MinorSceneHandler scene_handlers[45];

typedef struct _MajorScene {
	u8 preload;
	u8 idx;
	u16 flags;
	void (*Load)();
	void (*Unload)();
	void (*Init)();
	MinorScene** minor_scenes;
} MajorScene; //803DACA4

typedef struct _MinorScene {
	u8 idx;
	u8 preload;
	u16 flags;

	void (*Prep)();
	void (*Decide)();

	u8 class_id;
	u8 unk_1;
	u8 unk_2;
	u8 unk_3;

	void* unk_struct_0;
	void* unk_struct_1;
} MinorScene;

extern MajorScene major_scenes[45];

typedef struct _unk_8046B0F0 {
	u32 unk00;
	u32 unk04;
	u32 unk08;
	u32 unk0C;
} unk_8046B0F0;

extern unk_8046B0F0 dword_8046B0F0;

u8* Scene_ProcessMajor(u8);

#endif