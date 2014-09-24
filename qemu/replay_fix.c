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

#include <assert.h>
#include "replay_fix.h"

struct _replay_issues replay_issues;

void fix_replay_stuff()
{
    if(replay_issues.fatal_error){
        assert(0);
    }
    if(0 != replay_issues.ram_size){
        qemu_ram_free(0);
        ram_addr_t offset = qemu_ram_alloc(NULL, replay_issues.ram_name, replay_issues.ram_size);
        cpu_register_physical_memory(0, replay_issues.ram_size, offset | IO_MEM_RAM);
        
    }
}
