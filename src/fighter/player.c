#include "player.h"

#include "actionstate.h"

static f32* r13_514C;

//8004CBE8
void Player_MoveGroundTypeOffset(Player* player)
{
    player->x6F0_physics.x3C_floor_id_actionstate = player->x6F0_physics.x14C_floor_id;
}

//8004CBF4
void Player_SetInvalidGround(Player* player)
{
    player->x6F0_physics.x3C_floor_id_actionstate = -1;
}

//800693AC
void Player_ChangeActionState(HSD_GObj* gobj, u32 state, u32 flags, HSD_GObj* gobj_2, f32 start_frame, f32 frame_rate, f32 unk3)
{
    /*
    HSD_JObj* jobj = (HSD_JObj*)gobj->hsd_obj;
    Player* player = GOBJ_PLAYER(gobj);
    player->x10_action_state = state;
    player->x30_facedir_model = player->x2C_facedir;

    assert(jobj != NULL);
    jobj->position = player->xB0_xB8_pos;
    if ((jobj->flags & MTX_INDEP_SRT) == 0) {
        HSD_JObjSetMtxDirty(jobj);
    }
    sub_80076724(gobj, player->x60C_gfx);

    if ((flags & 8) == 0) {
        if ((player->x2219_flags >> 4 & 1) != 0) {
            sub_8007AFF8(gobj);
        }
        if ((player->x2219_flags >> 3 & 1) != 0) {
            sub_8007C114(gobj);
        }
    }
    if ((flags & 0x100) == 0) {
        player->x1198_thrown = 0;
    }
    if ((flags & 4) == 0) {
        if (player->x1988_bodystate != 0) {
            FUN_8007b62c(gobj, 0);
        }
        if ((player->x221A_flags >> 2 & 1) != 0) {
            FUN_8007b0c0(gobj, 0);
        }
    }
    if ((player->x221A_flags >> 1 & 1) != 0) {
        FUN_8007b4e0(gobj);
    }
    if ((flags & 0x10) == 0 && (player->x221D_flags >> 5 & 1) != 0) {
        FUN_80074a8c(gobj);
    }
    if ((flags & 0x80) == 0 && (player->x221E_flags & 1) != 0) {
        FUN_80070654(gobj);
    }
    if ((flags & 0x400) == 0) {
        player->x2221_flags &= 0xF7;
        s32 res = FUN_8007e994(gobj);
        if ((res != -1) && (res = FUN_8007e994(gobj), res != 6)) {
            FUN_8007e83c(gobj, 6, 0.F);
        }
    }
    
    FUN_80070f28(gobj);
    FUN_80070e74(gobj);
    FUN_8007ecd4(player, 7);
    FUN_8007ecd4(player, 8);
    FUN_8007ecd4(player, 0x24);
    if ((flags & 0x800) == 0) {
        FUN_8007ecd4(player, 1);
        FUN_8007ecd4(player, 0x19);
    }
    if ((flags & 0x10000) == 0 && (player->x2221_flags >> 6 & 1) != 0) {
        FUN_8007b760(gobj, FUN_8007b6ec(gobj));
        player->x2221_flags &= 0xBF;
    }
    FUN_8007e2f4(player, 0);
    if (*(int*)&player->field_0x18f4 != 0) {
        *(undefined4*)&player->field_0x18f4 = 0;
        player->x2220_flags &= 0xF7;
    }
    if ((flags & 0x80000) == 0) {
        player->x2222_flags &= 0xDF;
    }
    if ((flags & 0x800000) == 0) {
        player->x2223_flags &= 0xF7;
    }
    if ((flags & 0x8000000) == 0) {
        player->x2227_flags &= 0xDF;
    }
    if ((flags & 0x10000000) == 0 && (player->x221C_flags >> 1 & 1) != 0) {
        player->x221C_flags &= 0xFD;
        *(undefined2*)&player->field_0x2098 = (short)*(undefined4*)(*(int*)(in_r13 + -0x514c) + 0x4cc);
    }

    player->x221F_flags &= 0xEF;
    player->x2219_flags &= 0xBF;
    player->x2219_flags &= 0xDF;
    player->field_0x182c = *(undefined4*)(in_r2 + -0x7790);
    uVar2 = *(undefined4*)(in_r2 + -0x778c);
    player->field_0x1850 = uVar2;
    player->field_0x18a8 = uVar2;
    player->field_0x18b4 = uVar2;
    player->field_0x18a0 = uVar2;
    player->x221A_flags &= 0xFE;
    player->x221B_flags &= 0x7F;
    player->x221C_flags &= 0xEF;
    *(undefined4*)&player->field_0x19b4 = uVar2;
    *(undefined4*)&player->field_0x19b8 = uVar2;
    *(byte*)&player->field_0x221d = *(byte*)&player->field_0x221d & 0xfb;
    *(byte*)&player->field_0x2218 = *(byte*)&player->field_0x2218 & 0xef;
    *(byte*)&player->field_0x2218 = *(byte*)&player->field_0x2218 & 0xfd;
    player->x221C_flags &= 0xF7;
    *(undefined2*)&player->field_0x1a6a = 0;
    player->x221D_flags &= 0xFE;
    player->x221E_flags &= 0x7F;
    player->x221E_flags &= 0xBF;
    player->x221E_flags &= 0xDF;
    player->x221F_flags &= 0xBF;
    player->x221E_flags &= 0xFB;
    player->x221E_flags &= 0xFD;
    *(byte*)&player->field_0x2220 = *(byte*)&player->field_0x2220 & 0xef;
    *(byte*)&player->field_0x2220 = *(byte*)&player->field_0x2220 & 0xfe;
    *(undefined2*)&player->field_0x209c = 0;
    player->field_0x2223 = player->field_0x2223 & 0x7f;
    *(byte*)&player->field_0x2222 = *(byte*)&player->field_0x2222 & 0xef;
    player->x2224_flags = player->x2224_flags & 0xFB;
    *(byte*)&player->field_0x2225 = *(byte*)&player->field_0x2225 & 0xbf;
    *(byte*)&player->field_0x2225 = *(byte*)&player->field_0x2225 & 0xf7;
    FUN_8004cbf4(&player->x6F0_physics);
    FUN_800deea8(gobj);
    *(undefined4*)&player->field_0x2138 = *(undefined4*)(in_r2 + -0x7788);
    player->x2224_flags &= 0xF7;
    if ((flags & 0x40000) == 0) {
        player->x221E_flags = (player->x221E_flags & 0xEF) | 0x10;
    } else {
        if ((player->x221E_flags >> 4 & 1) == 0) {
            FUN_8007f578(gobj);
        }
    }
    if ((flags & 0x400000) == 0) {
        *(byte*)&player->x221E_flags = *(byte*)&player->x221E_flags & 0xf7 | 8;
    }
    if ((flags & 0x4000000) == 0) {
        *(byte*)&player->field_0x2225 = *(byte*)&player->field_0x2225 & 0xdf | 0x20;
    }
    if ((flags & 1) == 0) {
        player->x221A_flags &= 0xF7;
    }
    if ((flags & 0x1000) == 0) {
        FUN_800c0134(player);
    }
    if (((flags & 2) == 0) && ((char)player->field_0x2219 < '\0')) {
        FUN_8007db24(gobj);
    }
    if (((flags & 0x2000) == 0) && (*(int**)&player->field_0x20a0 != (int*)0x0)) {
        HSD_JObjRemoveAll(*(int**)&player->field_0x20a0);
        *(undefined4*)&player->field_0x20a0 = 0;
    }
    if (player->xE0_in_air == 0) {
        if (player->x4_internal_id == 9) {
            player->x222C_flags = 1;
        }
        player->x2221_flags &= 0xFB;
        player->x2221_flags = (player->x2221_flags & 0xFE) | 1;
        player->x2221_flags = (player->x2221_flags & 0xFD) | 2;
        player->x2224_flags &= 0xBF;
        player->x2227_flags = player->x2227_flags & 0xbf;
        *(undefined4*)&player->field_0x213c = 0xffffffff;
        if (((byte)player->x2227_flags >> 3 & 1) != 0) {
            sub_8003FE1C(player->xC_slot, ((player->x221F_flags >> 3) & 1));
            player->x2227_flags = player->x2227_flags & 0xf7;
        }
        player->x2227_flags = player->x2227_flags & 0xfb;
        FUN_80040330((double)*(float*)&player->field_0x2140, (uint)player->slot,
            (uint)(player->field_0x221f >> 3) & 1);
        *(undefined4*)&player->field_0x2140 = *(undefined4*)(in_r2 + -0x778c);
        *(byte*)&player->field_0x2228 = *(byte*)&player->field_0x2228 & 0xfd;
        *(undefined4*)&player->field_0x2180 = 6;
    }
    if ((((state != 0xe) && (state != 0xf)) && (state != 0x10)) && (state != 0x11)) {
        *(undefined4*)&player->field_0x196c = *(undefined4*)(in_r2 + -0x778c);
    }
    if ((flags & 0x200) == 0) {
        FUN_80088884(player);
        FUN_800888e0(player);
        FUN_800887cc(player);
    }
    if ((flags & 0x20000) == 0) {
        *(undefined*)&player->field_0x2100 = 0xff;
    }
    if ((flags & 0x8000) == 0) {
        *(undefined2*)&player->field_0x209a = 0;
    }
    *(byte*)&player->field_0x2222 = *(byte*)&player->field_0x2222 & 0xfe;
    if ((flags & 0x100000) == 0) {
        *(undefined4*)&player->field_0x100 = *(undefined4*)(in_r2 + -0x7788);
    } else {
        *(undefined4*)&player->field_0x100 = *(undefined4*)(in_r2 + -0x778c);
    }
    if ((flags & 0x1000000) == 0) {
        *(u16*)&player->x221C_flags = *(u16*)&player->x221C_flags & 0xFE3F;
    }
    *(undefined4*)&player->field_0x6bc = *(undefined4*)(in_r2 + -0x778c);
    FUN_8007592c(player, 0);
    FUN_80075af0((double)(float)(*(double*)(in_r2 + -0x7770) * (double)player->x2C_facedir), player, 0);
    FUN_80075cb4((double)*(float*)(in_r2 + -0x778c), player, 0);
    
    if (state < player->x18_tblconstant) {
        puVar8 = (uint*)((int)player->shared_action_table + state * 0x20);
    } else {
        puVar8 = (uint*)((int)player->move_table + (state - player->x18_tblconstant) * 0x20);
    }
    if (((player->xE0_in_air == 0) && ((flags & 0x40) == 0)) && (((*(byte*)((int)puVar8 + 9) >> 6 & 1) != 0 && (*(int*)&player->field_0x18c8 == -1)))) {
        iVar5 = *(int*)(*(int*)(in_r13 + -0x514c) + 0x814);
        if (iVar5 < 1) {
            *(undefined4*)&player->field_0x18c8 = 1;
        } else {
            *(int*)&player->field_0x18c8 = iVar5;
        }
    }
    local_50 = *(undefined4*)&player->field_0x2070;
    FUN_800890d0(player, (uint) * (byte*)(puVar8 + 2));
    FUN_800895e0(player, puVar8[1]);
    *(byte*)&player->field_0x2225 = *(byte*)((int)puVar8 + 9) >> 3 & 0x10 | *(byte*)&player->field_0x2225 & 0xef;
    if ((*(byte*)&player->field_0x2226 >> 3 & 1) != 0) {
        if ((*(byte*)&player->field_0x2071 >> 2 & 1) != 0) {
            FUN_800c8b2c(player, 0x7e, 0);
        }
        if ((*(byte*)&player->field_0x2071 >> 1 & 1) != 0) {
            FUN_800c8b2c(player, 0x7f, 0);
        }
    }
    if (*(code**)&player->field_0x21ec != (code*)0x0) {
        (**(code**)&player->field_0x21ec)(gobj);
        *(undefined4*)&player->field_0x21ec = 0;
    }
    if ((flags & 0x2000000) == 0) {
        FUN_80037c60(gobj, local_50);
    }
    player->anim_state = *puVar8;
    *(float*)&player->field_0x89c = (float)frame_rate;
    *(float*)&player->field_0x8a0 = (float)frame_rate;
    *(float*)&player->field_0x894 = (float)(dVar13 - (double)*(float*)&player->field_0x89c);
    *(undefined4*)&player->field_0x898 = *(undefined4*)(in_r2 + -0x778c);
    if (((char)player->field_0x594 < '\0') || ((player->field_0x594 >> 2 & 1) != 0)) {
        bVar1 = true;
    } else {
        bVar1 = false;
    }
    if (player->anim_state != 0xffffffff) {
        uVar3 = *(ushort*)&player->field_0x596 >> 6;
        if ((flags & 0x200000) != 0) {
            player->field_0x2223 = player->field_0x2223 & 0x7f | 0x80;
            *(undefined*)&player->field_0x104 = 0x14;
            frame_rate = (double)*(float*)(in_r2 + -0x778c);
            *(float*)&player->field_0x89c = *(float*)(in_r2 + -0x778c);
        }
        if (gobj_2 == (HSD_GObj*)0x0) {
            ppAVar6 = player->state_ptr + player->anim_state * 6;
            puVar9 = &player->field_0x28->classifier + player->anim_state;
        } else {
            ppAVar6 = (AnimData**)(*(int*)((int)gobj_2->data + 0x24) + player->anim_state * 0x18);
            puVar9 = (ushort*)(*(int*)((int)gobj_2->data + 0x28) + player->anim_state * 2);
        }
        *(AnimData**)&player->field_0x594 = ppAVar6[4];
        FUN_8009e7b4(player, puVar9);
        if ((flags & 0x20000000) == 0) {
            if (gobj_2 == (HSD_GObj*)0x0) {
                FUN_80085cd8(player, player, player->anim_state);
            } else {
                FUN_80085cd8(player, gobj_2->data, player->anim_state);
                FUN_8007b8cc(player, gobj_2);
            }
            *(AnimData**)&player->field_0x3ec = ppAVar6[3];
            *(undefined4*)&player->field_0x3f0 = 0;
            dVar12 = (double)*(float*)(in_r2 + -0x778c);
            if (dVar13 == dVar12) {
                if (*(int*)&player->field_0x590 != 0) {
                    if (((double)*(float*)(in_r2 + -0x7788) != param_3) && (bVar11 = param_3 == dVar12, dVar12 = param_3, bVar11)) {
                        uStack68 = (uint) * (byte*)puVar9;
                        local_48 = 0x43300000;
                        dVar12 = (double)(float)((double)CONCAT44(0x43300000, uStack68) - *(double*)(in_r2 + -0x7768));
                    }
                    FUN_8006ebe8(dVar13, frame_rate, dVar12, gobj);
                }
                *(undefined4*)&player->field_0x3e4 = *(undefined4*)(in_r2 + -0x778c);
            } else {
                if (*(int*)&player->field_0x590 != 0) {
                    if (((double)*(float*)(in_r2 + -0x7788) != param_3) && (bVar11 = param_3 == dVar12, dVar12 = param_3, bVar11)) {
                        uStack68 = (uint) * (byte*)puVar9;
                        local_48 = 0x43300000;
                        dVar12 = (double)(float)((double)CONCAT44(0x43300000, uStack68) - *(double*)(in_r2 + -0x7768));
                    }
                    FUN_8006ebe8((double)(float)(dVar13 - frame_rate), frame_rate, dVar12, gobj);
                }
                FUN_8006e9b4(gobj);
                if ((char)player->field_0x594 < '\0') {
                    fVar4 = *(float*)(in_r2 + -0x778c);
                    *(float*)&player->field_0x6b8 = fVar4;
                    *(float*)&player->field_0x6b4 = fVar4;
                    *(float*)&player->field_0x6b0 = fVar4;
                    player->field_0x6ac = fVar4;
                    *(float*)&player->field_0x6a8 = fVar4;
                    *(float*)&player->field_0x6a4 = fVar4;
                    *(undefined4*)&player->field_0x698 = *(undefined4*)&player->field_0x68c;
                    *(undefined4*)&player->field_0x69c = *(undefined4*)&player->field_0x690;
                    *(undefined4*)&player->field_0x6a0 = *(undefined4*)&player->field_0x694;
                }
                if ((player->field_0x594 >> 2 & 1) != 0) {
                    uVar2 = *(undefined4*)(in_r2 + -0x778c);
                    *(undefined4*)&player->field_0x6ec = uVar2;
                    *(undefined4*)&player->field_0x6e8 = uVar2;
                    *(undefined4*)&player->field_0x6e4 = uVar2;
                    *(undefined4*)&player->field_0x6e0 = uVar2;
                    *(undefined4*)&player->field_0x6dc = uVar2;
                    *(undefined4*)&player->field_0x6d8 = uVar2;
                    *(undefined4*)&player->field_0x6cc = *(undefined4*)&player->field_0x6c0;
                    *(undefined4*)&player->field_0x6d0 = *(undefined4*)&player->field_0x6c4;
                    *(undefined4*)&player->field_0x6d4 = *(undefined4*)&player->field_0x6c8;
                }
                *(float*)&player->field_0x3e4 = (float)-dVar13;
            }
            FUN_8006e9b4(gobj);
            if (((uVar3 & 7) != 0) && (*(byte*)puVar9 != 0)) {
                iVar5 = ((uint)uVar3 & 7) * 0x10;
                iVar10 = *(int*)(player->bone_lookup_table + iVar5 + 4);
                if (iVar10 == 0) {
                    assert(in_r13 + -0x7ca0, 0x3d3, in_r13 + -0x7c98);
                }
                local_5c = *(undefined4*)(iVar10 + 0x38);
                local_58 = *(undefined4*)(iVar10 + 0x3c);
                local_54 = *(undefined4*)(iVar10 + 0x40);
                iVar7 = *(int*)(player->bone_lookup_table + iVar5);
                if (iVar7 == 0) {
                    assert(in_r13 + -0x7ca0, 0x394, in_r13 + -0x7c98);
                }
                *(undefined4*)(iVar7 + 0x38) = local_5c;
                *(undefined4*)(iVar7 + 0x3c) = local_58;
                *(undefined4*)(iVar7 + 0x40) = local_54;
                if (((*(uint*)(iVar7 + 0x14) & 0x2000000) == 0) && (iVar7 != 0)) {
                    if (iVar7 == 0) {
                        assert(in_r13 + -0x7ca0, 0x234, in_r13 + -0x7c98);
                    }
                    bVar11 = false;
                    if (((*(uint*)(iVar7 + 0x14) & 0x800000) == 0) && ((*(uint*)(iVar7 + 0x14) & 0x40) != 0)) {
                        bVar11 = true;
                    }
                    if (!bVar11) {
                        HSD_JObjSetMtxDirtySub(iVar7);
                    }
                }
                if (iVar10 == 0) {
                    assert(in_r13 + -0x7ca0, 699, in_r13 + -0x7c98);
                }
                local_6c = *(undefined4*)(iVar10 + 0x1c);
                local_68 = *(undefined4*)(iVar10 + 0x20);
                local_64 = *(undefined4*)(iVar10 + 0x24);
                local_60 = *(undefined4*)(iVar10 + 0x28);
                FUN_8007584c(*(undefined4*)(player->bone_lookup_table + iVar5), &local_6c);
            }
            if ((char)player->field_0x594 < '\0') {
                fVar4 = *(float*)(in_r2 + -0x778c);
                if (dVar13 == (double)fVar4) {
                    *(float*)&player->field_0x6b8 = fVar4;
                    *(float*)&player->field_0x6b4 = fVar4;
                    *(float*)&player->field_0x6b0 = fVar4;
                    player->field_0x6ac = fVar4;
                    *(float*)&player->field_0x6a8 = fVar4;
                    *(float*)&player->field_0x6a4 = fVar4;
                    *(undefined4*)&player->field_0x698 = *(undefined4*)&player->field_0x68c;
                    *(undefined4*)&player->field_0x69c = *(undefined4*)&player->field_0x690;
                    *(undefined4*)&player->field_0x6a0 = *(undefined4*)&player->field_0x694;
                } else {
                    if (((flags & 0x20) == 0) && (player->xE0_in_air == 0)) {
                        fVar4 = player->field_0x6ac * player->x2C_facedir;
                        (player->x80_x88_self_vel).x = fVar4;
                        player->xEC_vel_ground_self_x = fVar4;
                    }
                }
            }
            if ((player->field_0x594 >> 2 & 1) != 0) {
                fVar4 = *(float*)(in_r2 + -0x778c);
                if (dVar13 == (double)fVar4) {
                    *(float*)&player->field_0x6ec = fVar4;
                    *(float*)&player->field_0x6e8 = fVar4;
                    *(float*)&player->field_0x6e4 = fVar4;
                    *(float*)&player->field_0x6e0 = fVar4;
                    *(float*)&player->field_0x6dc = fVar4;
                    *(float*)&player->field_0x6d8 = fVar4;
                    *(undefined4*)&player->field_0x6cc = *(undefined4*)&player->field_0x6c0;
                    *(undefined4*)&player->field_0x6d0 = *(undefined4*)&player->field_0x6c4;
                    *(undefined4*)&player->field_0x6d4 = *(undefined4*)&player->field_0x6c8;
                } else {
                    if (((flags & 0x20) == 0) && (player->xE0_in_air == 0)) {
                        fVar4 = *(float*)&player->field_0x6e0 * player->x2C_facedir;
                        (player->x80_x88_self_vel).x = fVar4;
                        player->xEC_vel_ground_self_x = fVar4;
                    }
                }
            }
            if ((flags & 0x4000) == 0) {
                if (dVar13 == (double)*(float*)(in_r2 + -0x778c)) {
                    FUN_800c0408(gobj);
                    FUN_80073240(gobj);
                } else {
                    FUN_80073354(gobj);
                }
            } else {
                FUN_8007349c(gobj);
            }
        } else {
            player->anim_state = -1;
        }
    }
    if (player->anim_state == -1) {
        *(undefined4*)&player->field_0x594 = 0;
        FUN_80070758(jobj);
        FUN_80070758(*(undefined4*)&player->field_0x8ac);
        *(undefined4*)&player->field_0x3ec = 0;
        uVar2 = *(undefined4*)(in_r2 + -0x778c);
        *(undefined4*)&player->field_0x8a4 = uVar2;
        *(undefined4*)&player->field_0x8a8 = uVar2;
    }
    if (((bVar1) && (bVar1 = -1 < (char)player->field_0x594, bVar1)) && (bVar1)) {
        FUN_8007cc78((double)*(float*)&player->field_0x138, player);
    }
    *(uint*)&player->field_0x21a0 = puVar8[3];
    *(uint*)&player->field_0x219c = puVar8[4];
    *(uint*)&player->field_0x21a4 = puVar8[5];
    *(uint*)&player->field_0x21a8 = puVar8[6];
    *(uint*)&player->field_0x21ac = puVar8[7];
    *(undefined4*)&player->field_0x21b0 = 0;
    *(undefined4*)&player->field_0x21bc = 0;
    *(undefined4*)&player->field_0x21c0 = 0;
    *(undefined4*)&player->field_0x21c4 = 0;
    *(undefined4*)&player->field_0x21c8 = 0;
    *(undefined4*)&player->field_0x21d0 = 0;
    *(undefined4*)&player->field_0x21cc = 0;
    *(undefined4*)&player->field_0x21d8 = 0;
    *(undefined4*)&player->field_0x21d4 = 0;
    *(undefined4*)&player->field_0x21dc = 0;
    *(undefined4*)&player->field_0x21f0 = 0;
    *(undefined4*)&player->field_0x21f4 = 0;
    *(undefined4*)&player->field_0x21f8 = 0;
    *(undefined4*)&player->field_0x21e4 = 0;*/
}

