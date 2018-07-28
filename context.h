#ifndef LIBGC_CONTEXT_H
#define LIBGC_CONTEXT_H

// -------------------------------------------------------------------------- //

#include "types.h"
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// -------------------------------------------------------------------------- //

typedef void * (* GCallocfn)(GCsize sz);
typedef void (* GCfreefn)(void * ptr);

typedef struct {

	/* these are read-only */

	GCuint32 major; // major version number of the library
	GCuint32 minor; // minor version number of the library
	GCuint32 revision; // revision number of the library

	GCendian endian; // system endianness (deduced automatically)

	/* these are read/write */

	GCallocfn allocfn; // memory allocator; if NULL, malloc will be used
	GCfreefn freefn; // memory deallocator; if NULL, free will be used

} GCcontext;

// -------------------------------------------------------------------------- //

// this function sets all fields to default values.
// this makes all existing code forward-compatible
// in the event new fields are added in the future.
GCerror gcInitContext(GCcontext * ctx);

// utility memory function. mostly intended for internal
// use by the library but are safe to be called by user code.
void * gcAllocMem(const GCcontext * ctx, GCsize sz);
void gcFreeMem(const GCcontext * ctx, void * ptr);

// -------------------------------------------------------------------------- //

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LIBGC_CONTEXT_H
