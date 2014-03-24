/* PANDABEGINCOMMENT
 * 
 * Authors:
 *  Tim Leek               tleek@ll.mit.edu
 *  Ryan Whelan            rwhelan@ll.mit.edu
 *  Joshua Hodosh          josh.hodosh@ll.mit.edu
 *  Michael Zhivich        mzhivich@ll.mit.edu
 *  Brendan Dolan-Gavitt   brendandg@gatech.edu
 * 
 * This work is licensed under the terms of the GNU GPL, version 2. 
 * See the COPYING file in the top-level directory. 
 * 
PANDAENDCOMMENT */
// This needs to be defined before anything is included in order to get
// the PRIx64 macro
#define __STDC_FORMAT_MACROS

//#define LIVE_FLASH_DEBUG

extern "C" {

#include "config.h"
#include "qemu-common.h"
#include "monitor.h"
#include "cpu.h"

#include "panda_plugin.h"
    
#include "hw/hw.h"

#include "hw/goldfish_device.h"
#include "../hw/goldfish_nand_reg.h"
#include "../hw/mmc.h"
#include "hw/goldfish_nand.h"
#include "../hw/goldfish_mmc.h"
#include <assert.h>

}
#include <inttypes.h>
#include <memory>
#include <map>
#include <string>

#include "tsk_yaffs.h"

// These need to be extern "C" so that the ABI is compatible with
// QEMU/PANDA, which is written in C
extern "C" {

bool init_plugin(void *);
void uninit_plugin(void *);
int mem_write_callback(CPUState *env, target_ulong pc, target_ulong addr, target_ulong size, void *buf);
int mem_read_callback(CPUState *env, target_ulong pc, target_ulong addr, target_ulong size, void *buf);

}

GoldfishNandDevice  __GoldfishNandDevice;


GoldfishMmcDevice  __GoldfishMmcDevice;

static const uint32_t MMC_LOCATION = 0xff005000;
static const uint32_t NAND_LOCATION = 0xff016000;
FILE* writelog;
FILE* sdcard_log;
static int nand_dma_read_outstanding_bytes = 0;
static uint32_t nand_current_buffer_address = 0;
static uint32_t nand_next_read_flash_address;
static int mmc_dma_read_outstanding_sectors = 0;
static uint32_t mmc_dma_read_next_block = 0;
static uint32_t mmc_current_buffer_address = 0;
YAFFSFS_INFO state;


FILE* debug_log = nullptr;
#ifdef LIVE_FLASH_DEBUG
#define DLOG(...) xlog(__VA_ARGS__)
#else
#define DLOG(...) ((void)0)
#endif

static void
xlog(FILE* file, const char*  format, ... )
{
    
    va_list  args;
    va_start(args, format);
    vfprintf(file, format, args);
    va_end(args);
    
}

static void printYaffsHeader(FILE* file, YaffsHeader& header){
    DLOG(file, "obj_type: %#X parent_id: %#X, name: %s, file_mode: %#X is_shrink: %#X inband_obj_id\n",
            header.obj_type, header.parent_id, header.name, header.is_shrink, header.inband_obj_id);
}

