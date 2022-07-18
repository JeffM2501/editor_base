#include "editor/editor_panel.h"
#include "editor/editor_document.h"

#include "imgui_internal.h"
#include "extras/IconsFontAwesome6.h"

void EditorPanel::Update(EditorDocument* document)
{
    OnUpdate();
}

void EditorPanel::Show(EditorDocument* document)
{
    if (!IsOpen())
        return;

    bool open = true;
    const char* idStr = GetWindowId();
    bool draw = ImGui::Begin(idStr, &open, ImGuiWindowFlags_NoFocusOnAppearing);

    if (draw)
    {
        OnShow();
    }
    ImGui::End();

    if (!open)
        Opened = false;
}

const char* EditorPanel::GetWindowTitle()
{
    size_t id = EditorManager::GetWindowTypeIndex(this) + 1;
    sprintf(NameBuffer, "%s  Panel:%zu", ICON_FA_FACE_ANGRY, id);
    return NameBuffer;
}

