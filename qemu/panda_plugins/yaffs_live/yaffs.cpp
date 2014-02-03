#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "tsk_yaffs.h"

static void* tsk_malloc(size_t len)
{
    void *ptr;

    ptr = malloc(len);
    if (nullptr == ptr) {
       exit(1);
    }
    memset(ptr, 0, len);
    return (ptr);
}


ssize_t FS_Img::img_read(TSK_OFF_T offset, char* dst_buf, size_t count){
    if(offset >= this->buffer_len){
        return -1;
    }
    if(offset < 0) return -1;
    if(offset + count > this->buffer_len) return -1;
    if(count < 1) return -1;
    if(nullptr == dst_buf) return -1;
    if(!this->buffer) return -1;
    memcpy(dst_buf, this->buffer+offset, count);
    return count;
}


 uint8_t
yaffsfs_read_header(YAFFSFS_INFO * yfs, YaffsHeader  &header,
    TSK_OFF_T offset)
{
    unsigned char *hdr;
    ssize_t cnt;
    YaffsHeader *head = &header;
    FS_Img *fs = &(yfs->fs_info);

    if ((hdr = (unsigned char *) tsk_malloc(yfs->page_size)) == NULL) {
        return 1;
    }

    cnt = fs->img_read(offset, (char *) hdr, yfs->page_size);
    if (cnt == -1 || cnt < yfs->page_size) {
        free(hdr);
        return 1;
    }

    if ((head = (YaffsHeader *) tsk_malloc(sizeof(YaffsHeader))) == NULL) {
        free(hdr);
        return 1;
    }

    memcpy(&head->obj_type, hdr, 4);
    memcpy(&head->parent_id, &hdr[4], 4);
    memcpy(head->name, (char *) &hdr[0xA], YAFFS_HEADER_NAME_LENGTH);
    memcpy(&head->file_mode, &hdr[0x10C], 4);
    memcpy(&head->user_id, &hdr[0x110], 4);
    memcpy(&head->group_id, &hdr[0x114], 4);
    memcpy(&head->atime, &hdr[0x118], 4);
    memcpy(&head->mtime, &hdr[0x11C], 4);
    memcpy(&head->ctime, &hdr[0x120], 4);
    memcpy(&head->file_size, &hdr[0x124], 4);
    memcpy(&head->equivalent_id, &hdr[0x128], 4);
    memcpy(head->alias, (char *) &hdr[0x12C], YAFFS_HEADER_ALIAS_LENGTH);

    //memcpy(&head->rdev_mode, &hdr[0x1CC], 4);
    //memcpy(&head->win_ctime, &hdr[0x1D0], 8);
    //memcpy(&head->win_atime, &hdr[0x1D8], 8);
    //memcpy(&head->win_mtime, &hdr[0x1E0], 8);
    //memcpy(&head->inband_obj_id, &hdr[0x1E8], 4);
    //memcpy(&head->inband_is_shrink, &hdr[0x1EC], 4);

    // NOTE: This isn't in Android 3.3 kernel but is in YAFFS2 git
    //memcpy(&head->file_size_high, &hdr[0x1F0], 4);

    free(hdr);

    //*header = head;
    return 0;
}

/**
* Read and parse the YAFFS2 tags in the NAND spare bytes.
*
* @param info is a YAFFS fs handle
* @param spare YaffsSpare object to be populated
* @param offset, offset to read from
*
* @returns 0 on success and 1 on error
*/
 uint8_t
yaffsfs_read_spare(YAFFSFS_INFO * yfs, YaffsSpare &spare,
    TSK_OFF_T offset)
{
    unsigned char *spr;
    ssize_t cnt;
    YaffsSpare *sp = &spare;
    FS_Img *fs = &(yfs->fs_info);

    uint32_t seq_number;
    uint32_t object_id;
    uint32_t chunk_id;

    // Should have checked this by now, but just in case
    if ((yfs->spare_seq_offset + 4 > yfs->spare_size) ||
        (yfs->spare_obj_id_offset + 4 > yfs->spare_size) ||
        (yfs->spare_chunk_id_offset + 4 > yfs->spare_size)) {
        return 1;
    }

    if ((spr = (unsigned char *) tsk_malloc(yfs->spare_size)) == NULL) {
        return 1;
    }

    if (yfs->spare_size < 46) { // Why is this 46?
        printf("yaffsfs_read_spare: spare size is too small\n");
        free(spr);
        return 1;
    }

    cnt = fs->img_read(offset, (char *) spr, yfs->spare_size);
    if (cnt == -1 || cnt < yfs->spare_size) {
        // couldn't read sufficient bytes...
        //if (spare) {
            free(spr);
            //*spare = NULL;
        //}
        return 1;
    }

    if ((sp = (YaffsSpare *) tsk_malloc(sizeof(YaffsSpare))) == NULL) {
        return 1;
    }

    memset(sp, 0, sizeof(YaffsSpare));

    /*
     * Complete read of the YAFFS2 spare
     */


    // The format of the spare area should have been determined earlier
    memcpy(&seq_number, &spr[yfs->spare_seq_offset], 4);
    memcpy(&object_id, &spr[yfs->spare_obj_id_offset], 4);
    memcpy(&chunk_id, &spr[yfs->spare_chunk_id_offset], 4);

    if ((YAFFS_SPARE_FLAGS_IS_HEADER & chunk_id) != 0) {

        sp->seq_number = seq_number;
        sp->object_id = object_id & ~YAFFS_SPARE_OBJECT_TYPE_MASK;
        sp->chunk_id = 0;

        sp->has_extra_fields = 1;
        sp->extra_parent_id = chunk_id & YAFFS_SPARE_PARENT_ID_MASK;
        sp->extra_object_type = (object_id & YAFFS_SPARE_OBJECT_TYPE_MASK)
            >> YAFFS_SPARE_OBJECT_TYPE_SHIFT;
    }
    else {
        sp->seq_number = seq_number;
        sp->object_id = object_id;
        sp->chunk_id = chunk_id;

        sp->has_extra_fields = 0;
    }

    free(spr);
    //*spare = sp;

    return 0;
}

static uint8_t
yaffsfs_is_spare_valid(YAFFSFS_INFO * yfs, YaffsSpare * spare)
{
    if (spare == NULL) {
        return 1;
    }

    if ((spare->object_id > YAFFS_MAX_OBJECT_ID) ||
        (spare->seq_number < YAFFS_LOWEST_SEQUENCE_NUMBER) ||
        (spare->seq_number > YAFFS_HIGHEST_SEQUENCE_NUMBER)) {
        return 1;
    }

    return 0;
}

static uint8_t
yaffsfs_read_chunk(YAFFSFS_INFO * yfs,
    YaffsHeader  &header, YaffsSpare &spare, TSK_OFF_T offset)
{
    TSK_OFF_T header_offset = offset;
    TSK_OFF_T spare_offset = offset + yfs->page_size;


    if (yaffsfs_read_header(yfs, header, header_offset) != 0) {
        return 1;
    }

    if (yaffsfs_read_spare(yfs, spare, spare_offset) != 0) {
        return 1;
    }

    return 0;
}