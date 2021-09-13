#ifndef LIBGC_STREAM_H
#define LIBGC_STREAM_H

#include "types.h"
#include "context.h"

#ifdef __cplusplus
extern "C" {
#endif 

//This stream structure is meant for internal use only, using it may break your application in the future.

typedef struct {
    const uint8_t* buffer;
    GCsize position;
    GCsize size;
    GCendian endian;
    GCendian systemOrder;
} GCstream;

GCerror gcInitStream(const GCcontext* ctx, GCstream* stream, const void* buffer, GCsize size, GCendian endian);
GCerror gcStreamSeek(GCstream* stream, GCuint32 pos, GCuint32 from);


GCint8 gcStreamReadU8(GCstream* stream);
GCuint8 gcStreamRead8(GCstream* stream);

GCint16 gcStreamRead16(GCstream* stream);
GCuint16 gcStreamReadU16(GCstream* stream);

GCint32 gcStreamRead32(GCstream* stream);
GCuint32 gcStreamReadU32(GCstream* stream);

GCfloat gcStreamReadFloat(GCstream* stream);
GCdouble gcStreamReadDouble(GCstream* stream);
char* gcStreamReadStr(GCstream* stream, size_t size);


void gcStreamWrite8(GCstream* stream, int8_t r);
void gcStreamWriteU8(GCstream* stream, uint8_t r);

void gcStreamWrite16(GCstream* stream, int16_t r);
void gcStreamWriteU16(GCstream* stream, uint16_t r);

void gcStreamWrite32(GCstream* stream, int32_t r);
void gcStreamWriteU32(GCstream* stream, uint32_t r);

void gcStreamWriteStr(GCstream* stream, const char* str, size_t size);

#ifdef __cplusplus
}
#endif

#endif