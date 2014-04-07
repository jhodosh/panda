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


// This module tracks the file names associated with file descriptors.
// It currently DOES NOT handle links AT ALL
// It tracks open(), etc., so only knows the names used by those functions

#if defined(SYSCALLS_FDS_TRACK_LINKS)
#error "Hard and soft links are not supported"
#endif

#include <map>
#include <string>
#include <hw/elf_ops.h>

using namespace std;


typedef map<int, string> fdmap;

map<target_ulong, fdmap> asid_to_fds;

//mkdirs

//opens
void do_open(CPUState *env, target_ulong pc, target_ulong str_ptr){
    int fd = wait_return();
    fdmap[get_current_asid()][fd] = get_string(env, str_ptr);
}

void do_openat(CPUState *env, target_ulong pc, uint32_t dfd, target_ulong str_ptr){
    int fd = wait_return();
    
    fdmap[get_current_asid()][fd] = get_string(env, str_ptr);
}

// dups

// close

