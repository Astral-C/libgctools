#include "endian.h"

unsigned int toBEInt(unsigned int num){
	return ( ((num>>24)&0xFF) | ((num<<8) & 0xFF0000) | ((num>>8)&0xFF00) | ((num<<24)&0xFF000000) );
}

unsigned short toBEShort(unsigned short num){
	return ( ((num<<8)&0xFF00) | ((num>>8)&0x00FF) );
}

unsigned int toBEInt24(unsigned int num){
	return ( ((num<<8)&0x00FF) | ((num>>16)&0x000000FF) | ((num>>24) & 0x0000FF) | ((num>>8) & 0xFF));
}