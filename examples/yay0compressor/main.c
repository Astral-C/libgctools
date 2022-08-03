#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "types.h"
#include "context.h"
#include "compression.h"

int main(int argc, char* argv[]){
	GCerror err;
	GCcontext ctx;

/*
    char* inFile = NULL;
    char* outFile = NULL;

    int arg;
    while ((arg = getopt(argc, argv, ":i:o:")) != -1){
        switch (arg){
        case 'i':
            inFile = optarg;
        case 'o':
            outFile = optarg;
        }
    }
    
    if (inFile == NULL){
        printf("Please provide a file to compress.\n");
        return 1;
    }
    
    if(outFile == NULL){
        outFile = "out.szp";
    }

	if((err = gcInitContext(&ctx)) != GC_ERROR_SUCCESS){
		printf("Error initing context: %s\n", gcGetErrorMessage(err));
		return 1;
	}

    if(access(inFile, R_OK)){
        printf("Unable to open file %s\n", inFile);
        return 1;
    }

    FILE* f = fopen(inFile, "rb");

	fseek(f, 0L, SEEK_END);
	GCsize size = (GCsize)ftell(f);
	rewind(f);
	
	GCuint8* file = malloc(size);
	fread(file, 1, size, f);

    GCuint8* compressedFile = malloc(size);

    printf("Compressing \"%s\"...\n", inFile);
    GCsize cmpSize = gcYay0Compress(&ctx, file, compressedFile, size);

    FILE* out = fopen(outFile, "wb");
    fwrite(compressedFile, 1, cmpSize, out);
    printf("Finished.\n");

    free(file);
    free(compressedFile);
    
    fclose(f);
    fclose(out);
 */   
    return 0;

}