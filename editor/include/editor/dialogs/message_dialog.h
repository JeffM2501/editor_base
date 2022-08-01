#pragma once

#include "editor/editor_dialog.h"
#include "editor/utils/imgui_layout.h"

#include <string>

#include "imgui.h"

class MessageDialog : public EditorDialog
{
public:
    MessageDialog(const char* title, const char* text)
    {
        if (title != nullptr)
            WindowTile = title;
        else
            WindowTile = "Message";

        if (text != nullptr)
            MessageText = text;
    }

    inline static MessageDialog* Show(const char* title, const char* text)
    {
        return EditorManager::AddDialogBox<MessageDialog>(title, text);
    }

protected:
    inline void OnCreate() override
    {
        EditorDialog::OnCreate();
        ImVec2 size = ImGui::CalcTextSize(MessageText.c_str());
        size.y += ImGuiLayout::GetButtonSize("X").y;

        MinimumSize.x = std::max(size.x, MinimumSize.x);
        MinimumSize.y = std::max(size.y, MinimumSize.y);
    }

    inline void OnShowContent() override 
    {
        ImGui::TextUnformatted(MessageText.c_str());
        EditorDialog::OnShowContent();
    }

    std::string MessageText;
};