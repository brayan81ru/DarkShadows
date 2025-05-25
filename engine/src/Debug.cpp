#include "Debug.h"
#include <iostream>
namespace DSEnegine{
// Static member initialization
Debug::LogLevel Debug::s_minLevel = Debug::LogLevel::Info;
std::ofstream Debug::s_logFile;
bool Debug::s_consoleOutput = true;
std::mutex Debug::s_logMutex;
std::vector<void (*)(const DSString&, Debug::LogLevel)> Debug::s_callbacks;
std::vector<DSString> Debug::s_allowedTags;

void Debug::Log(const DSString& message) {
    InternalLog(LogLevel::Info, message);
}

void Debug::LogWarning(const DSString& message) {
    InternalLog(LogLevel::Warning, message);
}

void Debug::LogError(const DSString& message) {
    InternalLog(LogLevel::Error, message);
}

void Debug::LogWithTag(const DSString& tag, const DSString& message) {
    if (!s_allowedTags.empty()) {
        bool tagAllowed = false;
        for (const auto& allowedTag : s_allowedTags) {
            if (allowedTag == tag) {
                tagAllowed = true;
                break;
            }
        }
        if (!tagAllowed) return;
    }
    
    InternalLog(LogLevel::Info, DSString::Format("[%D] %D", &tag, &message));
}

void Debug::SetMinimumLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(s_logMutex);
    s_minLevel = level;
}

void Debug::EnableFileOutput(const DSString& filePath) {
    std::lock_guard<std::mutex> lock(s_logMutex);
    if (s_logFile.is_open()) {
        s_logFile.close();
    }
    s_logFile.open(filePath.c_str(), std::ios::out | std::ios::app);
}

void Debug::DisableFileOutput() {
    std::lock_guard<std::mutex> lock(s_logMutex);
    if (s_logFile.is_open()) {
        s_logFile.close();
    }
}

void Debug::EnableConsoleOutput(bool enable) {
    std::lock_guard<std::mutex> lock(s_logMutex);
    s_consoleOutput = enable;
}

void Debug::AddCustomOutput(void (*callback)(const DSString&, LogLevel)) {
    std::lock_guard<std::mutex> lock(s_logMutex);
    s_callbacks.push_back(callback);
}

void Debug::ClearCustomOutputs() {
    std::lock_guard<std::mutex> lock(s_logMutex);
    s_callbacks.clear();
}

void Debug::AddAllowedTag(const DSString& tag) {
    std::lock_guard<std::mutex> lock(s_logMutex);
    s_allowedTags.push_back(tag);
}

void Debug::ClearAllowedTags() {
    std::lock_guard<std::mutex> lock(s_logMutex);
    s_allowedTags.clear();
}

// Private implementation
void Debug::InternalLog(LogLevel level, const DSString& message) {
    if (level < s_minLevel) return;
    
    std::lock_guard<std::mutex> lock(s_logMutex);
    DSString formatted = FormatLogMessage(level, message);
    
    if (s_consoleOutput) {
        std::ostream& stream = (level == LogLevel::Error) ? std::cerr : std::cout;
        stream << formatted.c_str() << std::endl;
    }
    
    if (s_logFile.is_open()) {
        s_logFile << formatted.c_str() << std::endl;
        s_logFile.flush();
    }
    
    for (auto callback : s_callbacks) {
        callback(formatted, level);
    }
}

DSString Debug::FormatLogMessage(LogLevel level, const DSString& message) {
    const char* levelStr = "";
    switch (level) {
        case LogLevel::Info: levelStr = "INFO"; break;
        case LogLevel::Warning: levelStr = "WARN"; break;
        case LogLevel::Error: levelStr = "ERROR"; break;
    }
    
    return DSString::Format("[%s][%D] %D", levelStr, &GetCurrentTimeStamp(), &message);
}

DSString Debug::GetCurrentTimeStamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    struct tm timeInfo;
    localtime_s(&timeInfo, &in_time_t);
    
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeInfo);
    
    return DSString::Format("%s.%03d", buffer, static_cast<int>(milliseconds.count()));
}
}