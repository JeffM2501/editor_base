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

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    bool draw = ImGui::Begin(GetWindowId(), &open, 0);
    ImGui::PopStyleVar(1);

    if (ImGui::IsWindowAppearing())
        EditorManager::ForceDockWindow(GetWindowId(), DockingNodes::Central);

    if (draw)
    {
        Focused = ImGui::IsWindowFocused();
            
        OnShow();
    }

    ImGui::End();

    if (!open)
        Closing = true;
}

void EditorDocument::Update()
{

}

void EditorDocument::Close()
{

}

bool EditorDocument::WantClose()
{
    return Closing;
}


bool EditorDocument::IsFocused()
{
    return Focused;
}

const char* EditorDocument::GetWindowTitle()
{
    return "DEFAULT";
}
