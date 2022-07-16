#pragma once

#include "imgui.h"

#include <vector>

class EditorPanel;
class EditorDocument;
class EditorWindow;

enum class DockingNodes
{
    Left,
    Right,
    Bottom,
    Central
};

namespace EditorManager
{
    void Setup();
    void Shutdown();

    void Update();
    void Render();

    bool WantQuit();
    void Quit();

    bool WantMouse();

    void AddDocument(EditorDocument* document);

    void AddPanel(EditorPanel* panel);

    template <class T>
    inline T* AddPanel()
    {
        T* panel = new T();
        AddPanel(panel);
        return panel;
    }

    ImGuiID GetDockspaceId(DockingNodes node);

    void ForceDockWindow(const char* name, DockingNodes node);

    std::vector<EditorWindow*> GetWindowsOfType(size_t typeId);
    size_t GetWindowTypeIndex(EditorWindow* window);
}