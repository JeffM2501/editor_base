#pragma once

#include "editor/editor_panel.h"
#include <deque>

namespace LogSink
{
    struct LogItem
    {
        int Level = 0;
        std::string Prefix;
        std::string Text;
        ImVec4 Color = { 1,1,1,1 };
    };

    void Setup();
    bool PopLogLine(LogItem& line);
    void Flush();

    inline const char* GetLogLevelName(int logLevel)
    {
        switch (logLevel)
        {
        default:            return "All";
        case LOG_TRACE:     return "Trace";
        case LOG_DEBUG:     return "DEBUG";
        case LOG_INFO:      return "Info";
        case LOG_WARNING:   return "Warning";
        case LOG_ERROR:     return "ERROR";
        case LOG_FATAL:     return "FATAL";
        }
    }
}

class LogPanel : public EditorPanel
{
public:
    DECLARE_PANEL(LogPanel, EditorPanel)

    LogPanel();

    inline DockingNodes GetDefaultDockingNode() override { return DockingNodes::Bottom; }

protected:
    void OnShow() override;

    std::deque<LogSink::LogItem> LogLines;

    int ShowLevel = 0;

    char FilterText[512] = { 0 };
};