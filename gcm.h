#include "endian.h"

typedef struct {
	struct {
		u32 flags : 8;
		u32 nameOffset : 24;
	};
	union{
		u32 fileOffset;
		u32 parentOffset;
	};
	union {
		u32 fileLength;
		u32 nextOffset;
	};
} gcmFst;

typedef struct {

	FILE* stream;
	gcmFst* fst;
	uint32_t fstEntryCount;

} gcmHandle;

void gcmOpenROM(gcmHandle* handle, char* path);
void gcmLoadFst(gcmHandle* handle, void* data, size_t fstSize);
void gcmDump(gcmHandle* handle);
void gcmChangeDir(gcmHandle* handle, char* dirPath);
void gcmCloseStream(gcmHandle* handle);

size_t gcmGetFstSize(gcmHandle* handle);