//80074A4C
void Player_SetCharacterFlags(HSD_GObj* gobj, u32 offset, u32 flags)
{
    Player* player = GOBJ_PLAYER(gobj);
    *(u32*)(player + offset * 2 + 0x5F4) = flags; //TODO: Not rely on set offsets if possible
    player->x221D_flags = (player->x221D_flags & 0xDF) | 0x20;
}

//8007500C
u32 Player_BoneID2Index(Player* player, u32 bone_id)
{
    /*u32* r3 = -0x515C(r13)*/

    /*Points to an array pointers that go to the below struct
	static bone_id* bone_ids[ACTIVE_OBJECTS];

	Which points to struct:
	struct {
		u8* unk_ids;
		u8* bone_ids;
		u32 num_bones;
	}
	*/

    /*u32* result_1 = (u32*)(*((u32*)(r3 + (player->x4_internal_id * 4))));
	u8* bones = (result_1 + 0x4);
	return bone[bone_id];
	*/
    return 0;
}

//8007D174
void Player_CalculateAirMobility(Player* player, f32 vel, f32 mobility_1, f32 mobility_2, f32 friction)
{
    if (mobility_2 != 0.f) {
        if ((vel * mobility_1) > 0.f) {
            if (mobility_1 <= 0.f) {
                if ((vel + mobility_1) < mobility_2) {
                    mobility_1 = friction;
                    if (mobility_2 < (vel + friction)) {
                        mobility_1 = mobility_2 - vel;
                    }
                    if ((vel + mobility_1) < -player->attribs.horizontalAirMobility) {
                        mobility_1 = -player->attribs.horizontalAirMobility - vel;
                    }
                }
            } else if (mobility_2 < (vel + mobility_1)) {
                mobility_1 = -friction;
                if ((vel + mobility_1) < mobility_2) {
                    mobility_1 = mobility_2 - vel;
                }
                if (player->attribs.horizontalAirMobility < (vel + mobility_1)) {
                    mobility_1 = player->attribs.horizontalAirMobility - vel;
                }
            }
        }
        player->x74_mobility = mobility_1;
    } else {
        f32 x_vel = player->x80_x88_self_vel.x;
        if (x_vel < 0) {
            x_vel = -x_vel;
        }

        f32 frict = friction;
        if (frict < 0) {
            frict = -frict;
        }

        if (frict < x_vel) {
            if (player->x80_x88_self_vel.x > 0) {
                friction = -friction;
            }
        } else {
            friction = -player->x80_x88_self_vel.x;
        }
        player->x74_mobility = friction;
    }
}

