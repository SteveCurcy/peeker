/* GPL-2.0 Copyright (c) 2025 Steve */
#ifndef __PEEKER_H
#define __PEEKER_H

typedef enum {
    SYS_OPEN = 0,
    SYS_READ,
    SYS_WRITE,
    SYS_CLOSE,
    SYS_EXECVE
} sys_nr_e;

typedef enum __Behavior {
    CAT = 0,
    TOUCH,
    RM,
    GZIP,
    ZIP,
    UNZIP,
    SPLIT,
    CP,
    MV,
    OTHER
} behavior_e;

/**
 * The structure of the process information
 * counter method is not good. Instead, I wanna use
 * frequency method or interval method to identify the
 * behavior of the process.
 * member read_bytes and write_bytes are used to record
 * the number of bytes read and written by the process, and
 * to calculate the ratio of read and write.
 */
typedef struct {
    int   pid;          // process ID
    char comm[32];
    behavior_e behav_id;     // the behavior ID of the process
    __u64 interval;     // the average of interval between concerned system calls
    __u64 err_intv;     // the number of error calls
    __u64 read_bytes;   // the number of bytes read
    __u64 write_bytes;  // the number of bytes written
} proc_info_t;

/**
 * Calculate the hash value of the string
 * @param buf the string to be hashed
 * @return the hash value of the string
 */
__u32 str_hash(const char *buf);
/**
 * Get the behavior ID of the process
 * @param proc_name the name of the process
 * @return the behavior ID of the process which is a enum value
 */
behavior_e get_behavior(const char *proc_name);
/**
 * Get the behavior name of the process
 * @param behav_id the behavior ID of the process
 * @return the behavior name corresponding to the process ID
 */
const char* get_behavior_name(behavior_e behav_id);

__u32 str_hash(const char *buf) {
    __u32 hash = 5381;
    int len = 0;
    for (int i = 0; i < 32; i++) {
        if (buf[i] == '\0') {
            len = i;
            break;
        }
    }
    for (int i = 0; i < len; i++) {
        hash = ((hash << 5) + hash) + buf[i]; /* hash * 33 + c */
    }
    return hash;
}

behavior_e get_behavior(const char *proc_name) {
    __u32 str_hash_value = str_hash(proc_name);
    switch (str_hash_value) {
        case 0xb8864fd: return CAT;
        case 0x1070f788: return TOUCH;
        case 0x597964: return RM;
        case 0x7c979f7f: return GZIP;
        case 0xb88c7d8: return ZIP;
        case 0x108299bb: return UNZIP;
        case 0x105f45f1: return SPLIT;
        case 0x597778: return CP;
        case 0x5978c8: return MV;
        default: return OTHER;
    }
}

const char* get_behavior_name(behavior_e behav_id) {
    switch (behav_id) {
        case CAT: return "cat";
        case TOUCH: return "touch";
        case RM: return "rm";
        case GZIP: return "gzip";
        case ZIP: return "zip";
        case UNZIP: return "unzip";
        case SPLIT: return "split";
        case CP: return "cp";
        case MV: return "mv";
        default: return "other";
    }
}

#endif