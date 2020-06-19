#ifndef _soundfx_h_
#define _soundfx_h_

#include <gctypes.h>

void SFX_StopMusic();
void SFX_Menu_Common(u32);
void SFX_80026F2C(u32);
void SFX_RequestAudioLoad(u8, u32, u32, u32);
void SFX_AudioCacheUpdate();
void SFX_80027648();
void SFX_ProcessVolume();

#endif
