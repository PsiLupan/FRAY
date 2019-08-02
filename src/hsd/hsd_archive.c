#include "hsd_archive.h"

#include <string.h>

//80380358
void* HSD_ArchiveGetPublicAddress(HSD_Archive* archive, char* symbols){
    u32 iters;
    u32 offset;
    
    iters = 0;
    offset = 0;
    while( true ) {
        if (archive->header.nb_public <= iters) {
            return NULL;
        }
        s32 cmp_result = strcmp(archive->symbols + *(s32 *)((s32)&archive->public_info->symbol + offset), symbols);
        if (cmp_result == 0) //If both strings are equal, we've found the node
            break;
        offset += 8;
        iters += 1;
    }
    return archive->data + archive->public_info[iters].offset;
}

//803803FC
char* HSD_ArchiveGetExtern(HSD_Archive* archive, u32 offset){
    if((offset > -1) && (offset < archive->header.nb_extern)){
        return archive->symbols + archive->extern_info[offset].symbol;
    }
    return NULL;
}

//80380434
void HSD_ArchiveLocateExtern(HSD_Archive* archive, char* symbols, void* addr){
    u32 xref = 0;
    s32 n = -1;
    u32 offset = 0;
    do {
        if(archive->header.nb_extern <= xref){
TOP:
            if(n != -1){
                while ( n != -1 && n < archive->header.data_size){
                    s32* data = (s32*)(archive->data + n);
                    n = *data;
                    data = addr;
                }
            }
        }
        s32 res = strcmp(symbols, archive->symbols + *(u32*)(archive->extern_info->symbol + offset));
        if(res == 0){
            n = archive->extern_info[xref].offset;
            goto TOP;
        }
        offset += 8;
        ++xref;
    } while (true);
}

//803810E4
s32 HSD_ArchiveParse(HSD_Archive* archive, u8* src, u32 file_size){
    if(archive == NULL){
        return -1;
    }else{
        memset(archive, 0, sizeof(HSD_Archive));
        archive->flags |= 1;
        memcpy(archive, src, sizeof(HSD_ArchiveHeader));
        if(archive->header.file_size == file_size){
            u32 offset = sizeof(HSD_ArchiveHeader);
            if(archive->header.data_size != 0){ //Body Size
                archive->data = src + 0x20;
                offset = archive->header.data_size + 0x20;
            }
            if(archive->header.nb_reloc != 0){ //Relocation Size
                archive->reloc_info = (HSD_ArchiveRelocationInfo*)((s32)src + offset);
                offset = offset + archive->header.nb_reloc * 4;
            }
            if(archive->header.nb_public != 0){ //Root Size
                archive->public_info = (HSD_ArchivePublicInfo*)((s32)src + offset);
                offset = offset + archive->header.nb_public * 8;
            }
            if(archive->header.nb_extern != 0){ //XRef Size
                archive->extern_info = (HSD_ArchiveExternInfo*)((s32)src + offset);
                offset = offset + archive->header.nb_extern * 8;
            }
            if(offset < archive->header.file_size){ //File Size
                archive->symbols = (char*)((s32)src + offset);
            }
            archive->top_ptr = (void*)src;
            offset = 0;
            for(u32 i = 0; i < archive->header.nb_reloc; i++){
                u8* data = archive->data;
                u32 ptr = archive->reloc_info->offset + offset;
                offset += 4;
                *(u8**)(data + ptr) = data + *((u32*)(data + ptr)); 
            }
        }else{
            HSD_Panic("Archive_Parse: Byte-Order mismatch");
            return -1;
        }
    }
    return 0;
}
