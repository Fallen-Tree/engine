
#ifndef SRC_LOGGING_LOGGER_HPP_
#define SRC_LOGGING_LOGGER_HPP_

#include<string.h>
#include<cstdio>

enum LogLevel {
  INFO,
  WARN,
  ERROR
};

class Logger {
 private:
    static FILE* s_LoggingFile;
    static LogLevel s_LogLevel;
    static void Log(const char* logLevel, const char *format, va_list args);
    static void GetTime(char* buffer, size_t bufsiz);

 public:
    static void RedirectToFile(const char* fileName);
    static void SetLoggingLevel(LogLevel level);
    static void Info(const char *format, ...);
    static void Warn(const char *format, ...);
    static void Error(const char *format, ...);
};

#endif  // SRC_LOGGING_LOGGER_HPP_
