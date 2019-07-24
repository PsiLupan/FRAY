#include "hsd_pad_ext.h"

//80019894
s8 Pad_CheckQueue(){
  u32 intr = IRQ_Disable();
  s8 count = HSD_PadGetRawQueueCount();
  //sub_80019628 - Scene_CheckRunSpeedAdjust();
  IRQ_Restore(intr);
  return count;
}

//800198E0
void Pad_Renew(){
  HSD_PadRenewMasterStatus();
}

//801A3A74
void Pad_CheckInputs(){
  
}