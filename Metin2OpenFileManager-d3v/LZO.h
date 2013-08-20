#pragma once

class LZO
{
private:
	unsigned long CompWrap(const unsigned char* in ,  unsigned long  in_len, unsigned char* out,  unsigned long* out_len,  unsigned long  ti,  void* wrkmem);
public:
	int Decompress(const unsigned char* In , unsigned long InSize, unsigned char* Out, unsigned long* OutSize);
	int Compress(const unsigned char* In , unsigned long InSize, unsigned char* Out, unsigned long* OutSize);
};