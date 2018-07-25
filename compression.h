#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "endian.h"

typedef struct {
	char magic[4];
	u_int32_t decodedSize;
	u_int32_t linkOffset;
	u_int32_t chunkOffset;
	//TODO: flag table?
} yayHeader;

size_t yayGetDecodedSize(void* yay);
void yayDecompress(char* in, char* out, size_t size);