//8007D268
void Player_UpdateHorzVelocity(Player* player)
{
    Player_CalculateHorzMobility(player, player->x80_x88_self_vel.x);
}

//8007D28C
void Player_CalculateHorzMobility(Player* player, f32 hVel)
{
    f32 mobility = 0.f;
    if (0.f <= player->x620_joystick_x) {
        mobility = -player->attribs.x68_airMobilityConst;
    } else {
        mobility = player->attribs.x68_airMobilityConst;
    }
    f32 mobility_1 = (player->x620_joystick_x * player->attribs.x64_airMobilityMulti) + mobility;
    f32 mobility_2 = (player->x620_joystick_x * player->attribs.airMaxHorzSpeed);
    Player_CalculateAirMobility(player, hVel, mobility_1, mobility_2, player->attribs.airFriction);
}

//8007D440
void Player_ClampHorzVelocity(Player* player, f32 vel)
{
    f32 hVel = player->x80_x88_self_vel.x;
    if (hVel < -vel) {
        player->x80_x88_self_vel.x = -vel;
    } else {
        if (hVel > vel) {
            player->x80_x88_self_vel.x = vel;
        }
    }
}

//8007D468
void Player_ClampMaxHorzVelocity(Player* player)
{
    Player_ClampHorzVelocity(player, player->attribs.airMaxHorzSpeed);
}

