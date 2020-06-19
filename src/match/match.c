#include "match.h"

StartMelee sm_regularvs;
StartMelee sm_train;

MatchInfo match_info; //8046B6A0

MatchController match_controller;

static MatchInfo* match_ptr = &match_info; //r13_77C0

//8015CDEC
u8* Match_8015CDEC()
{
    u8* result; // r3@2

    for (u32 i = 0; i < 6; ++i) {
        result = (u8*)Match_8015CE44(i, 0x78);
        if (result != NULL)
            *result = 5; //stb r31, 0(r3); - so storing 5 in first byte of some struct
    }
    return result;
}

//8015CE44
u32* Match_8015CE44(u32 offset, u32 unk)
{
    if (unk != 0x78) {
        return (u32*)((u32*)match_ptr + 0x1F2C * (unk / 0x13) + 0x1A4 * (unk % 0x13) + 0x319A);
    }
    if (offset >= 4) {
        return NULL;
    }
    return (u32*)((u32*)match_ptr + offset + 0x588);
}

//801677C0
void MatchController_InitData(MatchController* mc)
{
    mc->frozen = 0;
    mc->unk11 = 0;
    mc->pause = 0;
    mc->unk13 = 0;
    mc->unk2C = 0;
    mc->unk28 = 0;
    mc->unk24 = 0;
    mc->unk20 = 0;
}

//8016795C
static void Match_InitPlayerData(s8* addr)
{
    memset(addr, 0, 0x24u);
    addr[0] = 33;
    addr[1] = 3;
    addr[2] = 0;
    addr[3] = 0;
    addr[4] = 0;
    addr[5] = -1;
    addr[6] = 0;
    addr[7] = 0;
    addr[8] = 9;
    addr[9] = 0;
    addr[10] = 120;
    addr[11] = 0;
    addr[12] &= 0x7Fu;
    addr[12] = (u8)(addr[12] & 0xBF) | 0x40;
    addr[14] = 4;
    addr[15] = 0;
    *((u16*)addr + 9) = 0;
    *((u16*)addr + 10) = 0;
    *((f32*)addr + 6) = 1.0f;
    *((f32*)addr + 7) = 1.0f;
    *((f32*)addr + 8) = 1.0f;
}

//80167A14
static void __InitPlayerData(s8* addr)
{
    for (u32 i = 0; i < 6; ++i) {
        Match_InitPlayerData(addr);
        addr += 0x24;
    }
}

//80167A64
static void Match_InitMatchStruct(s8* addr)
{
    memset(addr, 0, 0x60u);
    addr[0] = (addr[0] & 0xE3) | 0x10;
    addr[12] = 0;
    addr[11] = 2;
    *((u32*)addr + 9) = -1;
    *((u32*)addr + 8) = -1;
    *((u32*)addr + 10) = 0;
    addr[3] = (addr[3] & 0xBF) | 0x40;
    addr[3] = (addr[3] & 0xFB) | 4;
    addr[3] = (addr[3] & 0xF7) | 8;
    addr[4] = (addr[4] & 0x7F) | 0x80;
    addr[1] &= 0xFDu;
    addr[2] = (addr[2] & 0xFB) | 4;
    addr[2] = (addr[2] & 0xFD) | 2;
    *((f32*)addr + 11) = 1.0f;
    *((f32*)addr + 12) = 1.0f;
    *((f32*)addr + 13) = 1.0f;
    addr[4] = (addr[4] & 0xFD) | 2;
    addr[4] = (u8)(addr[4] & 0xFE) | 1;
    addr[13] = 110;
    addr[10] = 0;
}

//80167B50
void Match_ResetMatchStruct(StartMelee* sm_struct)
{
    Match_InitMatchStruct(&sm_struct->data_main[0]);
    __InitPlayerData(&sm_struct->player_1[0]);
    sm_struct->header[0] = -1;
    sm_struct->header[1] = -1;
    sm_struct->header[2] = -1;
}

/**
* Match Info
**/

//8016AE38
//8016AE44
MatchInfo* MatchInfo_GetStruct()
{
    return &match_info;
}

//8016AEDC
u32 MatchInfo_GetFrameCount()
{
    return match_info.frame_count;
}

//8016AEEC
u32 MatchInfo_GetSeconds()
{
    return match_info.seconds;
}

//8016AEFC
u16 MatchInfo_GetSubSeconds()
{
    return match_info.sub_seconds;
}

//8016AE94
f32 MatchInfo_Get34()
{
    return match_info.unk34;
}

//8016B1EC
u8 MatchInfo_00Cntlzw()
{
    u8 value = 0;
    asm("cntlzw %0, %1\n\t"
        : "=b"(value)
        : "b"(-match_info.unk00));
    return value / 32;
}

//8016B328
void MatchInfo_Set06True()
{
    match_info.unk06 = true;
}

//8016B184
bool MatchInfo_Is07Equal1()
{
    if (match_info.unk07 == 1)
        return true;
    return false;
}

//8016B33C
void MatchInfo_Set0BValPlusOne(u8 val)
{
    match_info.unk0B = val + 1;
}

//8016B350
void MatchInfo_Set10ValPlusOne(u32 val)
{
    match_info.unk10 = val + 1;
}

//8016B364
void MatchInfo_Set14ValPlusOne(u32 val)
{
    match_info.unk14 = val + 1;
}

//8016B378
void MatchInfo_Set18(u8 val)
{
    match_info.unk18 = val;
}

//801A4BA8
u32 MatchController_LoadTimer()
{
    return match_controller.timer;
}

//801A4BB8
u32 MatchController_LoadTimer2()
{
    return match_controller.timer2;
}

//801A4B60
void MatchController_ChangeScreen()
{
    match_controller.screen_ctrl = 1;
}

//801A4B74
void MatchController_ChangeScreen2()
{
    match_controller.screen_ctrl = 2;
}

//801A4624
bool MatchController_IsFrozen()
{
    return match_controller.frozen;
}

//801A4B08
void MatchController_Store14_18(u32* ptr, u32* ptr2)
{
    match_controller.unk14 = ptr;
    match_controller.unk18 = ptr2;
}

//801A4B40
void MatchController_StoreDevFuncPtr(u32* func)
{
    match_controller.dev_togglefunc = func;
}

//801A4B50
void MatchController_Store34(u32 val)
{
    match_controller.unk34 = val;
}

//801A5244
StartMelee* Match_VSMode_GetMatchStruct()
{
    return &sm_regularvs; //*(r13_77C0) + 0x590 = 804DBC30
}