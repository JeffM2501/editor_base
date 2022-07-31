#pragma once

#include "editor/utils/imgui_layout.h"
#include "imgui.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"


namespace ImGuiLayout
{
    ImVec2 GetButtonSize(const char* label)
    {
        return ImGui::CalcTextSize(label) + (ImGui::GetStyle().FramePadding + ImGui::GetStyle().FramePadding);
    }

    ImVec2 GetButtonsSize(const std::vector<const char*>& labels)
    {
        if (labels.empty())
            return ImVec2{ 0, ImGui::GetTextLineHeight() };

        ImVec2 size = GetButtonSize(labels[0]);

        for (size_t i = 1; i < labels.size(); i++)
        {
            size.x += GetButtonSize(labels[i]).x + ImGui::GetStyle().ItemSpacing.x;
        }

        return size;
    }

    void RightJustify(float width)
    {
        ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - width);
    }

    void BottomJustify(float height)
    {
        ImGui::SetCursorPosY(ImGui::GetWindowHeight() - (height + ImGui::GetStyle().WindowPadding.y));
    }
}