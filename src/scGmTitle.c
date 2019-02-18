#include "scGmTitle.h"

static u32 r13_6C98 = 0;

//801A1C18
void Menu_Title_Think(){
    sub_801A36A0(4u);
    u32* r13_4F8C = Scene_Load4F80_idx3();
    if(r13_4F8C != NULL){
        *r13_4F8C -= 1;
    }else{
        u32* pVal = Scene_Load4F80_idx2();
        *pVal += 1;
        if(*pVal <= 0x258u){
            if(*pVal & 0x1000){
                u32 res = sub_80026F2C(28);
                sub_8002702C(0xC, res, 0);
                sub_80027168();
                sub_80027648();
                SFX_Menu_CommonSound(1);
                *pVal = Music_DecideRandom();
                MatchController_ChangeScreen();
            }else if(r13_6C98 >= 3){
                if(*pVal & 0x100){
                    SFX_Menu_CommonSound(1);
                    *pVal = 0;
                    MatchController_ChangeScreen();
                }else if(*pVal & 0x400){
                    SFX_Menu_CommonSound(1);
                    *pVal = 0;
                    MatchController_ChangeScreen();
                }else if(*pVal & 0x800){
                    SFX_Menu_CommonSound(1);
                    *pVal = 0;
                    MatchController_ChangeScreen();
                }
            }
        }else{
            *pVal = 0;
            MatchController_ChangeScreen();
        }
    }
}

