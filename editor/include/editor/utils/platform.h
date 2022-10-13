#pragma once

#include <string>

namespace PlatformUtils
{
    const std::string& GetUserPrefsDir(const char* appName);

    void CreateDirecory(const char* path);
}