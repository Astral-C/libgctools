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

	//TODO: Test Rarc Loading
	FILE* f = fopen("dummy.arc", "rb");
	fseek(f, 0L, SEEK_END);
	GCsize size = (GCsize)ftell(f);
	rewind(f);
	
	void* file = malloc(size);
	
	fread(file, 1, size, f);
	
	if(gcLoadArchive(&archive, file, size) != GC_ERROR_SUCCESS){
		printf("Rarc Load Func Success!\n");
	}

	fclose(f);

	return 0;
}