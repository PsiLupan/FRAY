#include "scene.h"
#include "unknown.h"

//801A4014
/*u32* Scene_RunFunc(u8 a1)
{
  u32 v1; // r28@1
  u32 v2; // r4@1
  u8 *v3; // r5@1
  u32 v4; // ctr@1
  u8 *v5; // r6@2
  int i; // r3@2
  int v7; // r27@4
  int v8; // r0@6
  u32 *result; // r3@10
  u32 *v10; // r26@11
  u8 *v11; // r7@21
  int v12; // r4@21
  u8 *v13; // r6@21
  u8 *v14; // r3@21
  int v15; // r3@23
  signed int v16; // r3@31

  v1 = a1;
  v2 = game_state.unk03;
  v3 = *(u8 **)(a1 + 16);
  v4 = 255 - game_state.unk03;
  if ( game_state.unk03 < 255 )
  {
    do
    {
      v5 = v3;
      for ( i = 0; ; ++i )
      {
        v8 = *v5;
        if ( v8 == 255 )
          break;
        if ( v2 == v8 )
        {
          v7 = (int)&v3[24 * i];
          goto LABEL_9;
        }
        v5 += 24;
      }
      ++v2;
      --v4;
    }
    while ( v4 );
  }
  v7 = 0;
LABEL_9:
  game_state.unk03 = *(u8 *)v7;
  sub_801A3F48(v7);
  if ( *(_DWORD *)(v7 + 4) )
    return (_DWORD *)v7;
  v10 = (int *)sub_801A4CE0(*(u8 *)(v7 + 12));
  sub_801A4BD4();
  sub_801A4B88(v7 + 12);
  if ( v10[2] )
    return *(_DWORD **)(v7 + 16);
  sub_801A4D34(v10[1]);
  if ( !dword_8046B0F0.unk04 && v10[3] )
    return *(_DWORD **)(v7 + 20);
  if ( !dword_8046B0F0.unk04 )
  {
    if ( *(_DWORD *)(v7 + 8) )
      return (_DWORD *)v7;
    game_state.unk04 = game_state.unk03;
    if (game_state.unk04)
    {
      game_state.unk03 = game_state.unk04 - 1;
      game_state.unk05 = 0;
    }
    else
    {
      v11 = *(u8 **)(v1 + 16);
      v12 = 0;
      v13 = *(u8 **)(v1 + 16);
      v14 = *(u8 **)(v1 + 16);
      while ( *v13 != 255 )
      {
        if ( *v14 > (unsigned int)(u8)game_state.unk03 )
        {
          LOBYTE(v15) = v11[24 * v12];
          goto LABEL_28;
        }
        v14 += 24;
        v13 += 24;
        ++v12;
      }
      v15 = *v11;
      if ( v15 == 255 )
        LOBYTE(v15) = 0;
LABEL_28:
      game_state.unk03 = v15;
    }
  }
  sub_8001CDB4();
  sub_8001B760(11);
  result = sub_8001F800();
  if ( dword_8046B0F0.unk04 )
  {
    sub_80027DBC((unsigned int)result);
    sub_80377D18();
    do {
      v16 = sub_8001B6F8();
	} while ( v16 == 11 );
    if ( !DVD_CheckDisk(v16) )
      SYS_ResetSystem(1u, 0, 0);
    sub_8001F800();
    while (sub_8038EA50(1));
    CheckLanguage();
    memset(&game_state, 0, 20);
    sub_801A3EF4();
    dword_8046B0F0.unk00 = 1;
    Scene_SetPendingMajor(40);
    VIDEO_SetBlack(FALSE);
	result = 0;
  }
  return result;
}*/

//801A4310
u8 Scene_LoadCurrentMajor(){
	return game_state.curr_major;
}

//801A42E8
u8 Scene_UpdatePendingMajor(u8 val){
	game_state.pending_major = val;
	return val;
}

//801A42F8
u8 Scene_SetPendingMajor(u8 val){
	game_state.pending_major = val;
	game_state.pending = true;
	return val;
}

//801A42D4
struct GameState* Scene_SetPendingTrue(){
	game_state.pending = true;
	return &game_state;
}

//801A4320
u8 Scene_LoadPrevMajor(){
	return game_state.prev_major;
}

//801A42C4
u8 Scene_Get03(){
	return game_state.unk03;
}

//801A428C
u8 Scene_Set03And04(u8 val){
	game_state.unk03 = val;
	game_state.unk04 = val;
	return val;
}

//801A42B4
u8 Scene_Get04(){
  return game_state.unk04;
}

//801A42A0
struct GameState* Scene_Set05(u8 val){
	u8 v1;
	v1 = val + 1;
	game_state.unk05 = v1;
	return &game_state;
}

//801A4330
u32 Scene_StoreTo10(u32 val){
	game_state.unk10 = val;
	return val;
}

//801A4340
bool Scene_IsSinglePlayer(u8 scene){
	if ( scene == 28 )
		return true;
	if ( scene >= 28 )
	{
		if ( scene != 43 && (scene >= 43 || scene >= 39 || scene < 32) )
			return false;
		return true;
	}
	if ( scene == 15 || (scene < 15 && scene < 6 && scene >= 3) )
		return true;
	return false;
}
