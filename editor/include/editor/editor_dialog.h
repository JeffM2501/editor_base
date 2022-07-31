#pragma once

#include "editor/editor_manager.h"

#include <string>

enum class EditorDialogResults
{
    None,
    Accept,
    Decline,
    Cancel,
};

class EditorDialog
{
public:

    template <class T>
    static inline T* Show()
    {
        return EditorManager::AddDialogBox<T>();
    }

    template <class T, typename ...Args>
    inline T* Show(Args&&... args)
    {
        return EditorManager::AddDialogBox<T>(std::forward<Args>(args)...);
    }

    void Create();
    void ShowContent();
    void Update();
    
    bool IsActive();
    void SetDialogResults(EditorDialogResults result);
    EditorDialogResults GetDialogResults();

    inline virtual const char* GetName() const { return WindowTile.c_str(); }

    inline bool GetAllowCloseBox() const { return AllowCloseBox; }
    inline ImVec2 GetMinimumSize() const { return MinimumSize; }
    inline bool GetResizeable() const { return Resizeable; }

    static constexpr char Ok[] = "OK";
    static constexpr char Cancel[] = "Cancel";
    static constexpr char Yes[] = "Yes";
    static constexpr char No[] = "No";

    void SetAccept(const char* acceptStr = Ok);
    void SetAcceptCancel(const char* acceptStr = Ok, const char* cancelStr = Cancel);
    void SetAcceptDeclineCancel(const char* acceptStr = Yes, const char* declineStr = No, const char* cancelStr = Cancel);

    // status data
    std::string ImguiId;
    EditorDialog* Child = nullptr;

    bool IsCreated = false;

protected:
    virtual void OnCreate();
    inline virtual void OnResults(EditorDialogResults results) {}
    inline virtual void OnShowContent() {}
    inline virtual void OnUpdate() {}

    inline bool IsTopMost() const { return Child == nullptr; }

    inline virtual bool EnableAccept() { return true; }

    std::string WindowTile = "Dialog";

    EditorDialogResults DialogResults = EditorDialogResults::None;

    std::string AcceptButton;
    std::string DeclineButton;
    std::string CancelButton;

    ImVec2 MinimumSize = { 200,150 };
    bool Resizeable = false;
    bool AllowCloseBox = true;
};