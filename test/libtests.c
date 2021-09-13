#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"
#include "context.h"
#include "archive.h"

int main(int argc, char* argv[]){
	GCerror err;
	GCcontext ctx;
	GCarchive archive;

	if((err = gcInitContext(&ctx)) != GC_ERROR_SUCCESS){
		printf("Error initing context: %s\n", gcGetErrorMessage(err));
		return 1;
	}

	printf("Made using libgctools v%d.%d.%d\n", ctx.major, ctx.minor, ctx.revision);

	FILE* f = fopen("room_02.arc", "rb");
	FILE* fout = fopen("room_02_new.arc", "wb");
	fseek(f, 0L, SEEK_END);
	GCsize size = (GCsize)ftell(f);
	rewind(f);
	
	printf("Allocating %u bytes for file...\n", size);
	void* file = malloc(size);
	fread(file, 1, size, f);

	printf("Initing archive...\n");
	gcInitArchive(&archive, &ctx);
	if((err = gcLoadArchive(&archive, file, size)) != GC_ERROR_SUCCESS){
		printf("Error Loading Archive: %s\n", gcGetErrorMessage(err));
	}

	GCsize outSize = gcSaveArchive(&archive, NULL);
	printf("Archive size on save would be %u bytes\n", outSize);
	GCuint8* archiveOut = malloc(outSize);

	gcSaveArchive(&archive, archiveOut);

	fwrite(archiveOut, outSize, 1, fout);

	gcFreeArchive(&archive);
	
	fclose(f);
	fclose(fout);

	free(archiveOut);
	free(file);
	
	return 0;
}
