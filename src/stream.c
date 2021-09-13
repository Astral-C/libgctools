#include <stdio.h>
#include <string.h>
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
    return GC_ERROR_SUCCESS;
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
    int16_t r;
    memcpy(&r, OffsetPointer(stream->buffer, stream->position), sizeof(int16_t));
    stream->position += 2;
    return (stream->endian != stream->systemOrder ? gcSwap16(r) : r);
}

GCuint16 gcStreamReadU16(GCstream* stream){
    uint16_t r;
    memcpy(&r, OffsetPointer(stream->buffer, stream->position), sizeof(uint16_t));
    stream->position += 2;
    return (stream->endian != stream->systemOrder ? gcSwap16(r) : r);
}

GCint32 gcStreamRead32(GCstream* stream){
    int32_t r;
    memcpy(&r, OffsetPointer(stream->buffer, stream->position), sizeof(int32_t));
    stream->position += 4;
    return (stream->endian != stream->systemOrder ? gcSwap32(r) : r);
}

GCuint32 gcStreamReadU32(GCstream* stream){
    uint32_t r;
    memcpy(&r, OffsetPointer(stream->buffer, stream->position), sizeof(uint32_t));
    stream->position += 4;
    return (stream->endian != stream->systemOrder ? gcSwap32(r) : r);
}

char* gcStreamReadStr(GCstream* stream, size_t size){
    char* str = (char*)malloc(size);
    strncpy(str, (char*)OffsetPointer(stream->buffer, stream->position), size);
    stream->position += size;
    return str;
}

// Writer Functions

void gcStreamWrite8(GCstream* stream, int8_t r){
    *((int8_t*)OffsetPointer(stream->buffer, stream->position)) = r;
    stream->position++;
}

void gcStreamWriteU8(GCstream* stream, uint8_t r){
    *((uint8_t*)OffsetPointer(stream->buffer, stream->position)) = r;
    stream->position++;
}

void gcStreamWrite16(GCstream* stream, int16_t r){
    *((int16_t*)OffsetPointer(stream->buffer, stream->position)) = (stream->endian == GC_ENDIAN_LITTLE ? r : gcSwap16(r));
    stream->position += sizeof(int16_t);
}

void gcStreamWriteU16(GCstream* stream, uint16_t r){
    *((uint16_t*)OffsetPointer(stream->buffer, stream->position)) = (stream->endian == GC_ENDIAN_LITTLE ? r : gcSwap16(r));
    stream->position += sizeof(uint16_t);
}

void gcStreamWrite32(GCstream* stream, int32_t r){
    *((int32_t*)OffsetPointer(stream->buffer, stream->position)) = (stream->endian == GC_ENDIAN_LITTLE ? r : gcSwap32(r));
    stream->position += sizeof(int32_t);
}

void gcStreamWriteU32(GCstream* stream, uint32_t r){
    *((uint32_t*)OffsetPointer(stream->buffer, stream->position)) = (stream->endian == GC_ENDIAN_LITTLE ? r : gcSwap32(r));
    stream->position += sizeof(uint32_t);
}

void gcStreamWriteStr(GCstream* stream, const char* str, size_t size){
    strncpy((char*)OffsetPointer(stream->buffer, stream->position), str, size);
    stream->position += size;
}