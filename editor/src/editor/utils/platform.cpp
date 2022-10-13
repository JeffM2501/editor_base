#include "editor/utils/platform.h"

#ifdef _WIN32
#include <windows.h>
#include <shlobj_core.h>
#include <direct.h>
#else

#endif


namespace PlatformUtils
{
#ifdef _WIN32
    std::string UserPrefsDir;

    const std::string& GetUserPrefsDir(const char* appName)
    {
        if (!UserPrefsDir.empty())
            return UserPrefsDir;

        TCHAR appdata[MAX_PATH] = { 0 };
        SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, appdata);

        UserPrefsDir = appdata;

        UserPrefsDir += "\\";
        if (appName != nullptr)
        {
            UserPrefsDir += appName;
            UserPrefsDir += "\\";
        }

        return UserPrefsDir;
    }

    void CreateDirecory(const char* path)
    {
        mkdir(path);
    }

#else
    std::string UserPrefsDir;
    const std::string& GetUserPrefsDir(const char*) { return UserPrefsDir; }

    const std::string& CreateDirecory(const char*) { }
#endif
}