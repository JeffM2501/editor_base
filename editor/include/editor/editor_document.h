#pragma once

#include "editor/conent_render.h"
#include "editor/editor_panel.h"
#include "editor/editor_window.h"

#include "imgui.h"

#include <string>
#include <map>

class EditorDocumentPanelData
{
public:
    size_t PanelId = 0;
    virtual ~EditorDocumentPanelData() = default;
};

#define DECLARE_DOCUMENT(TYPE, PARENT) \
inline size_t GetTypeId() override { return reinterpret_cast<size_t>(#TYPE);}  \
static size_t TypeId() { return reinterpret_cast<size_t>(#TYPE);} \
static bool IsTypeId(size_t otherId) { if (otherId == TYPE::TypeId()) return true; return PARENT::IsTypeId(otherId); } \
virtual inline bool IsA(size_t otherId) override { return TYPE::IsTypeId(otherId);}  \
template<class T> \
inline bool IsA(size_t otherId) { return TYPE::IsTypeId(T::TypeId()); } \
inline const char* GetWindowUID() override { return #TYPE; } 

class EditorDocument : public EditorWindow
{
public:
    EditorDocument();
    virtual ~EditorDocument();

    inline size_t GetTypeId() override { return reinterpret_cast<size_t>("EditorDocument"); }
    static size_t TypeId() { return reinterpret_cast<size_t>("EditorDocument"); } 
    static bool IsTypeId(size_t otherId) { if (otherId == EditorDocument::TypeId()) return true; return false; }
    inline bool IsA(size_t otherId) override { return EditorDocument::IsTypeId(otherId);}
    template<class T> 
    inline bool IsA(size_t otherId) { return EditorDocument::IsTypeId(T::TypeId()); }

    template<class T>
    inline T* GetAs()
    {
        if (IsA(T::TypeId()))
            return reinterpret_cast<T*>(this);

        return nullptr;
    }

    const char* GetWindowTitle() override;
    inline const char* GetWindowUID() override { return "EditorDocument"; }

    void Create() override;
    void Show();
    virtual void Update();
    virtual void Close();

    bool WantClose();

    template <class T>
    inline T* GetPanelData(size_t panelId)
    {
        auto itr = PanelData.find(panelId);
        if (itr == PanelData.end())
            return reinterpret_cast<T*>(itr->second);

        PanelData[panelId] = new T();
        return PanelData[panelId];
    }

    template <class T>
    inline T* GetPanelData(EditorPanel* panel)
    {
        return GetPanelData<T>(panel->GetTypeId());
    }

    bool IsFocused() const;
    inline void ForceFocus() { ForceNextFocus = true; }

    inline bool IsHovered() const { return Hovered; }

    inline bool IsMouseDownIn(MouseButton button) const{ return IsHovered() && IsMouseButtonDown(button); }

protected:
    virtual void OnCreate() {};
    virtual void OnShow() {};
    virtual void OnUpdate() {};
    virtual void OnClose() {};

protected:
    bool Closing = false;
    bool Focused = false;
    bool ForceNextFocus = false;
    ImGuiID DockspaceId = 0;

    bool Hovered = false;

    std::map<size_t, EditorDocumentPanelData*> PanelData;
};
