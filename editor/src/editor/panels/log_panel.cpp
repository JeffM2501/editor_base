#pragma once

#include "editor/panels/log_panel.h"
#include "editor/utils/string_utils.h"

#include "raylib.h"

#include <vadefs.h>

namespace ImGuiColors
{
    inline ImVec4 Convert(::Color color)
    {
        return ImVec4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
    }
}


namespace LogSink
{
    std::deque<LogItem> LogLines;

    void GetLogLevelPrefix(int logLevel, LogItem& item)
    {
        item.Prefix = GetLogLevelName(logLevel);
        item.Prefix += ": ";
        switch (logLevel)
        {
        default:            item.Prefix.clear(); item.Color = ImGuiColors::Convert(WHITE); break;
        case LOG_TRACE:     item.Color = ImGuiColors::Convert(GRAY); break;
        case LOG_DEBUG:     item.Color = ImGuiColors::Convert(SKYBLUE); break;
        case LOG_INFO:      item.Color = ImGuiColors::Convert(GREEN); break;
        case LOG_WARNING:   item.Color = ImGuiColors::Convert(YELLOW); break;
        case LOG_ERROR:     item.Color = ImGuiColors::Convert(ORANGE); break;
        case LOG_FATAL:     item.Color = ImGuiColors::Convert(RED); break;
        }
    }

    void TraceLog(int logLevel, const char* text, va_list args)
    {
        static char logText[2048] = { 0 };
        LogItem item;
        item.Level = logLevel;
        GetLogLevelPrefix(logLevel, item);
        vsprintf(logText, text, args);
        item.Text += logText;
        LogLines.push_back(item);
    }

    void Setup()
    {
        SetTraceLogCallback(TraceLog);
    }

    bool PopLogLine(LogItem& line)
    {
        if (LogLines.size() == 0)
            return false;

        line = LogLines.front();
        LogLines.pop_front();
        return true;
    }

    void Flush()
    {
        LogLines.clear();
    }
}

LogPanel::LogPanel()
{
    WindowTitle = "Log";
    LogSink::Setup();
}

void LogPanel::OnShow()
{
    ImGui::TextUnformatted("Show:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(150);
    if (ImGui::BeginCombo("##LogLevel", LogSink::GetLogLevelName(ShowLevel)))
    {
        for (int i = 0; i < LOG_NONE; ++i)
        {
            bool is_selected = i == ShowLevel;
            if (ImGui::Selectable(LogSink::GetLogLevelName(i), is_selected))
                ShowLevel = i;

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(200);
    ImGui::InputTextWithHint("###filterText", "Filter", FilterText, 512);

    ImGui::SameLine();
    bool copy = false;
    if (ImGui::Button("Copy"))
    {
        copy = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("Clear"))
    {
        LogSink::Flush();
        LogLines.clear();
    }

    if (ImGui::BeginChild("###LogChild", ImGui::GetContentRegionAvail()))
    {
        bool scroollBottom = false;

        int count = 0;
        LogSink::LogItem item;
        while (LogSink::PopLogLine(item)/* && count <= 10*/)
        {
            LogLines.emplace_back(std::move(item));

            while (LogLines.size() > 50)
                LogLines.pop_front();

            scroollBottom = true;
            count++;
        }

        std::string copyBuffer;

        for (auto& line : LogLines)
        {
            if (ShowLevel != 0 && ShowLevel != line.Level)
                continue;

            if (FilterText[0] != '\0')
            {
                if (StringUtils::stristr(line.Text.c_str(), FilterText) == nullptr)
                    continue;
            }

            ImGui::TextColored(line.Color, "%s", line.Prefix.c_str());
            ImGui::SameLine();
            ImGui::TextUnformatted(line.Text.c_str());

            if (copy)
                copyBuffer += line.Prefix + line.Text + "\r\n";
        }

        if (copy)
        {
            SetClipboardText(copyBuffer.c_str());
        }

        if (scroollBottom)
            ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();
}
