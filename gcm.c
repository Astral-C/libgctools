#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "gcm.h"
#include "endian.h"

#define OffsetPointer(ptr, offset) ((void*)((char *)(ptr) + (offset)))

void gcmOpenROM(gcmHandle* handle, char* path){
	handle->stream = fopen(path, "rb");
}

size_t gcmGetFstSize(gcmHandle* handle){
	size_t fstSize = 0;
	fseek(handle->stream, 0x428, 0);
	fread(&fstSize, sizeof(int), 1, handle->stream);
	fstSize = toBEInt((unsigned int)fstSize);
	return fstSize;
}

void gcmCloseStream(gcmHandle* handle){
	fclose(handle->stream);
}

void gcmDump(gcmHandle* handle){
	//0 is always the root entry, handle it manually
	char* stringTable = OffsetPointer(handle->fst, (toBEInt(handle->fst->fileLength) * 0xC));
	for (int i = 0; i < toBEInt(handle->fst->fileLength)-1; i++){
		gcmFst* curEntry = &handle->fst[i];
		char* curEntryName =  OffsetPointer(stringTable, toBEInt24(curEntry->nameOffset));
		if(curEntry->flags == 1) {
			//skip for now
		}
		else if(curEntry->flags == 0) {
			void* fileBuffer = malloc(toBEInt(curEntry->fileLength));
			int ret = ftell(handle->stream);
			fseek(handle->stream, toBEInt(curEntry->fileOffset), 0);
			fread(fileBuffer, toBEInt(curEntry->fileLength), 1, handle->stream);
			fseek(handle->stream, ret, 0);
			FILE* out = fopen(curEntryName, "wb");
			fwrite(fileBuffer, toBEInt(curEntry->fileLength), 1, out);
			fclose(out);
			free(fileBuffer);
		}
	}
}

void gcmDumpDir(gcmHandle* handle){

}

void gcmLoadFst(gcmHandle* handle, void* data, size_t fstSize){
	int fstOffset = 0;
	fseek(handle->stream, 0x424, 0);
	fread(&fstOffset, sizeof(int), 1, handle->stream);
	fstOffset = toBEInt(fstOffset);
	fseek(handle->stream, fstOffset, 0);
	fread(data, fstSize, 1, handle->stream);
	handle->fst = data;
}