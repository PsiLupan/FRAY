#include "mnSlChar.h"

#include "scene/scene.h"

static void* match_struct; //r13_77C0

u8 menu_804D6730[6];

//801A5244
void* Menu_Get_MatchStruct()
{
    return match_struct; //+ 0x590;
}

//801A5598
void Menu_CSS_Init()
{
    //Scene_InitUsableStructs(*(int *)(r13_77C0) + 0x590);
    //FUN_8015CDEC();
}

//801A55C4
//801A55EC
void Menu_CSS_Load()
{
    memset(&menu_804D6730, 0, 6);
}

//801A5618
void Menu_CSS_EraseCharData(void* state, void* match_struct, u8 mode)
{
    //Mode is assumed as it's some kind of index
}

//801A5680
void Menu_CSS_Decide(void* state, void* match_struct)
{
    u8* status = Scene_GetPadStatus((GameState*)state);

    if (status[3] == 0x2) {
        Scene_SetPendingMajor(1);
    } else {

    }
}

//801B14A0
void Menu_CSS_VSMode_Prep(void* state)
{
    Menu_CSS_EraseCharData(state, Menu_Get_MatchStruct(), 0);
}

//801B14DC
void Menu_CSS_VSMode_Decide(void* state)
{
    Menu_CSS_Decide(state, Menu_Get_MatchStruct());
}