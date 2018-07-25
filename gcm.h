#include <stdio.h>

typedef struct {
	struct {
		u_int32_t flags : 8;
		u_int32_t nameOffset : 24;
	};
	union{
		u_int32_t fileOffset;
		u_int32_t parentOffset;
	};
	union {
		u_int32_t fileLength;
		u_int32_t nextOffset;
	};
} gcmFst;

typedef struct {

	FILE* stream;
	gcmFst* fst;
	u_int32_t fstEntryCount;

} gcmHandle;

void gcmOpenROM(gcmHandle* handle, char* path);
void gcmLoadFst(gcmHandle* handle, void* data, size_t fstSize);
void gcmDump(gcmHandle* handle);
void gcmChangeDir(gcmHandle* handle, char* dirPath);
void gcmCloseStream(gcmHandle* handle);

size_t gcmGetFstSize(gcmHandle* handle);