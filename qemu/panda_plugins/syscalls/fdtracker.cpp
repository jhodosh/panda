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
#include <list>
#include "weak_callbacks.hpp"
#include "syscalls.hpp"
#include <iostream>

const target_ulong NULL_FD = 0;

using namespace std;


typedef map<int, string> fdmap;

map<target_ulong, fdmap> asid_to_fds;

#if defined(CONFIG_PANDA_VMI)
#include "introspection/DroidScope/LinuxAPI.h"

// copy any descriptors from parent ASID to child ASID that aren't set in child
static void copy_fds(target_asid parent_asid, target_asid child_asid){
    for(auto parent_mapping : asid_to_fds[parent_asid]){
        auto child_it = asid_to_fds[child_asid].find(parent_mapping.first);
        if (child_it == asid_to_fds[child_asid].end()){
            asid_to_fds[child_asid][parent_mapping.first] = parent_mapping.second;
        }
    }
}

list<target_asid> outstanding_child_asids;
map<target_ulong, target_asid> outstanding_child_pids;


/* Deal with all scheduling cases:
 * - Parent returns first: PID of child is logged for copying
 * - Child returns first, not in VMI table yet: ASID is logged for copy at next chance
 * - Child returns first, in VMI table: copy will occur when parent returns :)
 * - Parent returns 
 * 
 * - parent runs first, child runs second but this callback runs 
 *      BEFORE the VMI can register the child process
 */
static int return_from_fork(CPUState *env){
    target_ulong child_pid = get_return_val(env);
    if(0 == child_pid){
        // This IS the child!
        assert("return_from_fork should only ever be called for the parent!");
        target_asid  asid;
        target_ulong pc;
        target_ulong cs_base;
        int flags;
        cpu_get_tb_cpu_state(env, &pc, &cs_base, &flags);
        asid = get_asid(env, pc);
        // See if the VMI can tell us our PID
        ProcessInfo* self_child = findProcessByPGD(asid);
        if(nullptr == self_child){
            // no, we can't look up our PID yet
            outstanding_child_asids.push_back(get_asid(env, pc));
        }else{
            auto it = outstanding_child_pids.find(self_child->pid);
            if (it == outstanding_child_pids.end()){
                outstanding_child_asids.push_back(get_asid(env, pc));
            }else{
                target_asid parent_asid = it->second;
                copy_fds(parent_asid, asid);
                outstanding_child_pids.erase(it);
            }
        }
        return 0;
    }

    // returned to the parent
    ProcessInfo* child = findProcessByPID(child_pid);
    if(nullptr == child){
        // child hasn't run yet!
        target_ulong pc;
        target_ulong cs_base;
        int flags;
        cpu_get_tb_cpu_state(env, &pc, &cs_base, &flags);
        // log that this ASID is the parent of the child's PID
        outstanding_child_pids[child_pid] = get_asid(env, pc);
        return 0;
    }
    //we're in the parent and the child has run
    target_ulong pc;
    target_ulong cs_base;
    int flags;
    cpu_get_tb_cpu_state(env, &pc, &cs_base, &flags);

    copy_fds(get_asid(env, pc), child->pgd);
    outstanding_child_asids.remove(child->pgd);
    return 0;
}

static void preExecForkCopier(CPUState* env, target_ulong pc){
    //is this process in outstanding_child_pids?
    if (outstanding_child_pids.empty()) {
        return;
    }
    target_asid my_asid = get_asid(env, pc);
    ProcessInfo* my_proc = findProcessByPGD(my_asid);
    if(nullptr == my_proc){
        // VMI doen't know about me yet... weird
        return;
    }
    auto it = outstanding_child_pids.find(my_proc->pid);
    if (it == outstanding_child_pids.end()){
        return;
    }
    // this is a process we're looking for!
    copy_fds(it->second, my_asid);
    outstanding_child_pids.erase(it);
}

struct StaticBlock {
    StaticBlock(){
        registerExecPreCallback(preExecForkCopier);
    }
};
static StaticBlock staticBlock;

#endif

class OpenCallbackData : public CallbackData {
public:
    string path;
    target_ulong base_fd;
};

class DupCallbackData: public CallbackData {
public:
    target_ulong old_fd;
    target_ulong new_fd;
};

class ReadCallbackData : public CallbackData {
public:
    target_ulong fd;
    target_ulong guest_buffer;
    uint32_t len;
};

static target_ulong calc_retaddr(CPUState* env, target_ulong pc){
#if defined(TARGET_ARM)
    // Normal syscalls: return addr is stored in LR
    return mask_retaddr_to_pc(env->regs[14]);

    // Fork, exec
    uint8_t offset = 0;
    if(env->thumb == 0){
        offset = 4;
    } else {
        offset = 2;
    }
    return pc + offset;
#elif defined(TARGET_I386)
    
#else
    
#endif
}

