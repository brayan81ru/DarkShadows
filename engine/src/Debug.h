#pragma once
#include "DSString.h"
#include <vector>
#include <mutex>
#include <fstream>
#include <chrono>
#include <iomanip>
namespace DSEngine {
    class Debug {
    public:
        enum class LogLevel {
            Info,
            Warning,
            Error
        };

        // Core logging interface
        static void Log(const DSString& message);
        static void LogWarning(const DSString& message);
        static void LogError(const DSString& message);

        // Configuration
        static void SetMinimumLogLevel(LogLevel level);
        static void EnableFileOutput(const DSString& filePath);
        static void DisableFileOutput();
        static void EnableConsoleOutput(bool enable);

        // Advanced features
        static void AddCustomOutput(void (*callback)(const DSString&, LogLevel));
        static void ClearCustomOutputs();

        // Tag system for filtering
        static void LogWithTag(const DSString& tag, const DSString& message);
        static void AddAllowedTag(const DSString& tag);
        static void ClearAllowedTags();

    private:
        static LogLevel s_minLevel;
        static std::ofstream s_logFile;
        static bool s_consoleOutput;
        static std::mutex s_logMutex;
        static std::vector<void (*)(const DSString&, LogLevel)> s_callbacks;
        static std::vector<DSString> s_allowedTags;

        static void InternalLog(LogLevel level, const DSString& message);
        static DSString FormatLogMessage(LogLevel level, const DSString& message);
        static DSString GetCurrentTimeStamp();
    };
}