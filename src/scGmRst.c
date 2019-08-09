#include "scGmRst.h"

//8015CDEC
u8* sub_8015CDEC()
{
    u8* result; // r3@2

    for (s32 i = 0; i < 6; ++i) {
        result = (u8*)sub_8015CE44(i, 0x78);
        if (result)
            *result = 5; //stb r31, 0(r3); - so storing 5 in first byte of some struct
    }
    return result;
}

//801A5598
u8* Menu_GmRst_Init()
{
    Scene_InitUsableStructs((s8*)&sm_regularvs);
    return sub_8015CDEC();
}
