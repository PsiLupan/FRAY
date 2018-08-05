#ifndef _match_h_
#define _match_h_

#include <string.h>

#include <gccore.h>
#include <gctypes.h>

#include "unknown.h"

/**
* Match Info
**/

struct MatchInfo {
	u8 unk00;
	u8 unk01;
	u8 unk02;
	u8 unk03;
	u16 unk04;
	bool unk06;
	u8 unk07;
	u16 unk08;
	u8 unk0A;
	u8 unk0B;
	u32 unk0C;
	u32 unk10;
	u32 unk14;
	u8 unk18;
	u8 unk19;
	u16 unk1A;
	u32 unk1C;
	u32 unk20;
	u32 frame_count;
	u32 seconds;
	u16 sub_seconds;
	u16 unk2E;
	u32 unk30;
	f32 unk34;
	u32 unk38;
	u16 unk3C;
	u16 unk3E;
};

extern struct MatchInfo MatchInfo;

extern struct MatchInfo* MatchInfo_GetStruct();
extern u8 MatchInfo_00Cntlzw();
extern struct MatchInfo* MatchInfo_Set06True();
extern bool MatchInfo_Is07Equal1();
extern struct MatchInfo* MatchInfo_Set0BValPlusOne(u8);
extern struct MatchInfo* MatchInfo_Set10ValPlusOne(u32);
extern struct MatchInfo* MatchInfo_Set14ValPlusOne(u32);
extern struct MatchInfo* MatchInfo_Set18(u8);

extern u32 MatchInfo_GetFrameCount();
extern u32 MatchInfo_GetSeconds();
extern u16 MatchInfo_GetSubSeconds();

extern f32 MatchInfo_Get34();

/**
* Match Controller
**/

struct MatchController {
	u32 timer;
	u32 unk04;
	u32 timer2;
	u32 screen_ctrl;
	bool frozen;
	u8 unk11;
	u8 pause;
	u8 unk13;
	u32* unk14;
	u32* unk18;
	u32* dev_togglefunc;
	u32 unk34;
};

extern struct MatchController MatchController;

extern u32 MatchController_LoadTimer();

extern u32 MatchController_LoadTimer2();
extern struct MatchController* MatchController_ChangeScreen();
extern struct MatchController* MatchController_ChangeScreen2();
extern bool MatchController_IsFrozen();

extern u32* MatchController_StoreDevFuncPtr(u32*);
extern u32 MatchController_Store34(u32);

#endif