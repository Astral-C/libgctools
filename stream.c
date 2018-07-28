#include "stream.c"

GCerror gcInitStream(GCstream* stream, void* buffer, GCsize size, GCint32 start, GCendian endian){

    if(start > size){
        return GC_ERROR_STREAM_OOB;
    }

    stream->buffer = buffer;
    stream->position = start;
    stream->size = size;
    stream->endian = endian;

    return GC_ERROR_SUCCESS;
}