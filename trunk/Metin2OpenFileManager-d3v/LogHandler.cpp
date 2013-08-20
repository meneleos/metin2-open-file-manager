#include "LogHandler.h"

void LogHandler::PrintMessage(char Message[],bool IsError)
{
	if (!(Out = fopen("OFMLogs.txt","w+")))
	{
		printf("[ERROR] Unable to write to the log file.\n");
		_getch();
		exit((int)Out);
	}

	char FinalMessage[500];
	memset(FinalMessage,0,sizeof(FinalMessage));

	if (!IsError)
		sprintf(FinalMessage,"[INFO] %s",Message);
	else
		sprintf(FinalMessage,"[ERROR] %s",Message);

	fwrite(Message,sizeof(char),strlen(Message),Out);
	fclose(Out);
}