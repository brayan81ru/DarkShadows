#pragma once
#include "Debug.h"

namespace DSEngine {
    std::mutex Debug::s_logMutex;

    void Debug::BaseLog(const char* message) {
        std::lock_guard<std::mutex> lock(s_logMutex);
        if(message) {
            SetConsoleColor(ConsoleColor::Cyan);
            std::cout << message << std::endl;
        }
        ResetConsoleColor();
    }

    void Debug::Log(DSString message) {
        BaseLog(message.c_str());
    }

    void Debug::SetConsoleColor(ConsoleColor color) {
        #ifdef _WIN32
        HANDLE hConsole = GetStdHandle((color >= ConsoleColor::BrightBlack) ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE);
        WORD winColor = 0;
        switch (color) {
            case ConsoleColor::Black: winColor = 0; break;
            case ConsoleColor::Red: winColor = FOREGROUND_RED; break;
            case ConsoleColor::Green: winColor = FOREGROUND_GREEN; break;
            case ConsoleColor::Yellow: winColor = FOREGROUND_RED | FOREGROUND_GREEN; break;
            case ConsoleColor::Blue: winColor = FOREGROUND_BLUE; break;
            case ConsoleColor::Magenta: winColor = FOREGROUND_RED | FOREGROUND_BLUE; break;
            case ConsoleColor::Cyan: winColor = FOREGROUND_GREEN | FOREGROUND_BLUE; break;
            case ConsoleColor::White: winColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; break;
            case ConsoleColor::BrightBlack: winColor = 0 | FOREGROUND_INTENSITY; break;
            case ConsoleColor::BrightRed: winColor = FOREGROUND_RED | FOREGROUND_INTENSITY; break;
            case ConsoleColor::BrightGreen: winColor = FOREGROUND_GREEN | FOREGROUND_INTENSITY; break;
            case ConsoleColor::BrightYellow: winColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY; break;
            case ConsoleColor::BrightBlue: winColor = FOREGROUND_BLUE | FOREGROUND_INTENSITY; break;
            case ConsoleColor::BrightMagenta: winColor = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY; break;
            case ConsoleColor::BrightCyan: winColor = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY; break;
            case ConsoleColor::BrightWhite: winColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY; break;
            default: winColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; break;
        }
        SetConsoleTextAttribute(hConsole, winColor);
        #else
        std::ostream& stream = (color >= ConsoleColor::BrightBlack) ? std::cout : std::cerr;
        stream << "\033[" << static_cast<int>(color) << "m";
        #endif
    }

    void Debug::ResetConsoleColor() {
        #ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        #else
        std::cout << "\033[0m";
        #endif
    }
}