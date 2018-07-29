#include "types.h"

GCuint32 gcSwap32(GCuint32 n) {
    return ( ((n>>24)&0xFF) | ((n<<8) & 0xFF0000) | ((n>>8)&0xFF00) | ((n<<24)&0xFF000000) );
}

GCuint16 gcSwap16(GCuint16 n) {
    return ( ((n<<8)&0xFF00) | ((n>>8)&0x00FF) );
}

const char* gcGetErrorMessage(GCerror err){
    switch(err){
        case GC_ERROR_NULL_CONTEXT:
            return "GCcontext Null";
        case GC_ERROR_STREAM_OOB:
            return "Invalid Stream Position";
        default:
            return "";
    }
}