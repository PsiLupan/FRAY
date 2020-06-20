#include "scGmRst.h"

#include "scene.h"
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

//801A5618
void Menu_CSS_EraseCharData(GameState* state, StartMelee* match_struct, u8 mode)
{
    //Mode is assumed as it's some kind of index
}

//801A5680
void Menu_CSS_Decide(GameState* state, StartMelee* match_struct)
{
    u8* status = Scene_GetData2(state);

    if (status[3] == 0x2) {
        Scene_SetPendingMajor(1);
    } else {

    }
}

//801B14A0
void Menu_CSS_VSMode_Prep(GameState* state)
{
    Menu_CSS_EraseCharData(state, Match_VSMode_GetMatchStruct(), 0);
}

//801B14DC
void Menu_CSS_VSMode_Decide(GameState* state)
{
    Menu_CSS_Decide(state, Match_VSMode_GetMatchStruct());
}