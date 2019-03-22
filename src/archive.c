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
    DVD_Close(&handle);
    IRQ_Restore(intr);
    return len;
}

//80016A54
void Archive_InitializeDAT(void* mem_1, void* mem_2, u32 file_length){
    assert(sub_803801E4(mem_1, mem_2, file_length) != -1);
    u32 counter = 0;
    void* val = NULL;
    void* temp;
    do
    {
        val = (unsigned __int8 *)sub_803803FC(mem_1, counter++);
        temp = val;
        if ( val != NULL )
            val = (unsigned __int8 *)sub_80380434(mem_1, val, 0);
    }
    while ( temp );
}

//80016C64
void Archive_LoadFileSections(char* filename, void* dat_start, u32 sections, ...){
    va_list ap;

    u32 file_length = Archive_GetFileLength(filename);
    void* mem_1 = sub_80015BD0(0, (file_length + 31) & 0xFFFFFFE0);
    void* mem_2 = sub_80015BD0(0, 68);
    sub_8001688C(filename, mem_2, unk_stack_obj);
    sub_80016A54(mem_2, mem_1, file_length);

    va_start(ap, sections);
    while(sections > 0) {
        char* section_name = va_arg(ap, char *);
        void* file_ptr = va_arg(ap, void *);
        file_ptr = Archive_GetFileSection(dat_start, section_name);
        sections -= 2;
    }
    va_end(ap);
}