//8007D494
void Player_UpdateFallingVelocity(Player* player, f32 gravity, f32 term_vel)
{
    f32 vVel = player->x80_x88_self_vel.y;
    term_vel = -term_vel;
    vVel = vVel - gravity;
    player->x80_x88_self_vel.y = vVel;
    if (player->x80_x88_self_vel.y < term_vel) {
        player->x80_x88_self_vel.y = term_vel;
    }
}

//8007D4E4
void Player_UpdateVelocityFromFastFall(Player* player)
{
    player->x80_x88_self_vel.y = -player->attribs.fallTermVel;
}

//8007D4F4
void Player_ClampVertVelocity(Player* player, f32 vel)
{
    if (player->x80_x88_self_vel.y > vel) {
        player->x80_x88_self_vel.y = vel;
    }
}

//8007D508
void Player_UpdateAscendingVelocity(Player* player, f32 vel, f32 term_vel)
{
    f32 vVel = player->x80_x88_self_vel.y;
    vVel = vVel + vel;
    player->x80_x88_self_vel.y = vVel;
    if (player->x80_x88_self_vel.y > term_vel) {
        player->x80_x88_self_vel.y = term_vel;
    }
}

//8007D528
BOOL Player_Interrupt_Fastfall(Player* player)
{
    if ((player->x221A_flags >> 3) & 1) {
        return FALSE;
    }
    if (player->x80_x88_self_vel.y >= 0.0f) {
        return FALSE;
    }
    //f32* unk = r13_514C + 0x88;
    if (player->x624_joystick_y > -0.6625f || player->x671_frames_tilt_y >= 4) { //Values here are hardcoded - Need to account for individual character data
        return FALSE;
    }
    player->x221A_flags = (player->x221A_flags & 0xF7) | 8;
    player->x671_frames_tilt_y = -2;
    Player_PlaySFX(player, 0x96, 0x7F, 0x40);
    return TRUE;
}

