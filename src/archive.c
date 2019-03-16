#include "archive.h"

//8001634C
void* Archive_LoadFileByEntryNum(s32 entry){
    void* addr;
    u32 intr = IRQ_Disable();
    DVDFileInfo handle;
    if(!DVDFastOpen(entry, &handle)){
        HSD_Halt("Archive_LoadFileByEntryNum: Could not open file");
    }
    addr = handle.startAddr;
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
        HSD_Halt("Archive_GetFileLength: Could not locate file");
    }
    u32 intr = IRQ_Disable();
    DVDFileInfo handle;
    if(!DVDFastOpen(entry, &handle)){
        HSD_Halt("Archive_GetFileLength: Could not open file");
    }
    len = handle.length;
    DVDClose(&handle);
    IRQ_Restore(intr);
    return len;
}

//80016C64
void Archive_LoadFileSections(char* filename, void* dat_start, u32 sections, ...){
    va_list ap;

    u32 file_length = Archive_GetFileLength(filename);

    va_start(ap, sections);
    while(sections > 0) {
        char* section_name = va_arg(ap, char *);
        void* file_ptr = va_arg(ap, void *);
        file_ptr = Archive_GetFileSection(dat_start, section_name);
        sections -= 2;
    }
    va_end(ap);
}

//80380358
void* Archive_GetFileSection(void* dat_start, char* section_name){
    u32 iters;
    u32 offset;
    
    iters = 0;
    offset = 0;
    while( true ) {
        if (*(u32 *)((u32)dat_start + 0xc) <= iters) { //Number of root nodes < iterations
            return NULL;
        }
        s32 cmp_result = strcmp((char *)(*(s32 *)((u32)dat_start + 0x30) + *(u32 *)(*(u32 *)((u32)dat_start + 0x28) + offset + 4)), section_name);
        if (cmp_result == 0) //If both strings are equal, we've found the node
            break;
        offset = offset + 8;
        iters += 1;
    }
    return (void *)(*(u32 *)((u32)dat_start + 0x20) + *(u32 *)(*(u32 *)((u32)dat_start + 0x28) + iters * 8));
}