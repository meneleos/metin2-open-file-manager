#pragma once
#include <iostream>
#include <conio.h>

class LogHandler
{
private:
	static FILE *Out;
public:
	static void PrintMessage(char Message[],bool IsError);
};