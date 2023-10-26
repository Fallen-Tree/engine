#include "logger.hpp"
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctime>

FILE* Logger::s_LoggingFile = stdout;
LogLevel Logger::s_LogLevel = INFO;

char buffer[32];

void Logger::GetTime() {
    struct timeval time_now {};
    gettimeofday(&time_now, nullptr);

    int milliseconds = time_now.tv_usec / 1000 % 1000;
    time_t now = time_now.tv_sec;
    tm tm_local;
    localtime_s(&tm_local, &now);

    snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d.%03d",
        tm_local.tm_hour, tm_local.tm_min, tm_local.tm_sec, milliseconds);
}

void Logger::RedirectToFile(const char* fileName) {
    if (Logger::s_LoggingFile != stdout) {
        fclose(Logger::s_LoggingFile);
    }
    Logger::s_LoggingFile = fopen(fileName, "w");
}

void Logger::SetLoggingLevel(LogLevel level) {
    Logger::s_LogLevel = level;
}

void Logger::Log(const char* logLevel, const char *format, va_list args) {
    GetTime();
    fprintf(Logger::s_LoggingFile, "%s | %s | ", buffer, logLevel);
    vfprintf(Logger::s_LoggingFile, format, args);
    fprintf(Logger::s_LoggingFile, "\n");
}

void Logger::Info(const char *format, ...) {
    if (Logger::s_LogLevel != INFO) return;
    va_list args;
    va_start(args, format);
    Logger::Log("INFO", format, args);
    va_end(args);
}

void Logger::Warn(const char *format, ...) {
    if (Logger::s_LogLevel == ERROR) return;
    va_list args;
    va_start(args, format);
    Logger::Log("WARNING", format, args);
    va_end(args);
}


void Logger::Error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    Logger::Log("ERROR", format, args);
    va_end(args);
}
