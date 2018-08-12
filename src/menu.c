#include "menu.h"

//8022BFBC
u32 sub_8022BFBC(u32 result){
  if ( result == 2 )
    return 0x0ED241FF;
  if ( result < 2 )
  {
    if ( result )
    {
      if ( result >= 0 )
        result = 0xFF5A41FF;
    }
    else
    {
      result = 0x5A73FFFF;
    }
  }
  else if ( result == 4 )
  {
    result = 0x9B41FFFF;
  }
  else if ( result < 4 )
  {
    result = 0xF0C85AFF;
  }
  return result;
}

//8022C010
s32 sub_8022C010(s32 result, s32 a2){
  if ( !result )
    return a2;
  switch ( result )
  {
    case 0:
      return result;
    case 1:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 33:
      result = 0;
      break;
    case 2:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
      result = 1;
      break;
    case 3:
      result = 2;
      break;
    case 4:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
      result = 3;
      break;
    case 5:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
    case 32:
      result = 4;
      break;
  }
  return result;
}

//8015CDEC
u8* sub_8015CDEC(){
	u8 *result; // r3@2
	
	for (s32 i = 0; i < 6; ++i){
		result = (u8 *)sub_8015CE44(i, 120);
		if ( result )
			*result = 5;
	}
	return result;
}

//8015CE44
int sub_8015CE44(s32 a1, int a2)
{
  int v2; // r13@0

  if ( a2 != 120 )
    return *(u32 *)(v2 - 30656) + 7980 * (a2 / 19) + 420 * (a2 % 19) + 12698;
  if ( a1 >= 4 )
    return 0;
  return *(u32 *)(v2 - 30656) + a1 + 1416;
}


//801A5598
u8* Menu_GmRst_StartupInit(){
	Menu_InitUsableStructs((s8*)&sm_regularvs);
	return sub_8015CDEC();
}

//801B2298
u32 Menu_GmTrain_StartupInit(){
  s8 *v1; // r31@1
  s32 v2; // ctr@1
  u8 *v3; // r6@1
  s32 v4; // r5@1
  u32 v5; // r7@1
  u32 v6; // r7@4
  u32 v7; // r0@4
  u32 v8; // r0@4
  u32 v9; // r0@4
  u32 v10; // r0@4
  u32 *v11; // r6@4
  s32 *v12; // r5@4
  u32 v13; // r0@6
  u32 v14; // r0@6
  u32 v15; // r0@6
  u32 result; // r3@6
  u32 v17; // r0@6
  u32 v18; // r0@6

  v1 = (s8*)&sm_train; //StartMelee ptr
  Menu_InitUsableStructs(v1);
  v2 = 2;
  v3 = 0; //byte_80473814
  v4 = (s32)v1;
  v5 = 0;
  do
  {
    *(u8 *)(v4 + 107) = v5;
    *(u8 *)(v4 + 118) = 0;
    if ( v5 )
      v1[140] = 33;
    v6 = v5 + 1;
    v7 = *(s32 *)(v4 + 108);
    *((s32 *)v3 + 5) = *(s32 *)(v4 + 104);
    *((s32 *)v3 + 6) = v7;
    v8 = *(s32 *)(v4 + 116);
    *((s32 *)v3 + 7) = *(s32 *)(v4 + 112);
    *((s32 *)v3 + 8) = v8;
    v9 = *(s32 *)(v4 + 124);
    *((s32 *)v3 + 9) = *(s32 *)(v4 + 120);
    *((s32 *)v3 + 10) = v9;
    v10 = *(s32 *)(v4 + 132);
    *((s32 *)v3 + 11) = *(s32 *)(v4 + 128);
    *((s32 *)v3 + 12) = v10;
    *((s32 *)v3 + 13) = *(s32 *)(v4 + 136);
    v11 = v3 + 36;
    *(u8 *)(v4 + 143) = v6;
    *(u8 *)(v4 + 154) = 0;
    v12 = (s32 *)(v4 + 36);
    if ( v6 )
      v1[140] = 33;
    v5 = v6 + 1;
    v13 = v12[27];
    v11[5] = v12[26];
    v11[6] = v13;
    v14 = v12[29];
    v11[7] = v12[28];
    v11[8] = v14;
    v15 = v12[31];
    v11[9] = v12[30];
    v11[10] = v15;
    result = v12[32];
    v17 = v12[33];
    v11[11] = result;
    v11[12] = v17;
    v18 = v12[34];
    v4 = (s32)(v12 + 9);
    v11[13] = v18;
    v3 = (u8 *)(v11 + 9);
    --v2;
  }
  while ( v2 );
  return result;
}

//801A55C4
//801A55EC
u8* Menu_ZeroFillPtr(){
	memset(&menu_804D6730, 0, 6);
	return &menu_804D6730[5];
}

//8016795C
u32 Menu_InitStartMeleeData(s8 *a1){
  s8 *v1 = a1; // r31@1
  u32 result; // r3@1
  double v4; // fp0@1

  memset(a1, 0, 0x24u);
  *v1 = 33;
  v1[1] = 3;
  v1[2] = 0;
  v1[3] = 0;
  v1[4] = 0;
  v1[5] = -1;
  v1[6] = 0;
  v1[11] = 0;
  v1[7] = 0;
  v1[8] = 9;
  v1[9] = 0;
  v1[12] &= 0x7Fu;
  v1[10] = 120;
  result = (u8)(v1[12] & 0xBF) | 0x40;
  v1[12] = result;
  v1[14] = 4;
  v1[15] = 0;
  *((u16 *)v1 + 9) = 0;
  *((u16 *)v1 + 10) = 0;
  v4 = 1.0f;
  *((f32 *)v1 + 6) = 1.0f;
  *((f32 *)v1 + 7) = v4;
  *((f32 *)v1 + 8) = v4;
  return result;
}

//80167A14
u32 __InitStartMeleeData(s8 *a1){
	s8 *v2 = a1;
	u32 result;

	for(u32 i = 0; i < 6; ++i){
		result = Menu_InitStartMeleeData(v2);
		v2 += 36;
	}
	return result;
}

//80167B50
u32 Menu_InitUsableStructs(s8* sm_struct){
  u32 result; // r3@2

  Match_InitStartMeleeStruct(&sm_struct[8]);
  result = __InitStartMeleeData(&sm_struct[0x68u]);
  sm_struct[0] = -1;
  sm_struct[1] = -1;
  sm_struct[2] = -1;
  return result;
}