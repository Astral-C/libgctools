#include "endian.h"

typedef struct {
	char magic[4];
	u32 decodedSize;
	u32 linkOffset;
	u32 chunkOffset;
	//TODO: flag table?
} yayHeader;

size_t yayGetDecodedSize(void* yay);
void yayDecompress(char* in, char* out, size_t size);