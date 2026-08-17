#pragma once

#include <stdint.h>
#include "includes.h"

struct clientdata_t 
{
	uint32_t ip;
	float version;
	char name[64];
	int endianess;
	int build;
	char connected;
};

struct bots_worker_args
{
	int epollfd;
	int listenfd;
};

struct clientdata_t *bots_get_clients();
void broadcast(char *msg, int max);
void *BotEventLoop(void *useless);
unsigned int BotsConnected();