static void open_callback(CallbackData* opaque, CPUState* env, target_asid asid){
    printf("open callback\n");
    OpenCallbackData* data = dynamic_cast<OpenCallbackData*>(opaque);
    if(!data){
        fprintf(stderr, "oops\n");
        return;
    }
    string dirname = "";
    auto& mymap = asid_to_fds[asid];
    
    if(NULL_FD != data->base_fd){
        dirname += mymap[data->base_fd];
    }
    dirname += "/" + data->path;
    mymap[get_return_val(env)] = dirname;
}

//mkdirs
void call_sys_mkdirat_callback(CPUState* env,target_ulong pc,uint32_t dfd,std::string pathname,uint32_t mode) { 
    OpenCallbackData* data = new OpenCallbackData;
    data->path = pathname;
    data->base_fd = dfd;
    appendReturnPoint(ReturnPoint(calc_retaddr(env, pc), get_asid(env, pc), data, open_callback));
}

void call_sys_mkdir_callback(CPUState* env,target_ulong pc,std::string pathname,uint32_t mode) { 
    OpenCallbackData* data = new OpenCallbackData;
    data->path = pathname;
    data->base_fd = NULL_FD;
    appendReturnPoint(ReturnPoint(calc_retaddr(env, pc), get_asid(env, pc), data, open_callback));
}
//opens

void call_sys_open_callback(CPUState *env, target_ulong pc, std::string filename,uint32_t flags,uint32_t mode){
    printf("open\n");
    OpenCallbackData* data = new OpenCallbackData;
    data->path = filename;
    data->base_fd = NULL_FD;
    appendReturnPoint(ReturnPoint(calc_retaddr(env, pc), get_asid(env, pc), data, open_callback));
}

void call_sys_openat_callback(CPUState* env,target_ulong pc,uint32_t dfd,std::string filename,uint32_t flags,uint32_t mode){
    printf("openat\n");
    OpenCallbackData* data = new OpenCallbackData;
    data->path = filename;
    data->base_fd = dfd;
    appendReturnPoint(ReturnPoint(calc_retaddr(env, pc), get_asid(env, pc), data, open_callback));
}

static void dup_callback(CallbackData* opaque, CPUState* env, target_asid asid){
    printf("dup callback\n");
    DupCallbackData* data = dynamic_cast<DupCallbackData*>(opaque);
    if(!data){
        fprintf(stderr, "oops\n");
        return;
    }
    target_ulong new_fd;
    if(data->new_fd != NULL_FD){
        new_fd = data->new_fd;
    }else{
        new_fd = get_return_val(env);
    }
    asid_to_fds[asid][new_fd] = asid_to_fds[asid][data->old_fd];
}

// dups
void call_sys_dup_callback(CPUState* env,target_ulong pc,uint32_t fildes) {
    DupCallbackData* data = new DupCallbackData;
    data->old_fd = fildes;
    data->new_fd = NULL_FD;
    appendReturnPoint(ReturnPoint(calc_retaddr(env, pc), get_asid(env, pc), data, dup_callback));
    
}
void call_sys_dup2_callback(CPUState* env,target_ulong pc,uint32_t oldfd,uint32_t newfd) { 
    target_asid asid = get_asid(env, pc);
    asid_to_fds[asid][newfd] = asid_to_fds[asid][oldfd];
    return;
    
    DupCallbackData* data = new DupCallbackData;
    data->old_fd = oldfd;
    data->new_fd = newfd;
    appendReturnPoint(ReturnPoint(calc_retaddr(env, pc), get_asid(env, pc), data, dup_callback));
    
}
void call_sys_dup3_callback(CPUState* env,target_ulong pc,uint32_t oldfd,uint32_t newfd,uint32_t flags) {
    target_asid asid = get_asid(env, pc);
    asid_to_fds[asid][newfd] = asid_to_fds[asid][oldfd];
    return;
    
    DupCallbackData* data = new DupCallbackData;
    data->old_fd = oldfd;
    data->new_fd = newfd;
    appendReturnPoint(ReturnPoint(calc_retaddr(env, pc), get_asid(env, pc), data, dup_callback));
    
}

// close
void call_sys_close_callback(CPUState* env,target_ulong pc,uint32_t fd) { }

void call_sys_readahead_callback(CPUState* env,target_ulong pc,uint32_t fd,uint64_t offset,uint32_t count) { }

static void read_callback(CallbackData* opaque, CPUState* env, target_asid asid){
    ReadCallbackData* data = dynamic_cast<ReadCallbackData*>(opaque);
    if(!data){
        fprintf(stderr, "oops\n");
        return;
    }
    string filename = asid_to_fds[asid][data->fd];
    if (filename.empty()){
        
    }
    auto retval = get_return_val(env);
    
}

