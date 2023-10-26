#include "logger.hpp"
#include <stdio.h>
#include <stdarg.h>
#include <ctime>
#include <chrono>

FILE* Logger::s_LoggingFile = stdout;
LogLevel Logger::s_LogLevel = INFO;

char buffer[32];

void Logger::GetTime() {
    const std::chrono::time_point<std::chrono::system_clock> nowh =
        std::chrono::system_clock::now();

    const std::time_t now = std::chrono::system_clock::to_time_t(nowh);

    auto milliseconds = nowh.time_since_epoch() / std::chrono::milliseconds(1) % 1000;
    tm* tm_local = localtime(&now);

    snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d.%03d",
        tm_local->tm_hour, tm_local->tm_min, tm_local->tm_sec, milliseconds);
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
