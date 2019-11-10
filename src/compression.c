#include "compression.h"

GCuint32 gcYaz0DecompressedSize(GCcontext* ctx, GCuint8* src_data, GCuint32 offset){
    return gcSwap32(*((GCuint32*)src_data + 1) - offset);
}

void gcYaz0Decompress(GCcontext* ctx, GCuint8* src_data, GCuint8* dst_data, GCsize length, GCuint32 offset){
    GCuint32 count = 0, decompressedSize, bits;

    GCuint8* endptr = OffsetPointer(dst_data, gcYaz0DecompressedSize(ctx, src_data, offset));
    
    src_data += 16;

    do {
        if(count == 0){
            bits = *src_data++;
            count = 8;
        }
        
        if(bits & 0x80){
            if(offset == 0){
                --length;
                *dst_data++ = *src_data;
                
                if(length == 0){
                    return;
                }
            } else {
                --offset;
            }
            
            ++src_data;

        } else {
            int offset = ((src_data[0] & 0xF) << 8) | src_data[1];

            int length = (src_data[0] >> 4);
            src_data += 2;
            GCuint8* ref_data = (dst_data - offset - 1);

            if(length == 0){
                length = (*src_data + 18);
                ++src_data;
            } else {
                length += 2;
            }

            do {
                if(offset == 0){
                    *dst_data++ = *ref_data;
                    --length;

                    if(length == 0){
                        return;
                    }
                } else {
                    --offset;
                }

                --length;
                ++ref_data;
            } while(length != 0);
        }
        
        bits <<= 1;
        --count;
    } while(dst_data != endptr);
}