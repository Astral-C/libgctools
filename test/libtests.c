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

	FILE* f = fopen("test/luige.arc", "rb");
	fseek(f, 0L, SEEK_END);
	GCsize size = (GCsize)ftell(f);
	rewind(f);
	
	printf("Allocating mem for file...");
	void* file = malloc(size);
	fread(file, 1, size, f);

	printf("Initing archive...");
	gcInitArchive(&archive, &ctx);
	if((err = gcLoadArchive(&archive, file, size)) != GC_ERROR_SUCCESS){
		printf("Error Loading Archive: %s\n", gcGetErrorMessage(err));
	}


	gcFreeArchive(&archive);
	fclose(f);
	free(file);
	return 0;
}