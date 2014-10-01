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

/* Type definitions for introspecting into a Linux guest.
 * 
 */
#define __STDC_LIMIT_MACROS // this header is sometimes called from C++, we want stdint values
#include "qemu-common.h" // includes stdint
#include "cpu.h"


// Physical memory address
typedef target_phys_addr_t gpa_t;
// Virtual memory address
typedef target_ulong gva_t;
// Page directory base/ASID value. Note that eg on i386/PAE this is not the same as guest physical address
typedef target_ulong target_asid_t;

typedef pid_t int32_t; // Linux PID is defined as signed 32-bit. see /usr/include/bits/typesizes.h
