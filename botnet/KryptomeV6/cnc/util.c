#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "includes.h"
#include "util.h"

int resolve(char * site , char* ip)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;
         
    if ( (he = gethostbyname( site ) ) == NULL)
    {
        herror("gethostbyname");
        return 1;
    }
 
    addr_list = (struct in_addr **) he->h_addr_list;
     
    for(i = 0; addr_list[i] != NULL; i++)
    {
        strcpy(ip , inet_ntoa(*addr_list[i]) );
        return 0;
    }
     
    return 1;
}

char *id2arch(int id)
{
	switch(id)
	{
		case -1:
			return "unknown";
		case 0:
			return "arm4";
		case 1:
			return "arm5";
		case 2:
			return "arm6";
		case 3:
			return "arm7";
		case 4:
			return "mips";
		case 5:
			return "mipsel";
		case 6:
			return "x86";
		case 7:
			return "x86_64";
		case 8:
			return "powerpc";
		case 9:
			return "sh4";
		default:
			return "unknown";
	}
}

char *id2endianess(int id)
{
	switch(id)
	{
		case 0:
			return "big_endian";
		case 1:
			return "little_endian";
		case 2:
			return "big_endian_w";
		case 3:
			return "little_endian_w";
		case 4:
			return "unknown";
	}
}

void util_zero(void *out) {
	uint8_t pos;
	for(pos = 0; pos < sizeof(out); pos++) {
		((unsigned char *)out)[pos] = '\0';
	}
}

int util_strlen(void *buf) {
	int x = 0;
	while(((unsigned char *)buf)[x] != 0) {
		x++;
	}
	return x;
}

void util_memcpy(void *dst, void *src, int len)
{
    char *r_dst = (char *)dst;
    char *r_src = (char *)src;
    while(len--)
    {
        *r_dst++ = *r_src++;
    }
}

int util_strcpy(void *dst, void *src)
{
    int l = util_strlen(src);

    util_memcpy(dst, src, l + 1);

    return l;
}


void *util_strdup(void *src)
{
    char* newstr = (char*) malloc( util_strlen( src) + 1);

    if (newstr) {
        util_strcpy(newstr, src);
    }

    return newstr;
}

void util_trim(char * str) {
	int i;
	int begin = 0;
	int end = util_strlen(str) - 1;
	while (isspace(str[begin])) begin++;
	while ((end >= begin) && isspace(str[end])) end--;
	for (i = begin; i <= end; i++) str[i - begin] = str[i];
	str[i - begin] = '\0';
}

int util_strcat(void *dest, void *src)
{
    int rc = util_strlen(dest)+util_strlen(src);
	char *r_dest = (char *)dest;
	char *r_src = (char *)src;
    while(*r_dest) r_dest++;
    while((*r_dest++ = *r_src++));

    return rc;
}
unsigned char **util_strip(char *buf, int *len, char delim) 
{
	unsigned char j_str[512];
	unsigned char pos = 0;
	unsigned char j = 0;
	unsigned char j_len = 0;
	unsigned char c = buf[pos];
	
	unsigned char **ret = calloc(1, sizeof(unsigned char *));
	unsigned char **tmp_ret = {NULL};
	
	util_zero(j_str);
	
	while(c != 0)
	{
		if(c == delim) {
			if(j_len == 0)
			{
				pos = pos+1;
				c = buf[pos];
				continue;
			}
			else
			{
				j_str[j_len] = 0;
				if(ret != NULL) {
					if(j > 0) {
						tmp_ret = malloc(j * sizeof(unsigned char *));
						int x;
						for(x = 0; x < j; x++) 
						{
							tmp_ret[x] = ret[x];
						}
					}
					free(ret);
					ret = NULL;
				}
				ret = malloc((j+1) * sizeof(unsigned char *));
				if(!ret) {
					j = 0;
					continue;
				}
				if(tmp_ret != NULL) {
					int x;
					for(x = 0; x < j; x++) 
					{
						ret[x] = tmp_ret[x];
					}
					free(tmp_ret);
					tmp_ret = NULL;
				}
				
				ret[j] = NULL;// below will allocate memory
				ret[j] = util_strdup(j_str);
				j = j+1;
				
				util_zero(j_str);
				j_len = 0;
				
				pos = pos+1;
				c = buf[pos];
				continue;
			}
		}
		else
		{
			j_str[j_len] = c;
			j_len = j_len+1;
			
			pos = pos+1;
			c = buf[pos];
			continue;
		}
	}
	
	if(buf[pos-1] != delim && j_len != 0)
	{
		if(ret != NULL) {
			tmp_ret = malloc(j * sizeof(unsigned char *));
			int x;
			for(x = 0; x < j; x++) 
			{
				tmp_ret[x] = ret[x];
			}
			free(ret);
			ret = NULL;
		}
		ret = malloc((j+1) * sizeof(unsigned char *));
		if(!ret) {
			j = 0;
			if(len != NULL) {
				*len = 0;
			}
			//free(j_str);
			return NULL;
		}
		if(tmp_ret != NULL) {
			int x;
			for(x = 0; x < j; x++) 
			{
				ret[x] = tmp_ret[x];
			}
			free(tmp_ret);
			tmp_ret = NULL;
		}

		j_str[j_len] = 0;
		ret[j] = NULL;
        ret[j] = util_strdup(j_str);
        j = j+1;

        util_zero(j_str);
        j_len = 0;

        pos = pos+1;
        c = buf[pos];
	}
	
	if(len != NULL) {
		*len = j;
    }
	//free(j_str);
	return ret;
}

