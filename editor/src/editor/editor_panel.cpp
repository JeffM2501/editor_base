#include "editor/editor_panel.h"
#include "editor/editor_document.h"

#include "imgui_internal.h"
#include "extras/IconsFontAwesome6.h"

void EditorPanel::Update(EditorDocument* document)
{
    CurrentDocument = document;

    OnUpdate();

    CurrentDocument = nullptr;
}

void EditorPanel::Show(EditorDocument* document)
{
    if (!IsOpen())
        return;

    CurrentDocument = document;

    bool open = true;
    const char* idStr = GetWindowId();
    bool draw = ImGui::Begin(idStr, &open, ImGuiWindowFlags_NoFocusOnAppearing);

    if (draw)
    {
        ImGui::BeginDisabled(!IsValid());
        OnShow();
        ImGui::EndDisabled();
    }
    ImGui::End();

    if (!open)
        Opened = false;

    CurrentDocument = nullptr;
}

const char* EditorPanel::GetWindowTitle()
{
    size_t id = EditorManager::GetWindowTypeIndex(this);
    if (id == 0)
        return WindowTitle.c_str();

    sprintf(NameBuffer, "%s:%zu", WindowTitle.c_str(), id);
    return NameBuffer;
}

