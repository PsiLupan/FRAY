#include "scGmTitle.h"

MinorScene GmTitle_Minors[2] = {
    {0, 3, 0, Menu_Title_Prep, Menu_Title_Decide, 0, 0, 0, 0, NULL, NULL/*struct 804D6878*/}, 
    {0xFF, 0, 0, NULL, NULL, 0, 0, 0, 0, NULL, NULL}
    };

GmTitle_File title_ptrs; //80479B28

//801B087C
void Menu_Title_Prep(){
    Scene_800174BC();
}

//801B089C
void Menu_Title_Decide(void* unk_struct){

}