static uint32_t nand_dev_do_cmd(GoldfishNandDevice *s, uint32_t cmd)
{
    uint32_t size;
    uint64_t addr;
    nand_dev *dev;
    DLOG(debug_log, "Doing command 0x%X on dev %d of %d \n", cmd, s->dev, s->nand_dev_count);
    if (cmd == NAND_CMD_WRITE_BATCH || cmd == NAND_CMD_READ_BATCH ||
        cmd == NAND_CMD_ERASE_BATCH) {
        struct batch_data bd;
        uint64_t bd_addr = ((uint64_t)s->batch_addr_high << 32) | s->batch_addr_low;

        cpu_physical_memory_read(bd_addr, (void*)&bd, sizeof(struct batch_data));
        s->dev = bd.dev;
        s->addr_low = bd.addr_low;
        s->addr_high = bd.addr_high;
        s->transfer_size = bd.transfer_size;
        s->data = bd.data;
    }
    addr = s->addr_low | ((uint64_t)s->addr_high << 32);
    size = s->transfer_size;
    if(s->dev >= s->nand_dev_count)
        return 0;
    dev = s->nand_devs + s->dev;

    switch(cmd) {
    case NAND_CMD_GET_DEV_NAME:
        if(size > dev->devname_len)
            size = dev->devname_len;
        DLOG(debug_log, "got dev name %s\n", dev->devname);
        //cpu_memory_rw(cpu_single_env, s->data, (uint8_t*)dev->devname, size, 1);
        return size;
    case NAND_CMD_READ_BATCH:
    case NAND_CMD_READ:
        if(addr >= dev->max_size)
            return 0;
        if(size > dev->max_size - addr)
            size = dev->max_size - addr;

//            return nand_dev_read_file(dev, s->data, addr, size);

  //      cpu_memory_rw(cpu_single_env,s->data, &dev->data[addr], size, 1);
        nand_current_buffer_address = s->data;
        nand_dma_read_outstanding_bytes = size;
        nand_next_read_flash_address = addr;
        DLOG(debug_log, "Read %d bytes at offset %#lX\n",size, addr);
        return size;
    case NAND_CMD_WRITE_BATCH:
    case NAND_CMD_WRITE:
      if(dev->flags & NAND_DEV_FLAG_READ_ONLY){
	fprintf(stderr, "Tried to write to read only flash\n");
            return 0;
      }
      if(addr >= dev->max_size){
	fprintf(stderr, "tried to write to address %#lx past end of flash at %#lx\n", addr, dev->max_size);
            return 0;
      }
        if(size > dev->max_size - addr)
            size = dev->max_size - addr;

//            return nand_dev_write_file(dev, s->data, addr, size);
        fprintf(writelog, "Wrote %d bytes at offset %#lX\n", size, addr);
        uint8_t tmp[size];
        cpu_memory_rw_debug(cpu_single_env,s->data, tmp, size, 0);
        state.fs_info.img_write(addr,tmp, size);
        if(state.page_size == (addr % (state.page_size + state.spare_size))){
            YaffsSpare spare;
            int ret = yaffsfs_read_spare(&state, spare, addr);
            if(ret)
                fprintf(stderr, "Error reading spare!\n");
            else{
                DLOG(debug_log, "Wrote spare seq %#X obj %#X chunk %#X\n", spare.seq_number, spare.object_id, spare.chunk_id);
            }
        }else if(0 == (addr % (state.page_size + state.spare_size))){
            YaffsHeader header;
            int ret = yaffsfs_read_header(&state, header, addr);
            if(ret)
                fprintf(stderr, "Error reading header!\n");
            else{
                printYaffsHeader(debug_log, header);
            }
        }else{
	  fprintf(stderr, "UNALIGNED WRITE!\n");
	}
        fwrite(tmp, 1, size,  writelog);
        return size;
    case NAND_CMD_ERASE_BATCH:
    case NAND_CMD_ERASE:
        if(dev->flags & NAND_DEV_FLAG_READ_ONLY)
            return 0;
        if(addr >= dev->max_size)
            return 0;
        if(size > dev->max_size - addr)
            size = dev->max_size - addr;

            //return nand_dev_erase_file(dev, addr, size);
        DLOG(debug_log, "Erased %d bytes at offset %#lX\n", size, addr);
        //memset(&dev->data[addr], 0xff, size);
        return size;
    case NAND_CMD_BLOCK_BAD_GET: // no bad block support
        return 0;
    case NAND_CMD_BLOCK_BAD_SET:
        if(dev->flags & NAND_DEV_FLAG_READ_ONLY)
            return 0;
        return 0;
    default:
        cpu_abort(cpu_single_env, "nand_dev_do_cmd: Bad command %x\n", cmd);
        return 0;
    }
}

