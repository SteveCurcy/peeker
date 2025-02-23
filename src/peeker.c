/* GPL-2.0
 * Copyright (c) 2025 Steve */
#include <pwd.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <linux/version.h>
#include <bpf/libbpf.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "peeker.h"
#include "peeker.skel.h"

static int libbpf_print_fn(enum libbpf_print_level level, const char *format, va_list args) {
    return vfprintf(stderr, format, args);
}

static volatile bool exiting = false;

static void sig_handler(int sig)
{
  exiting = true;
}

static int handle_event(void *ctx, void *data, size_t data_sz)
{
  proc_info_t *proc_info = (proc_info_t *)data;
  __u64 tot_bytes = proc_info->read_bytes + proc_info->write_bytes;
  double r_ratio = (double)(proc_info->read_bytes + 1) / (tot_bytes + 2);
  double w_ratio = (double)(proc_info->write_bytes + 1) / (tot_bytes + 2);
  double rw_ratio = r_ratio / w_ratio;
  if (rw_ratio >= 10.0) {
    rw_ratio = 10.0;
  }
  printf("pid: %d, comm: %s, behavior: %s, interval: %llu, err_intv: %llu, rw_ratio: %lf\n",
    proc_info->pid, proc_info->comm,
    get_behavior_name(proc_info->behav_id),
    proc_info->interval, proc_info->err_intv, rw_ratio);
  return 0;
}

int main(int argc, char **argv)
{
  struct ring_buffer *rb = NULL;
  struct peeker_bpf *skel;
  int err;

  printf("", sizeof(proc_info_t));

  /* Set up libbpf errors and debug info callback */
  libbpf_set_print(libbpf_print_fn);

  /* Cleaner handling of Ctrl-C */
  signal(SIGINT, sig_handler);
  signal(SIGTERM, sig_handler);

  /* Load and verify BPF application */
  skel = peeker_bpf__open_and_load();
  if (!skel) {
    fprintf(stderr, "Failed to open and load BPF skeleton\n");
    goto cleanup;
  }

  err = peeker_bpf__attach(skel);
  if (err) {
    fprintf(stderr, "Failed to attach BPF skeleton!\n");
        goto cleanup;
  }

  /* Set up ring buffer polling */
  rb = ring_buffer__new(bpf_map__fd(skel->maps.rb), handle_event, NULL, NULL);
  if (!rb) {
    err = -1;
    fprintf(stderr, "Failed to create ring buffer\n");
    goto cleanup;
  }

  /* Process events */
  while (!exiting) {
    err = ring_buffer__poll(rb, 100 /* timeout, ms */);
    /* Ctrl-C will cause -EINTR */
    if (err == -EINTR) {
      err = 0;
      break;
    }
    if (err < 0) {
      printf("Error polling perf buffer: %d\n", err);
      break;
    }
  }

cleanup:
  /* Clean up */
  ring_buffer__free(rb);
  peeker_bpf__destroy(skel);

  return err < 0 ? -err : 0;
}