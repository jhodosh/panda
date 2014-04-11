#include "weak_callbacks.hpp"
extern "C"{
#include "qemu-common.h"
}

// weak-defined default empty callbacks for all syscalls
void __attribute__((weak)) call_sys_restart_syscall_callback(CPUState* env,target_ulong pc) { }
void __attribute__((weak)) call_sys_exit_callback(CPUState* env,target_ulong pc,uint32_t error_code) { }
void __attribute__((weak)) call_fork_callback(CPUState* env,target_ulong pc) { }
void __attribute__((weak)) call_sys_read_callback(CPUState* env,target_ulong pc,uint32_t fd,target_ulong buf,uint32_t count) { }
void __attribute__((weak)) call_sys_write_callback(CPUState* env,target_ulong pc,uint32_t fd,target_ulong buf,uint32_t count) { }
void __attribute__((weak)) call_sys_open_callback(CPUState* env,target_ulong pc,std::string filename,uint32_t flags,uint32_t mode) { }
void __attribute__((weak)) call_sys_close_callback(CPUState* env,target_ulong pc,uint32_t fd) { }
void __attribute__((weak)) call_sys_creat_callback(CPUState* env,target_ulong pc,std::string pathname,uint32_t mode) { }
void __attribute__((weak)) call_sys_link_callback(CPUState* env,target_ulong pc,std::string oldname,std::string newname) { }
void __attribute__((weak)) call_sys_unlink_callback(CPUState* env,target_ulong pc,std::string pathname) { }
void __attribute__((weak)) call_execve_callback(CPUState* env,target_ulong pc,std::string filename,target_ulong argv,target_ulong envp) { }
void __attribute__((weak)) call_sys_chdir_callback(CPUState* env,target_ulong pc,std::string filename) { }
void __attribute__((weak)) call_sys_mknod_callback(CPUState* env,target_ulong pc,std::string filename,uint32_t mode,uint32_t dev) { }
void __attribute__((weak)) call_sys_chmod_callback(CPUState* env,target_ulong pc,std::string filename,uint32_t mode) { }
void __attribute__((weak)) call_sys_lchown16_callback(CPUState* env,target_ulong pc,std::string filename,uint32_t user,uint32_t group) { }
void __attribute__((weak)) call_sys_lseek_callback(CPUState* env,target_ulong pc,uint32_t fd,uint32_t offset,uint32_t origin) { }
void __attribute__((weak)) call_sys_getpid_callback(CPUState* env,target_ulong pc) { }
void __attribute__((weak)) call_sys_mount_callback(CPUState* env,target_ulong pc,std::string dev_name,std::string dir_name,std::string type,uint32_t flags,target_ulong data) { }
void __attribute__((weak)) call_sys_setuid16_callback(CPUState* env,target_ulong pc,uint32_t uid) { }
void __attribute__((weak)) call_sys_getuid16_callback(CPUState* env,target_ulong pc) { }
void __attribute__((weak)) call_sys_ptrace_callback(CPUState* env,target_ulong pc,uint32_t request,uint32_t pid,uint32_t addr,uint32_t data) { }
void __attribute__((weak)) call_sys_pause_callback(CPUState* env,target_ulong pc) { }
void __attribute__((weak)) call_sys_access_callback(CPUState* env,target_ulong pc,std::string filename,uint32_t mode) { }
void __attribute__((weak)) call_sys_nice_callback(CPUState* env,target_ulong pc,uint32_t increment) { }
void __attribute__((weak)) call_sys_sync_callback(CPUState* env,target_ulong pc) { }
void __attribute__((weak)) call_sys_kill_callback(CPUState* env,target_ulong pc,uint32_t pid,uint32_t sig) { }
void __attribute__((weak)) call_sys_rename_callback(CPUState* env,target_ulong pc,std::string oldname,std::string newname) { }
void __attribute__((weak)) call_sys_mkdir_callback(CPUState* env,target_ulong pc,std::string pathname,uint32_t mode) { }
void __attribute__((weak)) call_sys_rmdir_callback(CPUState* env,target_ulong pc,std::string pathname) { }
void __attribute__((weak)) call_sys_dup_callback(CPUState* env,target_ulong pc,uint32_t fildes) { }
void __attribute__((weak)) call_sys_pipe_callback(CPUState* env,target_ulong pc,target_ulong arg0) { }
void __attribute__((weak)) call_sys_times_callback(CPUState* env,target_ulong pc,target_ulong tbuf) { }
void __attribute__((weak)) call_sys_brk_callback(CPUState* env,target_ulong pc,uint32_t brk) { }
void __attribute__((weak)) call_sys_setgid16_callback(CPUState* env,target_ulong pc,uint32_t gid) { }
void __attribute__((weak)) call_sys_getgid16_callback(CPUState* env,target_ulong pc) { }
void __attribute__((weak)) call_sys_geteuid16_callback(CPUState* env,target_ulong pc) { }
void __attribute__((weak)) call_sys_getegid16_callback(CPUState* env,target_ulong pc) { }
void __attribute__((weak)) call_sys_acct_callback(CPUState* env,target_ulong pc,std::string name) { }
void __attribute__((weak)) call_sys_umount_callback(CPUState* env,target_ulong pc,std::string name,uint32_t flags) { }
void __attribute__((weak)) call_sys_ioctl_callback(CPUState* env,target_ulong pc,uint32_t fd,uint32_t cmd,uint32_t arg) { }
void __attribute__((weak)) call_sys_fcntl_callback(CPUState* env,target_ulong pc,uint32_t fd,uint32_t cmd,uint32_t arg) { }
void __attribute__((weak)) call_sys_setpgid_callback(CPUState* env,target_ulong pc,uint32_t pid,uint32_t pgid) { }
void __attribute__((weak)) call_sys_umask_callback(CPUState* env,target_ulong pc,uint32_t mask) { }
void __attribute__((weak)) call_sys_chroot_callback(CPUState* env,target_ulong pc,std::string filename) { }
void __attribute__((weak)) call_sys_ustat_callback(CPUState* env,target_ulong pc,uint32_t dev,target_ulong ubuf) { }
void __attribute__((weak)) call_sys_dup2_callback(CPUState* env,target_ulong pc,uint32_t oldfd,uint32_t newfd) { }
void __attribute__((weak)) call_sys_getppid_callback(CPUState* env,target_ulong pc) { }
void __attribute__((weak)) call_sys_getpgrp_callback(CPUState* env,target_ulong pc) { }
void __attribute__((weak)) call_sys_setsid_callback(CPUState* env,target_ulong pc) { }
void __attribute__((weak)) call_sigaction_callback(CPUState* env,target_ulong pc,uint32_t sig,target_ulong act,target_ulong oact) { }
void __attribute__((weak)) call_sys_setreuid16_callback(CPUState* env,target_ulong pc,uint32_t ruid,uint32_t euid) { }
void __attribute__((weak)) call_sys_setregid16_callback(CPUState* env,target_ulong pc,uint32_t rgid,uint32_t egid) { }
void __attribute__((weak)) call_sigsuspend_callback(CPUState* env,target_ulong pc,uint32_t restart,uint32_t oldmask,uint32_t mask) { }
void __attribute__((weak)) call_sys_sigpending_callback(CPUState* env,target_ulong pc,target_ulong set) { }
void __attribute__((weak)) call_sys_sethostname_callback(CPUState* env,target_ulong pc,std::string name,uint32_t len) { }
void __attribute__((weak)) call_sys_setrlimit_callback(CPUState* env,target_ulong pc,uint32_t resource,target_ulong rlim) { }
void __attribute__((weak)) call_sys_getrusage_callback(CPUState* env,target_ulong pc,uint32_t who,target_ulong ru) { }
void __attribute__((weak)) call_sys_gettimeofday_callback(CPUState* env,target_ulong pc,target_ulong tv,target_ulong tz) { }
void __attribute__((weak)) call_sys_settimeofday_callback(CPUState* env,target_ulong pc,target_ulong tv,target_ulong tz) { }
void __attribute__((weak)) call_sys_getgroups16_callback(CPUState* env,target_ulong pc,uint32_t gidsetsize,target_ulong grouplist) { }
void __attribute__((weak)) call_sys_setgroups16_callback(CPUState* env,target_ulong pc,uint32_t gidsetsize,target_ulong grouplist) { }
void __attribute__((weak)) call_sys_symlink_callback(CPUState* env,target_ulong pc,std::string old,std::string anew) { }
void __attribute__((weak)) call_sys_readlink_callback(CPUState* env,target_ulong pc,std::string path,target_ulong buf,uint32_t bufsiz) { }
void __attribute__((weak)) call_sys_uselib_callback(CPUState* env,target_ulong pc,std::string library) { }
void __attribute__((weak)) call_sys_swapon_callback(CPUState* env,target_ulong pc,std::string specialfile,uint32_t swap_flags) { }
void __attribute__((weak)) call_sys_reboot_callback(CPUState* env,target_ulong pc,uint32_t magic1,uint32_t magic2,uint32_t cmd,target_ulong arg) { }
void __attribute__((weak)) call_sys_munmap_callback(CPUState* env,target_ulong pc,uint32_t addr,uint32_t len) { }
void __attribute__((weak)) call_sys_truncate_callback(CPUState* env,target_ulong pc,std::string path,uint32_t length) { }
void __attribute__((weak)) call_sys_ftruncate_callback(CPUState* env,target_ulong pc,uint32_t fd,uint32_t length) { }
void __attribute__((weak)) call_sys_fchmod_callback(CPUState* env,target_ulong pc,uint32_t fd,uint32_t mode) { }
void __attribute__((weak)) call_sys_fchown16_callback(CPUState* env,target_ulong pc,uint32_t fd,uint32_t user,uint32_t group) { }
void __attribute__((weak)) call_sys_getpriority_callback(CPUState* env,target_ulong pc,uint32_t which,uint32_t who) { }
void __attribute__((weak)) call_sys_setpriority_callback(CPUState* env,target_ulong pc,uint32_t which,uint32_t who,uint32_t niceval) { }
void __attribute__((weak)) call_sys_statfs_callback(CPUState* env,target_ulong pc,std::string path,target_ulong buf) { }
void __attribute__((weak)) call_sys_fstatfs_callback(CPUState* env,target_ulong pc,uint32_t fd,target_ulong buf) { }
void __attribute__((weak)) call_sys_syslog_callback(CPUState* env,target_ulong pc,uint32_t type,target_ulong buf,uint32_t len) { }
void __attribute__((weak)) call_sys_setitimer_callback(CPUState* env,target_ulong pc,uint32_t which,target_ulong value,target_ulong ovalue) { }
void __attribute__((weak)) call_sys_getitimer_callback(CPUState* env,target_ulong pc,uint32_t which,target_ulong value) { }
void __attribute__((weak)) call_sys_newstat_callback(CPUState* env,target_ulong pc,std::string filename,target_ulong statbuf) { }
void __attribute__((weak)) call_sys_newlstat_callback(CPUState* env,target_ulong pc,std::string filename,target_ulong statbuf) { }
void __attribute__((weak)) call_sys_newfstat_callback(CPUState* env,target_ulong pc,uint32_t fd,target_ulong statbuf) { }
void __attribute__((weak)) call_sys_vhangup_callback(CPUState* env,target_ulong pc) { }
void __attribute__((weak)) call_sys_wait4_callback(CPUState* env,target_ulong pc,uint32_t pid,target_ulong stat_addr,uint32_t options,target_ulong ru) { }
void __attribute__((weak)) call_sys_swapoff_callback(CPUState* env,target_ulong pc,std::string specialfile) { }
void __attribute__((weak)) call_sys_sysinfo_callback(CPUState* env,target_ulong pc,target_ulong info) { }
void __attribute__((weak)) call_sys_fsync_callback(CPUState* env,target_ulong pc,uint32_t fd) { }
void __attribute__((weak)) call_sigreturn_callback(CPUState* env,target_ulong pc) { }
void __attribute__((weak)) call_clone_callback(CPUState* env,target_ulong pc,target_ulong fn,target_ulong child_stack,uint32_t flags,target_ulong arg,target_ulong arg4) { }
void __attribute__((weak)) call_sys_setdomainname_callback(CPUState* env,target_ulong pc,std::string name,uint32_t len) { }
void __attribute__((weak)) call_sys_newuname_callback(CPUState* env,target_ulong pc,target_ulong name) { }
void __attribute__((weak)) call_sys_adjtimex_callback(CPUState* env,target_ulong pc,target_ulong txc_p) { }
void __attribute__((weak)) call_sys_mprotect_callback(CPUState* env,target_ulong pc,uint32_t start,uint32_t len,uint32_t prot) { }
void __attribute__((weak)) call_sys_sigprocmask_callback(CPUState* env,target_ulong pc,uint32_t how,target_ulong set,target_ulong oset) { }
void __attribute__((weak)) call_sys_init_module_callback(CPUState* env,target_ulong pc,target_ulong umod,uint32_t len,std::string uargs) { }
void __attribute__((weak)) call_sys_delete_module_callback(CPUState* env,target_ulong pc,std::string name_user,uint32_t flags) { }
void __attribute__((weak)) call_sys_quotactl_callback(CPUState* env,target_ulong pc,uint32_t cmd,std::string special,uint32_t id,target_ulong addr) { }
void __attribute__((weak)) call_sys_getpgid_callback(CPUState* env,target_ulong pc,uint32_t pid) { }
void __attribute__((weak)) call_sys_fchdir_callback(CPUState* env,target_ulong pc,uint32_t fd) { }
void __attribute__((weak)) call_sys_bdflush_callback(CPUState* env,target_ulong pc,uint32_t func,uint32_t data) { }
void __attribute__((weak)) call_sys_sysfs_callback(CPUState* env,target_ulong pc,uint32_t option,uint32_t arg1,uint32_t arg2) { }
void __attribute__((weak)) call_sys_personality_callback(CPUState* env,target_ulong pc,uint32_t personality) { }
void __attribute__((weak)) call_sys_setfsuid16_callback(CPUState* env,target_ulong pc,uint32_t uid) { }
void __attribute__((weak)) call_sys_setfsgid16_callback(CPUState* env,target_ulong pc,uint32_t gid) { }
void __attribute__((weak)) call_sys_llseek_callback(CPUState* env,target_ulong pc,uint32_t fd,uint32_t offset_high,uint32_t offset_low,target_ulong result,uint32_t origin) { }
void __attribute__((weak)) call_sys_getdents_callback(CPUState* env,target_ulong pc,uint32_t fd,target_ulong dirent,uint32_t count) { }
void __attribute__((weak)) call_sys_select_callback(CPUState* env,target_ulong pc,uint32_t n,target_ulong inp,target_ulong outp,target_ulong exp,target_ulong tvp) { }
void __attribute__((weak)) call_sys_flock_callback(CPUState* env,target_ulong pc,uint32_t fd,uint32_t cmd) { }
void __attribute__((weak)) call_sys_msync_callback(CPUState* env,target_ulong pc,uint32_t start,uint32_t len,uint32_t flags) { }
void __attribute__((weak)) call_sys_readv_callback(CPUState* env,target_ulong pc,uint32_t fd,target_ulong vec,uint32_t vlen) { }
void __attribute__((weak)) call_sys_writev_callback(CPUState* env,target_ulong pc,uint32_t fd,target_ulong vec,uint32_t vlen) { }
void __attribute__((weak)) call_sys_getsid_callback(CPUState* env,target_ulong pc,uint32_t pid) { }
void __attribute__((weak)) call_sys_fdatasync_callback(CPUState* env,target_ulong pc,uint32_t fd) { }
void __attribute__((weak)) call_sys_sysctl_callback(CPUState* env,target_ulong pc,target_ulong args) { }
void __attribute__((weak)) call_sys_mlock_callback(CPUState* env,target_ulong pc,uint32_t start,uint32_t len) { }
void __attribute__((weak)) call_sys_munlock_callback(CPUState* env,target_ulong pc,uint32_t start,uint32_t len) { }
void __attribute__((weak)) call_sys_mlockall_callback(CPUState* env,target_ulong pc,uint32_t flags) { }
void __attribute__((weak)) call_sys_munlockall_callback(CPUState* env,target_ulong pc) { }
void __attribute__((weak)) call_sys_sched_setparam_callback(CPUState* env,target_ulong pc,uint32_t pid,target_ulong param) { }
void __attribute__((weak)) call_sys_sched_getparam_callback(CPUState* env,target_ulong pc,uint32_t pid,target_ulong param) { }
void __attribute__((weak)) call_sys_sched_setscheduler_callback(CPUState* env,target_ulong pc,uint32_t pid,uint32_t policy,target_ulong param) { }
void __attribute__((weak)) call_sys_sched_getscheduler_callback(CPUState* env,target_ulong pc,uint32_t pid) { }
void __attribute__((weak)) call_sys_sched_yield_callback(CPUState* env,target_ulong pc) { }
void __attribute__((weak)) call_sys_sched_get_priority_max_callback(CPUState* env,target_ulong pc,uint32_t policy) { }
void __attribute__((weak)) call_sys_sched_get_priority_min_callback(CPUState* env,target_ulong pc,uint32_t policy) { }
void __attribute__((weak)) call_sys_sched_rr_get_interval_callback(CPUState* env,target_ulong pc,uint32_t pid,target_ulong interval) { }
void __attribute__((weak)) call_sys_nanosleep_callback(CPUState* env,target_ulong pc,target_ulong rqtp,target_ulong rmtp) { }
void __attribute__((weak)) call_arm_mremap_callback(CPUState* env,target_ulong pc,uint32_t addr,uint32_t old_len,uint32_t new_len,uint32_t flags,uint32_t new_addr) { }
void __attribute__((weak)) call_sys_setresuid16_callback(CPUState* env,target_ulong pc,uint32_t ruid,uint32_t euid,uint32_t suid) { }
void __attribute__((weak)) call_sys_getresuid16_callback(CPUState* env,target_ulong pc,target_ulong ruid,target_ulong euid,target_ulong suid) { }
void __attribute__((weak)) call_sys_poll_callback(CPUState* env,target_ulong pc,target_ulong ufds,uint32_t nfds,uint32_t timeout) { }
void __attribute__((weak)) call_sys_nfsservctl_callback(CPUState* env,target_ulong pc,uint32_t cmd,target_ulong arg,target_ulong res) { }
void __attribute__((weak)) call_sys_setresgid16_callback(CPUState* env,target_ulong pc,uint32_t rgid,uint32_t egid,uint32_t sgid) { }
void __attribute__((weak)) call_sys_getresgid16_callback(CPUState* env,target_ulong pc,target_ulong rgid,target_ulong egid,target_ulong sgid) { }
void __attribute__((weak)) call_sys_prctl_callback(CPUState* env,target_ulong pc,uint32_t option,uint32_t arg2,uint32_t arg3,uint32_t arg4,uint32_t arg5) { }
void __attribute__((weak)) call_sigreturn_callback(CPUState* env,target_ulong pc) { }
void __attribute__((weak)) call_rt_sigaction_callback(CPUState* env,target_ulong pc,uint32_t sig,target_ulong act,target_ulong oact,uint32_t sigsetsize) { }
void __attribute__((weak)) call_sys_rt_sigprocmask_callback(CPUState* env,target_ulong pc,uint32_t how,target_ulong set,target_ulong oset,uint32_t sigsetsize) { }
void __attribute__((weak)) call_sys_rt_sigpending_callback(CPUState* env,target_ulong pc,target_ulong set,uint32_t sigsetsize) { }
void __attribute__((weak)) call_sys_rt_sigtimedwait_callback(CPUState* env,target_ulong pc,target_ulong uthese,target_ulong uinfo,target_ulong uts,uint32_t sigsetsize) { }
void __attribute__((weak)) call_sys_rt_sigqueueinfo_callback(CPUState* env,target_ulong pc,uint32_t pid,uint32_t sig,target_ulong uinfo) { }
void __attribute__((weak)) call_sys_rt_sigsuspend_callback(CPUState* env,target_ulong pc,target_ulong unewset,uint32_t sigsetsize) { }
void __attribute__((weak)) call_sys_pread64_callback(CPUState* env,target_ulong pc,uint32_t fd,target_ulong buf,uint32_t count,uint64_t pos) { }
void __attribute__((weak)) call_sys_pwrite64_callback(CPUState* env,target_ulong pc,uint32_t fd,target_ulong buf,uint32_t count,uint64_t pos) { }
void __attribute__((weak)) call_sys_chown16_callback(CPUState* env,target_ulong pc,std::string filename,uint32_t user,uint32_t group) { }
void __attribute__((weak)) call_sys_getcwd_callback(CPUState* env,target_ulong pc,target_ulong buf,uint32_t size) { }
void __attribute__((weak)) call_sys_capget_callback(CPUState* env,target_ulong pc,target_ulong header,target_ulong dataptr) { }
void __attribute__((weak)) call_sys_capset_callback(CPUState* env,target_ulong pc,target_ulong header,target_ulong data) { }
void __attribute__((weak)) call_do_sigaltstack_callback(CPUState* env,target_ulong pc,target_ulong uss,target_ulong uoss) { }
void __attribute__((weak)) call_sys_sendfile_callback(CPUState* env,target_ulong pc,uint32_t out_fd,uint32_t in_fd,target_ulong offset,uint32_t count) { }
void __attribute__((weak)) call_vfork_callback(CPUState* env,target_ulong pc) { }
void __attribute__((weak)) call_sys_getrlimit_callback(CPUState* env,target_ulong pc,uint32_t resource,target_ulong rlim) { }
void __attribute__((weak)) call_do_mmap2_callback(CPUState* env,target_ulong pc,uint32_t addr,uint32_t len,uint32_t prot,uint32_t flags,uint32_t fd,uint32_t pgoff) { }
void __attribute__((weak)) call_sys_truncate64_callback(CPUState* env,target_ulong pc,std::string path,uint64_t length) { }
void __attribute__((weak)) call_sys_ftruncate64_callback(CPUState* env,target_ulong pc,uint32_t fd,uint64_t length) { }
void __attribute__((weak)) call_sys_stat64_callback(CPUState* env,target_ulong pc,std::string filename,target_ulong statbuf) { }
void __attribute__((weak)) call_sys_lstat64_callback(CPUState* env,target_ulong pc,std::string filename,target_ulong statbuf) { }
void __attribute__((weak)) call_sys_fstat64_callback(CPUState* env,target_ulong pc,uint32_t fd,target_ulong statbuf) { }
void __attribute__((weak)) call_sys_lchown_callback(CPUState* env,target_ulong pc,std::string filename,uint32_t user,uint32_t group) { }
void __attribute__((weak)) call_sys_getuid_callback(CPUState* env,target_ulong pc) { }
void __attribute__((weak)) call_sys_getgid_callback(CPUState* env,target_ulong pc) { }
void __attribute__((weak)) call_sys_geteuid_callback(CPUState* env,target_ulong pc) { }
void __attribute__((weak)) call_sys_getegid_callback(CPUState* env,target_ulong pc) { }
void __attribute__((weak)) call_sys_setreuid_callback(CPUState* env,target_ulong pc,uint32_t ruid,uint32_t euid) { }
void __attribute__((weak)) call_sys_setregid_callback(CPUState* env,target_ulong pc,uint32_t rgid,uint32_t egid) { }
void __attribute__((weak)) call_sys_getgroups_callback(CPUState* env,target_ulong pc,uint32_t gidsetsize,target_ulong grouplist) { }
void __attribute__((weak)) call_sys_setgroups_callback(CPUState* env,target_ulong pc,uint32_t gidsetsize,target_ulong grouplist) { }
void __attribute__((weak)) call_sys_fchown_callback(CPUState* env,target_ulong pc,uint32_t fd,uint32_t user,uint32_t group) { }
void __attribute__((weak)) call_sys_setresuid_callback(CPUState* env,target_ulong pc,uint32_t ruid,uint32_t euid,uint32_t suid) { }
void __attribute__((weak)) call_sys_getresuid_callback(CPUState* env,target_ulong pc,target_ulong ruid,target_ulong euid,target_ulong suid) { }
void __attribute__((weak)) call_sys_setresgid_callback(CPUState* env,target_ulong pc,uint32_t rgid,uint32_t egid,uint32_t sgid) { }
void __attribute__((weak)) call_sys_getresgid_callback(CPUState* env,target_ulong pc,target_ulong rgid,target_ulong egid,target_ulong sgid) { }
void __attribute__((weak)) call_sys_chown_callback(CPUState* env,target_ulong pc,std::string filename,uint32_t user,uint32_t group) { }
void __attribute__((weak)) call_sys_setuid_callback(CPUState* env,target_ulong pc,uint32_t uid) { }
void __attribute__((weak)) call_sys_setgid_callback(CPUState* env,target_ulong pc,uint32_t gid) { }
void __attribute__((weak)) call_sys_setfsuid_callback(CPUState* env,target_ulong pc,uint32_t uid) { }
void __attribute__((weak)) call_sys_setfsgid_callback(CPUState* env,target_ulong pc,uint32_t gid) { }
void __attribute__((weak)) call_sys_getdents64_callback(CPUState* env,target_ulong pc,uint32_t fd,target_ulong dirent,uint32_t count) { }
void __attribute__((weak)) call_sys_pivot_root_callback(CPUState* env,target_ulong pc,std::string new_root,std::string put_old) { }
void __attribute__((weak)) call_sys_mincore_callback(CPUState* env,target_ulong pc,uint32_t start,uint32_t len,std::string vec) { }
void __attribute__((weak)) call_sys_madvise_callback(CPUState* env,target_ulong pc,uint32_t start,uint32_t len,uint32_t behavior) { }
void __attribute__((weak)) call_sys_fcntl64_callback(CPUState* env,target_ulong pc,uint32_t fd,uint32_t cmd,uint32_t arg) { }
void __attribute__((weak)) call_sys_gettid_callback(CPUState* env,target_ulong pc) { }
void __attribute__((weak)) call_sys_readahead_callback(CPUState* env,target_ulong pc,uint32_t fd,uint64_t offset,uint32_t count) { }
void __attribute__((weak)) call_sys_setxattr_callback(CPUState* env,target_ulong pc,std::string path,std::string name,target_ulong value,uint32_t size,uint32_t flags) { }
void __attribute__((weak)) call_sys_lsetxattr_callback(CPUState* env,target_ulong pc,std::string path,std::string name,target_ulong value,uint32_t size,uint32_t flags) { }
void __attribute__((weak)) call_sys_fsetxattr_callback(CPUState* env,target_ulong pc,uint32_t fd,std::string name,target_ulong value,uint32_t size,uint32_t flags) { }
void __attribute__((weak)) call_sys_getxattr_callback(CPUState* env,target_ulong pc,std::string path,std::string name,target_ulong value,uint32_t size) { }
void __attribute__((weak)) call_sys_lgetxattr_callback(CPUState* env,target_ulong pc,std::string path,std::string name,target_ulong value,uint32_t size) { }
void __attribute__((weak)) call_sys_fgetxattr_callback(CPUState* env,target_ulong pc,uint32_t fd,std::string name,target_ulong value,uint32_t size) { }
void __attribute__((weak)) call_sys_listxattr_callback(CPUState* env,target_ulong pc,std::string path,std::string list,uint32_t size) { }
void __attribute__((weak)) call_sys_llistxattr_callback(CPUState* env,target_ulong pc,std::string path,std::string list,uint32_t size) { }
void __attribute__((weak)) call_sys_flistxattr_callback(CPUState* env,target_ulong pc,uint32_t fd,std::string list,uint32_t size) { }
void __attribute__((weak)) call_sys_removexattr_callback(CPUState* env,target_ulong pc,std::string path,std::string name) { }
void __attribute__((weak)) call_sys_lremovexattr_callback(CPUState* env,target_ulong pc,std::string path,std::string name) { }
void __attribute__((weak)) call_sys_fremovexattr_callback(CPUState* env,target_ulong pc,uint32_t fd,std::string name) { }
void __attribute__((weak)) call_sys_tkill_callback(CPUState* env,target_ulong pc,uint32_t pid,uint32_t sig) { }
void __attribute__((weak)) call_sys_sendfile64_callback(CPUState* env,target_ulong pc,uint32_t out_fd,uint32_t in_fd,target_ulong offset,uint32_t count) { }
void __attribute__((weak)) call_sys_futex_callback(CPUState* env,target_ulong pc,target_ulong uaddr,uint32_t op,uint32_t val,target_ulong utime,target_ulong uaddr2,uint32_t val3) { }
void __attribute__((weak)) call_sys_sched_setaffinity_callback(CPUState* env,target_ulong pc,uint32_t pid,uint32_t len,target_ulong user_mask_ptr) { }
void __attribute__((weak)) call_sys_sched_getaffinity_callback(CPUState* env,target_ulong pc,uint32_t pid,uint32_t len,target_ulong user_mask_ptr) { }
void __attribute__((weak)) call_sys_io_setup_callback(CPUState* env,target_ulong pc,uint32_t nr_reqs,target_ulong ctx) { }
void __attribute__((weak)) call_sys_io_destroy_callback(CPUState* env,target_ulong pc,uint32_t ctx) { }
void __attribute__((weak)) call_sys_io_getevents_callback(CPUState* env,target_ulong pc,uint32_t ctx_id,uint32_t min_nr,uint32_t nr,target_ulong events,target_ulong timeout) { }
void __attribute__((weak)) call_sys_io_submit_callback(CPUState* env,target_ulong pc,uint32_t arg0,uint32_t arg1,target_ulong arg2) { }
void __attribute__((weak)) call_sys_io_cancel_callback(CPUState* env,target_ulong pc,uint32_t ctx_id,target_ulong iocb,target_ulong result) { }
void __attribute__((weak)) call_sys_exit_group_callback(CPUState* env,target_ulong pc,uint32_t error_code) { }
void __attribute__((weak)) call_sys_lookup_dcookie_callback(CPUState* env,target_ulong pc,uint64_t cookie64,target_ulong buf,uint32_t len) { }
void __attribute__((weak)) call_sys_epoll_create_callback(CPUState* env,target_ulong pc,uint32_t size) { }
void __attribute__((weak)) call_sys_epoll_ctl_callback(CPUState* env,target_ulong pc,uint32_t epfd,uint32_t op,uint32_t fd,target_ulong event) { }
void __attribute__((weak)) call_sys_epoll_wait_callback(CPUState* env,target_ulong pc,uint32_t epfd,target_ulong events,uint32_t maxevents,uint32_t timeout) { }
void __attribute__((weak)) call_sys_remap_file_pages_callback(CPUState* env,target_ulong pc,uint32_t start,uint32_t size,uint32_t prot,uint32_t pgoff,uint32_t flags) { }
void __attribute__((weak)) call_sys_set_tid_address_callback(CPUState* env,target_ulong pc,target_ulong tidptr) { }
void __attribute__((weak)) call_sys_timer_create_callback(CPUState* env,target_ulong pc,uint32_t which_clock,target_ulong timer_event_spec,target_ulong created_timer_id) { }
void __attribute__((weak)) call_sys_timer_settime_callback(CPUState* env,target_ulong pc,uint32_t timer_id,uint32_t flags,target_ulong new_setting,target_ulong old_setting) { }
void __attribute__((weak)) call_sys_timer_gettime_callback(CPUState* env,target_ulong pc,uint32_t timer_id,target_ulong setting) { }
void __attribute__((weak)) call_sys_timer_getoverrun_callback(CPUState* env,target_ulong pc,uint32_t timer_id) { }
void __attribute__((weak)) call_sys_timer_delete_callback(CPUState* env,target_ulong pc,uint32_t timer_id) { }
void __attribute__((weak)) call_sys_clock_settime_callback(CPUState* env,target_ulong pc,uint32_t which_clock,target_ulong tp) { }
void __attribute__((weak)) call_sys_clock_gettime_callback(CPUState* env,target_ulong pc,uint32_t which_clock,target_ulong tp) { }
void __attribute__((weak)) call_sys_clock_getres_callback(CPUState* env,target_ulong pc,uint32_t which_clock,target_ulong tp) { }
void __attribute__((weak)) call_sys_clock_nanosleep_callback(CPUState* env,target_ulong pc,uint32_t which_clock,uint32_t flags,target_ulong rqtp,target_ulong rmtp) { }
void __attribute__((weak)) call_sys_statfs64_callback(CPUState* env,target_ulong pc,std::string path,uint32_t sz,target_ulong buf) { }
void __attribute__((weak)) call_sys_fstatfs64_callback(CPUState* env,target_ulong pc,uint32_t fd,uint32_t sz,target_ulong buf) { }
void __attribute__((weak)) call_sys_tgkill_callback(CPUState* env,target_ulong pc,uint32_t tgid,uint32_t pid,uint32_t sig) { }
void __attribute__((weak)) call_sys_utimes_callback(CPUState* env,target_ulong pc,std::string filename,target_ulong utimes) { }
void __attribute__((weak)) call_sys_arm_fadvise64_64_callback(CPUState* env,target_ulong pc,uint32_t fd,uint32_t advice,uint64_t offset,uint64_t len) { }
void __attribute__((weak)) call_sys_pciconfig_iobase_callback(CPUState* env,target_ulong pc,uint32_t which,uint32_t bus,uint32_t devfn) { }
void __attribute__((weak)) call_sys_pciconfig_read_callback(CPUState* env,target_ulong pc,uint32_t bus,uint32_t dfn,uint32_t off,uint32_t len,target_ulong buf) { }
void __attribute__((weak)) call_sys_pciconfig_write_callback(CPUState* env,target_ulong pc,uint32_t bus,uint32_t dfn,uint32_t off,uint32_t len,target_ulong buf) { }
void __attribute__((weak)) call_sys_mq_open_callback(CPUState* env,target_ulong pc,std::string name,uint32_t oflag,uint32_t mode,target_ulong attr) { }
void __attribute__((weak)) call_sys_mq_unlink_callback(CPUState* env,target_ulong pc,std::string name) { }
void __attribute__((weak)) call_sys_mq_timedsend_callback(CPUState* env,target_ulong pc,uint32_t mqdes,std::string msg_ptr,uint32_t msg_len,uint32_t msg_prio,target_ulong abs_timeout) { }
void __attribute__((weak)) call_sys_mq_timedreceive_callback(CPUState* env,target_ulong pc,uint32_t mqdes,std::string msg_ptr,uint32_t msg_len,target_ulong msg_prio,target_ulong abs_timeout) { }
void __attribute__((weak)) call_sys_mq_notify_callback(CPUState* env,target_ulong pc,uint32_t mqdes,target_ulong notification) { }
void __attribute__((weak)) call_sys_mq_getsetattr_callback(CPUState* env,target_ulong pc,uint32_t mqdes,target_ulong mqstat,target_ulong omqstat) { }
void __attribute__((weak)) call_sys_waitid_callback(CPUState* env,target_ulong pc,uint32_t which,uint32_t pid,target_ulong infop,uint32_t options,target_ulong ru) { }
void __attribute__((weak)) call_sys_socket_callback(CPUState* env,target_ulong pc,uint32_t arg0,uint32_t arg1,uint32_t arg2) { }
void __attribute__((weak)) call_sys_bind_callback(CPUState* env,target_ulong pc,uint32_t arg0,target_ulong arg1,uint32_t arg2) { }
void __attribute__((weak)) call_sys_connect_callback(CPUState* env,target_ulong pc,uint32_t arg0,target_ulong arg1,uint32_t arg2) { }
void __attribute__((weak)) call_sys_listen_callback(CPUState* env,target_ulong pc,uint32_t arg0,uint32_t arg1) { }
void __attribute__((weak)) call_sys_accept_callback(CPUState* env,target_ulong pc,uint32_t arg0,target_ulong arg1,target_ulong arg2) { }
void __attribute__((weak)) call_sys_getsockname_callback(CPUState* env,target_ulong pc,uint32_t arg0,target_ulong arg1,target_ulong arg2) { }
void __attribute__((weak)) call_sys_getpeername_callback(CPUState* env,target_ulong pc,uint32_t arg0,target_ulong arg1,target_ulong arg2) { }
void __attribute__((weak)) call_sys_socketpair_callback(CPUState* env,target_ulong pc,uint32_t arg0,uint32_t arg1,uint32_t arg2,target_ulong arg3) { }
void __attribute__((weak)) call_sys_send_callback(CPUState* env,target_ulong pc,uint32_t arg0,target_ulong arg1,uint32_t arg2,uint32_t arg3) { }
void __attribute__((weak)) call_sys_sendto_callback(CPUState* env,target_ulong pc,uint32_t arg0,target_ulong arg1,uint32_t arg2,uint32_t arg3,target_ulong arg4,uint32_t arg5) { }
void __attribute__((weak)) call_sys_recv_callback(CPUState* env,target_ulong pc,uint32_t arg0,target_ulong arg1,uint32_t arg2,uint32_t arg3) { }
void __attribute__((weak)) call_sys_recvfrom_callback(CPUState* env,target_ulong pc,uint32_t arg0,target_ulong arg1,uint32_t arg2,uint32_t arg3,target_ulong arg4,target_ulong arg5) { }
void __attribute__((weak)) call_sys_shutdown_callback(CPUState* env,target_ulong pc,uint32_t arg0,uint32_t arg1) { }
void __attribute__((weak)) call_sys_setsockopt_callback(CPUState* env,target_ulong pc,uint32_t fd,uint32_t level,uint32_t optname,std::string optval,uint32_t optlen) { }
void __attribute__((weak)) call_sys_getsockopt_callback(CPUState* env,target_ulong pc,uint32_t fd,uint32_t level,uint32_t optname,std::string optval,target_ulong optlen) { }
void __attribute__((weak)) call_sys_sendmsg_callback(CPUState* env,target_ulong pc,uint32_t fd,target_ulong msg,uint32_t flags) { }
void __attribute__((weak)) call_sys_recvmsg_callback(CPUState* env,target_ulong pc,uint32_t fd,target_ulong msg,uint32_t flags) { }
void __attribute__((weak)) call_sys_semop_callback(CPUState* env,target_ulong pc,uint32_t semid,target_ulong sops,uint32_t nsops) { }
void __attribute__((weak)) call_sys_semget_callback(CPUState* env,target_ulong pc,uint32_t key,uint32_t nsems,uint32_t semflg) { }
void __attribute__((weak)) call_sys_semctl_callback(CPUState* env,target_ulong pc,uint32_t semid,uint32_t semnum,uint32_t cmd,uint32_t arg) { }
void __attribute__((weak)) call_sys_msgsnd_callback(CPUState* env,target_ulong pc,uint32_t msqid,target_ulong msgp,uint32_t msgsz,uint32_t msgflg) { }
void __attribute__((weak)) call_sys_msgrcv_callback(CPUState* env,target_ulong pc,uint32_t msqid,target_ulong msgp,uint32_t msgsz,uint32_t msgtyp,uint32_t msgflg) { }
void __attribute__((weak)) call_sys_msgget_callback(CPUState* env,target_ulong pc,uint32_t key,uint32_t msgflg) { }
void __attribute__((weak)) call_sys_msgctl_callback(CPUState* env,target_ulong pc,uint32_t msqid,uint32_t cmd,target_ulong buf) { }
void __attribute__((weak)) call_sys_shmat_callback(CPUState* env,target_ulong pc,uint32_t shmid,std::string shmaddr,uint32_t shmflg) { }
void __attribute__((weak)) call_sys_shmdt_callback(CPUState* env,target_ulong pc,std::string shmaddr) { }
void __attribute__((weak)) call_sys_shmget_callback(CPUState* env,target_ulong pc,uint32_t key,uint32_t size,uint32_t flag) { }
void __attribute__((weak)) call_sys_shmctl_callback(CPUState* env,target_ulong pc,uint32_t shmid,uint32_t cmd,target_ulong buf) { }
void __attribute__((weak)) call_sys_add_key_callback(CPUState* env,target_ulong pc,std::string _type,std::string _description,target_ulong _payload,uint32_t plen,uint32_t destringid) { }
void __attribute__((weak)) call_sys_request_key_callback(CPUState* env,target_ulong pc,std::string _type,std::string _description,std::string _callout_info,uint32_t destringid) { }
void __attribute__((weak)) call_sys_keyctl_callback(CPUState* env,target_ulong pc,uint32_t cmd,uint32_t arg2,uint32_t arg3,uint32_t arg4,uint32_t arg5) { }
void __attribute__((weak)) call_sys_semtimedop_callback(CPUState* env,target_ulong pc,uint32_t semid,target_ulong sops,uint32_t nsops,target_ulong timeout) { }
void __attribute__((weak)) call_sys_ioprio_set_callback(CPUState* env,target_ulong pc,uint32_t which,uint32_t who,uint32_t ioprio) { }
void __attribute__((weak)) call_sys_ioprio_get_callback(CPUState* env,target_ulong pc,uint32_t which,uint32_t who) { }
void __attribute__((weak)) call_sys_inotify_init_callback(CPUState* env,target_ulong pc) { }
void __attribute__((weak)) call_sys_inotify_add_watch_callback(CPUState* env,target_ulong pc,uint32_t fd,std::string path,uint32_t mask) { }
void __attribute__((weak)) call_sys_inotify_rm_watch_callback(CPUState* env,target_ulong pc,uint32_t fd,uint32_t wd) { }
void __attribute__((weak)) call_sys_mbind_callback(CPUState* env,target_ulong pc,uint32_t start,uint32_t len,uint32_t mode,target_ulong nmask,uint32_t maxnode,uint32_t flags) { }
void __attribute__((weak)) call_sys_get_mempolicy_callback(CPUState* env,target_ulong pc,target_ulong policy,target_ulong nmask,uint32_t maxnode,uint32_t addr,uint32_t flags) { }
void __attribute__((weak)) call_sys_set_mempolicy_callback(CPUState* env,target_ulong pc,uint32_t mode,target_ulong nmask,uint32_t maxnode) { }
void __attribute__((weak)) call_sys_openat_callback(CPUState* env,target_ulong pc,uint32_t dfd,std::string filename,uint32_t flags,uint32_t mode) { }
void __attribute__((weak)) call_sys_mkdirat_callback(CPUState* env,target_ulong pc,uint32_t dfd,std::string pathname,uint32_t mode) { }
void __attribute__((weak)) call_sys_mknodat_callback(CPUState* env,target_ulong pc,uint32_t dfd,std::string filename,uint32_t mode,uint32_t dev) { }
void __attribute__((weak)) call_sys_fchownat_callback(CPUState* env,target_ulong pc,uint32_t dfd,std::string filename,uint32_t user,uint32_t group,uint32_t flag) { }
void __attribute__((weak)) call_sys_futimesat_callback(CPUState* env,target_ulong pc,uint32_t dfd,std::string filename,target_ulong utimes) { }
void __attribute__((weak)) call_sys_fstatat64_callback(CPUState* env,target_ulong pc,uint32_t dfd,std::string filename,target_ulong statbuf,uint32_t flag) { }
void __attribute__((weak)) call_sys_unlinkat_callback(CPUState* env,target_ulong pc,uint32_t dfd,std::string pathname,uint32_t flag) { }
void __attribute__((weak)) call_sys_renameat_callback(CPUState* env,target_ulong pc,uint32_t olddfd,std::string oldname,uint32_t newdfd,std::string newname) { }
void __attribute__((weak)) call_sys_linkat_callback(CPUState* env,target_ulong pc,uint32_t olddfd,std::string oldname,uint32_t newdfd,std::string newname,uint32_t flags) { }
void __attribute__((weak)) call_sys_symlinkat_callback(CPUState* env,target_ulong pc,std::string oldname,uint32_t newdfd,std::string newname) { }
void __attribute__((weak)) call_sys_readlinkat_callback(CPUState* env,target_ulong pc,uint32_t dfd,std::string path,target_ulong buf,uint32_t bufsiz) { }
void __attribute__((weak)) call_sys_fchmodat_callback(CPUState* env,target_ulong pc,uint32_t dfd,std::string filename,uint32_t mode) { }
void __attribute__((weak)) call_sys_faccessat_callback(CPUState* env,target_ulong pc,uint32_t dfd,std::string filename,uint32_t mode) { }
void __attribute__((weak)) call_sys_unshare_callback(CPUState* env,target_ulong pc,uint32_t unshare_flags) { }
void __attribute__((weak)) call_sys_set_robust_list_callback(CPUState* env,target_ulong pc,target_ulong head,uint32_t len) { }
void __attribute__((weak)) call_sys_get_robust_list_callback(CPUState* env,target_ulong pc,uint32_t pid,target_ulong head_ptr,target_ulong len_ptr) { }
void __attribute__((weak)) call_sys_splice_callback(CPUState* env,target_ulong pc,uint32_t fd_in,target_ulong off_in,uint32_t fd_out,target_ulong off_out,uint32_t len,uint32_t flags) { }
void __attribute__((weak)) call_sys_sync_file_range2_callback(CPUState* env,target_ulong pc,uint32_t fd,uint32_t flags,uint64_t offset,uint64_t nbytes) { }
void __attribute__((weak)) call_sys_tee_callback(CPUState* env,target_ulong pc,uint32_t fdin,uint32_t fdout,uint32_t len,uint32_t flags) { }
void __attribute__((weak)) call_sys_vmsplice_callback(CPUState* env,target_ulong pc,uint32_t fd,target_ulong iov,uint32_t nr_segs,uint32_t flags) { }
void __attribute__((weak)) call_sys_move_pages_callback(CPUState* env,target_ulong pc,uint32_t pid,uint32_t nr_pages,target_ulong pages,target_ulong nodes,target_ulong status,uint32_t flags) { }
void __attribute__((weak)) call_sys_getcpu_callback(CPUState* env,target_ulong pc,target_ulong cpu,target_ulong node,target_ulong cache) { }
void __attribute__((weak)) call_sys_kexec_load_callback(CPUState* env,target_ulong pc,uint32_t entry,uint32_t nr_segments,target_ulong segments,uint32_t flags) { }
void __attribute__((weak)) call_sys_utimensat_callback(CPUState* env,target_ulong pc,uint32_t dfd,std::string filename,target_ulong utimes,uint32_t flags) { }
void __attribute__((weak)) call_sys_signalfd_callback(CPUState* env,target_ulong pc,uint32_t ufd,target_ulong user_mask,uint32_t sizemask) { }
void __attribute__((weak)) call_sys_timerfd_create_callback(CPUState* env,target_ulong pc,uint32_t clockid,uint32_t flags) { }
void __attribute__((weak)) call_sys_eventfd_callback(CPUState* env,target_ulong pc,uint32_t count) { }
void __attribute__((weak)) call_sys_fallocate_callback(CPUState* env,target_ulong pc,uint32_t fd,uint32_t mode,uint64_t offset,uint64_t len) { }
void __attribute__((weak)) call_sys_timerfd_settime_callback(CPUState* env,target_ulong pc,uint32_t ufd,uint32_t flags,target_ulong utmr,target_ulong otmr) { }
void __attribute__((weak)) call_sys_timerfd_gettime_callback(CPUState* env,target_ulong pc,uint32_t ufd,target_ulong otmr) { }
void __attribute__((weak)) call_sys_signalfd4_callback(CPUState* env,target_ulong pc,uint32_t ufd,target_ulong user_mask,uint32_t sizemask,uint32_t flags) { }
void __attribute__((weak)) call_sys_eventfd2_callback(CPUState* env,target_ulong pc,uint32_t count,uint32_t flags) { }
void __attribute__((weak)) call_sys_epoll_create1_callback(CPUState* env,target_ulong pc,uint32_t flags) { }
void __attribute__((weak)) call_sys_dup3_callback(CPUState* env,target_ulong pc,uint32_t oldfd,uint32_t newfd,uint32_t flags) { }
void __attribute__((weak)) call_sys_pipe2_callback(CPUState* env,target_ulong pc,target_ulong arg0,uint32_t arg1) { }
void __attribute__((weak)) call_sys_inotify_init1_callback(CPUState* env,target_ulong pc,uint32_t flags) { }
void __attribute__((weak)) call_ARM_breakpoint_callback(CPUState* env,target_ulong pc) { }
void __attribute__((weak)) call_ARM_cacheflush_callback(CPUState* env,target_ulong pc,uint32_t start,uint32_t end,uint32_t flags) { }
void __attribute__((weak)) call_ARM_user26_mode_callback(CPUState* env,target_ulong pc) { }
void __attribute__((weak)) call_ARM_usr32_mode_callback(CPUState* env,target_ulong pc) { }
void __attribute__((weak)) call_ARM_set_tls_callback(CPUState* env,target_ulong pc,uint32_t arg) { }
void __attribute__((weak)) call_ARM_cmpxchg_callback(CPUState* env,target_ulong pc,uint32_t val,uint32_t src,target_ulong dest) { }
void __attribute__((weak)) call_ARM_null_segfault_callback(CPUState* env,target_ulong pc) { }