static int  goldfish_mmc_bdrv_write(struct GoldfishMmcDevice *s,
                                    int64_t                    sector_number,
                                    target_phys_addr_t         dst_address,
                                    int                        num_sectors)
{
    int  ret = 0;

    while (num_sectors > 0) {
        uint8_t buf[512];
        cpu_physical_memory_read(dst_address, buf, 512);

        //ret = bdrv_write(s->bs, sector_number, s->buf, 1);
        //if (ret < 0)
        //    return ret;
        fprintf(sdcard_log, "Wrote to SDcard sector %#lX\n", sector_number);
        fwrite(buf, 1, 512, sdcard_log);

        dst_address   += 512;
        num_sectors   -= 1;
        sector_number += 1;
    }
    return ret;
}

static void goldfish_mmc_do_command(struct GoldfishMmcDevice *s, uint32_t cmd, uint32_t arg)
{
    int result;
    int new_status = MMC_STAT_END_OF_CMD;
    int opcode = cmd & 63;

// fprintf(stderr, "goldfish_mmc_do_command opcode: %s (0x%04X), arg: %d\n", get_command_name(opcode), cmd, arg);

    s->resp[0] = 0;
    s->resp[1] = 0;
    s->resp[2] = 0;
    s->resp[3] = 0;

#define SET_R1_CURRENT_STATE(s)    ((s << 9) & 0x00001E00) /* sx, b (4 bits) */

    switch (opcode) {
        case MMC_SEND_CSD: {
            break;
        }

        case MMC_SEND_EXT_CSD:
            s->resp[0] = arg;
            break;

        case MMC_APP_CMD:
            s->resp[0] = SET_R1_CURRENT_STATE(4) | R1_READY_FOR_DATA | R1_APP_CMD; //2336
            break;

        case SD_APP_OP_COND:
            s->resp[0] = 0x80FF8000;
            break;

        case SD_APP_SEND_SCR:
        {
#if 1 /* this code is actually endian-safe */
            const uint8_t  scr[8] = "\x02\x25\x00\x00\x00\x00\x00";
#else /* this original code wasn't */
            uint32_t scr[2];
            scr[0] = 0x00002502;
            scr[1] = 0x00000000;
#endif
            //cpu_physical_memory_write(s->buffer_address, (uint8_t*)scr, 8);

            s->resp[0] = SET_R1_CURRENT_STATE(4) | R1_READY_FOR_DATA | R1_APP_CMD; //2336
            new_status |= MMC_STAT_END_OF_DATA;
            break;
        }
        case MMC_SET_RELATIVE_ADDR:
            s->resp[0] = -518519520;
            break;

        case MMC_ALL_SEND_CID:
            s->resp[3] = 55788627;
            s->resp[2] = 1429221959;
            s->resp[1] = -2147479692;
            s->resp[0] = -436179883;
            break;

        case MMC_SELECT_CARD:
            s->resp[0] = SET_R1_CURRENT_STATE(3) | R1_READY_FOR_DATA; // 1792
            break;

         case MMC_SWITCH:
            if (arg == 0x00FFFFF1 || arg == 0x80FFFFF1) {
                uint8_t  buff0[64];
                memset(buff0, 0, sizeof buff0);
                buff0[13] = 2;
                //cpu_physical_memory_write(s->buffer_address, buff0, sizeof buff0);
                new_status |= MMC_STAT_END_OF_DATA;
            }
            s->resp[0] = SET_R1_CURRENT_STATE(4) | R1_READY_FOR_DATA | R1_APP_CMD; //2336
            break;

         case MMC_SET_BLOCKLEN:
            s->block_length = arg;
            s->resp[0] = SET_R1_CURRENT_STATE(4) | R1_READY_FOR_DATA; // 2304
            break;

        case MMC_READ_SINGLE_BLOCK:
            s->block_count = 1;
            // fall through
        case MMC_READ_MULTIPLE_BLOCK: {
            if (s->is_SDHC) {
                // arg is block offset
            } else {
                // arg is byte offset
                if (arg & 511) fprintf(stderr, "offset %d is not multiple of 512 when reading\n", arg);
                arg /= s->block_length;
            }
            //result = goldfish_mmc_bdrv_read(s, arg, s->buffer_address, s->block_count);
            new_status |= MMC_STAT_END_OF_DATA;
            mmc_dma_read_outstanding_sectors = s->block_count;
            mmc_dma_read_next_block = arg;
            mmc_current_buffer_address = s->buffer_address;
            DLOG(debug_log, "Reading %d sectors from SD\n", s->block_count);
            s->resp[0] = SET_R1_CURRENT_STATE(4) | R1_READY_FOR_DATA; // 2304
            break;
        }

        case MMC_WRITE_BLOCK:
            s->block_count = 1;
            // fall through
        case MMC_WRITE_MULTIPLE_BLOCK: {
            if (s->is_SDHC) {
                // arg is block offset
            } else {
                // arg is byte offset
                if (arg & 511) fprintf(stderr, "offset %d is not multiple of 512 when writing\n", arg);
                arg /= s->block_length;
            }
            // arg is byte offset
            result = goldfish_mmc_bdrv_write(s, arg, s->buffer_address, s->block_count);
//            bdrv_flush(s->bs);
            new_status |= MMC_STAT_END_OF_DATA;
            s->resp[0] = SET_R1_CURRENT_STATE(4) | R1_READY_FOR_DATA; // 2304
            break;
        }

        case MMC_STOP_TRANSMISSION:
            s->resp[0] = SET_R1_CURRENT_STATE(5) | R1_READY_FOR_DATA; // 2816
            break;

        case MMC_SEND_STATUS:
            s->resp[0] = SET_R1_CURRENT_STATE(4) | R1_READY_FOR_DATA; // 2304
            break;
     }

    s->int_status |= new_status;

    if ((s->int_status & s->int_enable)) {
        //goldfish_device_set_irq(&s->dev, 0, (s->int_status & s->int_enable));
    }
}



