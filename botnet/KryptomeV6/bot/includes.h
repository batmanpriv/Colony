#pragma once

#define INET_ADDR(o1,o2,o3,o4) (htonl((o1 << 24) | (o2 << 16) | (o3 << 8) | (o4 << 0)))
#define LOCALHOST (INET_ADDR(127,0,0,1))

void main_handle();
