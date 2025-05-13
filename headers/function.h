#ifndef FUNCTION_H
#define FUNCTION_H

#include <windows.h>
#include <iostream>
#include <string>

//clear screen
void ClearConsole(const bool &showart);

//Show ascii art
void showArt();

//show performance of the device
void showTaskPerf();

// Base case for variadic template recursion
inline void printColoredImpl(HANDLE hConsole, WORD saved_attributes) {
    std::cout << std::endl; // Add newline to terminate output
}

// Variadic template function to print arguments
template <typename T, typename... Args>
void printColoredImpl(HANDLE hConsole, WORD saved_attributes, const T& value, Args... args) {
    std::cout << value << " ";
    printColoredImpl(hConsole, saved_attributes, args...); // Recurse for remaining arguments
}

template <typename... Args>
void setconsolecolor(int colorV, Args... args) {
    // Handle to console
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Save the current attribute
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    WORD saved_attributes = consoleInfo.wAttributes;

    // Set new color
    switch (colorV) {
        case 0:
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
            printColoredImpl(hConsole, saved_attributes, args...);
            break;
        case 1:
            SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
            printColoredImpl(hConsole, saved_attributes, args...);
            break;
        case 2:
            SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
            printColoredImpl(hConsole, saved_attributes, args...);
            break;
        default:
            SetConsoleTextAttribute(hConsole, saved_attributes);
            printColoredImpl(hConsole, saved_attributes, args...);
            break;
    }

    // Restore original color
    SetConsoleTextAttribute(hConsole, saved_attributes);
    //CloseHandle(hConsole);
}

//commands prototypes
void installSpicetify();
std::string lowercase(std::string& s);
bool enablebluetooth();
bool disablebluetooth();

#endif