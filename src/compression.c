#include "compression.h"

GCuint32 gcDecompressedSize(GCcontext* ctx, GCuint8* src_data, GCuint32 offset){
    return gcSwap32(*((GCuint32*)src_data + 1) - offset);
}

void gcYaz0Decompress(GCcontext* ctx, GCuint8* src_data, GCuint8* dst_data, GCsize length, GCuint32 offset){
    GCuint32 count = 0, decompressedSize, bits;

    GCuint8* endptr = OffsetPointer(dst_data, gcDecompressedSize(ctx, src_data, offset));
    
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

void gcYay0Decompress(GCcontext* ctx, GCuint8* src_data, GCuint8* dst_data, GCsize length, GCuint32 offset){
    GCuint32 expand_limit, bit_offset, expand_offset, ref_offset, read_offset, bit_count, bits;

	expand_limit = ((src_data[8] << 24) | (src_data[9] << 16) | (src_data[10] << 8) | src_data[11]);
	ref_offset = ((src_data[12] << 24) | (src_data[13] << 16) | (src_data[14] << 8) | src_data[15]);
	read_offset = ((src_data[16] << 24) | (src_data[17] << 16) | (src_data[18] << 8) | src_data[19]);
    
    expand_offset = 0;
    bit_count = 0;
    bit_offset = 16;

    if(length == 0 || offset > expand_limit){
        return;
    }

    GCuint8* read_dst = dst_data;
    GCuint8* read_src = OffsetPointer(src_data, read_offset);

    do {
        if(bit_count == 0){
            GCuint8* data = (src_data + bit_offset);
            bits = ((data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]);
        
            bit_count = 32;
            bit_offset += 4;
        }

        if(bits & 0x80000000) {
            if(offset == 0){

                *read_dst = *read_src;

                if(--length == 0){
                    return;
                }
            } else {
                --offset;
            }
            ++expand_offset;
            ++read_dst;
            ++read_offset;
            ++read_src;
        } else {
            GCuint8* data = (src_data + ref_offset);
            ref_offset += 2;

            GCuint16 run = (data[0] << 8 | data[1]);
            GCuint16 run_length = (run >> 12);
            GCuint16 run_offset = (run & 0xFFF);
            GCuint32 ref_start = (expand_offset - run_offset);
            GCuint32 ref_length;

            if(run_length == 0){
                ref_length = (src_data[read_offset++] + 18);
                ++read_src;
            } else {
                ref_length = (run_length + 2);
            }

            if(ref_length > (expand_limit - expand_offset)){
                ref_length = expand_limit - expand_offset;
            }

            GCuint8* ref_ptr = (dst_data + expand_offset);
            while(ref_length > 0){
                if(offset == 0){
                    *ref_ptr = dst_data[ref_start - 1];
                    if(--length == 0){
                        return;
                    }
                } else {
                    --offset;
                }

                ++expand_offset;
                ++read_dst;
                ++ref_ptr;
                ++ref_start;
                --ref_length;
            }
        }

        bits <<= 1;
        --bit_count;
    } while(expand_offset < expand_limit);

}