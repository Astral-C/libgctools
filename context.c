#include <stdio.h>
#include <assert.h>
#include "context.h"
#include "types.h"


//Set up GCcontext with sane defaults
GCerror gcInitContext(GCcontext* ctx){
    if(ctx == NULL){
        return GC_ERROR_NULL_CONTEXT;
    }

    ctx->major = 0;
    ctx->minor = 0;
    ctx->revision = 1;
	
    //get system endiness and set context endian to it
    union {
		uint32_t integer;
		uint8_t bytes[sizeof(uint32_t)];
	} check;
	check.integer = 0x01020304U; 

    ctx->endian = (check.bytes[0] == 0x01 ? GC_ENDIAN_BIG : GC_ENDIAN_LITTLE);

    ctx->allocfn = NULL; //will use malloc by default
    ctx->freefn = NULL; //will use free by default
    
    return GC_ERROR_SUCCESS;
}

void* gcAllocMem(const GCcontext * ctx, GCsize sz){
    assert(ctx != NULL);

    GCallocfn fn = ctx->allocfn;
    if(fn == NULL){
        fn = malloc;
    }

    return fn(sz);
}

void gcFreeMem(const GCcontext * ctx, void * ptr){
    assert(ctx != NULL);

    GCfreefn fn = ctx->freefn;
    if(fn == NULL){
        fn = free;
    }

    return fn(ptr);
}