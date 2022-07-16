#pragma once

#include <string>

class EditorWindow
{
public:
    virtual ~EditorWindow() = default;

    inline virtual size_t GetTypeId() { return reinterpret_cast<size_t>("EditorWindow"); }

    virtual const char* GetWindowId();
    inline virtual const char* GetWindowTitle() { return ""; }
    inline virtual const char* GetWindowUID() { return "EditorWindow"; }
    inline virtual bool IsA(size_t otherId) { return false; }

    virtual void Create() {}

protected:
    virtual const char* GetWindowUIDSuffix();
    std::string WindowId;

    char NameBuffer[64] = { 0 };
};