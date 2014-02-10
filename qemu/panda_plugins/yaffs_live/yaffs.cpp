#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
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

static bool tsk_verbose = false;
static int tsk_fprintf(FILE* output, const char* format, ...){
    va_list myargs;

    va_start(myargs, format);

    /* Forward the '...' to vprintf */
    int ret = vfprintf(output,format, myargs);

    /* Clean up the va_list */
    va_end(myargs);

    return ret;
}



ssize_t FS_Img::img_read(TSK_OFF_T offset, char* dst_buf, size_t count){
    if(offset >= this->buffer_len){
        printf("ERROR: offset %#X vs len %#X\n", offset, this->buffer_len);
        return -1;
    }
    if(offset < 0){
        printf("ERROR offset %#X\n", offset);
        return -1;
    }
    if(offset + count > this->buffer_len){
        printf("ERROR: offset %#X + count %#X  vs len %#X\n", offset, count, this->buffer_len);
        return -1;
    }
    if(count < 1){
        printf("ERROR: count %#X\n", count);
        return -1;
    }
    if(nullptr == dst_buf){
        printf("ERROR: dst_buf null\n");
        return -1;
    }
    if(!this->buffer){
        printf("ERROR: buffer is null\n");
        return -1;
    }
    memcpy(dst_buf, this->buffer+offset, count);
    return count;
}

enum TSK_RETVAL_ENUM {
    TSK_ERR,
    TSK_OK,
} ;

