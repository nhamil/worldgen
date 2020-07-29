#include "Fjord/Core/Log.h" 

#include <stdio.h> 

namespace Fjord 
{

    Logger::Logger(LogLevel level, const char* prefix) 
        : MinLevel_(level)
        , Prefix_(prefix) {} 

    Logger::~Logger() {} 

    void Logger::Print(LogLevel level, const char* format, ...) 
    {
        if (level > MinLevel_) return; 

        va_list args; 
        va_start(args, format); 
        if (level != LogLevel::None && Prefix_) printf("%s", Prefix_); 
        printf("%s", GetLogLevelTag(level)); 
        if (level != LogLevel::None) printf(" "); 
        vprintf(format, args); 
        printf("\n"); 
        va_end(args); 
    }

    void Logger::FilePrint(LogLevel level, const char* file, int line, const char* format, ...) 
    {
        if (level > MinLevel_) return; 

        va_list args; 
        va_start(args, format); 
        if (level != LogLevel::None && Prefix_) printf("%s", Prefix_); 
        printf("%s[%s:%d] ", GetLogLevelTag(level), file, line); 
        vprintf(format, args); 
        printf("\n"); 
        va_end(args); 
    }

}