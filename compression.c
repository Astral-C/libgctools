#include "compression.h"
//yay decompression requires a buffer the size of the decoded file, 
//and since the user is required to allocate all memory themselves 
//it means that the user needs to know the size of the decompressed data
size_t yayGetDecodedSize(void* yay){
	yayHeader* head = &yay;
	printf("yay magic %c%c%c%c\n", head->magic[0], head->magic[1], head->magic[2], head->magic[3]);
	return toBEInt(head->decodedSize);
}