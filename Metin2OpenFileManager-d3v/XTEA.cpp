#include "XTEA.h"
#include "Standards.h"

int XTEA::Encrypt(unsigned long* Output, unsigned long* Input, unsigned char* Key, int Size)
{
	int round_count = 32;
	int rounds = 0;
	if (Size & 7) Size = (Size & 0xFFFFFFF8) + 8;
	if(Size) {
		do {
			Output[0] = Input[0];
			Output[1] = Input[1];
			unsigned int sum  = 0;
			for (int i=0; i < round_count; i++) {
				Output[0] += (((Output[1] << 4) ^ (Output[1] >> 5)) + Output[1]) ^ (sum  + *(unsigned long*)(Key + 4 * (sum  & 3)));
				sum  += XTEADelta;
				Output[1] += (((Output[0] << 4)  ^ (Output[0] >> 5)) + Output[0]) ^ (sum  + *(unsigned long*)(Key + 4 * ((sum  >> 11) & 3)));
			}
			Input += 2;
			Output += 2;
			++rounds;
		} while ( Size / 8 > rounds );
	}
	return Size;
}

int XTEA::Decrypt(unsigned long* Output, unsigned long* Input, unsigned char* Key, int Size)
{
	int round_count = 32;
	int rounds = 0;
	if (Size & 7) Size = (Size & 0xFFFFFFF8) + 8;
	if(Size) {
		do {
			Output[0] = Input[0];
			Output[1] = Input[1];
			unsigned int sum  = XTEADelta * round_count;
			for (int i=0; i < round_count; i++) {
				Output[1] -= (((Output[0] << 4)  ^ (Output[0] >> 5)) + Output[0]) ^ (sum  + *(unsigned long*)(Key + 4 * ((sum  >> 11) & 3)));
				sum  -= XTEADelta;
				Output[0] -= (((Output[1] << 4) ^ (Output[1] >> 5)) + Output[1]) ^ (sum  + *(unsigned long*)(Key + 4 * (sum  & 3)));
			}
			Input += 2;
			Output += 2;
			++rounds;
		} while ( Size / 8 > rounds );
	}
	return Size;
}