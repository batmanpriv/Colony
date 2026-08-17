#pragma once

int resolve(char *  , char *);
char *id2arch(int id);
char *id2endianess(int id);
void util_zero(void *out);
int util_strlen(void *buf);
void util_memcpy(void *dst, void *src, int len);
int util_strcpy(void *dst, void *src);
void *util_strdup(void *src);
void util_trim(char * str);
int util_strcat(void *dest, void *src);
unsigned char **util_strip(char *buf, int *len, char delim);
unsigned int util_strexists(void *haystack, void *needle);
int fdgets(unsigned char *buffer, int bufferSize, int fd);
void trim(char *str);
int make_socket_non_blocking(int sfd);
int create_and_bind(char *port);
void client_addr(struct sockaddr_in addr);
