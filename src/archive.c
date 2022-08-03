#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../include/types.h"
#include "../include/stream.h"
#include "../include/archive.h"

#define padTo32(x) (x + (32-1)) & ~(32-1)

GCuint16 gcHashName(const char* str){
    GCuint16 hash = 0;

    GCuint16 multiplyer = 1;
    if(strlen(str) + 1 == 2){
        multiplyer = 2;
    } else if(strlen(str) + 1 >= 3){
        multiplyer = 3;
    }

    for (size_t i = 0; i < strlen(str); i++){
        hash = (GCuint16)(hash * multiplyer);
        hash += (GCuint16)str[i];
    }

    return hash;
}

GCuint32 gcStringTableAddr(GCuint8* stringTableChunk, GCuint32* curStrCount, GCuint32* curStrTblOffset, const char* str){
    GCuint32 nameOffset = 0;
    
    GCuint32 offset = 0;
    for(int s = 0; s < *curStrCount; s++){
        if(strcmp(OffsetPointer(stringTableChunk, offset), str) == 0){
            nameOffset = offset;
            break;
        }
        offset += strlen(OffsetPointer(stringTableChunk, offset))+1;
    }

    if(nameOffset == 0){
        nameOffset = *curStrTblOffset;
        strncpy(OffsetPointer(stringTableChunk, nameOffset), str, strlen(str)+1);
        *curStrTblOffset += strlen(str)+1;
        *curStrCount += 1;
    }

    return nameOffset;
}

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
            
            arc->files[fileIndex].parent = &arc->dirs[i];
            if(arc->files[fileIndex].attr & 0x01){
                arc->files[fileIndex].data = gcAllocMem(arc->ctx, arc->files[fileIndex].size);
                memcpy(arc->files[fileIndex].data, (GCuint8*)OffsetPointer(stream.buffer, fsOffset + fsSize + start), arc->files[fileIndex].size);
            } else if(arc->files[fileIndex].attr & 0x02) {
                if(start != -1 && strcmp(arc->files[fileIndex].name, "..") != 0 && strcmp(arc->files[fileIndex].name, ".") != 0){
                    arc->dirs[start].parent = &arc->dirs[i];
                }
                arc->files[fileIndex].data = NULL; //No data because were a subdir

            }

        }        

        gcStreamSeek(&stream, pos, 0);
    }
    

    return GC_ERROR_SUCCESS;
}

