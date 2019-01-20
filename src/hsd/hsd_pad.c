#include "hsd_pad.h"

u8 pad_queue[0x400]; //Length is made up

//80376D04
void HSD_PadFlushQueue(u32 state){
    u32 intr = IRQ_Disable();
    switch(state){
        case 0:
        while(pad_queue[3] > 1){
            //TODO
        }
        break;

        case 1:
        pad_queue[1] = pad_queue[2];
        pad_queue[3] = 0;
        break;

        case 2:
        if(pad_queue[3] > 1){
            u8 num = 0;
            if(pad_queue[2] > 0){
                num = pad_queue[2] - 1;
            }else{
                num = pad_queue[0] - 1;
            }
            pad_queue[1] = num;
            pad_queue[3] = 1;
        }
        break;
    }
    IRQ_Restore(intr);
}

//80377D18
void HSD_PadReset(){
    u32 intr = IRQ_Disable();
    HSD_PadRumbleRemoveAll();
    u32 count = 0;
    do {
        HSD_PadRumbleOffH(count++);
    } while (count < 4);
    HSD_PadFlushQueue(1);
    PAD_Recalibrate(0xF0000000);
    pad_queue[43] = 0;
    IRQ_Restore(intr);
}