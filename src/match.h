#ifndef _match_h_
#define _match_h_

#include <string.h>

#include <gccore.h>
#include <gctypes.h>

struct StartMelee {
	u8 header[0x8];
	u8 data_main[0x60];
	u8 data1[0x24];
	u8 data2[0x24];
	u8 data3[0x24];
	u8 data4[0x24];
	u8 data5[0x24];
	u8 data6[0x24];
};

extern struct StartMelee sm_regularvs;
extern struct StartMelee sm_train;

extern void Match_InitStartMeleeStruct(s8*);

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
extern void MatchInfo_Set06True();
extern bool MatchInfo_Is07Equal1();
extern void MatchInfo_Set0BValPlusOne(u8);
extern void MatchInfo_Set10ValPlusOne(u32);
extern void MatchInfo_Set14ValPlusOne(u32);
extern void MatchInfo_Set18(u8);

extern u32 MatchInfo_GetFrameCount();
extern u32 MatchInfo_GetSeconds();
extern u16 MatchInfo_GetSubSeconds();

extern f32 MatchInfo_Get34();

/**
* Match Controller
**/

typedef struct _MatchController {
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
	u32* dev_togglefunc; //0x1C
	u32 unk20;
	u32 unk24;
	u32 unk28;
	u32 unk2C;
	u32 unk30;
	u32 unk34;
} MatchController;

extern MatchController match_controller;

extern u32 MatchController_LoadTimer();

extern u32 MatchController_LoadTimer2();
extern void MatchController_ChangeScreen();
extern void MatchController_ChangeScreen2();
extern bool MatchController_IsFrozen();

extern void MatchController_Store14_18(u32*, u32*);
extern void MatchController_StoreDevFuncPtr(u32*);
extern void MatchController_Store34(u32);

#endif