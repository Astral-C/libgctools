#include <stdio.h>
#include "types.h"
#include "stream.h"
#include "archive.h"

GCerror gcInitArchive(GCarchive * arc, const GCcontext * ctx){
    arc->ctx = ctx;
}

GCerror gcLoadArchive(GCarchive * arc, const void * ptr, GCsize sz){
    GCstream stream;

    gcInitStream(arc->ctx, &stream, ptr, sz, GC_ENDIAN_BIG);
    gcStreamSeek(&stream, 8, 0);
    GCuint32 fsOffset = gcStreamReadU32(&stream);
    GCuint32 fsSize = gcStreamReadU32(&stream);

    printf("FS Offset: %x : FS Size %x\n", fsOffset, fsSize);
    return GC_ERROR_SUCCESS;
}