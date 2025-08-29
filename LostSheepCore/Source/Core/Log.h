#pragma once

void LogTrace(const char* fmt, ...);
void LogInfo(const char* fmt, ...);
void LogWarn(const char* fmt, ...);
void LogError(const char* fmt, ...);
void LogFatal(const char* fmt, ...);

#define LSH_TRACE(...)	LogTrace(__VA_ARGS__)
#define LSH_INFO(...)	LogInfo(__VA_ARGS__)
#define LSH_WARN(...)	LogWarn(__VA_ARGS__)
#define LSH_ERROR(...)	LogError(__VA_ARGS__)
#define LSH_FATAL(...)	LogFatal(__VA_ARGS__)