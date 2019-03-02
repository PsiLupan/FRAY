#include "archive.h"

//8001634C
void* Archive_LoadFileByEntryNum(s32 entry){
    void* addr;
    u32 intr = IRQ_Disable();
    DVDHandle handle;
    if(!DVDFastOpen(entry, &handle)){
        HSD_Halt("Archive_LoadFileByEntryNum: Could not open file");
    }
    //addr = handle.addr;
    DVD_Close(&handle);
    IRQ_Restore(intr);
    return addr;
}

//800163D8
void* Archive_LoadFile(char* filename){
    void* addr = NULL;
    char* file_path = sub_80016204(filename);
    s32 entry = DVDConvertPathToEntryNum(file_path);
    if(entry == -1){
        HSD_Halt("Archive_LoadFile: Could not locate file");
    }
    u32 intr = IRQ_Disable();
    DVDHandle handle;
    if(!DVDFastOpen(entry, &handle)){
        HSD_Halt("Archive_LoadFile: Could not open file");
    }
    //addr = handle.addr;
    DVD_Close(&handle);
    IRQ_Restore(intr);
    return addr;
}

//80016C64
void Archive_LoadFileSections(char* filename, void** file_ptr, char* section_1, void** section_1_ptr, char* section_2, void** section_2_ptr, char* section_3, void** section_3_ptr){
    void* file_addr = Archive_LoadFile(filename);
}