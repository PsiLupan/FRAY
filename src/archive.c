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
u32 Archive_GetFileSize(char* filename){
    u32 len = 0;
    char* file_path = sub_80016204(filename);
    s32 entry = DVDConvertPathToEntryNum(file_path);
    if(entry == -1){
        HSD_Halt("Archive_GetFileSize: Could not locate file");
    }
    u32 intr = IRQ_Disable();
    DVDFileInfo handle;
    if(!DVDFastOpen(entry, &handle)){
        HSD_Halt("Archive_GetFileSize: Could not open file");
    }
    len = handle.length;
    DVDClose(&handle);
    IRQ_Restore(intr);
    return len;
}

//80016A54
void Archive_InitializeDAT(void* mem_1, void* mem_2, u32 file_size){
    assert(Archive_Parse(mem_1, mem_2, file_size) != -1);
    u32 counter = 0;
    s32 val = NULL;
    void* temp;
    while(true){
        val = Archive_Check((s32*)mem_1, counter++);
        temp = val;
        if ( val == 0 )
            break;
        val = sub_80380434(mem_1, val, 0);
    }
}

//80016C64
void Archive_LoadFileSections(char* filename, void* dat_start, u32 sections, ...){
    va_list ap;

    u32 file_size = Archive_GetFileSize(filename);
    void* mem_1 = sub_80015BD0(0, (file_size + 31) & 0xFFFFFFE0);
    void* mem_2 = sub_80015BD0(0, 68);
    sub_8001688C(filename, mem_2, unk_stack_obj);
    Archive_InitializeDAT((u32*)mem_2, (u32*)mem_1, file_size);

    //Still TODO here w/ Mallocs, etc..

    va_start(ap, sections);
    while(sections > 0) {
        void* file_ptr = va_arg(ap, void *);
        char* section_name = va_arg(ap, char *);
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

//803803FC
s32 Archive_Check(u32* src, u32 counter){
    if((counter > -1) && (counter < src[4])){
        return src[12] + *(s32*)(src[11] + counter * 8 + 4);
    }
}

//803810E4
s32 Archive_Parse(s32* dst, s32* src, u32 file_size){
    if(dst == NULL){
        return -1;
    }else{
        memset(dst, 0, 0x44);
        dst[0xF] = dst[0xF] | 1;
        memcpy(dst, src, 0x20);
        if(dst[0] == file_size){
            u32 offset = 0x20;
            if(dst[1] != 0){
                dst[8] = src[8];
                offset = src[1] + 0x20;
            }
            if(dst[2] != 0){
                dst[9] = (s32)src + offset;
                offset = offset + dst[2] * 4;
            }
            if(dst[3] != 0){
                dst[10] = (s32)src + offset;
                offset = offset + dst[3] * 8;
            }
            if(dst[4] != 0){
                dst[11] = (s32)src + offset;
                offset = offset + dst[4] * 8;
            }
            if(offset < dst[0]){
                dst[12] = (s32)src + offset;
            }
            u32 iter = 0;
            *(u32**)(dst + 0x10) = src;
            offset = 0;
            for(u32 i = 0; i < dst[2]; i++){
                u32 val = dst[8];
                u32* ptr = (u32*)(dst[9] + offset);
                offset += 4;
                *(u32*)(val + *ptr) = *(u32*)(val + *ptr) + val; 
            }
        }else{
            HSD_Panic("Archive_Parse: Byte-Order mismatch");
            return -1;
        }
    }
    return 0;
}