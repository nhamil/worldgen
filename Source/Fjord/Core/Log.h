#pragma once 

#include <cstdarg> 
#include <sstream> 
#include <string.h> 

// #ifdef _WIN32 
//     #define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
// #else 
//     #define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
// #endif 

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : ((strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)))

#define FJ_LOG(level, ...) Fjord::GetLogger()->Print(Fjord::LogLevel::level, __VA_ARGS__) 
#define FJ_FLOG(level, ...) Fjord::GetLogger()->FilePrint(Fjord::LogLevel::level, __FILENAME__, __LINE__, __VA_ARGS__) 
#define FJ_LOG_LEVEL(level) Fjord::GetLogger()->SetLogLevel(Fjord::LogLevel::level); 

#define FJ_FINE(...) FJ_LOG(Fine, __VA_ARGS__) 
#define FJ_DEBUG(...) FJ_LOG(Debug, __VA_ARGS__) 
#define FJ_INFO(...) FJ_LOG(Info, __VA_ARGS__) 
#define FJ_WARN(...) FJ_LOG(Warn, __VA_ARGS__) 
#define FJ_ERROR(...) FJ_LOG(Error, __VA_ARGS__) 
#define FJ_FATAL(...) FJ_LOG(Fatal, __VA_ARGS__) 

#define FJ_FFINE(...) FJ_FLOG(Fine, __VA_ARGS__) 
#define FJ_FDEBUG(...) FJ_FLOG(Debug, __VA_ARGS__) 
#define FJ_FINFO(...) FJ_FLOG(Info, __VA_ARGS__) 
#define FJ_FWARN(...) FJ_FLOG(Warn, __VA_ARGS__) 
#define FJ_FERROR(...) FJ_FLOG(Error, __VA_ARGS__) 
#define FJ_FFATAL(...) FJ_FLOG(Fatal, __VA_ARGS__) 

#define FJ_ELOG(level, ...) Fjord::GetEngineLogger()->Print(Fjord::LogLevel::level, __VA_ARGS__) 
#define FJ_EFLOG(level, ...) Fjord::GetEngineLogger()->FilePrint(Fjord::LogLevel::level, __FILENAME__, __LINE__, __VA_ARGS__) 
#define FJ_ELOG_LEVEL(level) Fjord::GetEngineLogger()->SetLogLevel(Fjord::LogLevel::level); 

#define FJ_EFINE(...) FJ_ELOG(Fine, __VA_ARGS__) 
#define FJ_EDEBUG(...) FJ_ELOG(Debug, __VA_ARGS__) 
#define FJ_EINFO(...) FJ_ELOG(Info, __VA_ARGS__) 
#define FJ_EWARN(...) FJ_ELOG(Warn, __VA_ARGS__) 
#define FJ_EERROR(...) FJ_ELOG(Error, __VA_ARGS__) 
#define FJ_EFATAL(...) FJ_ELOG(Fatal, __VA_ARGS__) 

#define FJ_EFFINE(...) FJ_EFLOG(Fine, __VA_ARGS__) 
#define FJ_EFDEBUG(...) FJ_EFLOG(Debug, __VA_ARGS__) 
#define FJ_EFINFO(...) FJ_EFLOG(Info, __VA_ARGS__) 
#define FJ_EFWARN(...) FJ_EFLOG(Warn, __VA_ARGS__) 
#define FJ_EFERROR(...) FJ_EFLOG(Error, __VA_ARGS__) 
#define FJ_EFFATAL(...) FJ_EFLOG(Fatal, __VA_ARGS__) 

namespace Fjord 
{

    class Logger; 

    Logger* GetLogger(); 
    Logger* GetEngineLogger(); 

    enum class LogLevel 
    {
        None, 
        Fatal, 
        Error, 
        Warn, 
        Info, 
        Debug, 
        Fine, 

        count 
    };

    inline const char* GetLogLevelTag(LogLevel level) 
    {
        switch (level) 
        {
            case LogLevel::Fatal: return "[FATAL]"; 
            case LogLevel::Error: return "[ERROR]"; 
            case LogLevel::Warn: return "[WARN ]"; 
            case LogLevel::Info: return "[INFO ]"; 
            case LogLevel::Debug: return "[DEBUG]"; 
            case LogLevel::Fine: return "[FINE ]"; 
            default: return ""; 
        }
    }

    class Logger 
    {
    public: 
        Logger(LogLevel level, const char* prefix = ""); 
        ~Logger(); 

        LogLevel GetLogLevel() { return MinLevel_; } 
        void SetLogLevel(LogLevel level) 
        {
            if ((unsigned) level >= (unsigned) LogLevel::count) level = LogLevel::Fine; 
            MinLevel_ = level; 
        }

        void Print(LogLevel level, const char* format, ...); 
        void FilePrint(LogLevel level, const char* file, int line, const char* format, ...); 

    private: 
        template <class First, class... Rest> 
        static void BuildString(std::stringstream& out, First first, Rest... rest); 
        static void BuildString(std::stringstream& out) { (void) out; } 

        LogLevel MinLevel_; 
        const char* Prefix_; 
    };

}

#include "Fjord/Common.h" 
