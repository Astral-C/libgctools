#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gcm.h"
#include "compression.h"
#include "rarc.h"

int main(int argc, char* argv[]){

	gcmHandle test;	
	gcmOpenROM(&test, argv[1]);
	size_t fstSize = gcmGetFstSize(&test);
	void* fst = malloc(fstSize);
	gcmLoadFst(&test, fst, fstSize);
	gcmDump(&test);
	gcmCloseStream(&test);
	free(fst);

/*
	FILE* yayArchive = fopen(argv[1], "rb");
	void* testBuffer = malloc(100); // only need 100 bytes to test this for now
	fread(testBuffer, 100, 1, yayArchive);
	fclose(yayArchive);
	printf("yay0 decoded size: %d", yayGetDecodedSize(testBuffer));
	free(testBuffer);
*/
	return 0;
}