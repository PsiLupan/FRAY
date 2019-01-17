#include "scGmTitle.h"

static u32 r13_4F88 = 0;
static u32 r13_4F8C = 0;
static u32 r13_6C98 = 0;

//801A1C18
void Menu_Title_Think(){
    sub_801A36A0(4u);
    if(r13_4F8C != 0){
        r13_4F8C -= 1;
    }else{
        u32 val = r13_4F88;
        r13_4F88 += 1;
        if(r13_4F88 <= 0x258u){
            if(val & 0x1000){
                u32 res = sub_80026F2C(28);
                sub_8002702C(0xC, res, 0);
                sub_80027168();
                sub_80027648();
                SFX_Menu_CommonSound(1);
                u32* scene_4F88 = Scene_Load4F80Relative08();
                *scene_4F88 = Music_DecideRandom();
                MatchController_ChangeScreen();
            }else if(r13_6C98 >= 3){
                if(val & 0x800){
                    SFX_Menu_CommonSound(1);
                    *Scene_Load4F80Relative08() = r13_4F8C;
                    MatchController_ChangeScreen();
                }else if(val & 0x100){
                    SFX_Menu_CommonSound(1);
                    *Scene_Load4F80Relative08() = r13_4F8C;
                    MatchController_ChangeScreen();
                }else if(val & 0x400){
                    SFX_Menu_CommonSound(1);
                    *Scene_Load4F80Relative08() = r13_4F8C;
                    MatchController_ChangeScreen();
                }
            }
        }else{
            *Scene_Load4F80Relative08() = 0;
            MatchController_ChangeScreen();
        }
    }
}

