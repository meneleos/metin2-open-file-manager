#include "CRC4.h"

unsigned long CRC4::ComputeHash(unsigned char* Buffer, int Size)
{
	unsigned long crc = 0xffffffff;
    for (int i = 0; i < Size; i++) 
		crc = (crc >> 8) ^ crctable[Buffer[i] ^ crc & 0x000000FF];
    return ~crc;
}