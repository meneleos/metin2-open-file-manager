#pragma once

class XTEA
{
private:

public:
	int Encrypt(unsigned long* Output, unsigned long* Input, unsigned char* Key, int Size);
	int Decrypt(unsigned long* Output, unsigned long* Input, unsigned char* Key, int Size);
};