// Reads from the NAND
static int nand_read_callback(uint8_t offset){
    struct GoldfishNandDevice* s = &__GoldfishNandDevice;
    nand_dev *dev;
    
    switch (offset) {
    case NAND_VERSION:
        return NAND_VERSION_CURRENT;
    case NAND_NUM_DEV:
        return s->nand_dev_count;
    case NAND_RESULT:
        return s->result;
    }

    if(s->dev >= s->nand_dev_count)
        return 0;

    dev = s->nand_devs + s->dev;

    switch (offset) {
    case NAND_DEV_FLAGS:
        return dev->flags;

    case NAND_DEV_NAME_LEN:
        return dev->devname_len;

    case NAND_DEV_PAGE_SIZE:
        return dev->page_size;

    case NAND_DEV_EXTRA_SIZE:
        return dev->extra_size;

    case NAND_DEV_ERASE_SIZE:
        return dev->erase_size;

    case NAND_DEV_SIZE_LOW:
        return (uint32_t)dev->max_size;

    case NAND_DEV_SIZE_HIGH:
        return (uint32_t)(dev->max_size >> 32);

    default:
        cpu_abort(cpu_single_env, "nand_dev_read: Bad offset %x\n", offset);
        return 0;
    }
}

int mem_read_callback(CPUState *env, target_ulong pc, target_ulong addr, target_ulong size, void *buf){
    if ((addr & 0xFFFFFF00) == NAND_LOCATION){
        assert(size == sizeof(uint32_t));
        uint8_t offset = addr & 0xFF;
        return nand_read_callback(offset);
    }
    return -1;
}

// Writes to the NAND
static int nand_write_callback(uint8_t offset, uint32_t value){
    struct GoldfishNandDevice* s = &__GoldfishNandDevice;
    switch(offset){
        case NAND_DEV:
        s->dev = value;
       // if(s->dev >= nand_dev_count) {
        //    cpu_abort(cpu_single_env, "nand_dev_write: Bad dev %x\n", value);
        //}
        break;
    case NAND_ADDR_HIGH:
        s->addr_high = value;
        break;
    case NAND_ADDR_LOW:
        s->addr_low = value;
        break;
    case NAND_BATCH_ADDR_LOW:
        s->batch_addr_low = value;
        break;
    case NAND_BATCH_ADDR_HIGH:
        s->batch_addr_high = value;
        break;
    case NAND_TRANSFER_SIZE:
        s->transfer_size = value;
        break;
    case NAND_DATA:
        s->data = value;
        break;
    case NAND_COMMAND:
        s->result = nand_dev_do_cmd(s, value);
        if (value == NAND_CMD_WRITE_BATCH || value == NAND_CMD_READ_BATCH ||
            value == NAND_CMD_ERASE_BATCH) {
            //struct batch_data bd;
            //uint64_t bd_addr = ((uint64_t)s->batch_addr_high << 32) | s->batch_addr_low;
            //bd.result = s->result;
        }
        break;
    default:
        cpu_abort(cpu_single_env, "nand_dev_write: Bad offset %x\n", offset);
        break; 
        
    }
    return 0;
}

