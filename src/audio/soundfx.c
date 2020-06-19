#include "soundfx.h"

s32 sfx_804337C4[55];

//800236DC
void SFX_StopMusic()
{
}

//80024030
void SFX_Menu_Common(u32 id)
{
    if (id < 11) {
        //SFX_Play()
    }
}

//80026F2C
void SFX_80026F2C(u32 flags)
{
    /*u32* puVar3;
  bool bVar4;
  u64 uVar5;
  
  u32 uVar2 = 0;
  u32 flag_out = 0;
  if ((flags & 1) != 0) {
    uVar2 = 3;
    flag_out = 0x480000;
  }
  if ((flags & 2) != 0) {
    bVar4 = 0xffffffc3 < uVar2;
    uVar2 = uVar2 + 0x3c;
    flag_out = flag_out + (u32)bVar4;
  }
  if ((flags & 4) != 0) {
    bVar4 = 0x3f < uVar2;
    uVar2 = uVar2 - 0x40;
    flag_out = flag_out + (u32)bVar4 + 0x800003;
  }
  if ((flags & 8) != 0) {
    flag_out = flag_out + 0x23fffc;
  }
  if ((flags & 0x10) != 0) {
    flag_out = flag_out + 0x140000;
  }
  
  uVar5 = concat_to_64(flag_out, uVar2);
  puVar3 = &sfx_804337C4[0];
  flag_out = 0;
  do {
    if ((uVar5 & 1) != 0) {
      *puVar3 = 0xffffffff;
    }
    uVar5 = __shr2u((uVar5 >> 32), uVar5, 1);
    flag_out = flag_out + 1;
    puVar3 = puVar3 + 1;
  } while (flag_out < 55);*/
}

//8002702C
void SFX_RequestAudioLoad(u8 id, u32 unused, u32 unk_r5, u32 unk_r6)
{
}

//80027618
void SFX_AudioCacheUpdate()
{
}

//80027648
void SFX_80027648()
{
}

//80027DF8
void SFX_ProcessVolume()
{
}

//8038CFF4
//SFX_Play(){}