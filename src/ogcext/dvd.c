#include "dvd.h"

#include <ogc/lwp.h>
#include <ogc/irq.h>

s32 dvd_fd = -1;
static total_entries = 0; //r13_441C
static char (*string_table)[] = NULL; //r13_4420
static FSTEntry (*entry_table)[] = NULL; //r13_4424
static u32* start_memory = NULL; //r13_4428
static lwpq_t dvd_wait_queue;

static void __DVDFSInit(){
    start_memory = (u32*)(0x80000000);
    entry_table = (FSTEntry*)start_memory[14]; //0x80000038 points to the start of the file system
    if(entry_table == NULL){
        return;
    }
    total_entries = (*entry_table)[0].len;
    string_table = &entry_table[total_entries];
}

void DVDInit(){
    DVD_Init();
    DVD_Mount();
    __DVDFSInit();
    LWP_InitQueue(&dvd_wait_queue);
}

BOOL DVDFastOpen(s32 entrynum, dvdfileinfo* fileinfo){
    if(entrynum >= 0 && entrynum < total_entries){
        u32 data = (*entry_table)[entrynum].data;
        if((data & 0xFF000000) == T_FILE){
            fileinfo->addr = = (*entry_table)[entrynum].addr;
            fileinfo->len = (*entry_table)[entrynum].len;
            fileinfo->cb = NULL;
            fileinfo->cmd.state = 0;
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