static int sdcard_write_callback(uint8_t offset, uint32_t val){
     struct GoldfishMmcDevice *s = &__GoldfishMmcDevice;
    int status, old_status;

    switch(offset) {

        case MMC_INT_STATUS:
            status = s->int_status;
            old_status = status;
            status &= ~val;
            s->int_status = status;
            break;

        case MMC_INT_ENABLE:
            /* enable buffer interrupts */
            s->int_enable = val;
            s->int_status = 0;
            break;
        case MMC_SET_BUFFER:
            /* save pointer to buffer 1 */
            s->buffer_address = val;
            break;
        case MMC_CMD:
            goldfish_mmc_do_command(s, val, s->arg);
            break;
        case MMC_ARG:
            s->arg = val;
            break;
        case MMC_BLOCK_LENGTH:
            s->block_length = val + 1;
            break;
        case MMC_BLOCK_COUNT:
            s->block_count = val + 1;
            break;

        default:
            cpu_abort (cpu_single_env, "goldfish_mmc_write: Bad offset %x\n", offset);
    }
    return 0;
}

int mem_write_callback(CPUState *env, target_ulong pc, target_ulong addr,
                       target_ulong size, void *buf) {
    if ((addr & 0xFFFFFF00) == NAND_LOCATION){
        //Offsets are all <0xFF
        assert(size == sizeof(uint32_t));
        uint32_t value = *static_cast<uint32_t*>(buf);
        uint8_t offset = addr & 0xFF;
        return nand_write_callback(offset, value);
    }
    if ((addr & 0xFFFFFF00) == MMC_LOCATION){
        assert(size == sizeof(uint32_t));
        uint32_t value = *static_cast<uint32_t*>(buf);
        uint8_t offset = addr & 0xFF;
        return sdcard_write_callback(offset, value);
    }
    return -1;
}


struct unique_file_deleter {
    void operator()( FILE* file )
    {
        fclose(file);
    }
};

static void setup_nand(int index, char* name, int namelen, int flags, int page_size, int extra_size, int erase_size, int max_size){
    __GoldfishNandDevice.nand_devs[index].devname = name;
    __GoldfishNandDevice.nand_devs[index].devname_len = namelen;
    __GoldfishNandDevice.nand_devs[index].flags = flags;
    __GoldfishNandDevice.nand_devs[index].page_size = page_size;
    __GoldfishNandDevice.nand_devs[index].extra_size = extra_size;
    __GoldfishNandDevice.nand_devs[index].erase_size = erase_size;
    __GoldfishNandDevice.nand_devs[index].max_size = max_size;
}


int before_loadvm(void){
    // register ourselves as the loadvm handler for mmc and nand!!
    DeviceInfo *info;
    const VMStateDescription* nand_vmsd = nullptr, *mmc_vmsd = nullptr;

    for (info = device_info_list; info != NULL; info = info->next) {
        // the fields are name, fw_name, and alias
        if(info->name && 0 == strncmp(info->name, "goldfish_nand", strlen("goldfish_nand"))){
            nand_vmsd = info->vmsd;
        }else if (info->name && 0 == strncmp(info->name, "goldfish_mmc", strlen("goldfish_mmc"))){
            mmc_vmsd = info->vmsd;
        }
    }
    
    if(!nand_vmsd || !mmc_vmsd){
        fprintf(stderr, "YAFFS live: Failed to find VMSD for NAND or MMC\n");
        exit(1);
    }
    
    vmstate_unregister_all(nand_vmsd);
    vmstate_unregister_all(mmc_vmsd);
    
    vmstate_register(nullptr,0,nand_vmsd,&__GoldfishNandDevice);
    vmstate_register(nullptr,0,mmc_vmsd,&__GoldfishMmcDevice);

    printf("NAND device 0 devname is at %#lx devname_len is at %#lx\n", &__GoldfishNandDevice.nand_devs[0].devname, &__GoldfishNandDevice.nand_devs[0].devname_len);
    printf("NAND device 0 flags is at %#lx\n", &__GoldfishNandDevice.nand_devs[0].flags);
    return 0;
}

