#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "endian.h"
#include "rarc.h"

#define OffsetPointer(ptr, offset) ((void*)((char *)(ptr) + (offset)))

size_t rarcGetSize(void* file){
	size_t size = ((rarcHeader*)file)->totalSize;
	return toBEInt(size);
}

void rarcLoadArchive(rarcArchive* arc, void* data){
	arc->header = data;
	if (arc->header->sig == 0x52415243){
		arc->fileSystem = OffsetPointer(arc->header, arc->header->fileSystemOffset);
		arc->dirs = OffsetPointer(arc->fileSystem, arc->fileSystem->dirOffset);
		arc->files = OffsetPointer(arc->fileSystem, arc->fileSystem->fileOffset);
		arc->names = OffsetPointer(arc->fileSystem, arc->fileSystem->strOffset);
		arc->fileData = OffsetPointer(arc->header, (arc->header->fileSystemOffset + arc->header->fileSystemSize));
	} else if(arc->header->sig == 0x43524152){
		arc->header->totalSize = toBEInt(arc->header->totalSize);
		arc->header->fileSystemOffset = toBEInt(arc->header->fileSystemOffset);
		arc->header->fileSystemSize = toBEInt(arc->header->fileSystemSize);

		arc->fileSystem = OffsetPointer(arc->header, arc->header->fileSystemOffset);

		arc->fileSystem->dirCount = toBEInt(arc->fileSystem->dirCount);
		arc->fileSystem->dirOffset = toBEInt(arc->fileSystem->dirOffset);
		arc->fileSystem->fileCount = toBEInt(arc->fileSystem->fileCount); 
		arc->fileSystem->fileOffset =  toBEInt(arc->fileSystem->fileOffset);
		arc->fileSystem->strSize =  toBEInt(arc->fileSystem->strSize);
		arc->fileSystem->strOffset =  toBEInt(arc->fileSystem->strOffset);

		arc->dirs = OffsetPointer(arc->fileSystem, arc->fileSystem->dirOffset);
		arc->files = OffsetPointer(arc->fileSystem, arc->fileSystem->fileOffset);
		arc->names = OffsetPointer(arc->fileSystem, arc->fileSystem->strOffset);
		arc->fileData = OffsetPointer(arc->header, (arc->header->fileSystemOffset + arc->header->fileSystemSize));

		for(int i = 0; i < arc->fileSystem->dirCount; i++){
			arc->dirs[i].id = toBEInt(arc->dirs[i].id);
			arc->dirs[i].nameOffset = toBEInt(arc->dirs[i].nameOffset);
			arc->dirs[i].hash = toBEShort(arc->dirs[i].hash);
			arc->dirs[i].count = toBEShort(arc->dirs[i].count);
			arc->dirs[i].first = toBEInt(arc->dirs[i].first);
		}

		for(int i = 0; i < arc->fileSystem->fileCount; i++){
			arc->files[i].id = toBEShort(arc->files[i].id);
			arc->files[i].hash = toBEShort(arc->files[i].hash);
			arc->files[i].nameOffset = toBEInt24(arc->files[i].nameOffset);
			arc->files[i].start = toBEInt(arc->files[i].start);
			arc->files[i].size = toBEInt(arc->files[i].size);
		}
	}
}

void rarcDumpFile(const rarcArchive* arc, char* path){
	/*
	const char* dirname = (arc->names + arc->dirs[dirIndex].nameOffset);
	for (int i = 0; i < arc->dirs[dirIndex].count; i++){
		const rarcFile * f = &arc->files[arc->dirs[dirIndex].first + i];
		const char * fname = (arc->names + f->nameOffset);

		if (f->attributes & 0x01 && f->id != 0xFFFF && strncmp(fname, path, strlen(fname)); == 0) {
			FILE * fhandle = fopen(fname, "wb");
			fwrite((arc->fileData + f->start), 1, f->size, fhandle);
			fclose(fhandle);
		}
	}*/
}

void rarcDumpDir(const rarcArchive* arc, char* name, int startIndex){
	const char * dirname = (arc->names + arc->dirs[startIndex].nameOffset);
	if(strncmp(name, dirname, strlen(dirname)) == 0){
		rarcDump(arc, startIndex);
	} else {
		for (int i = 0; i < arc->dirs[startIndex].count; i++){
			const rarcFile * f = &arc->files[arc->dirs[startIndex].first + i];
			const char * fname = (arc->names + f->nameOffset);
			
			if (f->attributes & 0x2) {
				if (strcmp(fname, ".") == 0 || strcmp(fname, "..") == 0) {
					continue;
				}
				rarcDumpDir(arc, name, f->start);
			}
		}
	}
}

char* rarcGetFile(const rarcArchive* arc, const char* name, int dirIndex){
	const char * dirname = (arc->names + arc->dirs[dirIndex].nameOffset);
	for (int i = 0; i < arc->dirs[dirIndex].count; i++){
		const rarcFile * f = &arc->files[arc->dirs[dirIndex].first + i];
		const char * fname = (arc->names + f->nameOffset);
		
		if (f->attributes & 0x2) {
			if (strcmp(fname, ".") == 0 || strcmp(fname, "..") == 0) {
				continue;
			}
			printf("File not found in %s, recursing to %s\n", dirname, (char*)(arc->names + arc->dirs[f->start].nameOffset));
			char* ret = rarcGetFile(arc, name, f->start);
			if(ret == NULL){
				continue;
			} 
			else {
				return ret;
			}
		}
		else if (f->attributes & 0x01 && f->id != 0xFFFF && strncmp(name, fname, strlen(fname)) == 0) {
			printf("File %s Found in %s\n", fname, dirname);
			return (arc->fileData + f->start);
		}
	}
	printf("File %s Not Found.\n", name);
	return NULL;	
}

void rarcDump(const rarcArchive* arc, int dirIndex){
	const char * dirname = (arc->names + arc->dirs[dirIndex].nameOffset);
#ifdef __linux__
	mkdir(dirname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#elif _WIN32
	mkdir(dirname, S_IRWXU | S_IROTH | S_IXOTH);
#else
#endif
	chdir(dirname);
	for (int i = 0; i < arc->dirs[dirIndex].count; i++){
		const rarcFile * f = &arc->files[arc->dirs[dirIndex].first + i];
		const char * fname = (arc->names + f->nameOffset);
		
		if (f->attributes & 0x2) {
			if (strcmp(fname, ".") == 0 || strcmp(fname, "..") == 0) {
				continue;
			}
			rarcDump(arc, f->start);
		} else if (f->attributes & 0x01 && f->id != 0xFFFF) {
			FILE * fhandle = fopen(fname, "wb");
			fwrite((arc->fileData + f->start), 1, f->size, fhandle);
			fclose(fhandle);
		}
	}
	chdir("..");
}