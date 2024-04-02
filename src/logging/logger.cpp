#include "logger.hpp"
#include <stdio.h>
#include <stdarg.h>
#include <ctime>
#include <chrono>

FILE* Logger::s_LoggingFile = stdout;
LogLevel Logger::s_LogLevel = LogLevel::INFO;

void Logger::GetTime(char* buffer, size_t bufsiz) {
    const std::chrono::time_point<std::chrono::system_clock> now =
        std::chrono::system_clock::now();

    int64_t milliseconds = now.time_since_epoch() / std::chrono::milliseconds(1);
    int64_t seconds = milliseconds / 1000;
    int64_t minutes = seconds / 60;
    int64_t hours = minutes / 60;

    snprintf(buffer, bufsiz, "%02d:%02d:%02d.%03d",
        hours % 24 , minutes % 60, seconds % 60, milliseconds % 1000);
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
    char buffer[32];
    GetTime(buffer, sizeof(buffer));
    fprintf(Logger::s_LoggingFile, "%s | %s | ", buffer, logLevel);
    vfprintf(Logger::s_LoggingFile, format, args);
    fprintf(Logger::s_LoggingFile, "\n");
}

void Logger::Info(const char *format, ...) {
    if (Logger::s_LogLevel != LogLevel::INFO) return;
    va_list args;
    va_start(args, format);
    Logger::Log("INFO", format, args);
    va_end(args);
}

void Logger::Warn(const char *format, ...) {
    if (Logger::s_LogLevel == LogLevel::ERROR) return;
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
