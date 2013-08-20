#include <stdio.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <windows.h>
#include <conio.h>
#include "Standards.h"
#include "LogHandler.h"
#include "LZO.h"
#include "XTEA.h"
#include "CRC4.h"
#include "Structures.h"
#include "ConfigurationHandler.h"
#define mkfolder(dirname) CreateDirectoryA(dirname, NULL)
using namespace std;

int crashi = 0;

IndexEntry tmpIE;
IndexFileInfo tmpFI;
IndexInfo tmpI;

int main(int argc, char * argv[])
{
	printf("\t**********************************\n"
		   "\t** Original Project by Balika01 **\n"
		   "\t**     Improved by d3v1l401     **\n"
		   "\t**      http://d3vsite.org      **\n"
		   "\t**********************************\n\n");
	
	if(argc != 2)
	{
		printf("Usage: M2OFM <.dat file>\n\nunpack:\n  unpack|pack_name|out_folder|out_dat_name\npack:\n  pack|pack_name\n\tfile_name|file_path|file_type\ncomment:\n  start line whit \"--\"\n");
		_getch();
		return -1;
	}
	
	IniReader IniRead(argv[2]);
	if (!IniRead.CheckFile())
	{
		LogHandler::PrintMessage("Can't read configuration file.\n",1);
		_getch();
		exit(-1);
	}

	if (!strcmp(argv[1],"-e"))
	{
		IniRead.ReadString("Extract","InFile","");
		IniRead.ReadString("Extract","OutFolder","");
		IniRead.ReadString("Extract","OutINI","");
	}

	if (!strcmp(argv[1],"-p"))
	{
		IniRead.ReadString("Pack","EterName","");
		IniRead.ReadString("Pack","FileName","");
		IniRead.ReadString("Pack","FilePath","");
		IniRead.ReadInteger("Pack","Type",0);
	}

	if (Configuration.is_open())
	{
		while(1)
		{
			string line;
			getline(Configuration,line);
LAB1:
			if(strlen(line.c_str()) == 0 || line.substr(0, 2) == "--") if(!Configuration.good()) break; else continue;
			if(line.substr(0, 4) == "pack")
			{
				if(strlen(line.c_str())-5 < 1) continue;
				std::string outfile = line.substr(5, strlen(line.c_str())-5);
				bool agin = 0;
				std::string file[9999];
				std::string path[9999];
				int type[9999];
				int filecount = 0;
				while(1)
				{
					std::getline(myfile,line);
					if(strlen(line.c_str()) == 0 || line.substr(0, 2) == "--") if(!myfile.good()) break; else continue;
					if(line.substr(0, 1) != "\t") 
					{
						agin = 1;
						break;
					}
					if(strlen(line.c_str())-1 < 1 || line.substr(2, 3) == "--") if(!myfile.good()) break; else continue;
					std::string filename = line.substr(1, strlen(line.c_str())-1);
					file[filecount] = strtok((char*)(filename.c_str()),"|");
					path[filecount] = strtok(NULL, "|");
					type[filecount] = atoi(strtok(NULL, "|"));
					++filecount;
					if(!myfile.good()) break;
				}
				unsigned char* eixdecompressedBuffer = new unsigned char[0x0C + filecount * IndexBlockSize];
				memset(eixdecompressedBuffer, 0, 0x0C + filecount * IndexBlockSize);

				eixdecompressedBuffer += 0x0C;

				unsigned char* epkBuffer = new unsigned char[536870911]; //512MB

				int lastoffset = 0;
				int donefiles = 0;
				for(int i=0; i < filecount; ++i) {
					unsigned char* Buffer;
					unsigned long Size;
					
					FILE* File = fopen(path[i].c_str() , "rb" );
					if (File == NULL) {
						printf("Could not open %s file.\n", path[i].c_str());
						if(agin) goto LAB1; else return 0;
					}
					fseek(File , 0 , SEEK_END);
					Size = ftell(File);
					rewind(File);
					Buffer = new unsigned char[Size];
					if (Buffer == NULL) {
						printf("Memory error when make buffer for %s file.\n", path[i].c_str());
						if(agin) goto LAB1; else return 0;
					}
					if (fread(Buffer,1,Size,File) != Size) {
						printf("Could not read %s file.\n", path[i].c_str());
						if(agin) goto LAB1; else return 0;
					}
					fclose(File);

					*(unsigned long*)eixdecompressedBuffer = i;
					memcpy(eixdecompressedBuffer + 4, file[i].c_str(), strlen(file[i].c_str()));
					*(unsigned long*)(eixdecompressedBuffer + 168) = CRC32((unsigned char*)file[i].c_str(),strlen(file[i].c_str()));
					*(unsigned long*)(eixdecompressedBuffer + 184) = lastoffset;
					*(unsigned char*)(eixdecompressedBuffer + 188) = type[i];

					if(type[i] == 0){
						*(unsigned long*)(eixdecompressedBuffer + 172) = Size;
						*(unsigned long*)(eixdecompressedBuffer + 176) = Size;
						*(unsigned long*)(eixdecompressedBuffer + 180) = CRC32(Buffer, Size);
						memset(epkBuffer + lastoffset, 0, Size);
						memcpy(epkBuffer + lastoffset, Buffer, Size);
						lastoffset += Size;
					}
					else if(type[i] == 1){
						unsigned char* compressedBuffer = new unsigned char[Size + (Size / 16) + 64 + 3 + 20];
						memset(compressedBuffer, 0, Size + (Size / 16) + 64 + 3 + 20);
	
						unsigned long compSize = 0;
						if(lzo_compress(Buffer, Size, compressedBuffer + 20, &compSize) != LZO_E_OK)
						{
							printf("Error when compressing %s.\n", file[i].c_str());
							delete [] compressedBuffer;
							pause();
							if(agin) goto LAB1; else return 0;
						}
						
						*(unsigned long*)compressedBuffer = FourCC;
						*(unsigned long*)(compressedBuffer + 8) = compSize;
						*(unsigned long*)(compressedBuffer + 12) = Size;
						*(unsigned long*)(compressedBuffer + 16) = FourCC;

						memset(epkBuffer + lastoffset, 0, compSize + 20);
						memcpy(epkBuffer + lastoffset, compressedBuffer, compSize + 20);

						*(unsigned long*)(eixdecompressedBuffer + 172) = Size;
						*(unsigned long*)(eixdecompressedBuffer + 176) = compSize + 4;
						*(unsigned long*)(eixdecompressedBuffer + 180) = CRC32(Buffer, Size);
						lastoffset += compSize + 20;
						delete[] compressedBuffer;
					}
					else if(type[i] == 2)
					{
						unsigned char* compressedBuffer = new unsigned char[Size + (Size / 16) + 64 + 3 + 4];
						memset(compressedBuffer, 0, Size + (Size / 16) + 64 + 3 + 4);
	
						unsigned long compSize = 0;
						if(lzo_compress(Buffer, Size, compressedBuffer + 4, &compSize) != LZO_E_OK)
						{
							printf("Error when compressing %s.\n", file[i].c_str());
							delete [] compressedBuffer;
							pause();
							if(agin) goto LAB1; else return 0;
						}
						*(unsigned long*)compressedBuffer = FourCC;

						unsigned char* encryptedBuffer = new unsigned char[compSize + 4 + 7];
						memset(encryptedBuffer, 0, compSize + 4 + 7);
	
						unsigned long cryptSize = XteaEncrypt((unsigned long*)encryptedBuffer, (unsigned long*)compressedBuffer, DataXTEA, compSize + 4);

						memset(epkBuffer + lastoffset, 0, cryptSize + 16);
						*(unsigned long*)(epkBuffer + lastoffset) = FourCC;
						*(unsigned long*)(epkBuffer + lastoffset + 4)  = cryptSize;
						*(unsigned long*)(epkBuffer + lastoffset + 8)  = compSize;
						*(unsigned long*)(epkBuffer + lastoffset + 12)  = Size;
						memcpy(epkBuffer + lastoffset + 16, encryptedBuffer, cryptSize);

						
						*(unsigned long*)(eixdecompressedBuffer + 172) = 512 + (512 / (cryptSize + 16));
						*(unsigned long*)(eixdecompressedBuffer + 176) = cryptSize;
						*(unsigned long*)(eixdecompressedBuffer + 180) = CRC32(epkBuffer + lastoffset, cryptSize);
						lastoffset += cryptSize + 16;
						delete[] compressedBuffer;
						delete[] encryptedBuffer;
					}
					else
					{
						printf("type %i is unsupported\n", type[i]);
						delete [] Buffer;
						continue;
					}
					eixdecompressedBuffer += IndexBlockSize;
					++donefiles;
					delete [] Buffer;
				}

				unsigned long eixdecompressedSize = 0x0C + donefiles * IndexBlockSize;
				eixdecompressedBuffer -= eixdecompressedSize;

				*(unsigned long*)eixdecompressedBuffer = IndexHeader;
				*(unsigned long*)(eixdecompressedBuffer + 4) = IndexVersion;
				*(unsigned long*)(eixdecompressedBuffer + 8) = donefiles;

				unsigned char* compressedBuffer = new unsigned char[eixdecompressedSize + (eixdecompressedSize / 16) + 64 + 3 + 4];
				memset(compressedBuffer, 0, eixdecompressedSize + (eixdecompressedSize / 16) + 64 + 3 + 4);
	
				unsigned long compSize = 0;
				if(lzo_compress(eixdecompressedBuffer, eixdecompressedSize, compressedBuffer + 4, &compSize) != LZO_E_OK)
				{
					printf("Error when compressing index.\n");
					delete [] compressedBuffer;
					pause();
					if(agin) goto LAB1; else return 0;
				}
				*(unsigned long*)compressedBuffer = FourCC;

				unsigned char* encryptedBuffer = new unsigned char[compSize + 4];
				memset(encryptedBuffer, 0, compSize + 4);
	
				unsigned long cryptSize = XteaEncrypt((unsigned long*)encryptedBuffer, (unsigned long*)compressedBuffer, IndexXTEA, compSize + 4);

				unsigned char* DoneBuffer = new unsigned char[cryptSize + 16];
				memset(DoneBuffer, 0, cryptSize + 16);
				*(unsigned long*)DoneBuffer = FourCC;
				*(unsigned long*)(DoneBuffer + 4)  = cryptSize;
				*(unsigned long*)(DoneBuffer + 8)  = compSize;
				*(unsigned long*)(DoneBuffer + 12)  = eixdecompressedSize;
				memcpy(DoneBuffer + 16, encryptedBuffer, cryptSize);

                FILE * indexof = fopen((outfile + IndexName).c_str(), "wb");
				if(indexof)
				{
					fwrite(DoneBuffer, 1, cryptSize + 16, indexof);
					fclose(indexof);
				}
				
				FILE * dataof = fopen((outfile + DataName).c_str(), "wb");
				if(dataof)
				{
					fwrite(epkBuffer, 1, lastoffset + 16, dataof);
					fclose(dataof);
				}
				
				printf("Done!\n");
				if(agin) goto LAB1;
			}
			else if(line.substr(0, 6) == "unpack")
			{
				if(strlen(line.c_str())-7 < 1) continue;
				std::string in = line.substr(7, strlen(line.c_str())-7);
				char* infile = strtok((char*)(in.c_str()),"|");
				std::string outfolder = strtok(NULL, "|");
				char* outfile = strtok(NULL, "|");

				std::string eixName_ = (std::string)infile + IndexName;
				std::string epkName_ = (std::string)infile + DataName;
				const char * eixName = eixName_.c_str();
				const char * epkName = epkName_.c_str();
	
				unsigned char* eixBuffer;
				unsigned long eixSize;
				
				FILE* eixFile = fopen(eixName , "rb" );
				if (eixFile == NULL) {
					printf("Could not open %s file.\n", eixName);
					if(!myfile.good()) {pause(); return 0;} else goto LAB1;
				}
				fseek(eixFile , 0 , SEEK_END);
				eixSize = ftell(eixFile);
				rewind(eixFile);
				eixBuffer = new unsigned char[eixSize];
				if (eixBuffer == NULL) {
					printf("Memory error when make buffer for %s file.\n", eixName);
					if(!myfile.good()) {pause(); return 0;} else goto LAB1;
				}
				if (fread(eixBuffer,1,eixSize,eixFile) != eixSize) {
					printf("Could not read %s file.\n", eixName);
					if(!myfile.good()) {pause(); return 0;} else goto LAB1;
				}
				fclose(eixFile);
				
				unsigned char* epkBuffer;
				unsigned long epkSize;
				
				FILE* epkFile = fopen(epkName , "rb" );
				if (epkFile == NULL) {
					printf("Could not open %s file.\n", epkName);
					if(!myfile.good()) {pause(); return 0;} else goto LAB1;
				}
				fseek(epkFile , 0 , SEEK_END);
				epkSize = ftell(epkFile);
				rewind(epkFile);
				epkBuffer = new unsigned char[epkSize];
				if (epkBuffer == NULL) {
					printf("Memory error when make buffer for %s file.\n", epkName);
					if(!myfile.good()) {pause(); return 0;} else goto LAB1;
				}
				if (fread(epkBuffer,1,epkSize,epkFile) != epkSize) {
					printf("Could not read %s file.\n", epkName);
					if(!myfile.good()) {pause(); return 0;} else goto LAB1;
				}
				fclose(epkFile);

				if(eixSize < 0x0C)
				{
					printf("The file size for the %s file is too small. The program will now exit.\n", eixName);
					pause();
					if(!myfile.good()) {pause(); return 0;} else goto LAB1;
				}

				unsigned char* eixdecompressedBuffer;
				unsigned long eixdecompressedSize;

				if (*(unsigned long*)eixBuffer != IndexHeader)
				{
					unsigned long eixheader = *(unsigned long*)eixBuffer;
					unsigned long eixcryptedSize = *(unsigned long*)(eixBuffer + 4);
					unsigned long eixcompressedSize = *(unsigned long*)(eixBuffer + 8);
					eixdecompressedSize = *(unsigned long*)(eixBuffer + 12);

					if (eixheader != FourCC)
					{
						printf("The FourCC of %s is incorrect.\n", eixName);
						pause();
						if(!myfile.good()) return 0; else goto LAB1;
					}
		
					if(eixcryptedSize == 0)
					{
						printf("[TODO] -- Index of %s is not encrypted!\n", eixName);
						pause();
						if(!myfile.good()) return 0; else goto LAB1;
					}

					unsigned char* eixcompressedBuffer = new unsigned char[eixcryptedSize];
					memset(eixcompressedBuffer, 0, eixcryptedSize);

					XteaDecrypt((unsigned long*)eixcompressedBuffer, (unsigned long*)(eixBuffer + 0x10), IndexXTEA, eixcryptedSize);
					if(*(unsigned long*)eixcompressedBuffer != FourCC)
					{
						delete [] eixcompressedBuffer;
						printf("The XTEA Key of %s is incorrect.\n", eixName);
						pause();
						if(!myfile.good()) return 0; else goto LAB1;
					}

					eixdecompressedBuffer = new unsigned char[eixdecompressedSize];
					memset(eixdecompressedBuffer, 0, eixdecompressedSize);

					unsigned long finalSize = 0;
					if(lzo_decompress(eixcompressedBuffer + 4, eixcompressedSize, eixdecompressedBuffer, &finalSize) != LZO_E_OK || finalSize != eixdecompressedSize)
					{
						delete [] eixdecompressedBuffer;
						delete [] eixcompressedBuffer;
						printf("There was an error when decompressing %s.\n", eixName);
						if(!myfile.good()) return 0; else goto LAB1;
					}
					delete [] eixcompressedBuffer;
				}
				else
				{
					eixdecompressedBuffer = eixBuffer + 4;
					eixdecompressedSize = eixSize - 4;
				}

				unsigned long eixheader2 = *(unsigned long*)eixdecompressedBuffer;
				unsigned long eixversion = *(unsigned long*)(eixdecompressedBuffer + 4);
				unsigned long eixfileCount = *(unsigned long*)(eixdecompressedBuffer + 8);

				if(eixversion != IndexVersion)
				{
					delete [] eixdecompressedBuffer;
					printf("The version of %s file is incorrect. Found: %i Supported: %i).\n", eixName, eixversion, IndexVersion);
					pause();
					if(!myfile.good()) return 0; else goto LAB1;
				}

				std::string filename = ((std::string)infile).substr(1 + ((std::string)infile).find_last_of("\\/"));

				eixdecompressedBuffer += 0x0C;

				FILE * of = fopen(outfile, "w");
				if(!of)
				{
					printf("There was an error creating %s file.\n", "name");
					pause();
					if(!myfile.good()) return 0; else goto LAB1;
				}
				if(of) fprintf(of, "pack|%s\n", infile);

				for(unsigned long x = 0; x < eixfileCount; ++x)
				{
					unsigned long index = *(unsigned long*)eixdecompressedBuffer;
					char filename[160];
					memcpy(filename,eixdecompressedBuffer + 4,160);
					unsigned long filenameCRC = *(unsigned long*)(eixdecompressedBuffer + 168);
					unsigned long dw3 = *(unsigned long*)(eixdecompressedBuffer + 172);
					unsigned long dwSrcSize = *(unsigned long*)(eixdecompressedBuffer + 176);
					unsigned long unpackedCRC = *(unsigned long*)(eixdecompressedBuffer + 180);
					unsigned long dwFileOffset = *(unsigned long*)(eixdecompressedBuffer + 184);
					unsigned char packedType = *(unsigned char*)(eixdecompressedBuffer + 188);
					eixdecompressedBuffer += IndexBlockSize;
					std::string outfilename;
					if(packedType == 0)
					{
						std::stringstream dirPath;
						std::vector<std::string> pathTokens;
						size_t p0 = 0, p1 = std::string::npos;
						while(p0 != std::string::npos)
						{
							p1 = ((const std::string&)filename).find_first_of("\\/", p0);
							if(p1 != p0)
							{
								std::string token = ((const std::string&)filename).substr(p0, p1 - p0);
								pathTokens.push_back(token);
							}
							p0 = ((const std::string&)filename).find_first_not_of("\\/", p1);
						}
						dirPath << outfolder;
						mkfolder(dirPath.str().c_str());
						dirPath << "\\";
						size_t index = 0;
						for(index = 0; index < pathTokens.size() - 1; ++index)
						{
							if(pathTokens[index].find_first_of(":") != std::string::npos)
								continue;
							dirPath << pathTokens[index];
							mkfolder(dirPath.str().c_str());
							dirPath << "\\";
						}
						dirPath << pathTokens[index];
						outfilename = dirPath.str();
						FILE * of = fopen(outfilename.c_str(), "wb");
						if(!of)
						{
							std::string buff = outfolder + "\\crashfile" + (char)crashi;
							++crashi;
							of = fopen(buff.c_str(), "wb");
							printf("Crash file %s saved as: %s\n", outfilename.c_str(), buff.c_str());
							outfilename = buff;
						}
						if(of)
						{
							fwrite(epkBuffer + dwFileOffset, 1, dwSrcSize, of);
							fclose(of);
						}
						else
						{
							printf("Could not save the file %s\n", outfilename.c_str());
						}
					}
					else if(packedType == 1)
					{
						unsigned long dataheader = *(unsigned long*)(epkBuffer + dwFileOffset);
						unsigned long datacryptedSize = *(unsigned long*)(epkBuffer + dwFileOffset + 4);
						unsigned long datacompressedSize = *(unsigned long*)(epkBuffer + dwFileOffset + 8);
						unsigned long datadecompressedSize = *(unsigned long*)(epkBuffer + dwFileOffset + 12);

						if(dataheader != FourCC)
						{
							printf("The FourCC is incorrect of %s.\n", filename);
							continue;
						}

						unsigned char* datauncompressedBuffer = new unsigned char[datadecompressedSize];
						memset(datauncompressedBuffer, 0, datadecompressedSize);

						unsigned long finalSize = 0;
						if(lzo_decompress(epkBuffer + dwFileOffset + 16 + 4, datacompressedSize, datauncompressedBuffer, &finalSize) == LZO_E_OK && finalSize == datadecompressedSize)
						{
							std::stringstream dirPath;
							std::vector<std::string> pathTokens;
							size_t p0 = 0, p1 = std::string::npos;
							while(p0 != std::string::npos)
							{
								p1 = ((const std::string&)filename).find_first_of("\\/", p0);
								if(p1 != p0)
								{
									std::string token = ((const std::string&)filename).substr(p0, p1 - p0);
									pathTokens.push_back(token);
								}
								p0 = ((const std::string&)filename).find_first_not_of("\\/", p1);
							}
							dirPath << outfolder;
							mkfolder(dirPath.str().c_str());
							dirPath << "\\";
							size_t index = 0;
							for(index = 0; index < pathTokens.size() - 1; ++index)
							{
								if(pathTokens[index].find_first_of(":") != std::string::npos)
									continue;
								dirPath << pathTokens[index];
								mkfolder(dirPath.str().c_str());
								dirPath << "\\";
							}
							dirPath << pathTokens[index];
							outfilename = dirPath.str();
							FILE * of = fopen(outfilename.c_str(), "wb");
							if(!of)
							{
								std::string buff = outfolder + "\\crashfile" + (char)crashi;
								++crashi;
								of = fopen(buff.c_str(), "wb");
								printf("Crash file %s saved as: %s\n", outfilename.c_str(), buff.c_str());
								outfilename = buff;
							}
							if(of)
							{
								fwrite(datauncompressedBuffer, 1, datadecompressedSize, of);
								fclose(of);
							}
							else
							{
								printf("Could not save the file %s\n", outfilename.c_str());
							}
						}
						else
						{
							printf("There was an error when decompressing %s file.\n", filename);
						}

						delete [] datauncompressedBuffer;
					}
					else if(packedType == 2)
					{
						unsigned long dataheader = *(unsigned long*)(epkBuffer + dwFileOffset);
						unsigned long datacryptedSize = *(unsigned long*)(epkBuffer + dwFileOffset + 4);
						unsigned long datacompressedSize = *(unsigned long*)(epkBuffer + dwFileOffset + 8);
						unsigned long datadecompressedSize = *(unsigned long*)(epkBuffer + dwFileOffset + 12);

						if(dataheader != FourCC)
						{
							printf("The FourCC is incorrect of %s.\n", filename);
							continue;
						}

						unsigned char* datadecryptedBuffer = new unsigned char[datacryptedSize];
						memset(datadecryptedBuffer, 0, datacryptedSize);
			
						XteaDecrypt((unsigned long*)datadecryptedBuffer, (unsigned long*)(epkBuffer + dwFileOffset + 16), DataXTEA, datacryptedSize);
						if(*(unsigned long*)datadecryptedBuffer != FourCC)
						{
							printf("There was an error decrypting the data for the %s file. It will be skipped.\n", filename);
							delete [] datadecryptedBuffer;
							continue;
						}

						unsigned char* datauncompressedBuffer = new unsigned char[datadecompressedSize];
						memset(datauncompressedBuffer, 0, datadecompressedSize);

						unsigned long finalSize = 0;
						if(lzo_decompress(datadecryptedBuffer + 4, datacompressedSize, datauncompressedBuffer, &finalSize) == LZO_E_OK && finalSize == datadecompressedSize)
						{
							std::stringstream dirPath;
							std::vector<std::string> pathTokens;
							size_t p0 = 0, p1 = std::string::npos;
							while(p0 != std::string::npos)
							{
								p1 = ((const std::string&)filename).find_first_of("\\/", p0);
								if(p1 != p0)
								{
									std::string token = ((const std::string&)filename).substr(p0, p1 - p0);
									pathTokens.push_back(token);
								}
								p0 = ((const std::string&)filename).find_first_not_of("\\/", p1);
							}
							dirPath << outfolder;
							mkfolder(dirPath.str().c_str());
							dirPath << "\\";
							size_t index = 0;
							for(index = 0; index < pathTokens.size() - 1; ++index)
							{
								if(pathTokens[index].find_first_of(":") != std::string::npos)
									continue;
								dirPath << pathTokens[index];
								mkfolder(dirPath.str().c_str());
								dirPath << "\\";
							}
							dirPath << pathTokens[index];
							outfilename = dirPath.str();
							FILE * of = fopen(outfilename.c_str(), "wb");
							if(!of)
							{
								std::string buff = outfolder + "\\crashfile" + (char)crashi;
								++crashi;
								of = fopen(buff.c_str(), "wb");
								printf("Crash file %s saved as: %s\n", outfilename.c_str(), buff.c_str());
								outfilename = buff;
							}
							if(of)
							{
								fwrite(datauncompressedBuffer, 1, datadecompressedSize, of);
								fclose(of);
							}
							else
							{
								printf("Could not save the file %s\n", outfilename.c_str());
							}
						}
						else
						{
							printf("There was an error when decompressing %s file.\n", filename);
						}

						delete [] datadecryptedBuffer;
						delete [] datauncompressedBuffer;
					}
					else
					{
						printf("Unsupported type: %i (%s).\n", packedType, filename);
						continue;
					}
					fprintf(of, "\t%s|%s|%i\n", filename, outfilename.c_str(), packedType);
				}
				fclose(of);
				printf("Done!\n");
			}
			else printf("dat file error!\n");
			if(!myfile.good()) break;
		}
		printf("All done!\n");
		myfile.close();
	}
	else printf("%s file not found!\n", argv[1]);

	pause();
	return 0;
}