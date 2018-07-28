#include "context.h"
#include "types.h"


//Set up GCcontext with sane defaults
GCerror gcInitContext(GCcontext* ctx){
    if(ctx == NULL){
        return GC_ERROR_NULL_CONTEXT;
    }

    ctx->major = 0;
    ctx->minor = 1;
    ctx->revision = 0;
	
    //get system endiness and set context endian to it
    union {
		uint32_t integer;
		uint8_t bytes[sizeof(uint32_t)];
	} check;
	check.integer = 0x01020304U; 

    ctx->endian = (check.bytes[0] == 0x01 ? GC_ENDIAN_BIG : GC_ENDIAN_LITTLE);
    ctx->allocfn = malloc; //set default allocator to malloc
    ctx->freefn = free; //set default deallocated to free
    return GC_ERROR_SUCCESS;
}

void* gcAllocMem(const GCcontext * ctx, GCsize sz){
    return ctx->allocfn(sz);
}

void gcFreeMem(const GCcontext * ctx, void * ptr){
    return ctx->freefn(ptr);
}