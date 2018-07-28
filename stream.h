#ifndef LIBGC_STREAM_H
#define LIBGC_STREAM_H

#include "types.h"

typedef struct stream stream 

struct {
    void* buffer;
    GCsize position;
    GCsize size;
    GCendian endian;
} GCstream;

GCerror gcStreamInit(GCstream* stream, void* buffer, GCsize size, GCint32 start=0);

GCint8 gcStreamReadU8(GCstream* stream);
GCuint8 gcStreamRead8(GCstream* stream);

GCint16 gcStreamRead16(GCstream* stream);
GCuint16 gcStreamReadU16(GCstream* stream);

GCint32 gcStreamReadU32(GCstream* stream);
GCuint32 gcStreamReadU32(GCstream* stream);

GCfloat gcStreamReadFloat(GCstream* stream);
GCdouble gcStreamReadDouble(GCstream* stream);

#endif