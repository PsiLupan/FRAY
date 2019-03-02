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
u32 Archive_GetFileLength(char* filename){
    u32 len = 0;
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
    //len = handle.length;
    DVD_Close(&handle);
    IRQ_Restore(intr);
    return len;
}

//80016C64
void Archive_LoadFileSections(char* filename, void* dat_start, u32 sections, ...){
    va_list ap;

    u32 file_length = Archive_GetFileLength(filename);

    va_start(ap, sections);
    while(sections > 0) {
        char* section_name = va_arg(ap, char*);
        void* file_ptr = va_arg(ap, void*);
        file_ptr = Archive_GetFileSection(dat_start, section_name);
        sections -= 2;
    }
    va_end(ap);
}