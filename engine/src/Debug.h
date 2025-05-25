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
    /**
     * The Debug class is the responsible to show information on the output console.
     */
    class Debug {

        /**
         * Colors used to change the console output message colors.
         */
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
        /**
         * Shows an information message on the output console.
         *
         * @param message The message text to show.
         */
        static void Log(const DSString& message);

        /**
         * Shows an error message on the output console.
         *
         * @param message The message text to show.
         */
        static void LogError(const DSString& message);

        /**
         * Shows a warning message on the output console.
         *
         * @param message The message text to show.
         */
        static void LogWarning(const DSString& message);

    private:
        static std::mutex s_logMutex;

        static void SetConsoleColor(ConsoleColor color);

        static void ResetConsoleColor();

        static void BaseLog(const char* message);

    };
}