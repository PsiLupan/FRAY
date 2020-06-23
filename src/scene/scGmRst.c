#include "scGmRst.h"

#include "match/match.h"

u8 menu_804D6730[6];

static HSD_CObjDesc* css_cobjdesc = NULL; //r13_4ADC

static void* r13_49F0 = NULL; //r13_49F0
static MnSlChar_file* css_data_table = NULL; // r13_49EC
static HSD_GObj* css_gobj_camera = NULL; //r13_49E8
static u32 r13_49E4 = 0; // r13_49E4

static HSD_Archive* css_file = NULL; // r13_49D0
static HSD_Archive* menu_ext_file = NULL; // r13_49CC
static MnSlChar_joints* css_joints = NULL; // r13_49C8
static u32 r13_49C4 = 0; // r13_49C4
static u32 r13_49C0 = 0; // r13_49C0
static u32 r13_49BC = 0; // r13_49BC
static u32 r13_49B8 = 0; // r13_49B8
static u32 r13_49B4 = 0; // r13_49B4
static s8 r13_49B0 = 0; // r13_49B0
static u8 r13_49AF = 0; // r13_49AF
static u8 r13_49AE = 0; // r13_49AE
static u8 r13_49AD = 0; // r13_49AD
static u8 r13_49AC = 0; // r13_49AC
static u8 r13_49AB = 0; // r13_49AB
static u8 r13_49AA = 0; // r13_49AA

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

static void __CopyMatchStruct(u32* copy, StartMelee* match)
{
    u32* original = (u32*)(&match[-1]);

    for (u32 i = 40; i > 0; --i) {
        copy[2] = original[2];
        copy[3] = original[3];

        copy += 2;
        original += 2;
    }
}

//801A5618
void Menu_CSS_PrepCache(GameState* state, StartMelee* match, u8 mode)
{
    u8* data = Scene_GetData(state);
    data[2] = mode;
    ((u32*)data)[1] = 0; //&r13_4F70;

    __CopyMatchStruct((u32*)data, match);
    Scene_PrepCache();
}

//801A5680
void Menu_CSS_Decide(GameState* state, StartMelee* match)
{
    u8* data = Scene_GetData2(state);

    if (data[3] == 0x2) {
        Scene_SetPendingMajor(1);
    } else {
        __CopyMatchStruct((u32*)data, match);

        for (u32 i = 0; i < 6; ++i) {
            /* 
            uVar6 = FUN_80026e84((int)*(char *)(data + 0x1c));
            i = i + 1;
            uVar4 = uVar4 | (uint)uVar6;
            uVar5 = uVar5 | (uint)((ulonglong)uVar6 >> 0x20);
            data = data + 9;
            */
        }

        SFX_80026F2C(0x14);
        //SFX_RequestAudioLoad(4,extraout_r4,uVar5,uVar4);
        SFX_AudioCacheUpdate();
    }
}

//801A5754
void Menu_SSS_CopyMatchStruct(GameState* state, StartMelee* match)
{
    u32* copy = (u32*)Scene_GetData(state);

    __CopyMatchStruct(copy, match);
}

//801A57A8
void Menu_SSS_CopyStageInfo(GameState* state, StartMelee* match, u32 pending_minor)
{
    u8* data = (u8*)Scene_GetData2(state);

    if (data[4] == 0) {
        Scene_SetPendingMinor(pending_minor);
    } else {
        __CopyMatchStruct((u32*)data, match);
        SFX_80026F2C(0x18);
        /*uVar5 = FUN_80026ebc((uint)*(ushort *)&match->field_0x16);
        SFX_RequestAudioLoad(8,(int)uVar5,(int)((ulonglong)uVar5 >> 0x20),(int)uVar5);*/
        SFX_AudioCacheUpdate();
    }
}

//801B14A0
void Menu_CSS_VSMode_Prep(GameState* state)
{
    //Menu_CSS_PrepCache(state, Match_VSMode_GetMatchStruct(), 0);
}

//801B14DC
void Menu_CSS_VSMode_Decide(GameState* state)
{
    Menu_CSS_Decide(state, Match_VSMode_GetMatchStruct());
}

