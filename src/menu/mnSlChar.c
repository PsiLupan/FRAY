#include "mnSlChar.h"

static void* match_struct; //r13_77C0

//801A5244
void* Menu_Get_MatchStruct()
{
    return match_struct; //+ 0x590;
}

//801A5618
void Menu_CSS_EraseCharData()
{
}

//801A5680
void Menu_CSS_Decide()
{
}

//801B14A0
void Menu_CSS_VSMode_Prep(void* state)
{
    /* 
    Menu_CSS_EraseCharData(state, Menu_Get_MatchStruct(), 0);
    */
}

//801B14DC
void Menu_CSS_VSMode_Decide(void* state)
{
    /*
    Menu_CSS_Decide(state, Menu_Get_MatchStruct());
    */
}