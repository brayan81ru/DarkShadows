#pragma once
#include "DSString.h"
#include <vector>
#include <mutex>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif

namespace DSEngine {
    class Debug {
        // Add this enum class above the Debug class
        enum class ConsoleColor {
            Reset = 0,
            Black = 30,
            Red,
            Green,
            Yellow,
            Blue,
            Magenta,
            Cyan,
            White,
            BrightBlack = 90,
            BrightRed,
            BrightGreen,
            BrightYellow,
            BrightBlue,
            BrightMagenta,
            BrightCyan,
            BrightWhite
        };


    public:
        static void Log(const DSString& message);
        // Add these new methods to the Debug class
        static void SetConsoleColor(ConsoleColor color);
        static void ResetConsoleColor();
    private:
        static std::mutex s_logMutex;
        static void BaseLog(const char* message);
    };
}