//801B1514
void Menu_SSS_VSMode_Prep(GameState* state)
{
    Menu_SSS_CopyMatchStruct(state, Match_VSMode_GetMatchStruct());
}

//801B154C
void Menu_SSS_VSMode_Decide(GameState* state)
{
    Menu_SSS_CopyStageInfo(state, Match_VSMode_GetMatchStruct(), 0);
}

//80263354
static void Menu_CSS_JObjCallback(HSD_GObj* gobj, u32 unused)
{
    HSD_JObj* jobj = GOBJ_HSD_JOBJ(gobj);

    if (r13_49B4 == (r13_49B4 / 200) * 200) {
        HSD_JObjReqAnimAll(jobj, 0.f);
    }
    HSD_JObjAnimAll(jobj);
}

//802640A0
void Menu_CSS_Setup()
{
    u32 r13_49AB_copy = 0;

    /*bVar1 = *(byte*)(*(int*)(unaff_r13 + -0x49f0) + 2);
    if (bVar1 == 0) {
        iVar17 = FUN_8015cc34();
        bVar1 = *(byte*)(iVar17 + 2);
        if (bVar1 == 2) {
            FUN_800237a8(0x7537, 0x7f, 0x40);
        } else {
            if (bVar1 < 2) {
                if (bVar1 == 0) {
                    FUN_800237a8(0x7535, 0x7f, 0x40);
                } else {
                    FUN_800237a8(0x7538, 0x7f, 0x40);
                }
            } else {
                if (bVar1 < 4) {
                    FUN_800237a8(0x7536, 0x7f, 0x40);
                }
            }
        }
    } else {
        FUN_800237a8(*(undefined4*)((uint)bVar1 * 8 + -0x7fc0f598), 0x7f, 0x40);
    }*/
    r13_49C0 = 0;
    r13_49C4 = 0;
    r13_49B8 = 0;
    r13_49BC = 0;
    r13_49AE = 0x1E;
    r13_49AD = 0;

    /*u8* unk_byte = ((u8**)r13_49F0)[2];
    if (*unk_byte < 0xb) {
        r13_49AB = 4;
        r13_49AB_copy = 4;
    } else {
        r13_49AB = 1;
        r13_49AB_copy = 1;
    }*/

    r13_49AA = 0;

    if (r13_49AB == 1 && r13_49B0 < 0) {
        r13_49B0 = 0;
    }
    if (r13_49B0 == 0) {
        r13_49AF = 1;
    }

    if (r13_49AC != 0) {
        u32 temp_offset = 0;
        u32 offset = 0;
        r13_49AC = 0;
        u32 i = 0;

        if (r13_49AB_copy != 0) {
            if (8 < r13_49AB_copy && (i = r13_49AB_copy - 1 >> 3, 0 < (r13_49AB_copy - 8))) {
                do {
                    ((u8*)r13_49F0)[temp_offset + 0x78] = 9;
                    ((u8*)r13_49F0)[temp_offset + 0x9C] = 9;
                    ((u8*)r13_49F0)[temp_offset + 0xC0] = 9;
                    ((u8*)r13_49F0)[temp_offset + 0xE4] = 9;
                    ((u8*)r13_49F0)[temp_offset + 0x108] = 9;
                    ((u8*)r13_49F0)[temp_offset + 0x12C] = 9;
                    ((u8*)r13_49F0)[temp_offset + 0x150] = 9;
                    ((u8*)r13_49F0)[temp_offset + 0x174] = 9;
                    temp_offset += 0x120;
                    offset += 8;
                    --i;
                } while (i != 0);
            }
            u32 n = offset * 0x24;
            temp_offset = r13_49AB_copy - offset;
            if (offset < r13_49AB_copy) {
                do {
                    ((u8*)r13_49F0)[n + 0x78] = 9;
                    n += 0x24;
                    temp_offset = temp_offset + -1;
                } while (temp_offset != 0);
            }
        }
    }

    css_gobj_camera = GObj_Create(2, 3, 0x80);
    css_cobjdesc = css_data_table->cobjdesc;
    GObj_InitKindObj(css_gobj_camera, GOBJ_KIND_MENU_COBJ, HSD_CObjLoadDesc(css_cobjdesc));
    GObj_SetupCameraGXLink(css_gobj_camera, GObj_SetCamera, 0);
    css_gobj_camera->gxlink_prios = 0x1F; //Priority ???
    //GObj_CreateProcWithCallback(css_gobj_camera, NULL /*sub_8022BA1C*/, 5);
    //iVar18 = Menu_CreateTextObj(0, css_gobj_camera, 7, 8, 0x80, 1, 0x80, 0);

    HSD_GObj* lobj_gobj = GObj_Create(3, 4, 0x80);
    HSD_LObj* lobj_1 = HSD_LObjLoadDesc(css_data_table->lightdesc_1);
    HSD_LObj* lobj_2 = HSD_LObjLoadDesc(css_data_table->lightdesc_2);
    HSD_CheckAssert("lobj1 in Menu_CSS_Setup == NULL", lobj_1 != NULL);
    lobj_1->next = lobj_2;
    GObj_InitKindObj(lobj_gobj, GOBJ_KIND_LIGHT, lobj_1);
    GObj_SetupGXLink(lobj_gobj, LObj_Setup_Callback, 0, 0x80);

    HSD_GObj* fog_gobj = GObj_Create(0xE, 2, 0);
    HSD_Fog* fog = HSD_FogLoadDesc(css_data_table->fogdesc);
    GObj_InitKindObj(fog_gobj, GOBJ_KIND_FOG, fog);
    GObj_SetupGXLink(fog_gobj, Fog_Set_Callback, 0, 0x80);

    HSD_GObj* jobj_gobj = GObj_Create(4, 5, 0x80);
    HSD_JObj* jobj = HSD_JObjLoadJoint(css_joints->jobjdesc);
    HSD_JObjAddAnimAll(jobj, css_joints->animjoint, css_joints->matanimjoint, css_joints->shapeanimjoint);
    GObj_InitKindObj(jobj_gobj, GOBJ_KIND_JOBJ, jobj);
    GObj_SetupGXLink(jobj_gobj, JObj_SetupInstanceMtx_Callback, 1, 0x80);
    GObj_CreateProcWithCallback(jobj_gobj, Menu_CSS_JObjCallback, 4);
    HSD_JObjReqAnimAll(jobj, 0.f);
    HSD_JObjAnimAll(jobj);

    //TODO
}

