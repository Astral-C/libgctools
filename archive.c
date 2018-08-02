#include <stdio.h>
#include <assert.h>
#include "types.h"
#include "stream.h"
#include "archive.h"

GCerror gcInitArchive(GCarchive * arc, const GCcontext * ctx){
    arc->ctx = ctx;
    arc->dirnum = 0;
    arc->filenum = 0;
    arc->dirs = NULL;
    arc->files = NULL;
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

    arc->dirnum = dirCount;
    arc->dirs = gcAllocMem(arc->ctx, (sizeof(GCarcdir)*dirCount));
    
    gcStreamSeek(&stream, dirOffset, fsOffset);
    for(size_t i = 0; i < arc->dirnum; i++)
    {
        arc->dirs[i].ctx = arc->ctx;
        arc->dirs[i].arc = arc;
        gcStreamRead32(&stream);
        GCuint32 nameOff = gcStreamRead32(&stream); 
        size_t curPos = stream.position;
        gcStreamSeek(&stream, nameOff+strOffset, fsOffset);
        arc->dirs[i].name = gcStreamReadStr(&stream, strSize);
        printf("Got Dir %d Name At 0x%x: %s\n", i, stream.position, arc->dirs[i].name);
        gcStreamSeek(&stream, curPos, 0);
        gcStreamRead32(&stream);
        gcStreamRead32(&stream);
    }
    

    return GC_ERROR_SUCCESS;
}

GCerror gcFreeArchive(GCarchive * arc){

    for(size_t i = 0; i < arc->dirnum; i++)
    {
        printf("Freeing Dir Name %s\n", arc->dirs[i].name);
        free(arc->dirs[i].name);
    }
    printf("Freeing Dir Structures\n");
    free(arc->dirs);
}