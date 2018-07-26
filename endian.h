#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef _WIN32
#include <io.h>
#elif __linux__
#include <unistd.h>
#endif
#include <sys/stat.h>

#define u32 uint32_t
#define u16 uint16_t
#define u8 uint8_t
#define OffsetPointer(ptr, offset) ((void*)((char *)(ptr) + (offset)))

unsigned int toBEInt(unsigned int num);
unsigned short toBEShort(unsigned short num);
unsigned int toBEInt24(unsigned int num);