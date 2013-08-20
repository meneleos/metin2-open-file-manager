#pragma once
#include <list>

// EterIndex Informations Structure
// Entry of the buffer.
typedef struct IndexEntryStr
{
	char Header[3];
	unsigned long EncryptedSize;
	unsigned long CompressedSize;
	unsigned long DecompressedSize;
} IndexEntry;

// Index informations.
typedef struct IndexInfoStr
{
	char Header[3];
	unsigned long Version;
	unsigned long FileCount;
} IndexInfo;

// Individual packed file informations.
typedef struct IndexFileInfoStr
{
	unsigned long CRC;
	unsigned long Dword3;
	unsigned long SrcSize;
	unsigned long UnpackedCRC;
	unsigned long Offset;
	unsigned char Type;
} IndexFileInfo;

// EterPack extraction informations.
typedef struct PackFileInfoStr
{
	char Header[3];
	unsigned long DataEncryptedSize;
	unsigned long DataCompressedSize;
	unsigned long DataDecompressedSize;
} PackFileInfo;

typedef struct ToPack
{
	char FileName[255];
	char FilePath[255];
	int Type;
} ToPackInfo;

std::list<IndexEntry> IEntry;
std::list<IndexFileInfoStr> IFileInfo;
std::list<ToPackInfo> TPInfo;