static bool golfish_nand_specific_yaffs_dma(
    target_phys_addr_t& last_aligned_target, /* */
    uint32_t num_bytes,
    uint32_t dest_addr
)
{

    if(num_bytes == 1028){
        if (last_aligned_target){ return true; }
        else {
            last_aligned_target = dest_addr;
            return false;
        }
    }else if(num_bytes == 2048){
        last_aligned_target = dest_addr;
        return true;
    }
    return false;
}

static void print_nand(void){
  for(int index =0; index < __GoldfishNandDevice.nand_dev_count; index++){
    char* name =__GoldfishNandDevice.nand_devs[index].devname;
    //    __GoldfishNandDevice.nand_devs[index].devname_len;
    uint32_t flagts = __GoldfishNandDevice.nand_devs[index].flags;
    uint32_t page_zie = __GoldfishNandDevice.nand_devs[index].page_size;
    uint32_t extra = __GoldfishNandDevice.nand_devs[index].extra_size;
    uint32_t erase = __GoldfishNandDevice.nand_devs[index].erase_size;
    uint32_t max = __GoldfishNandDevice.nand_devs[index].max_size;
    printf("%d: %s flags %#x page %#X extra %#X erase %#x max %#x\n", index, name, flagts, page_zie, extra, erase, max);
  }
}
static bool first_dma = true;