GCsize gcSaveArchive(GCarchive * arc, const GCuint8* ptr){
    GCstream dirStream;
    GCstream fileStream;
    GCstream headerStream;
    GCstream fileSysStream;

    GCerror err;

    GCsize archiveSize = 0x40 + (arc->dirnum * 0x10) + (arc->filenum * 0x14);
    
    GCsize stringTableSize = 0;
    GCsize fileDataSize = 0;
    GCsize stringTableCount = 0;

    for (GCsize d = 0; d < arc->dirnum; d++){
        GCarcdir dir = arc->dirs[d];
        stringTableSize += strlen(dir.name);
        stringTableCount++;
        for (GCsize f = dir.fileoff; f < dir.fileoff + dir.filenum; f++){
            GCarcfile file = arc->files[f]; 
            if(file.attr & 0x01){
                stringTableSize += strlen(file.name) + 1;
                fileDataSize += padTo32(file.size);
                stringTableCount++;
            }
        }
        
    }
    
    stringTableSize += stringTableCount; //We need to add 1 null terminator per string, so add the number of strings
    stringTableSize = padTo32(stringTableSize);

    if(ptr == NULL) return padTo32(archiveSize + stringTableSize + fileDataSize);
    
    //Get pointers to each chunk of the file so we can generate offsets and indices as we go
    GCuint8* dirChunk = OffsetPointer(ptr, 0x40);
    GCuint8* fileChunk = OffsetPointer(ptr, 0x40 + (arc->dirnum * 0x10));
    GCuint8* fileDataChunk = OffsetPointer(ptr, padTo32(archiveSize + stringTableSize));
    //Even though we won't be doing anything with this until we've generated the string table, still generate the pointer to it now
    GCuint8* stringTableChunk = OffsetPointer(ptr, archiveSize);

    //Ensure that this buffer is empty before we write to it
    memset((void*)ptr, 0, archiveSize + stringTableSize + fileDataSize);

    //Initialize the write streams for the different file segments
    gcInitStream(arc->ctx, &headerStream, ptr, 0x20, GC_ENDIAN_BIG);
    gcInitStream(arc->ctx, &fileSysStream, OffsetPointer(ptr, 0x20), 0x20, GC_ENDIAN_BIG);
    gcInitStream(arc->ctx, &dirStream, dirChunk, (arc->dirnum * 0x10), GC_ENDIAN_BIG);
    gcInitStream(arc->ctx, &fileStream, fileChunk, (arc->filenum * 0x14), GC_ENDIAN_BIG);

    
    //Where is the current end of all file data
    GCuint32 curFileOffset = 0;

    //Offset into string table of current end
    GCuint32 curStrTblOffset = 0;

    //How many strings have been written thus far
    GCuint32 curStrCount = 0;

    //Used to keep track of the file indices/ids
    GCuint16 curFileIndex = 0;

    for (GCsize d = 0; d < arc->dirnum; d++){
        GCarcdir dir = arc->dirs[d];

        GCuint32 nameOffset = gcStringTableAddr(stringTableChunk, &curStrCount, &curStrTblOffset, dir.name);

        if(d == 0){
            gcStreamWriteStr(&dirStream, "ROOT", 4);
        } else {
            gcStreamWriteStr(&dirStream, dir.name, 4);
        }
        gcStreamWriteU32(&dirStream, nameOffset);
        gcStreamWriteU16(&dirStream, gcHashName(dir.name));
        gcStreamWriteU16(&dirStream, dir.filenum);
        gcStreamWriteU32(&dirStream, curFileIndex);

        for (size_t f = dir.fileoff; f < dir.fileoff + dir.filenum; f++){
            GCarcfile file = arc->files[f]; 

            GCuint32 nameOffset = gcStringTableAddr(stringTableChunk, &curStrCount, &curStrTblOffset, file.name);;

            //Subdirs use 0xFFFF instead of a proper index
            if(file.attr & 0x01){
                gcStreamWriteU16(&fileStream, curFileIndex);
            } else if (file.attr & 0x02){
                gcStreamWriteU16(&fileStream, 0xFFFF);
            }
            curFileIndex++;
            
            gcStreamWriteU16(&fileStream, gcHashName(file.name));
            gcStreamWriteU8(&fileStream, file.attr);
            gcStreamWriteU8(&fileStream, 0); //bad
            gcStreamWriteU16(&fileStream, nameOffset);
            

            if(file.attr & 0x01){
                gcStreamWriteU32(&fileStream, curFileOffset);
                gcStreamWriteU32(&fileStream, file.size);
                memcpy(OffsetPointer(fileDataChunk, curFileOffset), file.data, file.size);

                curFileOffset += padTo32(file.size);
            } else if(file.attr & 0x02){

                //Subdir file entries replace the offset with the index of the dir it points to
                GCuint32 dirIndex = 0;
                
                if(strcmp(file.name, "..") == 0){
                    dirIndex = dir.parent - arc->dirs; //get index
                } else if(strcmp(file.name, ".") == 0){
                    dirIndex = d;
                } else {
                    for (GCsize td = 0; td < arc->dirnum; td++){
                        if(file.parent == arc->dirs[td].parent && strcmp(arc->dirs[td].name, file.name) == 0){
                            dirIndex = (GCuint32)td + 1;
                            break;
                        }
                    }
                }

                gcStreamWriteU32(&fileStream, dirIndex);
                gcStreamWriteU32(&fileStream, 0x10); //Size is written as size of dir node
            }

            // Padding
            gcStreamWriteU32(&fileStream, 0);
        }
    }


    //Now we can write all of the header data
    gcStreamWriteStr(&headerStream, "RARC", 4);
    gcStreamWriteU32(&headerStream, archiveSize + stringTableSize + fileDataSize);
    gcStreamWriteU32(&headerStream, 0x20);
    gcStreamWriteU32(&headerStream, padTo32(archiveSize + stringTableSize - 0x20));
    gcStreamWriteU32(&headerStream, fileDataSize);
    gcStreamWriteU32(&headerStream, 0); //MRAM, unsupported
    gcStreamWriteU32(&headerStream, 0); //ARAM, unsupported
    gcStreamWriteU32(&headerStream, 0); //Padding

    //Filesystem header
    gcStreamWriteU32(&fileSysStream, arc->dirnum);
    gcStreamWriteU32(&fileSysStream, 0x20);
    gcStreamWriteU32(&fileSysStream, arc->filenum);
    gcStreamWriteU32(&fileSysStream, 0x20 + (0x10 * arc->dirnum));
    gcStreamWriteU32(&fileSysStream, stringTableSize);
    gcStreamWriteU32(&fileSysStream, archiveSize - 0x20);
    gcStreamWriteU16(&fileSysStream, curFileIndex);
    gcStreamWriteU8(&fileSysStream, 0);
    gcStreamWriteU8(&fileSysStream, 0);
    gcStreamWriteU32(&fileSysStream, 0);

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

