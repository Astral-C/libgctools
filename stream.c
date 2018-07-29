#include <stdio.h>
#include "stream.h"

GCerror gcInitStream(const GCcontext* ctx, GCstream* stream, const void* buffer, GCsize size, GCendian endian){

    stream->buffer = buffer;
    stream->position = 0;
    stream->size = size;
    stream->endian = endian;
    stream->systemOrder = ctx->endian;

    return GC_ERROR_SUCCESS;
}

GCerror gcStreamSeek(GCstream* stream, GCuint32 pos, GCuint32 from){
    if(pos > stream->size || from > stream->size || from < 0 || pos < 0){
        return GC_ERROR_STREAM_OOB;
    }
    stream->position = from + pos;
}

GCuint8 gcStreamRead8(GCstream* stream){
    int8_t r = *((int8_t*)OffsetPointer(stream->buffer, stream->position));
    stream->position++;
    return r;
}

GCint8 gcStreamReadU8(GCstream* stream){
    uint8_t r = *((uint8_t*)OffsetPointer(stream->buffer, stream->position));
    stream->position++;
    return r;
}

GCint16 gcStreamRead16(GCstream* stream){
    int16_t r = *((int16_t*)OffsetPointer(stream->buffer, stream->position));
    stream->position += 2;
    return (stream->endian != stream->systemOrder ? gcSwap16(r) : r);
}

GCuint16 gcStreamReadU16(GCstream* stream){
    int16_t r = *((int16_t*)OffsetPointer(stream->buffer, stream->position));
    stream->position += 2;
    return (stream->endian != stream->systemOrder ? gcSwap16(r) : r);
}

GCint32 gcStreamRead32(GCstream* stream){
    int32_t r = *((int32_t*)OffsetPointer(stream->buffer, stream->position));
    stream->position += 4;
    return (stream->endian != stream->systemOrder ? gcSwap32(r) : r);
}

GCuint32 gcStreamReadU32(GCstream* stream){
    uint32_t r = *((uint32_t*)OffsetPointer(stream->buffer, stream->position));
    stream->position += 4;
    return (stream->endian != stream->systemOrder ? gcSwap32(r) : r);
}

GCfloat gcStreamReadFloat(GCstream* stream){
    //TODO
}

GCdouble gcStreamReadDouble(GCstream* stream){
    //TODO
}