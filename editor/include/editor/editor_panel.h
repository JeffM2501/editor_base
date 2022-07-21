#pragma once

#include "editor/editor_manager.h"
#include "editor/editor_window.h"

#include <string>

class EditorDocument;

#define DECLARE_PANEL(TYPE, PARENT) \
inline size_t GetTypeId() override { return reinterpret_cast<size_t>(#TYPE);}  \
static size_t TypeId() { return reinterpret_cast<size_t>(#TYPE);} \
static bool IsTypeId(size_t otherId) { if (otherId == TYPE::TypeId()) return true; return PARENT::IsTypeId(otherId); } \
inline bool IsA(size_t otherId) override { return TYPE::IsTypeId(otherId);}  \
template<class T> \
inline bool IsA(size_t otherId) { return TYPE::IsTypeId(T::TypeId()); } \
inline const char* GetWindowUID() override {return #TYPE;} \

class EditorPanel : public EditorWindow
{
public:
    virtual ~EditorPanel() = default;

    inline virtual size_t GetTypeId() { return reinterpret_cast<size_t>("EditorPanel"); }
    static size_t TypeId() { return reinterpret_cast<size_t>("EditorPanel"); }
    static bool IsTypeId(size_t otherId) { if (otherId == EditorPanel::TypeId()) return true; return false; }
    inline bool IsA(size_t otherId) override { return EditorPanel::IsTypeId(otherId); }
    template<class T>
    inline bool IsA(size_t otherId) { return EditorPanel::IsTypeId(T::TypeId()); }

    virtual void Update(EditorDocument* document);
    virtual void Show(EditorDocument* document);

    inline virtual DockingNodes GetDefaultDockingNode() { return DockingNodes::Left; }

    const char* GetWindowTitle() override;
    inline const char* GetWindowUID() override { return "EditorPanel"; }

    inline bool IsOpen() const { return Opened; }
    inline void SetOpen( bool open ) { Opened = open; }

protected:
    inline virtual void OnUpdate() {}
    inline virtual void OnShow() {}

    bool Opened = true;

    inline bool IsValid() { return CurrentDocument != nullptr; }

    template<class T>
    inline bool IsValid() { return CurrentDocument != nullptr && CurrentDocument->IsA(T::TypeId()); }

    EditorDocument* CurrentDocument = nullptr;

    template<class T>
    inline T* GetDocumentAs() { return IsValid<T>() ? reinterpret_cast<T*>(CurrentDocument) : nullptr; }
};