#pragma once

#define ENC_ENCRYPT 1
#define ENC_DECRYPT 0

void enc_push(const uint32_t);
void enc_writebuffer(uint32_t);
void enc_encipher(unsigned int, uint32_t, uint32_t const);
void enc_decipher(unsigned int, uint32_t, uint32_t const);
void enc_switch(uint8_t *, uint8_t *, int);
