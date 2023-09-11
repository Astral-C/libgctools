#include "compression.h"
#include <string.h>

GCuint32 gcDecompressedSize(GCcontext* ctx, GCuint8* src_data, GCuint32 offset){
    return gcSwap32(*((GCuint32*)src_data + 1) - offset);
}

void gcYaz0Decompress(GCcontext* ctx, GCuint8* src_data, GCuint8* dst_data, GCsize length, GCuint32 offset){
    GCuint32 count = 0, src_pos = 0, dst_pos = 0;
    GCuint8 bits;

    src_pos = 16;
    while(dst_pos < length) {
        if(count == 0){
            bits = src_data[src_pos];
            ++src_pos;
            count = 8;
        }
        
        if((bits & 0x80) != 0){
            dst_data[dst_pos] = src_data[src_pos];
            dst_pos++;
            src_pos++;

        } else {
            GCuint8 b1 = src_data[src_pos], b2 = src_data[src_pos + 1];

            GCuint32 len = b1 >> 4;
            GCuint32 dist = ((b1 & 0xF) << 8) | b2;
            GCuint32 copy_src = dst_pos - (dist + 1);

            src_pos += 2;

            if(len == 0){
                len = src_data[src_pos] + 0x12;
                src_pos++;
            } else {
                len += 2;
            }

            for (GCsize i = 0; i < len; ++i)
            {
                dst_data[dst_pos] = dst_data[copy_src];
                copy_src++;
                dst_pos++;
            }
            
        }
        
        bits <<= 1;
        --count;
    }
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
    GCuint8* read_src = GCOffsetPointer(src_data, read_offset);

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

    memcpy(GCOffsetPointer(out_buffer, 0x10), maskBuffer, maskPtr*sizeof(GCuint32));
    memcpy(GCOffsetPointer(out_buffer, linkSecOff), linkBuffer, linkPtr*sizeof(GCuint16));
    memcpy(GCOffsetPointer(out_buffer, chunkSecOff), chunkBuffer, chunkPtr);

    gcFreeMem(ctx, maskBuffer);
    gcFreeMem(ctx, linkBuffer);
    gcFreeMem(ctx, chunkBuffer);

    return compressedSize;
}

// Compresses a file into a buffer and returns the compressed data. Use `dstout_size` to know the compressed data's size.
// `level` denotes the compression level: 0 for none, 9 for best.
GCuint8* gcYaz0Compress(GCcontext* ctx, GCuint8* src_data, GCsize srcout_size, GCsize* dstout_size, GCuint8 level) {
    GCuint8* dataptr = src_data;
    GCsize maxbacklevel = 0x10e0ULL * (level / 9.0) - 0x0e0ULL;
    // Calculation for result pointer's size.
    GCsize length = srcout_size + srcout_size / 8 + 0x10;
    GCuint8* result = (GCuint8*)gcAllocMem(ctx, length);
    // First 16 bytes will be written by the GCstream.
    GCuint8* resultptr = &result[16];
    GCstream* stream = (GCstream*)gcAllocMem(ctx, sizeof(GCstream));
    gcInitStream(ctx, stream, result, length, GC_ENDIAN_BIG);
    gcStreamWriteStr(stream, "Yaz0", 4);
    gcStreamWriteU32(stream, srcout_size);
    // Last 8 bytes of header are unknown.
    gcStreamWriteU32(stream, 0);
    gcStreamWriteU32(stream, 0);
    gcFreeMem(ctx, stream);
    GCsize dstoffs = 16;
    GCsize offs = 0;
    while (1) {
        GCsize headeroffs = dstoffs++;
        resultptr++;
        GCuint8 header = 0;
        for (int i = 0; i < 8; i++) {
            GCuint8 comp = 0;
            ptrdiff_t back = 1;
            GCsize nr = 2;
            GCuint8* ptr = dataptr - 1;
            GCsize maxnum = 0x111;
            if (length - offs < maxnum) maxnum = length - offs;
            GCsize maxback = maxbacklevel;
            if (offs < maxback) maxback = offs;
            maxback = ((GCsize)dataptr) - maxback;
            GCsize tmpnr = 0;
            while (maxback <= (GCsize)(ptr)) {
                if (*(GCuint16*)ptr == *(GCuint16*)dataptr && ptr[2] == dataptr[2]) {
                    tmpnr = 3;
                    while(tmpnr < maxnum && ptr[tmpnr] == dataptr[tmpnr]) tmpnr++;
                    if (tmpnr > nr) {
                        if (offs + tmpnr > length) {
                            nr = length - offs;
                            back = (ptrdiff_t)(dataptr - ptr);
                            break;
                        }
                        nr = tmpnr;
                        back = (ptrdiff_t)(dataptr - ptr);
                        if (nr == maxback) break;
                    }
                }
                --ptr;
            }
            if (nr > 2) {
                offs += nr;
                dataptr += nr;
                if (nr >= 0x12) {
                    *resultptr++ = (GCuint8)(((back - 1) >> 8) & 0xF);
                    *resultptr++ = (GCuint8)((back - 1) & 0xFF);
                    *resultptr++ = (GCuint8)((nr - 0x12) & 0xFF);
                    dstoffs += 3;
                } else {
                    *resultptr++ = (GCuint8)((((back - 1) >> 8) & 0xF) | (((nr - 2) & 0xF) << 4));
                    *resultptr++ = (GCuint8)((back - 1) & 0xFF);
                    dstoffs += 2;
                }
                comp = 1;
            }
            else {
                *resultptr++ = *dataptr++;
                dstoffs++;
                offs++;
            }
            header = (GCuint8)((header << 1) | ((comp == 1) ? 0 : 1));
            if (offs >= length) {
                header = (GCuint8)(header << (7 - i));
                break;
            }
        }
        result[headeroffs] = header;
        if (offs >= length) break;
    }
    // dstoffs should be >= length once this is run
    while ((dstoffs % 4) != 0) dstoffs++;
    GCuint8* realresult = gcAllocMem(ctx, dstoffs);
    memset(realresult, 0, dstoffs);
    memcpy(realresult, result, dstoffs);
    gcFreeMem(ctx, result);
    *dstout_size = dstoffs;
    return realresult;
}