//8026688C
void Menu_CSS_OnLoad(void* data_struct)
{
    /*
      FUN_8001c550();
    FUN_8001d164(0);*/
    r13_49F0 = data_struct;
    /*
    uVar1 = **(undefined2 **)(unaff_r13 + -0x49f0);
    *(undefined *)(unaff_r13 + -0x49a8) = 0;
    *(char *)(unaff_r13 + -0x49b0) = (char)uVar1 + -1;
    while ((*(byte *)(unaff_r13 + -0x49a8) < 0x78 && (iVar2 = FUN_8023754c(), iVar2 != 0))) {
        *(char *)(unaff_r13 + -0x49a8) = *(char *)(unaff_r13 + -0x49a8) + '\x01';
    }
    *(undefined *)(unaff_r13 + -0x49a7) = 0xff;
    *(char *)(unaff_r13 + -0x49a8) = *(char *)(unaff_r13 + -0x49a8) + '\x01';
    DAT_803f0e05 = 0;
    DAT_803f0e29 = 0;
    DAT_803f0e4d = 0;
     DAT_803f0e71 = 0;
    SFX_80026f2c(0x12);
    SFX_RequestAudioLoad(2,extraout_r4,0,8);
    SFX_AudioCacheUpdate();
    FUN_80027648();
    */

    css_file = Archive_LoadFile("MnSlChr.usd");
    menu_ext_file = Archive_LoadFile("MnExtAll.usd");

    css_data_table = HSD_ArchiveGetPublicAddress(css_file, "MnSelectChrDataTable");
    css_joints = &css_data_table->joints;

    //sub_803A62A0(0, "SdSlChr.usd", "SIS_SelCharData");
    r13_49AC = 0;

    Menu_CSS_Setup();
}

//802669F4
void Menu_CSS_OnFrame()
{
    r13_49B4 += 1;

    //TODO
}

//80266D70
void Menu_CSS_OnLeave()
{
}