#include <stdio.h>
#include <assert.h>
#include "types.h"
#include "stream.h"
#include "archive.h"

GCerror gcInitArchive(GCarchive * arc, const GCcontext * ctx){
    arc->ctx = ctx;
}

GCerror gcLoadArchive(GCarchive * arc, const void * ptr, GCsize sz){
    GCstream stream;
    GCerror err;

    gcInitStream(arc->ctx, &stream, ptr, sz, GC_ENDIAN_BIG);
    
    if((err = gcStreamSeek(&stream, 8, 0)) != GC_ERROR_SUCCESS){
        printf("Seek Error: %s\n", gcGetErrorMessage(err));
        return GC_ERROR_STREAM_OOB;
    }

    GCuint32 fsOffset = gcStreamReadU32(&stream);
    GCuint32 fsSize = gcStreamReadU32(&stream);

    printf("FS Offset: %x : FS Size %x\n", fsOffset, fsSize);

    if((err = gcStreamSeek(&stream, fsOffset, 0)) != GC_ERROR_SUCCESS){
        printf("Seek Error: %s\n", gcGetErrorMessage(err));
        return GC_ERROR_STREAM_OOB;
    }

    GCuint32 dirCount = gcStreamReadU32(&stream);
    GCuint32 dirOffset = gcStreamReadU32(&stream);

    printf("Dir Count: %d, Dir Offset: %d\n", dirCount, dirOffset);

    GCuint32 fileCount = gcStreamReadU32(&stream);
    GCuint32 fileOffset = gcStreamReadU32(&stream);

    GCuint32 strSize = gcStreamReadU32(&stream);
    GCuint32 strOffset = gcStreamReadU32(&stream);

    printf("Allocating Memory for Directory Structures\n");

    arc->dirs = gcAllocMem(arc->ctx, (sizeof(GCarcdir)*dirCount));

    return GC_ERROR_SUCCESS;
}