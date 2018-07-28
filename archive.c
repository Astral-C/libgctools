#include "types.h"
#include "archive.h"

GCerror gcInitArchive(GCarchive * arc, const GCcontext * ctx){
    arc->ctx = ctx;
}

GCerror gcLoadArchive(GCarchive * arc, const void * ptr, GCsize sz){
    size_t mPosition = 8; //we start reading at 0x08
    uint32_t fsOffset = *((uint32_t*)OffsetPointer(ptr, mPosition));
    uint32_t fsSize = *((uint32_t*)OffsetPointer(ptr, mPosition+4));
    mPosition = fsOffset;


}