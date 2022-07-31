#pragma once

#include <vector>
#include "imgui.h"

namespace ImGuiLayout
{
    ImVec2 GetButtonSize(const char* label);
    ImVec2 GetButtonsSize(const std::vector<const char*>& labels); 

    void RightJustify(float width);
    void BottomJustify(float height);
}