unsigned int util_strexists(void *haystack, void *needle) {
	uint8_t haystack_len = util_strlen(haystack);
	uint8_t needle_len = util_strlen(needle);
	uint8_t needle_found = 0;
	uint8_t c_last = 0;
	uint8_t c_cur = 0;
	uint8_t pos = 0;
	
	for(pos = 0; pos < haystack_len; pos++) {
		c_cur = ((unsigned char *)haystack)[pos];
		if(c_cur == 0)
			break;
		if(c_cur == ((unsigned char *)needle)[needle_found] && (c_last == 0 || c_last == ((unsigned char *)needle)[needle_found-1])) {
			needle_found++;
			if(needle_found == needle_len) return 1;
		} else {
			needle_found = 0;
			c_last = 0;
			continue;
		}
		c_last = c_cur;
	}
	
	return 0;
}


int fdgets(unsigned char *buffer, int bufferSize, int fd)
{
	int total = 0, got = 1;
	while (got == 1 && total < bufferSize && *(buffer + total - 1) != '\n')
	{
		got = read(fd, buffer + total, 1);
		total++;
	}

	return got;
}

void trim(char *str)
{
	int i;
	int begin = 0;
	int end = strlen(str) - 1;
	while (isspace(str[begin])) begin++;
	while ((end >= begin) && isspace(str[end])) end--;
	for (i = begin; i <= end; i++) str[i - begin] = str[i];
	str[i - begin] = '\0';
}

int make_socket_non_blocking(int sfd)
{
	int flags, s;
	flags = fcntl(sfd, F_GETFL, 0);
	if (flags == -1)
	{
		perror("fcntl");
		return -1;
	}

	flags |= O_NONBLOCK;
	s = fcntl(sfd, F_SETFL, flags);
	if (s == -1)
	{
		perror("fcntl");
		return -1;
	}

	return 0;
}

int create_and_bind(char *port)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int s, sfd;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	s = getaddrinfo(NULL, port, &hints, &result);
	if (s != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		return -1;
	}

	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1) continue;
		int yes = 1;
		if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) perror("setsockopt");
		s = bind(sfd, rp->ai_addr, rp->ai_addrlen);
		if (s == 0)
		{
			break;
		}

		close(sfd);
	}

	if (rp == NULL)
	{
		fprintf(stderr, "Could not bind\n");
		return -1;
	}

	freeaddrinfo(result);
	return sfd;
}

void client_addr(struct sockaddr_in addr)
{
	printf("IP:%d.%d.%d.%d\n",
		addr.sin_addr.s_addr &0xFF,
		(addr.sin_addr.s_addr &0xFF00) >> 8,
		(addr.sin_addr.s_addr &0xFF0000) >> 16,
		(addr.sin_addr.s_addr &0xFF000000) >> 24);
	FILE * logFile;
	logFile = fopen("logfiles/ip.log", "a");
	fprintf(logFile, "\nIP:%d.%d.%d.%d ",
		addr.sin_addr.s_addr &0xFF,
		(addr.sin_addr.s_addr &0xFF00) >> 8,
		(addr.sin_addr.s_addr &0xFF0000) >> 16,
		(addr.sin_addr.s_addr &0xFF000000) >> 24);
	fclose(logFile);
}
