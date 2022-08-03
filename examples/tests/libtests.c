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

	//FILE* fout = fopen("map2_new.szp", "wb");
	FILE* f = fopen("map2.arc", "rb");
	FILE* arcOut = fopen("map2_new.arc", "wb");
	fseek(f, 0L, SEEK_END);
	GCsize size = (GCsize)ftell(f);
	rewind(f);
	
	printf("Allocating %u bytes for file...\n", size);
	void* file = malloc(size);
	fread(file, 1, size, f);

	//GCuint32 decompressedSize = gcDecompressedSize(&ctx, file, 0);
	//GCuint8* decompressedFile = malloc(decompressedSize);
	//gcYay0Decompress(&ctx, file, decompressedFile, size, 0);

	//FILE* decompdOut = fopen("map2_clean.arc", "wb");
	//fwrite(decompressedFile, decompressedSize, 1, decompdOut);
	//fclose(decompdOut);

	printf("Initing archive...\n");
	gcInitArchive(&archive, &ctx);
	if((err = gcLoadArchive(&archive, file, size)) != GC_ERROR_SUCCESS){
		printf("Error Loading Archive: %s\n", gcGetErrorMessage(err));
	}

	GCsize outSize = gcSaveArchive(&archive, NULL);
	printf("Archive size on save would be %u bytes\n", outSize);
	GCuint8* archiveOut = malloc(outSize);
	//GCuint8* archiveCmp = malloc(outSize);

	gcSaveArchive(&archive, archiveOut);

	//GCsize cmpSize = gcYay0Compress(&ctx, archiveOut, archiveCmp, outSize);

	//fwrite(archiveCmp, cmpSize, 1, fout);
	fwrite(archiveOut, outSize, 1, arcOut);

	gcFreeArchive(&archive);
	
	fclose(f);
	//fclose(fout);
	fclose(arcOut);

	free(archiveOut);
	free(file);
	//free(decompressedFile);
	
	return 0;
}
