#ifndef CONSOLE_MANAGER_H
#define CONSOLE_MANAGER_H

#include <string>

class ConsoleManager {
public:
    static void clearScreen();
    static void printHeader();
    static void cmdScreenList();
    static void attachToProcess(const std::string &processName);
};

#endif