//8007D5BC
void Player_ECBBottom_EnableUpdate(Player* player)
{
    player->x88C_ecb_inactive_frames = 0;
    player->x6F0_physics.x130_unk &= 0xFFFFFFEF;
}

//8007D5D4
void Player_LoseGroundJump_ECBDisable(Player* player)
{
    player->xE0_in_air = TRUE;
    player->xEC_vel_ground_self_x = 0;
    player->xA0_unk = 0;
    player->xB0_xB8_pos.z = 0;
    player->x1968_jumps_used = 1;
    player->x88C_ecb_inactive_frames = 10;
    player->x6F0_physics.x130_unk |= 0x10;
}

//8007D60C
void Player_LoseAllJumps_ECBDisable(Player* player)
{
    if ((player->x2227_flags >> 7) != 0 && player->x1968_jumps_used < 2) {
        //sub_8003FC44(player->slot, (player->x221F_flags >> 3) & 1);
    }
    player->xE0_in_air = TRUE;
    player->xEC_vel_ground_self_x = 0.f;
    player->x78_unk = 0.f;
    player->x1968_jumps_used = player->attribs.maxJumps;
    player->x88C_ecb_inactive_frames = 5;
    player->x6F0_physics.x130_unk |= 0x10;
}

//8007D698
void Player_UseAllJumps(Player* player)
{
    player->x1968_jumps_used = player->attribs.maxJumps;
}

