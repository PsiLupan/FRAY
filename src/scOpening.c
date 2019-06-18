#include "scOpening.h"
#include "scene.h"

//801BF8F8
void Menu_Opening_ProgressiveScan_Prep(GameState* state){
    *Scene_Get10(state) = 1;
}

//801BF920
void Menu_Opening_ProgressiveScan_Decide(GameState* state){
    //u32 res = Scene_Get14(state); - It calls this function and immediately discards the result
    Scene_SetPendingMajor(0x28);
}

//801BF948
void Menu_Opening_Prep(){

}

//801BF9A8
void Menu_Opening_Decide(){

}
