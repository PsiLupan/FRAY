#include "scGmRst.h"

#include "match/match.h"

u8 menu_804D6730[6];

//801A5598
void Menu_CSS_Init()
{
    Match_ResetMatchStruct(Match_VSMode_GetMatchStruct());
    Match_8015CDEC();
}

//801A55C4
//801A55EC
void Menu_CSS_Load()
{
    memset(&menu_804D6730, 0, 6);
}

void __CopyMatchStruct(u32* copy, StartMelee* match)
{
    u32* original = (u32*)(&match[-1]);

    for (u32 i = 40; i > 0; --i) {
        copy[2] = original[2];
        copy[3] = original[3];

        copy += 2;
        original += 2;
        --i;
    }
}

//801A5618
void Menu_CSS_PrepCache(GameState* state, StartMelee* match, u8 mode)
{
    u8* data = Scene_GetData(state);
    data[2] = mode;
    ((u32*)data)[1] = 0; //&r13_4F70;


    __CopyMatchStruct((u32*)data, match);
    Scene_PrepCache();
}

//801A5680
void Menu_CSS_Decide(GameState* state, StartMelee* match)
{
    u8* data = Scene_GetData2(state);

    if (data[3] == 0x2) {
        Scene_SetPendingMajor(1);
    } else {
        __CopyMatchStruct((u32*)data, match);

        for (u32 i = 0; i < 6; ++i) {
            /* 
            uVar6 = FUN_80026e84((int)*(char *)(data + 0x1c));
            i = i + 1;
            uVar4 = uVar4 | (uint)uVar6;
            uVar5 = uVar5 | (uint)((ulonglong)uVar6 >> 0x20);
            data = data + 9;
            */
        }

        SFX_80026F2C(0x14);
        //SFX_RequestAudioLoad(4,extraout_r4,uVar5,uVar4);
        SFX_AudioCacheUpdate();
    }
}

//801A5754
void Menu_SSS_CopyMatchStruct(GameState* state, StartMelee* match)
{
    u32* copy = (u32*)Scene_GetData(state);

    __CopyMatchStruct(copy, match);
}

//801A57A8
void Menu_SSS_CopyStageInfo(GameState* state, StartMelee* match, u32 pending_minor)
{
    u8* data = (u8*)Scene_GetData2(state);

    if (data[4] == 0) {
        Scene_SetPendingMinor(pending_minor);
    } else {
        __CopyMatchStruct((u32*)data, match);
        SFX_80026F2C(0x18);
        /*uVar5 = FUN_80026ebc((uint)*(ushort *)&match->field_0x16);
        SFX_RequestAudioLoad(8,(int)uVar5,(int)((ulonglong)uVar5 >> 0x20),(int)uVar5);*/
        SFX_AudioCacheUpdate();
    }
}

//801B14A0
void Menu_CSS_VSMode_Prep(GameState* state)
{
    Menu_CSS_PrepCache(state, Match_VSMode_GetMatchStruct(), 0);
}

//801B14DC
void Menu_CSS_VSMode_Decide(GameState* state)
{
    Menu_CSS_Decide(state, Match_VSMode_GetMatchStruct());
}

//801B1514
void Menu_SSS_VSMode_Prep(GameState* state)
{
    Menu_SSS_CopyMatchStruct(state, Match_VSMode_GetMatchStruct());
}

//801B154C
void Menu_SSS_VSMode_Decide(GameState* state)
{
    Menu_SSS_CopyStageInfo(state, Match_VSMode_GetMatchStruct(), 0);
}