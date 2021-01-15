#include <iostream>
#include "Log.h"

void Log::LogInfo(const char* message)
{
    std::cout << message << std::endl;
}

void Log::LogInfo(const std::string& message)
{
    std::cout << message << std::endl;
}

void Log::LogError(const char* message)
{
    std::cout << "ERROR: " << message << std::endl;
}

void Log::LogError(const std::string& message)
{
    std::cout << message << std::endl;
}
