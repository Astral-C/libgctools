#include "compression.h"
#include <string.h>

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

	expand_limit = ((src_data[4] << 24) | (src_data[5] << 16) | (src_data[6] << 8) | src_data[7]);
	ref_offset = ((src_data[8] << 24) | (src_data[9] << 16) | (src_data[10] << 8) | src_data[11]);
	read_offset = ((src_data[12] << 24) | (src_data[13] << 16) | (src_data[14] << 8) | src_data[15]);
    
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

// Compresses a file into work buffer and return the total size of the compressed data. work_buffer should be same size as src_data.
GCsize gcYay0Compress(GCcontext* ctx, GCuint8* src_data, GCuint8* out_buffer, GCsize srcout_size){
    // Method adapted from Cuyler36's GCNToolkit.
    // This is a stopgap solution until I can get more knowledgeable about the format and write my own compressor


    const GCuint32 OFSBITS = 12;
    GCint32 decPtr = 0;
    
    // Set up for mask buffer
    GCuint32 maskMaxSize = (srcout_size + 32) >> 3;
    GCuint32 maskBitCount = 0, mask = 0;
    GCint32 maskPtr = 0;
    
    // Set up for link buffer
    GCuint32 linkMaxSize = srcout_size >> 1;
    GCuint16 linkOffset = 0;
    GCint32 linkPtr = 0;
    GCuint16 minCount = 3, maxCount = 273;

    // Set up chunk and window settings
    GCint32 chunkPtr = 0, windowPtr = 0, windowLen = 0, length = 0, maxlen = 0;

    // Initialize all the buffers with proper size
    GCuint32* maskBuffer = gcAllocMem(ctx, (maskMaxSize >> 2)*sizeof(GCuint32));
    GCuint16* linkBuffer = gcAllocMem(ctx, linkMaxSize*sizeof(GCuint16));
    GCuint8* chunkBuffer = gcAllocMem(ctx, srcout_size);

    while(decPtr < srcout_size){
        printf("%d/%d\r", decPtr, srcout_size);
        if(windowLen >= 1 << OFSBITS){
            windowLen -= (1 << OFSBITS);
            windowPtr = decPtr - windowLen;
        }

        if(srcout_size - decPtr < maxCount){
            maxCount = (GCuint16)(srcout_size - decPtr);
        }

        maxlen = 0;

        for (GCsize i = 0; i < windowLen; i++)
        {
            for (length = 0; length < (windowLen - i) && length < maxCount; length++)
            {
                if(src_data[decPtr + length] != src_data[windowPtr + length + i]) break;
            }
            if(length > maxlen)
            {
                maxlen = length;
                linkOffset = (GCuint16)windowLen - i;

            }
            
        }
        
        length = maxlen;

        mask <<= 1;
        if(length >= minCount){
            GCuint16 link = (GCuint16)((linkOffset - 1) & 0x0FFF);

            if(length < 18){
                link |= (GCuint16)((length - 2) << 12);
            } else {
                chunkBuffer[chunkPtr++] = (GCuint8)(length - 18);
            }

            linkBuffer[linkPtr++] = gcSwap16(link);
            decPtr += length;
            windowLen += length;
        } else {
            chunkBuffer[chunkPtr++] = src_data[decPtr++];
            windowLen++;
            mask |= 1;
        }

        maskBitCount++;
        if(maskBitCount == 32){
            maskBuffer[maskPtr] = gcSwap32(mask);
            maskPtr++;
            maskBitCount =0;
        }

    }

    if(maskBitCount > 0){
        mask <<= 32 - maskBitCount;
        maskBuffer[maskPtr] = gcSwap32(mask);
        maskPtr++;
    }

    GCsize compressedSize = 0x10 + (sizeof(GCuint32) * maskPtr) + (sizeof(GCuint16) * linkPtr) + chunkPtr;


    const char* fourcc = "Yay0";
    GCuint32 linkSecOff = 0x10 + (sizeof(GCuint32) * maskPtr);
    GCuint32 chunkSecOff = linkSecOff + (sizeof(GCuint16) * linkPtr);

    GCstream out;
    gcInitStream(ctx, &out, out_buffer, srcout_size, GC_ENDIAN_BIG);
    gcStreamWriteStr(&out, fourcc, 4);
    gcStreamWriteU32(&out, srcout_size);
    gcStreamWriteU32(&out, linkSecOff);
    gcStreamWriteU32(&out, chunkSecOff);

    memcpy(OffsetPointer(out_buffer, 0x10), maskBuffer, maskPtr*sizeof(GCuint32));
    memcpy(OffsetPointer(out_buffer, linkSecOff), linkBuffer, linkPtr*sizeof(GCuint16));
    memcpy(OffsetPointer(out_buffer, chunkSecOff), chunkBuffer, chunkPtr);

    gcFreeMem(ctx, maskBuffer);
    gcFreeMem(ctx, linkBuffer);
    gcFreeMem(ctx, chunkBuffer);

    return compressedSize;
}