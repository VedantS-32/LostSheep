#include "Log.h"

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#define GREEN_STRING(string) "\x1b[92m" string "\x1b[0m"
#define YELLOW_STRING(string) "\x1b[93m" string "\x1b[0m"
#define BRIGHT_RED_STRING(string) "\x1b[91m" string "\x1b[0m"
#define RED_STRING(string) "\x1b[31m" string "\x1b[0m"

static const char* GetCurrentTimeString()
{
	time_t now = time(NULL);
	struct tm* t = localtime(&now);
	static char timeString[9];
	strftime(timeString, sizeof(timeString), "%H:%M:%S", t);
	return timeString;
}

void LogTrace(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	printf("[%s] LostSheep: ", GetCurrentTimeString());
	vprintf(fmt, args);
	va_end(args);
	printf("\n");
}

void LogInfo(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	printf(GREEN_STRING("[%s] LostSheep: "), GetCurrentTimeString());

	char formattedMessage[1024]; 
	vsnprintf(formattedMessage, sizeof(formattedMessage), fmt, args);
	printf(GREEN_STRING("%s\n"), formattedMessage);

	va_end(args);
}

void LogWarn(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	printf(YELLOW_STRING("[%s] LostSheep: "), GetCurrentTimeString());

	char formattedMessage[1024]; 
	vsnprintf(formattedMessage, sizeof(formattedMessage), fmt, args);
	printf(YELLOW_STRING("%s\n"), formattedMessage);

	va_end(args);
	printf("\n");
}

void LogError(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	printf(RED_STRING("[%s] LostSheep: "), GetCurrentTimeString());

	char formattedMessage[1024]; 
	vsnprintf(formattedMessage, sizeof(formattedMessage), fmt, args);
	printf(RED_STRING("%s\n"), formattedMessage);

	va_end(args);
	printf("\n");
}

void LogFatal(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	printf(RED_STRING("[%s] FATAL: "), GetCurrentTimeString());

	char formattedMessage[1024]; 
	vsnprintf(formattedMessage, sizeof(formattedMessage), fmt, args);
	printf(RED_STRING("%s\n"), formattedMessage);

	va_end(args);
	printf("\n");
}
