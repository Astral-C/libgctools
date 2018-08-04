#include <stdio.h>
#include <string.h>
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

    arc->filenum = fileCount;
    arc->files = gcAllocMem(arc->ctx, (sizeof(GCarcfile)*fileCount));

    arc->stringTable = gcAllocMem(arc->ctx, strSize);
    memcpy(arc->stringTable, OffsetPointer(stream.buffer, strOffset+fsOffset), strSize);

    gcStreamSeek(&stream, dirOffset, fsOffset);
    
    for(size_t i = 0; i < arc->dirnum; i++)
    {
        arc->dirs[i].ctx = arc->ctx;
        arc->dirs[i].arc = arc;

        gcStreamReadU32(&stream);
        GCuint32 nameOff = gcStreamReadU32(&stream);
        gcStreamReadU16(&stream);

        arc->dirs[i].filenum = gcStreamReadU16(&stream);
        arc->dirs[i].fileoff = gcStreamReadU32(&stream);
        arc->dirs[i].name = (char*)OffsetPointer(arc->stringTable, nameOff);
        printf("Dir %d\nName:%s\nFile Count: %d\nFirst File Index: %d\n", i, arc->dirs[i].name, arc->dirs[i].filenum, arc->dirs[i].fileoff);


        //get files
        size_t pos = stream.position;
        gcStreamSeek(&stream, fileOffset+(arc->dirs[i].fileoff*20), fsOffset);
        printf("Reading %d Files for Directory\n", arc->dirs[i].filenum);
        for(size_t i = 0; i < arc->dirs[i].filenum; i++)
        {
            arc->files[arc->dirs[i].fileoff+i].ctx = arc->ctx;
            arc->files[arc->dirs[i].fileoff+i].arc = arc;

            gcStreamReadU32(&stream);
            GCuint32 fileAttrs = gcStreamReadU32(&stream);
            GCuint32 nameOff = fileAttrs & 0x00FFFFFF;
            GCuint32 attrs = fileAttrs & 0xFF;
            gcStreamReadU32(&stream);
            gcStreamReadU32(&stream);
            gcStreamReadU32(&stream);

            arc->files[arc->dirs[i].fileoff+i].name = (char*)OffsetPointer(arc->stringTable, nameOff);
            printf("Read File %s from %d\n", arc->files[arc->dirs[i].fileoff+i].name, nameOff);
        }        

        gcStreamSeek(&stream, pos, 0);
        printf("===\n");
    }
    

    for(size_t i = 0; i < arc->filenum; i++)
    {
        arc->files[i].ctx = arc->ctx;
        arc->files[i].arc = arc;

    }
    

    return GC_ERROR_SUCCESS;
}

GCerror gcFreeArchive(GCarchive * arc){

    gcFreeMem(arc->ctx, arc->stringTable);
    gcFreeMem(arc->ctx, arc->dirs);
}