#pragma once

#define MAXFDS 1000000
#define ADMIN_LOGIN_FILE "logins.txt"

void add_operator();
void del_operator();
int get_operator();
void add_dup();
int get_dup();
