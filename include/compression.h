#ifndef LIBGC_COMPRESSION_H
#define LIBGC_COMPRESSION_H

// -------------------------------------------------------------------------- //

#include "context.h"
#include "stream.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// -------------------------------------------------------------------------- //

GCuint32 gcDecompressedSize(GCcontext* ctx, GCuint8* src_data, GCuint32 offset);
void gcYaz0Decompress(GCcontext* ctx, GCuint8* src_data, GCuint8* dst_data, GCsize length, GCuint32 offset);
void gcYay0Decompress(GCcontext* ctx, GCuint8* src_data, GCuint8* dst_data, GCsize length, GCuint32 offset);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //LIBGC_COMPRESSION_H