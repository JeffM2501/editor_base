#include "editor/editor_document.h"
#include "editor/editor_manager.h"

#include "imgui.h"
#include "imgui_internal.h"

EditorDocument::EditorDocument()
{
}

EditorDocument::~EditorDocument()
{
    for (const std::pair<const size_t, EditorDocumentPanelData*> pData : PanelData)
    {
        delete(pData.second);
    }
}

void EditorDocument::Create()
{
    OnCreate();
}

void EditorDocument::Show()
{
    // main window tab
    bool open = true;
    Hovered = false;

    if (ForceNextFocus)
        ImGui::SetNextWindowFocus();
    ForceNextFocus = false;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    bool draw = ImGui::Begin(GetWindowId(), &open, 0);
    ImGui::PopStyleVar(1);

    if (ImGui::IsWindowAppearing())
        EditorManager::ForceDockWindow(GetWindowId(), DockingNodes::Central);

    if (draw)
    {
        Hovered = ImGui::IsWindowHovered();
        Focused = ImGui::IsWindowFocused(ImGuiHoveredFlags_RootWindow) || (Hovered && ImGui::IsAnyMouseDown());
            
        OnShow();
    }

    ImGui::End();

    if (!open)
        Closing = true;
}

void EditorDocument::Update()
{
    OnUpdate();
}

void EditorDocument::Close()
{

}

bool EditorDocument::WantClose()
{
    return Closing;
}


bool EditorDocument::IsFocused() const
{
    return Focused;
}

const char* EditorDocument::GetWindowTitle()
{
    return "DEFAULT";
}
