#include "dvd.h"

#include <ogc/lwp.h>
#include <ogc/irq.h>

static dvdcbcallback cb = NULL; //r13_43E4
static u32 status = 0; //r13_43E8
static u32 total_entries = 0; //r13_441C
static char (*string_table)[] = NULL; //r13_4420
static FSTEntry (*entry_table)[] = NULL; //r13_4424
static u32* start_memory = NULL; //r13_4428
static lwpq_t dvd_wait_queue;

typedef FSTEntry (*TableP)[];
typedef char (*STableP)[];

static void __DVDFSInit(){
    start_memory = (u32*)(0x80000000);
    entry_table = (TableP)start_memory[14]; //0x80000038 points to the start of the file system
    if(entry_table == NULL){
        return;
    }
    total_entries = (*entry_table)[0].len;
    string_table = (STableP)(&(*entry_table)[total_entries]);
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
            fileinfo->addr = (*entry_table)[entrynum].addr;
            fileinfo->len = (*entry_table)[entrynum].len;
            fileinfo->cb = NULL;
            fileinfo->block.state = 0;
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
        case DVD_STATE_BUSY:{
            
        }
        break;

        case DVD_STATE_WAITING:{
            if(cmd->cb != NULL){
                (*cmd->cb)(0, cmd);
            }
        }
        break;

        case 3:{
            switch(cmd->cmd){
                case 4:
                case 5:
                case 13:
                case 15:{
                    if(callback != NULL){
                        (*callback)(0, cmd);
                    }
                }
                if(status != 0){
                    IRQ_Restore(intr);
                    return 0;
                }
                cb = callback;
                status = 1;
                break;
            }
        }
        break;

        case DVD_STATE_NO_DISK:
        case DVD_STATE_COVER_OPEN:
        case DVD_STATE_WRONG_DISK:
        case DVD_STATE_MOTOR_STOPPED:
        case DVD_STATE_RETRY:{
            switch(cmd->state){
                case 4:
                status = 3;
                break;

                case 5:
                status = 4;
                break;

                case 6:
                status = 1;
                break;

                case 7:
                status = 7;
                break;

                case 11:
                status = 1;
                break;
            }
            cmd->state = 10;
            if(cmd->cb != NULL){
                (*cmd->cb)(DVD_ERROR_CANCELED, cmd);
            }
            if(callback != NULL){
                (*callback)(0, cmd);
            }
        }
        break;

        case DVD_STATE_FATAL_ERROR:
        case DVD_STATE_END:
        case DVD_STATE_CANCELED:{
            if(callback != NULL){
                (*callback)(DVD_ERROR_OK, cmd);
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
            if(1 >= (state + 1) || state == DVD_STATE_CANCELED || state == DVD_STATE_COVER_CLOSED){
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
    //DVDCancel(&fileinf->cmd);
    DVD_Reset(DVD_RESETSOFT);
    return TRUE;
}

typedef struct _entry { 
    const char* name; 
    s32 entrynum; 
} entry;

entry fst_files[] = {
    {"/audio/opening.hps", 107},
    {"/audio/us/kirby.ssm", 161},
    {"/audio/us/link.ssm", 167},
    {"/audio/us/luigi.ssm", 168},
    {"/audio/us/main.ssm", 169},
    {"/audio/us/smash2.sem", 190},
    {"GmTtAll.dat", 381},
    {"GmTtAll.usd", 382},
    {"LbRb.dat", 492},
    {"LbRf.dat", 493},
    {"MnExtAll.dat", 494},
    {"MnExtAll.usd", 495},
    {"MnMaAll.dat", 496},
    {"MnMaAll.usd", 497},
    {"MvOpen.mth", 530}
};

/* 
Cheater function where we have a lookup table to respond to every request
*/
s32 DVDConvertFilenameToEntrynum(char* filename){
    for(entry *p = fst_files; p->name != NULL; ++p){
        if(strcmp(p->name, filename) == 0){
            return p->entrynum;
        }
    }
    return -1;
}

s32 DVDConvertPathToEntrynum(char* filepath){
    
}