/*
* Function to attempt to determine the layout of the yaffs spare area.
* Results of the analysis (if the format could be determined) will be stored
* in yfs variables. 
*
* @param yfs File system being anlayzed
* @param maxBlocksToTest Number of block groups to scan to detect spare area or 0 if there is no limit.
* @returns TSK_ERR if format could not be detected and TSK_OK if it could be.
*/
static TSK_RETVAL_ENUM 
yaffs_initialize_spare_format(YAFFSFS_INFO * yfs, TSK_OFF_T maxBlocksToTest){

    // Testing parameters - can all be changed
    unsigned int blocksToTest = 10;  // Number of blocks (64 chunks) to test
    unsigned int chunksToTest = 10;  // Number of chunks to test in each block 
    unsigned int minChunksRead = 10; // Minimum number of chunks we require to run the test (we might not get the full number we want to test for a very small file)

    unsigned int chunkSize = yfs->page_size + yfs->spare_size;
    unsigned int blockSize = yfs->chunks_per_block * chunkSize;

    FS_Img *fs = &(yfs->fs_info);
    unsigned int cnt;
    unsigned char *spareBuffer;

    unsigned int blockIndex;
    unsigned int chunkIndex;

    unsigned int currentOffset;

    unsigned char * allSpares;
    unsigned int allSparesLength;
    TSK_OFF_T offset;
    TSK_OFF_T maxBlocks;

    bool skipBlock;
    int goodOffset;
    unsigned int nGoodSpares;
    unsigned int nBlocksTested;

    int okOffsetFound = 0;   // Used as a flag for if we've found an offset that sort of works but doesn't seem great
    int goodOffsetFound = 0; // Flag to mark that we've found an offset that also passed secondary testing
    int bestOffset = 0;

    bool allSameByte; // Used in test that the spare area fields not be one repeated byte

    unsigned int i;

    int thisChunkBase;
    int lastChunkBase;
#if(0)
    // The spare area needs to be at least 16 bytes to run the test
    if(yfs->spare_size < 16){
        if(tsk_verbose && (! yfs->autoDetect)){
            tsk_fprintf(stderr,
                "yaffs_initialize_spare_format failed - given spare size (%d) is not large enough to contain needed fields\n", yfs->spare_size);
        }
        return TSK_ERR;
    }

    if ((spareBuffer = (unsigned char*) tsk_malloc(yfs->spare_size)) == NULL) {
        return TSK_ERR;
    }

    allSparesLength = yfs->spare_size * blocksToTest * chunksToTest;
    if ((allSpares = (unsigned char*) tsk_malloc(allSparesLength)) == NULL) {
        free(spareBuffer);
        return TSK_ERR;
    }
#endif

    // Initialize the pointers to one of the configurations we've seen (thought these defaults should not get used)
    yfs->spare_seq_offset = 0;
    yfs->spare_obj_id_offset = 4;
    yfs->spare_chunk_id_offset = 8;
    yfs->spare_nbytes_offset = 12;

    // Assume the data we want is 16 consecutive bytes in the order:
    //  seq num, obj id, chunk id, byte count
    //  (not sure we're guaranteed this but we wouldn't be able to deal with the alternative anyway)
    // Seq num is the important one. This number is constant in each block (block = 64 chunks), meaning
    //  all chunks in a block will share the same sequence number. The YAFFS2 descriptions would seem to
    //  indicate it should be different for each block, but this doesn't seem to always be the case.
    //  In particular we frequently see the 0x1000 seq number used over multiple blocks, but this isn't the only
    //  observed exception.
#if (0)
    // Calculate the number of blocks in the image
    maxBlocks = yfs->fs_info.img_info->size / (yfs->chunks_per_block * chunkSize);

    // If maxBlocksToTest = 0 (unlimited), set it to the total number of blocks
    // Also reduce the number of blocks to test if it is larger than the total number of blocks
    if((maxBlocksToTest == 0) || (maxBlocksToTest > maxBlocks)){
        maxBlocksToTest = maxBlocks;
    }

    nGoodSpares = 0;
    nBlocksTested = 0;
    for(TSK_OFF_T blockIndex = 0;blockIndex < maxBlocksToTest;blockIndex++){

        // Read the last spare area that we want to test first
        offset = (TSK_OFF_T)blockIndex * blockSize + (chunksToTest - 1) * chunkSize + yfs->page_size;
        cnt = tsk_img_read(fs->img_info, offset, (char *) spareBuffer,
            yfs->spare_size);
        if (cnt == -1 || cnt < yfs->spare_size) {
            break;
        }

        // Is the spare all 0xff / 0x00?
        // If not, we know we should have all allocated chunks since YAFFS2 writes sequentially in a block
        //  - can't have an unallocated chunk followed by an allocated one
        // We occasionally see almost all null spare area with a few 0xff, which is not a valid spare.
        skipBlock = true;
        for(i = 0;i < yfs->spare_size;i++){
            if((spareBuffer[i] != 0xff) && (spareBuffer[i] != 0x00)){
                skipBlock = false;
                break;
            }
        }

        if(skipBlock){
            continue;
        }

        // If this block is potentially valid (i.e., the spare contains something besides 0x00 and 0xff), copy all the spares into
        // the big array of extracted spare areas

        // Copy this spare area
        nGoodSpares++;
        for(i = 0;i < yfs->spare_size;i++){
            allSpares[nBlocksTested * yfs->spare_size * chunksToTest + (chunksToTest - 1) * yfs->spare_size + i] = spareBuffer[i];
        }

        // Copy all earlier spare areas in the block
        for(chunkIndex = 0;chunkIndex < chunksToTest - 1;chunkIndex++){
            offset = blockIndex * blockSize + chunkIndex * chunkSize + yfs->page_size;
            cnt = tsk_img_read(fs->img_info, offset, (char *) spareBuffer,
                yfs->spare_size);
            if (cnt == -1 || cnt < yfs->spare_size) {
                // We really shouldn't run out of data here since we already read in the furthest entry
                break; // Break out of chunksToTest loop
            }

            nGoodSpares++;
            for(i = 0;i < yfs->spare_size;i++){
                allSpares[nBlocksTested * yfs->spare_size * chunksToTest + chunkIndex * yfs->spare_size + i] = spareBuffer[i];
            }
        }

        // Record that we've found a potentially valid block
        nBlocksTested++;

        // If we've found enough potentailly valid blocks, break
        if(nBlocksTested >= blocksToTest){
            break;
        }
    }

    // Make sure we read enough data to reasonably perform the testing
    if(nGoodSpares < minChunksRead){

        if(tsk_verbose && (! yfs->autoDetect)){
            tsk_fprintf(stderr,
                "yaffs_initialize_spare_format failed - not enough potentially valid data could be read\n");
        }

        free(spareBuffer);
        free(allSpares);
        return TSK_ERR;
    }

    if(tsk_verbose && (! yfs->autoDetect)){
        tsk_fprintf(stderr,
            "yaffs_initialize_spare_format: Testing potential offsets for the sequence number in the spare area\n");
    }

    // Print out the collected spare areas if we're in verbose mode
    if(tsk_verbose && (! yfs->autoDetect)){
        for(blockIndex = 0;blockIndex < nBlocksTested;blockIndex++){
            for(chunkIndex = 0;chunkIndex < chunksToTest;chunkIndex++){
                for(i = 0;i < yfs->spare_size;i++){
                    fprintf(stderr, "%02x", allSpares[blockIndex * yfs->spare_size * chunksToTest + chunkIndex * yfs->spare_size + i]);
                }
                fprintf(stderr, "\n");
            }
        }
    }

    // Test all indices into the spare area (that leave enough space for all 16 bytes)
    for(currentOffset = 0;currentOffset <= yfs->spare_size - 16;currentOffset++){
        goodOffset = 1;
        for(blockIndex = 0;blockIndex < nBlocksTested;blockIndex++){
            for(chunkIndex = 1;chunkIndex < chunksToTest;chunkIndex++){

                lastChunkBase = blockIndex * yfs->spare_size * chunksToTest + (chunkIndex - 1) * yfs->spare_size;
                thisChunkBase = lastChunkBase + yfs->spare_size;

                // Seq num should not be all 0xff (we tested earlier that the chunk has been initialized)
                if((0xff == allSpares[thisChunkBase + currentOffset]) &&
                    (0xff == allSpares[thisChunkBase + currentOffset + 1]) &&
                    (0xff == allSpares[thisChunkBase + currentOffset + 2]) &&
                    (0xff == allSpares[thisChunkBase + currentOffset + 3])){
                        if(tsk_verbose && (! yfs->autoDetect)){
                            tsk_fprintf(stderr,
                                "yaffs_initialize_spare_format: Elimimating offset %d - invalid sequence number 0xffffffff\n", 
                                currentOffset);
                        }
                        goodOffset = 0;
                        break;
                }

                // Seq num should not be zero
                if((0 == allSpares[thisChunkBase + currentOffset]) &&
                    (0 == allSpares[thisChunkBase + currentOffset + 1]) &&
                    (0 == allSpares[thisChunkBase + currentOffset + 2]) &&
                    (0 == allSpares[thisChunkBase + currentOffset + 3])){
                        if(tsk_verbose && (! yfs->autoDetect)){
                            tsk_fprintf(stderr,
                                "yaffs_initialize_spare_format: Elimimating offset %d - invalid sequence number 0\n", 
                                currentOffset);
                        }
                        goodOffset = 0;
                        break;
                }

                // Seq num should match the previous one in the block
                if((allSpares[lastChunkBase + currentOffset] != allSpares[thisChunkBase + currentOffset]) ||
                    (allSpares[lastChunkBase + currentOffset + 1] != allSpares[thisChunkBase + currentOffset + 1]) ||
                    (allSpares[lastChunkBase + currentOffset + 2] != allSpares[thisChunkBase + currentOffset + 2]) ||
                    (allSpares[lastChunkBase + currentOffset + 3] != allSpares[thisChunkBase + currentOffset + 3])){
                        if(tsk_verbose && (! yfs->autoDetect)){
                            tsk_fprintf(stderr,
                                "yaffs_initialize_spare_format: Elimimating offset %d - did not match previous chunk sequence number\n", 
                                currentOffset);
                        }
                        goodOffset = 0;
                        break;
                }

                // Obj id should not be zero
                if((0 == allSpares[thisChunkBase + currentOffset + 4]) &&
                    (0 == allSpares[thisChunkBase + currentOffset + 5]) &&
                    (0 == allSpares[thisChunkBase + currentOffset + 6]) &&
                    (0 == allSpares[thisChunkBase + currentOffset + 7])){
                        if(tsk_verbose && (! yfs->autoDetect)){
                            tsk_fprintf(stderr,
                                "yaffs_initialize_spare_format: Elimimating offset %d - invalid object id 0\n", 
                                currentOffset);
                        }
                        goodOffset = 0;
                        break;
                }

                // All 16 bytes should not be the same
                // (It is theoretically possible that this could be valid, but incredibly unlikely)
                allSameByte = true;
                for(i = 1;i < 16;i++){
                    if(allSpares[thisChunkBase + currentOffset] != allSpares[thisChunkBase + currentOffset + i]){
                        allSameByte = false;
                        break;
                    }
                }
                if(allSameByte){
                    if(tsk_verbose && (! yfs->autoDetect)){
                        tsk_fprintf(stderr,
                            "yaffs_initialize_spare_format: Elimimating offset %d - all repeated bytes\n", 
                            currentOffset);
                    }
                    goodOffset = 0;
                    break;
                }

            } // End of loop over chunks

            if(!goodOffset){ // Break out of loop over blocks
                break;
            }
        }
        if(goodOffset){

            // Note that we've found an offset that is at least promising
            if((! goodOffsetFound) && (! okOffsetFound)){
                bestOffset = currentOffset;
            }
            okOffsetFound = 1;

            if(tsk_verbose && (! yfs->autoDetect)){
                tsk_fprintf(stderr,
                    "yaffs_initialize_spare_format: Found potential spare offsets:  %d (sequence number), %d (object id), %d (chunk id), %d (n bytes)\n",
                    currentOffset, currentOffset+4, currentOffset+8, currentOffset+12);
            }

            // Now do some more tests
            // Really need some more real-world test data to do this right.
            int possibleError = 0;

            // We probably don't want the first byte to always be 0xff
            int firstByteFF = 1;
            for(blockIndex = 0;blockIndex < nBlocksTested;blockIndex++){
                for(chunkIndex = 1;chunkIndex < chunksToTest;chunkIndex++){
                    if(allSpares[blockIndex * yfs->spare_size * chunksToTest + chunkIndex * yfs->spare_size + currentOffset] != 0xff){
                        firstByteFF = 0;
                    }
                }
            }

            if(firstByteFF){
                if(tsk_verbose && (! yfs->autoDetect)){
                    tsk_fprintf(stderr,
                        "yaffs_initialize_spare_format:  Previous data starts with all 0xff bytes. Looking for better offsets.\n");
                }
                possibleError = 1;
            }

            if(! possibleError){

                // If we already have a good offset, print this one out but don't record it
                if(! goodOffsetFound){

                    goodOffsetFound = 1;
                    bestOffset = currentOffset;

                    // Offset passed additional testing and we haven't seen an earlier good one, so go ahead and use it
                    if(tsk_verbose && (! yfs->autoDetect)){
                        tsk_fprintf(stderr,
                            "yaffs_initialize_spare_format:  Previous offsets appear good - will use as final offsets\n");
                    }

                }
                else{
                    // Keep using the old one
                    if(tsk_verbose && (! yfs->autoDetect)){
                        tsk_fprintf(stderr,
                            "yaffs_initialize_spare_format:  Previous offsets appear good but staying with earlier valid ones\n");
                    }
                }
            }
        }
    }

    free(spareBuffer);
    free(allSpares);

    if(okOffsetFound || goodOffsetFound){
        // Record everything
        yfs->spare_seq_offset = bestOffset;
        yfs->spare_obj_id_offset = bestOffset + 4;
        yfs->spare_chunk_id_offset = bestOffset + 8;
        yfs->spare_nbytes_offset = bestOffset + 12;

        if(tsk_verbose && (! yfs->autoDetect)){
            tsk_fprintf(stderr,
                "yaffs_initialize_spare_format: Final offsets: %d (sequence number), %d (object id), %d (chunk id), %d (n bytes)\n",
                bestOffset, bestOffset+4, bestOffset+8, bestOffset+12);
            tsk_fprintf(stderr,
                "If these do not seem valid: %s\n", YAFFS_HELP_MESSAGE);
        }
        return TSK_OK;
    }
    else{
        return TSK_ERR;
    }
#endif
    return TSK_OK;
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
        printf("Failed to read page\n");
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
        printf("%d %d %d\n", yfs->spare_seq_offset +4, yfs->spare_obj_id_offset +4, yfs->spare_chunk_id_offset +4);
        printf("Error on spare sanity: spare size is %d\n", yfs->spare_size);
        return 1;
    }

    if ((spr = (unsigned char *) tsk_malloc(yfs->spare_size)) == NULL) {
        printf("Error on spare malloc\n");
        return 1;
    }

    if (yfs->spare_size < 46) { // Why is this 46?
        printf("yaffsfs_read_spare: spare size is too small\n");
        free(spr);
        return 1;
    }

    cnt = fs->img_read(offset, (char *) spr, 28);
    if (cnt == -1) {
        printf("error on spare read\n");
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

bool yaffs_info_init(YAFFSFS_INFO& info){
    yaffs_initialize_spare_format(&info, -1);
    return true;
}
