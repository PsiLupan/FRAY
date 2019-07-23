#include "mnSlChar.h"

static void* stage_select_struct; //r13_77C0

//801A5244
void* Menu_Get_StageSelectStruct(){
    return stage_select_struct;
}

//801A5618
void Menu_CSS_EraseCharData(){

}

//801A5680
void Menu_CSS_Decide(){

}

//801B14A0
void Menu_CSS_VSMode_Prep(void* state){
    /* uVar1 = Menu_Get_StageSelectStruct();
    Menu_CSS_EraseCharData(state, uVar1, 0);
     */
}

//801B14DC
void Menu_CSS_VSMode_Decide(void *state){
    /* uVar1 = Menu_Get_StageSelectStruct();
    Menu_CSS_Decide(state, uVar1);
     */
}