#include "mnMain.h"

//801B0FF8
void Menu_MainMenu_Prep(GameState* state)
{
    
}

//801B138C
void Menu_MainMenu_Decide(GameState* state)
{
    Scene_UpdatePendingMajor(*(u8*)state->data_2);
    Scene_SetPendingTrue();
}