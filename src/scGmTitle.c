#include "scGmTitle.h"

MinorScene GmTitle_Minor = {0, 3, 0, 0, Menu_Title_Prep, Menu_Title_Decide, 0, 0, 0, 0, NULL, /*struct 804D6878*/};
MinorScene Null_Scene = {0xFF, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, NULL, NULL};

MinorScene GmTitle_Minors[2] = {GmTitle_Minor, Null_Scene};
MajorScene GmTitle_Major = {1, 0, 0, NULL, NULL, NULL, &GmTitle_Minors};

GmTitle_File title_ptrs; //80479B28

//801B087C
void Menu_Title_Prep(){
    Scene_800174BC();
}

//801B089C
void Menu_Title_Decide(void* unk_struct){

}