//8007D7FC
void Player_SetGroundedState(Player* player)
{
    if (player->xE0_in_air == TRUE) {
        u32 res = FUN_803224dc(player->xB0_xB8_pos.x, player->x18A4_unk, player->x8_spawn_ctr);
        if (res != 0) {
            player->x18A4_unk = 0.f;
        }
        if (player->x2227_flags < 0) {
            if (player->x1968_jumps_used < 2) {
                StaticPlayer_UpdateFlags_xE8C(player->xC_slot, (player->x221F_flags >> 3) & 1);
            }
        }
    }

    if (player->x594_flags < 0) {
        player->x80_x88_self_vel.x = player->x6AC_unk * player->x2C_facedir;
    }

    f32 max_vel = player->attribs.x34_grMaxHorzVel;
    if (-max_vel <= player->xEC_vel_ground_self_x) {
        if (max_vel < player->xEC_vel_ground_self_x) {
            player->xEC_vel_ground_self_x = max_vel;
        }
    } else {
        player->xEC_vel_ground_self_x = -max_vel;
    }

    player->xE0_in_air = FALSE;
    player->xEC_vel_ground_self_x = player->x80_x88_self_vel.x;
    player->x1968_jumps_used = 0;
    player->x1969_walljumps_used = 0;
    player->x2227_flags = player->x2227_flags & 0x7F;
    player->x88C_ecb_inactive_frames = 0;
    player->x6F0_physics.x130_unk = player->x6F0_physics.x130_unk & 0xFFFFFFEF;
    HSD_CheckAssert("No ground under fighter", sub_80084a18(player->parent));
}

