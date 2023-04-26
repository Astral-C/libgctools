#ifndef LIBGC_TYPES_H
#define LIBGC_TYPES_H

// -------------------------------------------------------------------------- //

#include <stddef.h>
#include <stdint.h>
#define GCOffsetPointer(ptr, offset) ((void*)((char *)(ptr) + (offset)))

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// -------------------------------------------------------------------------- //

typedef int GCbool;

typedef int8_t GCint8;
typedef int16_t GCint16;
typedef int32_t GCint32;

typedef uint8_t GCuint8;
typedef uint16_t GCuint16;
typedef uint32_t GCuint32;

typedef float GCfloat;
typedef double GCdouble;

typedef size_t GCsize;

typedef enum {

	GC_ENDIAN_LITTLE,
	GC_ENDIAN_BIG,

} GCendian;

typedef enum {

	GC_ERROR_SUCCESS,
	GC_ERROR_NULL_CONTEXT,
	GC_ERROR_STREAM_OOB,
	// put error codes here with the prefix 'GC_ERROR_'

} GCerror;

// -------------------------------------------------------------------------- //

// gets a nice error message string for a given error code.
// this string is statically allocated and should never be freed.
// NULL will be returned if the error is unknown
const char * gcGetErrorMessage(GCerror err);

// -------------------------------------------------------------------------- //

//For internal library use only
GCuint32 gcSwap32(GCuint32 n);
GCuint16 gcSwap16(GCuint16 n);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LIBGC_TYPES_H
