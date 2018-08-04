//#include "endian.h"
#include "rarc.h"

#define OffsetPointer(ptr, offset) ((void*)((char *)(ptr) + (offset)))

//size_t rarcGetSize(void* file){
//	size_t size = ((rarcHeader*)file)->totalSize;
//	return toBEInt(size);
//}
void rarcLoadArchive(rarcArchive* arc, void* data){
	long fSize = 1*sizeof(rarcFile);
	arc->header = data;

	uint32_t fsOffset = swap32(arc->header->fileSystemOffset);
	uint32_t fsSize = swap32(arc->header->fileSystemSize);

	arc->fileSystem = OffsetPointer(arc->header, fsOffset);
	arc->dirs = OffsetPointer(arc->fileSystem, swap32(arc->fileSystem->dirOffset));
	arc->files = OffsetPointer(arc->fileSystem, swap32(arc->fileSystem->fileOffset));
	arc->names = OffsetPointer(arc->fileSystem, swap32(arc->fileSystem->strOffset));
	arc->fileData = OffsetPointer(arc->header, (fsOffset + fsSize));
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
			//if(ret == NULL){
			//	continue;
			//} 
			//else {
			//	return ret;
			//}
		}
		else if (f->attributes & 0x01 && strncmp(name, fname, strlen(fname)) == 0) {
			printf("File %s Found in %s\n", fname, dirname);
			return (arc->fileData + f->start);
		}
	}
	printf("File %s Not Found.\n", name);
	//return NULL;	
}

void rarcDump(const rarcArchive* arc, int dirIndex){
	const char * dirname = (arc->names + arc->dirs[dirIndex].nameOffset);
#ifdef __linux__
//	mkdir(dirname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
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
		//	FILE * fhandle = fopen(fname, "wb");
		//	fwrite((arc->fileData + f->start), 1, f->size, fhandle);
		//	fclose(fhandle);
		}
	}
	chdir("..");
}
