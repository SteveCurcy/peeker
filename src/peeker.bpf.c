/* GPL-2.0 Copyright (c) 2025 Steve */
#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>
#include "peeker.h"

char LICENSE[] SEC("license") = "GPL";

struct {
  __uint(type, BPF_MAP_TYPE_HASH);
  __uint(max_entries, 8192);
  __type(key, pid_t);
  __type(value, proc_info_t);
} map_proc_info SEC(".maps");

struct {
  __uint(type, BPF_MAP_TYPE_HASH);
  __uint(max_entries, 8192);
  __type(key, __u32);
  __type(value, __u8);
} map_filter_list SEC(".maps");

struct {
  __uint(type, BPF_MAP_TYPE_RINGBUF);
  __uint(max_entries, 256 * 1024);
} rb SEC(".maps");

__always_inline static int handle_enter(sys_nr_e sys_nr)
{
  // NOTE that all variables should be initialized before using them.
  pid_t pid = -1;
  __u64 ts = 0;
  proc_info_t *p_proc_info = NULL;
  proc_info_t dummy = {0};
  char comm[32] = {0};

  // get current process ID and process information by it.
  pid = bpf_get_current_pid_tgid() >> 32;
  p_proc_info = bpf_map_lookup_elem(&map_proc_info, &pid);

  // check if information of process has been recorded.
  if (!p_proc_info) {
    if (sys_nr == SYS_OPEN) {
      // Initialize the process information if not.
      __builtin_memset(&(dummy.comm), 0, sizeof(dummy.comm));
      bpf_get_current_comm(&(dummy.comm), 32);
      bpf_get_current_comm(&comm, sizeof(comm));
      dummy.behav_id = get_behavior(comm);
      dummy.pid = pid;
      dummy.interval = 1;
      dummy.err_intv = 0;
      dummy.read_bytes = 0;
      dummy.write_bytes = 0;
      bpf_map_update_elem(&map_proc_info, &pid, &dummy, BPF_NOEXIST);
    }
  } else {
    p_proc_info->interval++;
    bpf_map_update_elem(&map_proc_info, &pid, p_proc_info, BPF_EXIST);
  }

  return 0;
}

static int handle_exit(long err, sys_nr_e sys_nr)
{
  pid_t pid = -1;
  proc_info_t *p_proc_info = NULL;
  char comm[32] = {0};

  pid = bpf_get_current_pid_tgid() >> 32;
  p_proc_info = bpf_map_lookup_elem(&map_proc_info, &pid);

  if (!p_proc_info) return 0;

  if (err < 0) {
    p_proc_info->err_intv++;
  } else {
    struct task_struct *ptask = (struct task_struct *)bpf_get_current_task();
    unsigned char *exe_name = (unsigned char *)BPF_CORE_READ(ptask, mm, exe_file, f_path.dentry, d_iname);
    switch (sys_nr) {
      case SYS_READ:
        p_proc_info->read_bytes += (__u64) err;
        break;
      case SYS_WRITE:
        p_proc_info->write_bytes += (__u64) err;
        break;
      default:
        break;
    }
  }
  bpf_map_update_elem(&map_proc_info, &pid, p_proc_info, BPF_EXIST);

  return 0;
}

SEC("tp/syscalls/sys_enter_openat")
int tracepoint__syscalls__sys_enter_openat(struct trace_event_raw_sys_enter *ctx) {
  handle_enter(SYS_OPEN);
  return 0;
}

SEC("tp/syscalls/sys_exit_openat")
int tracepoint__syscalls__sys_exit_openat(struct trace_event_raw_sys_exit *ctx) {
  long ret = BPF_CORE_READ(ctx, ret);
  handle_exit(ret, SYS_OPEN);
  return 0;
}

SEC("tp/syscalls/sys_enter_read")
int tracepoint__syscalls__sys_enter_read(struct trace_event_raw_sys_enter *ctx) {
  handle_enter(SYS_READ);
  return 0;
}

SEC("tp/syscalls/sys_exit_read")
int tracepoint__syscalls__sys_exit_read(struct trace_event_raw_sys_exit *ctx) {
  long ret = BPF_CORE_READ(ctx, ret);
  handle_exit(ret, SYS_READ);
  return 0;
}

SEC("tp/syscalls/sys_enter_write")
int tracepoint__syscalls__sys_enter_write(struct trace_event_raw_sys_enter *ctx) {
  handle_enter(SYS_WRITE);
  return 0;
}

SEC("tp/syscalls/sys_exit_write")
int tracepoint__syscalls__sys_exit_write(struct trace_event_raw_sys_exit *ctx) {
  long ret = BPF_CORE_READ(ctx, ret);
  handle_exit(ret, SYS_WRITE);
  return 0;
}

SEC("tp/syscalls/sys_enter_close")
int tracepoint__syscalls__sys_enter_close(struct trace_event_raw_sys_enter *ctx) {
  handle_enter(SYS_CLOSE);
  return 0;
}

SEC("tp/syscalls/sys_exit_close")
int tracepoint__syscalls__sys_exit_close(struct trace_event_raw_sys_exit *ctx) {
  long ret = BPF_CORE_READ(ctx, ret);
  handle_exit(ret, SYS_CLOSE);
  return 0;
}

// SEC("tp/syscalls/sys_enter_execve")
// int tracepoint__syscalls__sys_enter_execve(struct trace_event_raw_sys_enter *ctx) {
//   handle_enter(SYS_EXECVE);
//   return 0;
// }

// SEC("tp/syscalls/sys_exit_execve")
// int tracepoint__syscalls__sys_exit_execve(struct trace_event_raw_sys_exit *ctx) {
//   long ret = BPF_CORE_READ(ctx, ret);
//   handle_exit(ret, SYS_EXECVE);
//   return 0;
// }

SEC("tp/syscalls/sys_enter_exit_group")
int tracepoint__syscalls__sys_enter_exit_group(struct trace_event_raw_sys_enter *ctx) {

  pid_t pid = -1;
  proc_info_t *p_proc_info = NULL;
  proc_info_t *buffer = NULL;

  pid = bpf_get_current_pid_tgid() >> 32;
  p_proc_info = bpf_map_lookup_elem(&map_proc_info, &pid);
  
  if (!p_proc_info) return 0;
  bpf_map_delete_elem(&map_proc_info, &pid);

  struct task_struct *ptask = (struct task_struct *) bpf_get_current_task();
  __u64 sum_exec_runtime = BPF_CORE_READ(ptask, se.sum_exec_runtime);
  p_proc_info->interval = (sum_exec_runtime + 1) / (p_proc_info->interval + 1);
  p_proc_info->err_intv = (sum_exec_runtime + 1) / (p_proc_info->err_intv + 1);

  /* send the statistic information to userspace */
  buffer = bpf_ringbuf_reserve(&rb, sizeof(proc_info_t), 0);
  if (!buffer) {	// the same as `if (buffer == NULL)`, for performance.
    return 0;
  }

  /* copy the proc info into the ring buffer */
  __builtin_memcpy(buffer, p_proc_info, sizeof(proc_info_t));
  bpf_ringbuf_submit(buffer, 0);

    return 0;
}