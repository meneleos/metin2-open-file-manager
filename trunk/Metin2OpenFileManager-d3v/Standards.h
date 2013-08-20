#pragma once

// Archives Information.
#define FourCC_Archive 0x5A4F434D // MCOZ
#define FourCC_Item 0x5850494D // MIPX
#define FourCC_Mob 0x5450494D // MIPT
#define FourCC_Index 0x444B5045 // EPKD
#define IndexVersion 2
#define IndexMinimumBlockSize 0xC0

// XTEA's Prime number.
#define XTEADelta -1640531527 //-0x61C88647
// XTEA's Required symmetrical keys.
unsigned char IndexKey[] = 
{
	0xB9, 0x9E, 0xB0, 0x02, 
	0x6F, 0x69, 0x81, 0x05, 
	0x63, 0x98, 0x9B, 0x28, 
	0x79, 0x18, 0x1A, 0x00
};
unsigned char DataKey[] = 
{
	0x22, 0xB8, 0xB4, 0x04, 
	0x64, 0xB2, 0x6E, 0x1F, 
	0xAE, 0xEA, 0x18, 0x00, 
	0xA6, 0xF6, 0xFB, 0x1C
};

// Some LZO's returns.
#define LZO_E_OK 1
#define LZO_E_INPUT_OVERRUN (-1)
#define LZO_E_EOF_NOT_FOUND (-2)
#define LZO_E_INPUT_NOT_CONSUMED (-3)

// Files extension.
#define IndexExt ".eix"
#define ArchiveExt ".epk"