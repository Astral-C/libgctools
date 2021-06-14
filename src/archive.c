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

    if((err = gcStreamSeek(&stream, fsOffset, 0)) != GC_ERROR_SUCCESS){
        printf("Seek Error: %s\n", gcGetErrorMessage(err));
        return GC_ERROR_STREAM_OOB;
    }

    GCuint32 dirCount = gcStreamReadU32(&stream);
    GCuint32 dirOffset = gcStreamReadU32(&stream);

    GCuint32 fileCount = gcStreamReadU32(&stream);
    GCuint32 fileOffset = gcStreamReadU32(&stream);

    GCuint32 strSize = gcStreamReadU32(&stream);
    GCuint32 strOffset = gcStreamReadU32(&stream);

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

        size_t pos = stream.position;
        gcStreamSeek(&stream, fileOffset+(arc->dirs[i].fileoff*20), fsOffset);
        for(size_t f = 0; f < arc->dirs[i].filenum; f++)
        {
            GCuint32 fileIndex = (arc->dirs[i].fileoff + f);
            arc->files[fileIndex].ctx = arc->ctx;
            arc->files[fileIndex].arc = arc;

            GCuint16 id = gcStreamReadU16(&stream);
	        gcStreamReadU16(&stream);
            GCuint32 fileAttrs = gcStreamReadU32(&stream);
            GCuint32 nameOff = fileAttrs & 0x00FFFFFF;
            arc->files[fileIndex].attr = (fileAttrs >> 24) & 0xFF;
            GCuint32 start = gcStreamReadU32(&stream);
            arc->files[fileIndex].size = gcStreamReadU32(&stream);
            arc->files[fileIndex].name = (char*)OffsetPointer(arc->stringTable, nameOff);
            gcStreamReadU32(&stream);
            
            if(arc->files[fileIndex].attr & 0x01){
                arc->files[fileIndex].data = gcAllocMem(arc->ctx, arc->files[fileIndex].size);
                memcpy(arc->files[fileIndex].data, (GCuint8*)OffsetPointer(stream.buffer, fsOffset + fsSize + start), arc->files[fileIndex].size);
            } else if(arc->files[fileIndex].attr & 0x02) {
                arc->files[fileIndex].data = NULL; //No data because were a subdir
            }

        }        

        gcStreamSeek(&stream, pos, 0);
    }
    

    return GC_ERROR_SUCCESS;
}

GCerror gcFreeArchive(GCarchive * arc){
    for (size_t f = 0; f < arc->filenum; f++){
        if(arc->files[f].data != NULL){
            gcFreeMem(arc->ctx, arc->files[f].data);
        }
    }
    
    gcFreeMem(arc->ctx, arc->stringTable);
    gcFreeMem(arc->ctx, arc->files);
    gcFreeMem(arc->ctx, arc->dirs);
}
