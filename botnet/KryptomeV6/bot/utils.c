#ifdef DEBUG
#include <stdio.h>
#endif
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <ctype.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/net.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "includes.h"
#include "utils.h"

uint16_t checksum_generic(uint16_t *addr, uint32_t count)
{
    register unsigned long sum = 0;

    for(sum = 0; count > 1; count -= 2)
        sum += *addr++;
    if(count == 1)
        sum += (char)*addr;

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    
    return ~sum;
}

uint16_t checksum_tcpudp(struct iphdr *iph, void *buff, uint16_t data_len, int len)
{
    const uint16_t *buf = buff;
    uint32_t ip_src = iph->saddr;
    uint32_t ip_dst = iph->daddr;
    uint32_t sum = 0;
    
    while(len > 1)
    {
        sum += *buf;
        buf++;
        len -= 2;
    }

    if(len == 1)
        sum += *((uint8_t *) buf);

    sum += (ip_src >> 16) & 0xFFFF;
    sum += ip_src & 0xFFFF;
    sum += (ip_dst >> 16) & 0xFFFF;
    sum += ip_dst & 0xFFFF;
    sum += htons(iph->protocol);
    sum += data_len;

    while(sum >> 16) 
        sum = (sum & 0xFFFF) + (sum >> 16);

    return ((uint16_t) (~sum));
}
int util_memsearch(char *buf, int buf_len, char *mem, int mem_len)
{
    int i = 0, matched = 0;

    if(mem_len > buf_len)
        return -1;

    for(i = 0; i < buf_len; i++)
    {
        if(buf[i] == mem[matched])
        {
            if(++matched == mem_len)
                return i + 1;
        }
        else
            matched = 0;
    }

    return -1;
}

char *util_get_external_ip() {
	struct sockaddr_in serv;
    int sock = socket (AF_INET, SOCK_DGRAM, 0);
    if(sock < 0)
		return NULL;
	util_zero(&serv);
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INET_ADDR(8,8,8,8);
    serv.sin_port = htons(53);
    int err = connect(sock, (const struct sockaddr*)&serv, sizeof(serv));
	if(err < 0)
		return NULL;
    struct sockaddr_in name;
    socklen_t namelen = sizeof(name);
    err = getsockname(sock, (struct sockaddr*) &name, &namelen);
	if(err < 0)
		return NULL;
	char buffer[100];
    char* p = (char *)inet_ntop(AF_INET, &name.sin_addr, buffer, 100);
    close(sock);
    return p;
}

void util_zero(void *out) {
	int pos;
	for(pos = 0; pos < sizeof(out); pos++) {
		((unsigned char *)out)[pos] = 0;
	}
}

int util_strlen(void *buf) {
	int x = 0;
	while(((char *)buf)[x] != 0) {
		x++;
	}
	return x;
}

void *util_strdup(void *src)
{
    char* newstr = (char*) malloc( util_strlen( src) + 1);

    if (newstr) {
        util_strcpy(newstr, src);
    }

    return newstr;
}

int util_strcpy(void *dst, void *src)
{
    int l = util_strlen(src);

    util_memcpy(dst, src, l + 1);

    return l;
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

int util_strcat(void *dest, void *src)
{
    int rc = util_strlen(dest)+util_strlen(src);
	char *r_dest = (char *)dest;
	char *r_src = (char *)src;
    while(*r_dest) r_dest++;
    while((*r_dest++ = *r_src++));

    return rc;
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

void swap(char c, char c2)
{
	char tmp = c2;
	c2 = c;
	c = tmp;
}

/* A utility function to reverse a string  */
void reverse(char str[], int length)
{
    int start = 0;
    int end = length -1;
    while (start < end)
    {
		char tmp = *(str+end);
		*(str+end) = *(str+start);
        *(str+start) = tmp;
        start++;
        end--;
    }
}
  
// Implementation of itoa()
char* util_itoa(int num, char* str, int base)
{
    int i = 0;
    uint8_t isNegative = 0;
  
    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
  
    // In standard itoa(), negative numbers are handled only with 
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = 1;
        num = -num;
    }
  
    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }
  
    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';
  
    str[i] = '\0'; // Append string terminator
  
    // Reverse the string
    reverse(str, i);
  
    return str;
}
/*
char *util_itoa(int value, char* result, int base) {
    if (base < 2 || base > 36) { *result = '\0'; return result; }

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}*/

unsigned int util_strsplit(unsigned char *ptr, unsigned int *pos, unsigned char *buffer, int buffer_len, unsigned char delim) 
{
    unsigned char token[512];
    unsigned char *tmp;
    unsigned char found_delim = 0;
    unsigned char c;
    unsigned int j = 0; // stores token length
    unsigned int x = 0; // stores token number
    unsigned int i;

    tmp = buffer;

    for (i = 0; i < buffer_len + 1; i++) 
	{
        c = tmp[i];
        if (c == delim) // found occourence of the delimeter
        {
            if (*pos == x) // found next valid token
            {
                *pos = *pos + 1;
                util_strcpy(ptr, token);
                //ptr = ("%s", token);
                return 1;
            }
            util_zero(token);
            j = 0;
            x++;
            found_delim++;
        } 
		else if (c != '\0') // gather a token for when we find delimeter
        {
            token[j] = c;
            j++;
        } 
		else 
		{
            *pos = *pos + 1;
            util_strcpy(ptr, token);
            //ptr = ("%s", token);
            return 0;
        }
    }

    ptr = NULL;

    if (found_delim) 
	{ // we reached the end of the line
        return 0;
    }

    return 0; // no delim found
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

