#define _GNU_SOURCE

#include <stdlib.h>
#include <signal.h>
#include <stdint.h>
#include <stdarg.h>
#include "headers/util.h"
#include "headers/includes.h"

void enc_push(const uint32_t b)
{
  	buffer[p] = (uint8_t)b;
  	p++;
  	buffer[p] = '\0';
}

void enc_writebuffer(uint32_t v[2])
{
  	enc_push(v[0] & 0x000000ff);
  	enc_push((v[0] & 0x0000ff00) >> 8);
  	enc_push((v[0] & 0x00ff0000) >> 16);
  	enc_push((v[0] & 0xff000000) >> 24);

  	enc_push(v[1] & 0x000000ff);
  	enc_push((v[1] & 0x0000ff00) >> 8);
  	enc_push((v[1] & 0x00ff0000) >> 16);
  	enc_push((v[1] & 0xff000000) >> 24);
}

void enc_encipher(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4])
{
  	unsigned int i;
  	uint32_t v0 = v[0], v1 = v[1], sum = 0, delta = 0x2E5673EA;

  	for (i = 0; i < num_rounds; i++)
  	{
	    v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
	    sum += delta;
	    v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 11) & 3]);
  	}

  	v[0] = v0; v[1] = v1;
}

void enc_decipher(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4])
{
  	unsigned int i;
  	uint32_t v0 = v[0], v1 = v[1], delta = 0x2E5673EA, sum = delta * num_rounds;

    while (sum)
  	{
    	v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 11) & 3]);
    	sum -= delta;
    	v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
  	}

  	v[0] = v0; v[1] = v1;
}

void enc_switch(uint8_t *d, uint8_t *key, int mode)
{
  	uint32_t k[4], v[2];
  	int i = 0, n = 0, len = strlen_(d);

  	k[0] = key[3] << 24 | key[2] << 16 | key[1] << 8 | key[0];
  	k[1] = key[7] << 24 | key[6] << 16 | key[5] << 8 | key[4];
  	k[2] = key[11] << 24 | key[10] << 16 | key[9] << 8 | key[8];
  	k[3] = key[15] << 24 | key[14] << 16 | key[13] << 8 | key[12];

  	memset_(buffer, 0, sizeof(buffer));
  	p = 0;

  	for (i = 0; i < len; i = i+4)
  	{
    	if ((i + 3) < len)
    	{
      		if (n == 0)
      		{
        		v[0] = d[i + 3] << 24 | d[i + 2] << 16 | d[i + 1] << 8 | d[i];
        		n = 1;
      		}
      		else if (n == 1)
      		{
        		v[1] = d[i + 3]<<24 | d[i + 2] << 16 | d[i + 1] << 8 | d[i];
        		n = 0;

        		if (mode)
          			enc_encipher(32, v, k);
        		else
          			enc_decipher(32, v, k);

        		enc_writebuffer(v);
      		}
    	}
    	else
    	{
      		int empty = (i + 3) - len;
      		switch (empty)
      		{
      			case 1:
      			{
        			if (n == 0)
        			{
          				v[0] = 0 << 24 | d[i + 2] << 16 | d[i + 1] << 8 | d[i];
          				v[1] = 0;
        			}
        			else
        			{
          				v[0] = 0;
          				v[1] = 0 << 24 | d[i + 2] << 16 | d[i + 1] << 8 | d[i];
        			}
        			break;
      			}

      			case 2:
      			{
        			if (n == 0)
        			{
          				v[0] = 0 << 24 | 0 << 16 | d[i + 1] << 8 | d[i];
          				v[1] = 0;
        			}
        			else
        			{
          				v[0] = 0;
          				v[1] = 0 << 24 | 0 << 16 | d[i + 1] << 8 | d[i];
        			}
        			break;
      			}

      			case 3:
      			{
        			if (n == 0)
        			{
          				v[0] = 0 << 24 | 0 << 16 | 0 << 8 | d[i];
          				v[1] = 0;
        			}
        			else
        			{
          				v[0] = 0;
          				v[1] = 0 << 24 | 0 << 16 | 0 << 8 | d[i];
        			}
        			break;
      			}

      			default:
      			{
        			v[0] = 0;
        			v[1] = 0;
        			break;
        		}
      		}

      		if (mode)
        		enc_encipher(32, v, k);
      		else
        		enc_decipher(32, v, k);

      		enc_writebuffer(v);
    	}
  	}

  	for (i = 0; i < 32; i++)
  	{
  		if (buffer[i] == '*')
  			buffer[i] = 0;
  	}
}
