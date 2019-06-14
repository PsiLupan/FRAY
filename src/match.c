#include "match.h"

struct StartMelee sm_regularvs;
struct StartMelee sm_train;

MatchController match_controller;

static u32* unk77C0;

u32* sub_8015CE44(u32 offset, u32 unk){
	if(unk != 0x78){
		return (u32*)(unk77C0 + 0x1F2C * (unk / 0x13) + 0x1A4 * (unk % 0x13) + 0x319A);
	}
	if(offset >= 4){
		return NULL;
	}
	return (u32*)(unk77C0 + offset + 0x588); //r13 - 77C0 points to an address containing 0x8045A6C0
}

//80167A64
void Match_InitStartMeleeStruct(s8 *addr){
  s8 *v1; // r31@1
  double v3; // fp0@1
  int result; // r3@1

  v1 = addr;
  memset(addr, 0, 0x60u);
  v1[0] = v1[0] & 0xE3 | 0x10;
  v1[12] = 0;
  v1[11] = 2;
  *((u32 *)v1 + 9) = -1;
  *((u32 *)v1 + 8) = -1;
  *((u32 *)v1 + 10) = 0;
  v1[3] = v1[3] & 0xBF | 0x40;
  v1[3] = v1[3] & 0xFB | 4;
  v1[3] = v1[3] & 0xF7 | 8;
  v1[4] = v1[4] & 0x7F | 0x80;
  v1[1] &= 0xFDu;
  v1[2] = v1[2] & 0xFB | 4;
  v1[2] = v1[2] & 0xFD | 2;
  v3 = 1.0f;
  *((f32 *)v1 + 11) = v3;
  *((f32 *)v1 + 12) = v3;
  *((f32 *)v1 + 13) = v3;
  v1[4] = v1[4] & 0xFD | 2;
  result = (u8)(v1[4] & 0xFE) | 1;
  v1[4] = result;
  v1[13] = 110;
  v1[10] = 0;
}

/**
* Match Info
**/

//8016AE38
//8016AE44
struct MatchInfo* MatchInfo_GetStruct(){
	return &MatchInfo;
}

//8016B1EC
u8 MatchInfo_00Cntlzw(){
	u8 value = 0;
	asm("cntlzw %0, %1\n\t"
	: "=b" (value)
	: "b" (-MatchInfo.unk00)
	);
	return value / 32;
}

//8016B328
void MatchInfo_Set06True(){
	MatchInfo.unk06 = true;
}

//8016B184
bool MatchInfo_Is07Equal1(){
	if(MatchInfo.unk07 == 1)
		return true;
	return false;
}

//8016B33C
void MatchInfo_Set0BValPlusOne(u8 val){
	MatchInfo.unk0B = val + 1;
}

//8016B350
void MatchInfo_Set10ValPlusOne(u32 val){
	MatchInfo.unk10 = val + 1;
}

//8016B364
void MatchInfo_Set14ValPlusOne(u32 val){
	MatchInfo.unk14 = val + 1;
}

//8016B378
void MatchInfo_Set18(u8 val){
	MatchInfo.unk18 = val;
}

//8016AEDC
u32 MatchInfo_GetFrameCount(){
	return MatchInfo.frame_count;
}

//8016AEEC
u32 MatchInfo_GetSeconds(){
	return MatchInfo.seconds;
}

//8016AEFC
u16 MatchInfo_GetSubSeconds(){
	return MatchInfo.sub_seconds;
}

//8016AE94
f32 MatchInfo_Get34(){
	return MatchInfo.unk34;
}

/**
* Match Controller
**/

//801677C0
void MatchController_InitData(MatchController* mc){
	mc->frozen = 0;
	mc->unk11 = 0;
	mc->pause = 0;
	mc->unk13 = 0;
	mc->unk2C = 0;
	mc->unk28 = 0;
	mc->unk24 = 0;
	mc->unk20 = 0;
}

//801A4BA8
u32 MatchController_LoadTimer(){
	return match_controller.timer;
}

//801A4BB8
u32 MatchController_LoadTimer2(){
	return match_controller.timer2;
}

//801A4B60
void MatchController_ChangeScreen(){
	match_controller.screen_ctrl = 1;
}

//801A4B74
void MatchController_ChangeScreen2(){
	match_controller.screen_ctrl = 2;
}

//801A4624
bool MatchController_IsFrozen(){
	return match_controller.frozen;
}

//801A4B08
void MatchController_Store14_18(u32* ptr, u32* ptr2){
	match_controller.unk14 = ptr;
	match_controller.unk18 = ptr2;
}

//801A4B40
void MatchController_StoreDevFuncPtr(u32* func){
	match_controller.dev_togglefunc = func;
}

//801A4B50
void MatchController_Store34(u32 val){
	match_controller.unk34 = val;
}
