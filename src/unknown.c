#include "unknown.h"

unk_80432078 dword_80432078;
unk_8046B0F0 dword_8046B0F0;

//80018F58
void unkstatestruct_set970(void* scene_ptr){
	unk970 = scene_ptr;
}


/*
//8001822C
struct unk_8043207C* GetStruct8043207C(){
  return &dword_8043207C;
}

//801A3F48
u32* sub_801A3F48(u32* a1){
  u32 v1; // r31@1
  u32 v2; // r0@1
  u32 *v3; // r31@9

  v1 = a1;
  sub_80018CF4(*(u8*)(a1 + 1));
  v2 = *(u8*)(v1 + 12);
  if ( v2 == 8 )
  {
    sub_803A6048(9216);
    goto LABEL_9;
  }
  if ( v2 < 8 )
  {
    if ( v2 != 5 )
      goto LABEL_8;
LABEL_6:
    sub_803A6048(49152);
    goto LABEL_9;
  }
  if ( v2 == 43 )
    goto LABEL_6;
LABEL_8:
  sub_803A6048(18432);
LABEL_9:
  v3 = GetStruct8043207C();
  if ( !sub_80015BB8(2) )
    *v3 = 1;
  if ( !sub_80015BB8(3) )
    v3[1] = 1;
  sub_80018254();
  sub_8001C5A4();
  sub_8001D1F4();
  sub_8001E27C();
  sub_803127D4();
  return sub_8031C8B8();
}

//----- (801A3EF4) --------------------------------------------------------
struct SceneInfo* sub_801A3EF4(){
  struct SceneInfo *i; // r31@1
  
  for ( i = &scene_info; *(_BYTE *)(i + 1) != 45 && !*(_DWORD *)(i + 12); i += 20 )
    ;
  return &scene_info;
}

//----- (801A427C) --------------------------------------------------------
u32 sub_801A427C(u32 a1){
  return *(_DWORD *)(a1 + 16);
}

//----- (801A4284) --------------------------------------------------------
u32 sub_801A4284(u32 a1){
  return *(_DWORD *)(a1 + 20);
}

//----- (801A4CE0) --------------------------------------------------------
u8* sub_801A4CE0(u8 a1)
{
  u8 v1; // r31@1
  u8 *result; // r3@1
  u32 v3; // r4@4

  v1 = a1;
  for ( result = (u8*)sub_801A50A0(); ; result += 20 )
  {
    v3 = *result;
    if ( v3 == 45 )
      break;
    if ( v3 == v1 )
      return result;
  }
  return 0;
}*/