//8007D964
f64 Player_GetJoystickAngle_AbsX(Player* player)
{
    return atan2(player->x624_joystick_y, fabs(player->x620_joystick_x));
}

//8007D99C
f64 Player_GetCStickAngle_AbsX(Player* player)
{
    return atan2(player->x63C_cstick_y, fabs(player->x638_cstick_x));
}

//8007D9D4
f64 Player_GetJoystickAngle(Player* player)
{
    return atan2(player->x624_joystick_y, player->x620_joystick_x);
}

//8007D9FC
void Player_UpdateFacingFromStick(Player* player)
{
    if (player->x620_joystick_x < 0) {
        player->x2C_facedir = -1.0f;
    } else {
        player->x2C_facedir = 1.0f;
    }
}

//8007E2F4
void Player_SetGrabbableFlags(Player* player, u16 flags)
{
    player->x1A64_grabbable_flags = flags;
}

//8007EFC0
void Player_SetIndicatorDisplay(Player* player, u16 flags)
{
    player->x209A_flags = flags;
}

//80081D0C
BOOL Player_CollisonCheck_Ground(HSD_GObj* gobj)
{
    BOOL collided = FALSE;

    Player* ply = GOBJ_PLAYER(gobj);
    ply->x6F0_physics.x1C_x24_TopN_prev = ply->x6F0_physics.x4_xC_TopN;
    ply->x6F0_physics.x4_xC_TopN = ply->xB0_xB8_pos;
    collided = Physics_CollisionCheck_Ground(&ply->x6F0_physics);

    ply->xB0_xB8_pos = ply->x6F0_physics.x4_xC_TopN;
    if (sub_80081A00(gobj) == TRUE) {
        return collided;
    }
    return FALSE;
}

//80082708
u32 Player_CollisionCheck_FallOffLedge(HSD_GObj *gobj)
{
    u32 res = 0;
  Player *player = GOBJ_PLAYER(gobj);

  (player->x6F0_physics).x1C_x24_TopN_prev.x = (player->x6F0_physics).x4_xC_TopN.x;
  (player->x6F0_physics).x1C_x24_TopN_prev.y = (player->x6F0_physics).x4_xC_TopN.y;
  (player->x6F0_physics).x1C_x24_TopN_prev.z = (player->x6F0_physics).x4_xC_TopN.z;

  (player->x6F0_physics).x4_xC_TopN.x = (player->xB0_xB8_pos).x;
  (player->x6F0_physics).x4_xC_TopN.y = (player->xB0_xB8_pos).y;
  (player->x6F0_physics).x4_xC_TopN.z = (player->xB0_xB8_pos).z;

  res = Physics_FallOffLedge(&player->x6F0_physics);

  (player->xB0_xB8_pos).x = (player->x6F0_physics).x4_xC_TopN.x;
  (player->xB0_xB8_pos).y = (player->x6F0_physics).x4_xC_TopN.y;
  (player->xB0_xB8_pos).z = (player->x6F0_physics).x4_xC_TopN.z;

  return res != 0;
}

//80083F88
void Player_Collision_KneeBend(HSD_GObj* gobj)
{
    u32 res = 0;

    Player* ply = GOBJ_PLAYER(gobj);
    ply->x6F0_physics.x1C_x24_TopN_prev = ply->x6F0_physics.x4_xC_TopN;
    ply->x6F0_physics.x4_xC_TopN = ply->xB0_xB8_pos;
    res = Physics_FallOffLedge(&ply->x6F0_physics);
    ply->xB0_xB8_pos = ply->x6F0_physics.x4_xC_TopN;
    if (res == 0) {
        ActionState_Fall(gobj);
    }
}

