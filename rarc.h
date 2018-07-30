#pragma once
#include <stdint.h>
#define u32 uint32_t
#define u16 uint16_t
#define u8 uint8_t

typedef struct {
	u32 sig;
	u32 totalSize;
	u32 fileSystemOffset;
	u32 fileSystemSize;
} rarcHeader;

typedef struct {
	u32 dirCount;
	u32 dirOffset; //relative to header
	u32 fileCount;
	u32 fileOffset; //relative to header
	u32 strSize;
	u32 strOffset; //for string table?
} rarcFsHeader;

typedef struct {
	u32 id;
	u32 nameOffset; //relative to string offset
	u16 hash;
	u16 count; //num of files and directorys in dir
	u32 first;
} rarcDirectory;

typedef struct {
	u16 id;
	u16 hash;
	
	//u8 attributes;
	//u24 nameOffset;
	struct {
		u32 attributes : 8;
		u32 nameOffset : 24;
	} __attribute__((packed));

	u32 start;
	u32 size;
	u32 data;
} rarcFile;

typedef struct {
	rarcHeader* header;
	rarcFsHeader* fileSystem;
	rarcDirectory* dirs;
	rarcFile* files;
	char* names;
	char* fileData;
} rarcArchive;

void rarcLoadArchive(rarcArchive* arc, void* data);
void rarcDumpDir(const rarcArchive* arc, char* name, int startIndex);
char* rarcGetFile(const rarcArchive* arc, const char* name, int dirIndex);
void rarcDump(const rarcArchive *arc, int dirIndex);
//size_t rarcGetSize(void* file);