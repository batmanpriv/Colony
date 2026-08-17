#pragma once

#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/net.h>

uint16_t checksum_generic(uint16_t *, uint32_t);
uint16_t checksum_tcpudp(struct iphdr *, void *, uint16_t, int);
int util_memsearch(char *buf, int buf_len, char *mem, int mem_len);
char *util_get_external_ip();
void util_zero(void *);
int util_strlen(void *);
void *util_strdup(void *);
int util_strcpy(void *, void *);
void util_memcpy(void *dst, void *src, int len);
int util_strcat(void *, void *);
void util_trim(char * str);
char *util_itoa(int, char *, int);
unsigned int util_strsplit(unsigned char *ptr, unsigned int *pos, unsigned char *buffer, int buffer_len, unsigned char delim);
unsigned char **util_strip(char *, int *, char);
unsigned int util_strexists(void *, void *);
