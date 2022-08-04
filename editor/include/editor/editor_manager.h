#pragma once

#include "editor/event_callbacks.h"

#include "imgui.h"
#include "raylib.h"

#include <vector>
#include <memory>
#include <functional>

class EditorPanel;
class EditorDocument;
class EditorWindow;
class EditorDialog;

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

    template <class T>
    inline T* AddDocument()
    {
        T* document = new T();
        AddDocument(document);
        return document;
    }

    template <class T, typename ...Args>
    inline T* AddDocument(Args&&... args)
    {
        T* document = new T(std::forward<Args>(args)...);
        AddDocument(document);
        return document;
    }

    EditorDocument* GetActiveDocument();
    void SetActiveDocument(EditorDocument* document);

    void AddPanel(EditorPanel* panel);

    template <class T>
    inline T* AddPanel()
    {
        T* panel = new T();
        AddPanel(panel);
        return panel;
    }

    template <class T, typename ...Args>
    inline T* AddPanel(Args&&... args)
    {
        T* panel = new T(std::forward<Args>(args)...);
        AddPanel(panel);
        return panel;
    }

    void AddDialogBox(EditorDialog* dialogBox);

    template <class T>
    inline T* AddDialogBox()
    {
        T* dialog = new T();
        AddDialogBox(dialog);
        return dialog;
    }

    template <class T, typename ...Args>
    inline T* AddDialogBox(Args&&... args)
    {
        T* dialog = new T(std::forward<Args>(args)...);
        AddDialogBox(dialog);
        return dialog;
    }

    ImGuiID GetDockspaceId(DockingNodes node);

    void ForceDockWindow(const char* name, DockingNodes node);

    std::vector<EditorWindow*> GetWindowsOfType(size_t typeId);
    size_t GetWindowTypeIndex(EditorWindow* window);

    Vector2 GetWindowMousePostion();

    using DocumentCallback = std::function<void(EditorDocument*)>;

    extern EventHandler<DocumentCallback> OnDocumentChanged;
}