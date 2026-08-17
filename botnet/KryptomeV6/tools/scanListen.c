#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 1617

int main(int argc, char *argv[])
{
    struct sockaddr_in srvaddr;
    
    srvaddr.sin_addr.s_addr = INADDR_ANY;
    srvaddr.sin_port = htons(PORT);
    srvaddr.sin_family = AF_INET;

    while(1)
    {
        int srvFD = socket(AF_INET, SOCK_STREAM, 0);
        if(srvFD < 0)
        {
            printf("Failed to call socket() for listening socket\r\n");
            return 1;
        }

        if(bind(srvFD, (struct sockaddr *)&srvaddr, sizeof(srvaddr)) < 0)
        {
            printf("Failed to call bind() for listening socket\r\n");
            return 1;
        }

        listen(srvFD, 50);

        while(1)
        {
            struct sockaddr_in cliaddr;
            socklen_t cliaddr_len;
            int cliFD = accept(srvFD, (struct sockaddr *)&cliaddr, &cliaddr_len);
            if(cliFD < 0)
            {
                printf("Failed to call accept() recreating server socket\r\n");
                break;
            }
            char buffer[1024];
            if(recv(cliFD, buffer, 1024, MSG_NOSIGNAL) <= 0)
            {
                printf("Failed to call recv() from client socket\r\n");
                close(cliFD);
                continue;
            }
            
            uint32_t host;
            uint16_t port;
            char username[64], password[64];
            uint16_t username_len, password_len;

            memcpy(&host, buffer, sizeof(uint32_t));
            memcpy(&port, buffer+sizeof(uint32_t), sizeof(uint16_t));
            memcpy(&username_len, buffer+sizeof(uint32_t)+sizeof(uint16_t), sizeof(uint16_t));
            memcpy(username, buffer+sizeof(uint32_t)+sizeof(uint16_t)+sizeof(uint16_t), username_len);
            memcpy(&password_len, buffer+sizeof(uint32_t)+sizeof(uint16_t)+sizeof(uint16_t)+username_len, sizeof(uint16_t));
            memcpy(password, buffer+sizeof(uint32_t)+sizeof(uint16_t)+sizeof(uint16_t)+username_len+sizeof(uint16_t), password_len);

            printf("%d.%d.%d.%d:%d %s:%s\r\n", host & 0xff, (host >> 8) & 0xff, (host >> 16) & 0xff, (host >> 24) & 0xff, port, username, password);
            FILE *fp = fopen("scanListen_rep.txt", "a");
            if(!fp)
            {
                close(cliFD);
                continue;
            }

            fprintf(fp, "%d.%d.%d.%d:%d %s:%s\r\n", host & 0xff, (host >> 8) & 0xff, (host >> 16) & 0xff, (host >> 24) & 0xff, port, username, password);
            fflush(fp);
            fclose(fp);
            close(cliFD);
            continue;
        }
    }
}