#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;

class IniReader
{
public:
 IniReader(char* szFileName); 
 int ReadInteger(char* szSection, char* szKey, int iDefaultValue);
 float ReadFloat(char* szSection, char* szKey, float fltDefaultValue);
 bool ReadBoolean(char* szSection, char* szKey, bool bolDefaultValue);
 char* ReadString(char* szSection, char* szKey, const char* szDefaultValue);
 bool CheckFile();
private:
  char m_szFileName[255];
};