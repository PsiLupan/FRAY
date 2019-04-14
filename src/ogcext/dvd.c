#include "dvd.h"

#include <ogc/lwp.h>
#include <ogc/irq.h>

s32 dvd_fd = -1;
static lwpq_t dvd_wait_queue;

void DVDInit(){
    DVD_Init();
    DVD_Mount();
    LWP_InitQueue(&dvd_wait_queue);
}


BOOL DVDFastOpen(s32 entrynum, dvdfileinfo* fileinfo){
    if(entrynum >= 0){
        if(){
            return TRUE;
        }
    }
    return FALSE;
}

static void cbForCancelAsync(s32 result, dvdcmdblk* cmd){
    LWP_ThreadBroadcast(dvd_wait_queue);
}

BOOL DVDCancelAsync(dvdcmdblk* cmd, dvdcbcallback callback){
    u32 intr = IRQ_Disable();
    switch(cmd->state){
        case 1:{

        }
        break;

        case 2:{

        }
        break;

        case 3:{

        }
        break;

        case 4:
        case 5:
        case 6:
        case 7:
        case 11:{

        }
        break;

        case -1:
        case 0:
        case 10:{
            if(callback != NULL){
                (*callback)(0, cmd);
            }
        }
        break;
    }
    IRQ_Restore(intr);
    return TRUE;
}

s32 DVDCancel(dvdcmdblk* cmd){
    if(DVDCancelAsync(cmd, cbForCancelAsync) == FALSE){
        return -1;
    }else{
        u32 intr = IRQ_Disable();
        while(true){
            s32 state = cmd->state;
            if(1 >= (state + 1) || state == 10 || state == 3){
                break;
            }
            u32 command = cmd->cmd;
            if(1 >= (command - 4) || command == 13 || command == 15){
                break;
            }
            LWP_ThreadSleep(dvd_wait_queue);
        }
        IRQ_Restore(intr);
        return 0;
    }
}

BOOL DVDClose(dvdfileinfo* fileinfo){
    DVDCancel(&fileinfo->block);
    return TRUE;
}