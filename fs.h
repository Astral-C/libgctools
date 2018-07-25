#include <stdio.h>

/*
*
*	This file details the virtual file system used to read from gcms
*
*/

typedef struct {
	unsigned short id;
	size_t fileSize;
	int offset;

	vfsFile* first;
	vfsFile* next;
	vfsFile* prev;
	vfsFile* last;

} vfsFile;

struct vfsDir {
	unsigned short id;
	short fileCount;
	vfsFile* files;
};

struct vfsFileSystem {
	vfsDir* root;
};