//80084DB0
void Player_CheckFastFallAndUpdate(HSD_GObj* gobj)
{
    Player* ply = GOBJ_PLAYER(gobj);
    Player_Interrupt_Fastfall(ply);
    if ((ply->x221A_flags >> 3) & 1) {
        Player_UpdateVelocityFromFastFall(ply);
    } else {
        Player_UpdateFallingVelocity(ply, ply->attribs.gravity, ply->attribs.termVel);
    }
    Player_UpdateHorzVelocity(ply);
}

//80085FD4
u32* Player_FetchAnimHeader(Player* player, u32 anim_id)
{
    return NULL;
    /*if(player->x4_internal_id == INTERNAL_NANA){
		u32 type = StaticPlayer_GetSlotType(player->xC_slot);
		if(type != 2){
			if(player->x24_state_ptr[anim_id]->x14_unk == 0){
				return *((u32*)((*(u32**)(0x804598B8 + 0x2C)) + 0xC)[anim_id]);
			}
		}
	}
	return *(u32*)(player->x24_state_ptr[anim_id]);*/
}

//800865C0
f32 Player_GetFacing(HSD_GObj* gobj)
{
    Player* ply = GOBJ_PLAYER(gobj);
    return ply->x2C_facedir;
}

//800865CC
BOOL Player_IsInAir(HSD_GObj* gobj)
{
    Player* ply = GOBJ_PLAYER(gobj);
    return ply->xE0_in_air;
}

//80086BE0
u8 Player_GetPort(HSD_GObj* gobj)
{
    Player* ply = GOBJ_PLAYER(gobj);
    return ply->xC_slot;
}

//80086C0C
u32 Player_GetActionState(HSD_GObj* gobj)
{
    Player* ply = GOBJ_PLAYER(gobj);
    return ply->x10_action_state;
}

//800872A4
u32 Player_GetInternalID(HSD_GObj* gobj)
{
    Player* ply = GOBJ_PLAYER(gobj);
    return ply->x4_internal_id;
}

//800872B0
u32* Player_Get20A4(HSD_GObj* gobj)
{
    Player* ply = GOBJ_PLAYER(gobj);
    return &ply->x20A4_unk;
}

//80087300
u32 Player_GetLastAtkerIdx(HSD_GObj* gobj)
{
    Player* ply = GOBJ_PLAYER(gobj);
    return ply->x18C4_last_atkeridx;
}

//80087460
u32 Player_GetSpawnCount(HSD_GObj* gobj)
{
    Player* ply = GOBJ_PLAYER(gobj);
    return ply->x8_spawn_ctr;
}

//80088148
void Player_PlaySFX(Player* player, u32 pitch_id, u32 unk1, u32 unk2)
{
}

//8008A4D4
void Player_Interrupt_Wait()
{
}

//80094818
void Player_80094818(HSD_GObj* gobj)
{
    Player* ply = GOBJ_PLAYER(gobj);
    if (ply->x1978_unk == 0) {
        if (ply->x1974_held_item != NULL) {
            /*if(OnItemPickup_Func[ply->x4_internal_id] != NULL){
                (*OnItemPickup_Func[ply->x4_internal_id])();
            }
            sub_8003E17C(player->slot, (player->field_0x221f >> 3) & 1, player->x1974_held_item);
            */
        } else {
            //sub_8003E17C(player->slot, (player->field_0x221f >> 3) & 1, NULL);
        }
    }
}

//800A2040
BOOL Player_IsCPU(Player* player)
{
    if (sub_8003248C(player->xC_slot, (player->x221F_flags >> 3) & 1) == TRUE) {
        return player->x1A94_cpu_flags != 5;
    }
    return FALSE;
}

//800D65B8
void Player_Collision_SquatWait(HSD_GObj* gobj)
{
    u32 res = 0;

    Player* ply = GOBJ_PLAYER(gobj);
    ply->x6F0_physics.x1C_x24_TopN_prev = ply->x6F0_physics.x4_xC_TopN;
    ply->x6F0_physics.x4_xC_TopN = ply->xB0_xB8_pos;
    res = Physics_FallOffLedge(&ply->x6F0_physics);
    ply->xB0_xB8_pos = ply->x6F0_physics.x4_xC_TopN;
    if (res == 0) {
        ActionState_Fall(gobj);
    }
}

//800DE9B8
BOOL Player_IsDPadUpInstantPressed(HSD_GObj* gobj)
{
    Player* player = GOBJ_PLAYER(gobj);
    return (player->x668_instant_buttons & 8) != 0;
}

//8011D518
BOOL Player_SwapItem(HSD_GObj* gobj)
{
    Player* player = GOBJ_PLAYER(gobj);
    player->x2238_flags = 0;
    player->x21E8_func = NULL;
    player->x21DC_Player_OnHit = NULL;
    HSD_GObj* item = player->x223C_pend_item;
    if (item != NULL) {
        player->x1974_held_item = item;
        player->x223C_pend_item = NULL;
        Item_Unhide(player->x1974_held_item);
        Item_8026B73C(player->x1974_held_item);
        sub_80094818(gobj, 1);
    }
    return item != NULL;
}

//8022697C
u8 Player_GetCollisionBubbleFlags(HSD_GObj* gobj)
{
    if (GObj_IsPlayer(gobj)) {
        Player* ply = GOBJ_PLAYER(gobj);
        return ply->x21FC_collbubble_flags;
    }
    return 0;
}