void call_sys_read_callback(CPUState* env,target_ulong pc,uint32_t fd,target_ulong buf,uint32_t count) { 
    target_asid asid = get_asid(env, pc);
    cout << "Reading from " << asid_to_fds[asid][fd] << endl;
}
void call_sys_readv_callback(CPUState* env,target_ulong pc,uint32_t fd,target_ulong vec,uint32_t vlen) { 
    target_asid asid = get_asid(env, pc);
    cout << "Reading v from " << asid_to_fds[asid][fd] << endl;
}
void call_sys_pread64_callback(CPUState* env,target_ulong pc,uint32_t fd,target_ulong buf,uint32_t count,uint64_t pos) {
        target_asid asid = get_asid(env, pc);
        cout << "Reading p64 from " << asid_to_fds[asid][fd] << endl;
}
void call_sys_write_callback(CPUState* env,target_ulong pc,uint32_t fd,target_ulong buf,uint32_t count) {
    target_asid asid = get_asid(env, pc);
    cout << "Writing to " << asid_to_fds[asid][fd] << endl;
}
void call_sys_pwrite64_callback(CPUState* env,target_ulong pc,uint32_t fd,target_ulong buf,uint32_t count,uint64_t pos) { 
    target_asid asid = get_asid(env, pc);
    cout << "Writing pv64 to " << asid_to_fds[asid][fd] << endl;
}
void call_sys_writev_callback(CPUState* env,target_ulong pc,uint32_t fd,target_ulong vec,uint32_t vlen) {
    target_asid asid = get_asid(env, pc);
    cout << "Writing v to " << asid_to_fds[asid][fd] << endl;
}

#if defined(SYSCALLS_FDS_TRACK_SOCKETS)
// SOCKET OPERATIONS --------------------------------------------------------------------
// AF_UNIX, AF_LOCAL, etc
#include <sys/socket.h>
//kernel source says sa_family_t is an unsigned short

typedef map<int, sa_family_t> sdmap;

map<target_ulong, sdmap> asid_to_sds;

class SocketCallbackData : public CallbackData{
    string socketname;
    sa_family_t domain;
};

static void socket_callback(CallbackData* opaque, CPUState* env, target_asid asid){
    SocketCallbackData* data = dynamic_cast<SocketCallbackData*>(opaque);
    if(!data){
        fprintf(stderr, "oops\n");
        return;
    }
    target_ulong new_sd = get_return_val(env);
    auto& mymap = asid_to_fds[asid];
    mymap[new_sd] = data->socketname;
    if(AF_UNSPEC != data->domain){
        auto& mysdmap = asid_to_sds[asid];
        mysdmap[new_sd] = data->domain;
    }
}

/*
bind - updates name?
struct sockaddr {
               sa_family_t sa_family;
               char        sa_data[14];
           }
*/
void call_sys_bind_callback(CPUState* env,target_ulong pc,uint32_t sockfd,target_ulong sockaddr_ptr,uint32_t sockaddrlen){
   
}
/*
connect - updates name?
*/
void call_sys_connect_callback(CPUState* env,target_ulong pc,uint32_t sockfd,target_ulong sockaddr_ptr,uint32_t sockaddrlen){
}
/*
socket - fd
Return value should be labeled "unbound socket"
*/
void call_sys_socket_callback(CPUState* env,target_ulong pc,uint32_t domain,uint32_t type,uint32_t protocol){
    SocketCallbackData* data = new SocketCallbackData;
    data->socketname = "unbound socket";
    data->domain = domain;
    appendReturnPoint(ReturnPoint(calc_retaddr(env, pc), get_asid(env, pc), data, socket_callback));
}
/*
send, sendto, sendmsg - 
recv, recvfrom, recvmsg - gets datas!
listen
socketpair - two new fds
*/
class SockpairCallbackData : public CallbackData{
    target_ulong sd_array;
    uint32_t domain;
};
static void sockpair_callback(CallbackData* opaque, CPUState* env, target_asid asid){
    SockpairCallbackData* data = dynamic_cast<SockpairCallbackData*>(opaque);
    if(!data){
        fprintf(stderr, "oops\n");
        return;
    }
    target_ulong retval = get_return_val(env);
    //"On success, zero is returned.  On error, -1 is returned, and errno is set appropriately."
    if(0 != retval){
        return;
    }
    // sd_array is an array of ints, length 2. NOT target_ulong
    int sd_array[2];
    panda_virtual_memory_rw(env, data->sd_array, sd_array, 2*sizeof(int), 0);
    
}
void call_sys_socketpair_callback(CPUState* env,target_ulong pc,uint32_t domain,uint32_t type,uint32_t protocol,target_ulong sd_array){
    
}
/*
accept, accept4 - new fd*/
#endif // SYSCALLS_FDS_TRACK_SOCKETS
