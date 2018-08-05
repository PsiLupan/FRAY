#include "match.h"

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
struct MatchInfo* MatchInfo_Set06True(){
	MatchInfo.unk06 = true;
	return &MatchInfo;
}

//8016B184
bool MatchInfo_Is07Equal1(){
	if(MatchInfo.unk07 == 1)
		return true;
	return false;
}

//8016B33C
struct MatchInfo* MatchInfo_Set0BValPlusOne(u8 val){
	MatchInfo.unk0B = val + 1;
	return &MatchInfo;
}

//8016B350
struct MatchInfo* MatchInfo_Set10ValPlusOne(u32 val){
	MatchInfo.unk10 = val + 1;
	return &MatchInfo;
}

//8016B364
struct MatchInfo* MatchInfo_Set14ValPlusOne(u32 val){
	MatchInfo.unk14 = val + 1;
	return &MatchInfo;
}

//8016B378
struct MatchInfo* MatchInfo_Set18(u8 val){
	MatchInfo.unk18 = val;
	return &MatchInfo;
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

//801A4BA8
u32 MatchController_LoadTimer(){
	return MatchController.timer;
}

//801A4BB8
u32 MatchController_LoadTimer2(){
	return MatchController.timer2;
}

//801A4B60
struct MatchController* MatchController_ChangeScreen(){
	MatchController.screen_ctrl = 1;
	return &MatchController;
}

//801A4B74
struct MatchController* MatchController_ChangeScreen2(){
	MatchController.screen_ctrl = 2;
	return &MatchController;
}

//801A4624
bool MatchController_IsFrozen(){
	return MatchController.frozen;
}

//801A4B08
u32* MatchController_Store14_18(u32* ptr, u32* ptr2){
	MatchController.unk14 = ptr;
	MatchController.unk18 = ptr2;
	return ptr;
}

//801A4B40
u32* MatchController_StoreDevFuncPtr(u32* func){
	MatchController.dev_togglefunc = func;
	return func;
}

//801A4B50
u32 MatchController_Store34(u32 val){
	MatchController.unk34 = val;
	return val;
}