static target_phys_addr_t last_aligned_target = 0;
int on_dma(CPUState *env, uint32_t is_write, uint8_t* src_addr, uint64_t dest_addr, uint32_t num_bytes){
  if(unlikely(first_dma)){
    printf("%#x %#x\n", sizeof(__GoldfishNandDevice), sizeof(__GoldfishNandDevice.nand_devs[0]));
    print_nand();
    first_dma = false;
  }

    /* QEMU will break up DMA into multiple chunks, because the guest's physical memory is not contiguous
       in QEMU's virtual memory. */
    if(is_write && mmc_dma_read_outstanding_sectors > 0 && dest_addr == mmc_current_buffer_address){
        fprintf(sdcard_log, "FILE READ\n");
        fwrite(src_addr,1,num_bytes, sdcard_log);
        fprintf(sdcard_log, "\n");
        mmc_dma_read_outstanding_sectors -= 1;
        mmc_current_buffer_address+=num_bytes;
        mmc_dma_read_next_block+= 1;
    }else if(is_write && nand_dma_read_outstanding_bytes > 0 && cpu_single_env &&
        panda_virt_to_phys(cpu_single_env, nand_current_buffer_address) == dest_addr){
        fprintf(writelog, "FILE READ %d\n", num_bytes);
        fwrite(src_addr,1,num_bytes, writelog);
        fprintf(writelog, "\n");
        nand_dma_read_outstanding_bytes -= num_bytes;
        nand_current_buffer_address += num_bytes;
    
        state.fs_info.img_write(nand_next_read_flash_address,src_addr, num_bytes);

        if(state.page_size == (nand_next_read_flash_address % (state.page_size + state.spare_size))){
            YaffsSpare spare;
            int ret = yaffsfs_read_spare(&state, spare, nand_next_read_flash_address);
            if(ret)
                fprintf(stderr, "Error reading spare!\n");
            else{
                DLOG(debug_log, "Read spare seq %#X obj %#X chunk %#X\n", spare.seq_number, spare.object_id, spare.chunk_id);
            }
        }else if(0 == (nand_next_read_flash_address % (state.page_size + state.spare_size)) ||
            1024 == (nand_next_read_flash_address % (state.page_size + state.spare_size))
        ){
	uint32_t offset = (nand_next_read_flash_address % (state.page_size + state.spare_size));
            //bool pageReady = golfish_nand_specific_yaffs_dma(last_aligned_target, num_bytes, nand_next_read_flash_address);
            bool pageReady = last_aligned_target != 0;
	    pageReady = (offset == 1024);
            if(pageReady){
                YaffsHeader header;
                int ret = yaffsfs_read_header(&state, header, nand_next_read_flash_address - 1024);
                if(ret)
                    fprintf(stderr, "error reading header\n");
                else{
                    printYaffsHeader(debug_log, header);
                }
                last_aligned_target = 0;
            } else {
                last_aligned_target = nand_next_read_flash_address;
            }
        }else{
	  fprintf(stderr,"READ UNALIGNED!!! %#X\n", nand_next_read_flash_address);
	}
        nand_next_read_flash_address += num_bytes;
    }
}
bool init_plugin(void *self) {
    panda_cb pcb;

    printf("Initializing plugin yaffs_live\n");

    
    writelog = nullptr;
    sdcard_log = nullptr;
    
    
    //std::unique_ptr<FILE, unique_file_deleter> nandstate(fopen(vmstatefile,"r"));
    //setup_nand(0, "system",6, 0, 0x800, 0x40, 0x21000, 0xCB28000);
    //setup_nand(1, "userdata", 8, 0, 0x800, 0x40, 0x21000, 0x4FEC000);
    //setup_nand(2, "cache", 5, 0, 0x800, 0x40, 0x21000, 0x4200000);
    //__GoldfishNandDevice.nand_dev_count = 3;
    //__GoldfishMmcDevice.is_SDHC = 0;
    
    // Enable memory logging
    panda_enable_memcb();

    pcb.before_loadvm = before_loadvm;
    panda_register_callback(self, PANDA_CB_BEFORE_REPLAY_LOADVM, pcb);
    
    
    pcb.phys_mem_write = mem_write_callback;
    panda_register_callback(self, PANDA_CB_PHYS_MEM_WRITE, pcb);

    pcb.phys_mem_read = mem_read_callback;
    panda_register_callback(self, PANDA_CB_PHYS_MEM_READ, pcb);
    
    pcb.replay_before_cpu_physical_mem_rw_ram = on_dma;
    panda_register_callback(self, PANDA_CB_REPLAY_BEFORE_CPU_PHYSICAL_MEM_RW_RAM, pcb);

    writelog = fopen("/scratch/writelog.txt","wb");
    sdcard_log = fopen("/scratch/sdcardlog.txt", "wb");
    
    yaffs_info_init(state);
    
    state.page_size = 2048;
    state.spare_size = 64;

#ifdef LIVE_FLASH_DEBUG
    debug_log = fopen("/scratch/debuglog.txt");
    if(!debug_log) return false;
#endif
    
    return true;
}

void uninit_plugin(void *self) {

    if(writelog)
        fclose(writelog);
    if(sdcard_log)
        fclose(sdcard_log);
}

bool FS_Img::img_write(TSK_OFF_T offset, uint8_t* src_buff, size_t count){
    DLOG(debug_log, "Writing %d bytes at offset %#x\n", count, offset);
    bool largeEnough = this->ensure_capacity(offset+count);
    if (largeEnough)
        memcpy(this->buffer + offset, src_buff, count);
    else
        fprintf(stderr, "FAILED TO ALLOCATE %d bytes\n", count);
    return largeEnough;
}

bool FS_Img::ensure_capacity(size_t size){
    if (size > this->buffer_len){
        void* tmp = realloc(this->buffer, size);
        if(!tmp) return false;
        this->buffer = static_cast<uint8_t*>(tmp);
        this->buffer_len = size;
        DLOG(debug_log, "Expanded buffer to %#X\n", size);
        return true;
    }else{
        return true;
    }
}

FS_Img::FS_Img(char* filename){}
FS_Img::FS_Img(){}
