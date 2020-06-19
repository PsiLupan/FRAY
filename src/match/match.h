#ifndef _match_h_
#define _match_h_

#include <string.h>

#include <gccore.h>
#include <gctypes.h>

typedef struct _StartMelee {
    s8 header[0x8];
    s8 data_main[0x60];
    s8 player_1[0x24];
    s8 plyaer_2[0x24];
    s8 player_3[0x24];
    s8 player_4[0x24];
    s8 player_5[0x24];
    s8 player_6[0x24];
} StartMelee;

/**
* Match Info
**/

typedef struct _MatchInfo {
    u8 unk00;
    u8 unk01;
    u8 unk02;
    u8 unk03;
    u16 unk04;
    u8 unk06;
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
} MatchInfo;

/**
* Match Controller
**/

typedef struct _MatchController {
    u32 timer;
    u32 unk04;
    u32 timer2;
    u32 screen_ctrl;
    u8 frozen;
    u8 unk11;
    u8 pause;
    u8 unk13;
    u32* unk14;
    u32* unk18;
    u32* dev_togglefunc; //0x1C
    s32 unk20;
    s32 unk24;
    u32 unk28;
    void (*unk2C)();
    u32 unk30;
    u32 unk34; //0x80479D8C
    u8 flags; //0x80479D90
} MatchController;

extern MatchController match_controller;

u8* Match_8015CDEC();
u32* Match_8015CE44(u32 offset, u32 unk);

void Match_ResetMatchStruct(StartMelee*);

MatchInfo* MatchInfo_GetStruct();
u8 MatchInfo_00Cntlzw();
void MatchInfo_Set06True();
bool MatchInfo_Is07Equal1();
void MatchInfo_Set0BValPlusOne(u8);
void MatchInfo_Set10ValPlusOne(u32);
void MatchInfo_Set14ValPlusOne(u32);
void MatchInfo_Set18(u8);

u32 MatchInfo_GetFrameCount();
u32 MatchInfo_GetSeconds();
u16 MatchInfo_GetSubSeconds();

f32 MatchInfo_Get34();

void MatchController_InitData(MatchController*);
u32 MatchController_LoadTimer();

u32 MatchController_LoadTimer2();
void MatchController_ChangeScreen();
void MatchController_ChangeScreen2();
bool MatchController_IsFrozen();

void MatchController_Store14_18(u32*, u32*);
void MatchController_StoreDevFuncPtr(u32*);
void MatchController_Store34(u32);

StartMelee* Match_VSMode_GetMatchStruct();

#endif
