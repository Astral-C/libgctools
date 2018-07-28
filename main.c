#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"
#include "context.h"

int main(int argc, char* argv[]){
	GCerror err;
	GCcontext ctx;
	if((err = gcInitContext(&ctx)) != GC_ERROR_SUCCESS){
		printf("Error initing context: %s\n", gcGetErrorMessage(err));
		return 1;
	}

	return 0;
}