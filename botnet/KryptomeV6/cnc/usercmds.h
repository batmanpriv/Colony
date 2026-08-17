#pragma once

struct bots_count_t {
    char id[64];
    int count;
};

int usercmds_chat(int fd);
void usercmds_admin_bots_count_name(int fd);
void usercmds_admin_bots_count_architecture(int fd);
void usercmds_admin_bots_count_version(int fd);
void usercmds_admin_bots_count_endianess(int fd);
