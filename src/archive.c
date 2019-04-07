#include "archive.h"
#include "hsd/hsd_memory.h"

static s32 file_load_status = 0;

Archive_Allocation alloc_data[6];

//80015BD0
void* Archive_Alloc(u32 offset, u32 size){
    void* mem;
    u32 intr = IRQ_Disable();
    if(alloc_data[offset].x28_unk == 0){
        if(alloc_data[offset].x20_unk == 0){
            /* This is where the custom allocator would be useful
            OSHeapHandle heap_handle = HSD_GetHeap();
            HSD_SetHeap(alloc_data[offset].heap_handle);
            mem = HSD_MemAlloc(size);
            HSD_SetHeap(heap_handle);
            */
           mem = HSD_MemAlloc(size);
        }else{
            //mem = sub_80014FC8(alloc_data[offset].x14_unk, size);
            mem = HSD_MemAlloc(size);
            if(alloc_data[offset].x20_unk == 3){
                mem = (void*)((u32*)mem + 4);
            }
        }
    }else{
        mem = NULL;
    }
    IRQ_Restore(intr);
    return mem;
}

//800161A0
static s32 Archive_GetFileLoadStatus(){
    //sub_800195D0();
    return file_load_status;
}

//8001615C
static void Archive_DVDCallback(s32 result, DVDFileInfo* fileInfo){
    if(result >= 0){
        file_load_status = 1;
    }
    file_load_status = -1;
    HSD_Halt("DVDReadAsync failed");
}

//8001634C
u32 Archive_GetDVDFileLengthByEntry(s32 entry){
    u32 len;
    u32 intr = IRQ_Disable();
    DVDFileInfo handle;
    if(!DVDFastOpen(entry, &handle)){
        HSD_Halt("Archive_GetDVDFileLength: Could not open file");
    }
    len = handle.length;
    DVD_Close(&handle);
    IRQ_Restore(intr);
    return len;
}

//800163D8
u32 Archive_GetDVDFileLengthByName(char* filename){
    u32 len = 0;
    char* file_path = sub_80016204(filename);
    s32 entry = DVDConvertPathToEntrynum(file_path);
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

//8001668C
void Archive_LoadFileIntoMemory(char* filename, void* mem, u32* filelength){
    file_load_status = 0;
    //Archive_PathFromFilename(filename);
    s32 entry = DVDConvertPathToEntrynum(filename);
    if(entry == -1){
        HSD_Halt("Archive_LoadFileIntoMemory: Could not locate file");
    }
    *filelength = Archive_GetDVDFileLengthByEntry(entry);
    /*u32 unk;
    if((int)mem < 0x80000000) {
        unk = 0x23;
    }else {
        unk = 0x21;
    }
    sub_8038FD64(entry, 0, mem, *filelength + 0x1fU & 0xffffffe0, unk, 1, Archive_8001615C, 0);*/

    /* The below is unique to Fray in order to accomplish the file loading */
    DVDFileInfo handle;
    if(!DVDFastOpen(entry, &handle)){
        HSD_Halt("Archive_LoadFileIntoMemory: Could not open file");
    }
    DVDReadAsync(&handle, mem, (*filelength + 0x1F) & 0xFFFFFFE0, 0, Archive_DVDCallback);
    s32 status = 0;
    do {
        status = Archive_GetFileLoadStatus();
    } while (status == 0);
}

//80016A54
void Archive_InitializeDAT(s32* header_info, s32* dat_file, u32 file_size){
    assert(Archive_InitHeaderInfo(header_info, dat_file, file_size) != -1);
    u32 offset = 0;
    s32 val = NULL;
    while(true){
        char* str = Archive_GetString(header_info, offset++);
        if ( str == NULL )
            break;
        Archive_InitXrefs(header_info, str, 0);
    }
}

//80016C64
void Archive_LoadFileSections(char* filename, u32 sections, ...){
    va_list ap;

    u32 file_size = Archive_GetDVDFileLengthByName(filename);
    void* dat_file = Archive_Alloc(0, (file_size + 0x1F) & 0xFFFFFFE0); //This (size + 0x1F) & 0xFFFFFFE0 aligns the file size along 0x20 sized boundaries, IE Anything from 0x74581 - 0x745A0 would become 0x745A0
    void* header_info = Archive_Alloc(0, 0x44);
    u32* filelength;
    Archive_LoadFileIntoMemory(filename, dat_file, &filelength);
    Archive_InitializeDAT((s32*)header_info, (s32*)dat_file, filelength);

    va_start(ap, sections);
    while(sections > 0) {
        void* file_ptr = va_arg(ap, void *);
        char* section_name = va_arg(ap, char *);
        file_ptr = Archive_GetFileSection(header_info, section_name);
        if(file_ptr == NULL){
            HSD_Halt("Could not find section");
        }
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
char* Archive_GetString(s32* src, u32 offset){
    if((offset > -1) && (offset < src[4])){
        u32* string_ptr = (src[12] + *(s32*)(src[11] + offset * 8 + 4));
        return (char*)string_ptr;
    }
}

//80380434
void Archive_InitXrefs(s32* header_info, char* str, u32 val){
    u32 xref = 0;
    s32 n = -1;
    u32 offset = 0;
    while(header_info[4] < xref){
        s32 res = strcmp(str, (char*)(header_info[12] + *(u32*)(header_info[11] + offset + 4)));
        if(res == 0){
            u32* addr = (u32*)header_info[11];
            n = addr[2 * xref];
            break;
        }
        offset += 8;
        ++xref;
    }
    if(n != -1){
        while ( n != -1 && n < header_info[1] ){
            u32* addr = (u32*)(header_info[8] + n);
            n = *addr;
            *addr = val;
        }
    }
}

//803810E4
s32 Archive_InitHeaderInfo(s32* dst, s32* src, u32 file_size){
    if(dst == NULL){
        return -1;
    }else{
        memset(dst, 0, 0x44);
        dst[0xF] = dst[0xF] | 1;
        memcpy(dst, src, 0x20);
        if(dst[0] == file_size){
            u32 offset = 0x20;
            if(dst[1] != 0){ //Body Size
                dst[8] = src[8];
                offset = src[1] + 0x20;
            }
            if(dst[2] != 0){ //Relocation Size
                dst[9] = (s32)src + offset;
                offset = offset + dst[2] * 4;
            }
            if(dst[3] != 0){ //Root Size
                dst[10] = (s32)src + offset;
                offset = offset + dst[3] * 8;
            }
            if(dst[4] != 0){ //XRef Size
                dst[11] = (s32)src + offset;
                offset = offset + dst[4] * 8;
            }
            if(offset < dst[0]){ //File Size
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