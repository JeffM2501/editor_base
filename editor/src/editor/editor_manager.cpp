#include "editor/conent_render.h"
#include "editor/editor_dialog.h"
#include "editor/editor_document.h"
#include "editor/editor_manager.h"
#include "editor/editor_menu_manager.h"
#include "editor/editor_panel.h"
#include "editor/editor_window.h"

#include "rlImGui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "ImGuizmo.h"

class PanelCommand : public EditorMenuItem
{
public:
    PanelCommand(int index, EditorPanel* _panel) :  Panel(_panel)
    {
        Order = index;
        if (Panel)
        {
            Label = Panel->GetWindowTitle();
        }
    }

    EditorPanel* Panel = nullptr;

protected:
    void OnExecute() override 
    {
        if (Panel == nullptr)
            return;

        Panel->SetOpen(!Panel->IsOpen());
    }

    bool OnChecked() override
    { 
        return Panel != nullptr && Panel->IsOpen(); 
    }
};

const char* EditorWindow::GetWindowId()
{
    if (WindowId.empty())
    {
        WindowId = GetWindowTitle();
        WindowId += "###";
        WindowId += GetWindowUID();

        const char* suffix = GetWindowUIDSuffix();
        if (suffix != nullptr)
            WindowId += suffix;
    }
    return WindowId.c_str();
}

const char* EditorWindow::GetWindowUIDSuffix()
{
    size_t id = EditorManager::GetWindowTypeIndex(this);
    sprintf(NameBuffer, "-%zu", id);
    return NameBuffer;
}

namespace EditorManager
{
    EventHandler<DocumentCallback> OnDocumentChanged;

    ImGuiID DockspaceId = 0;
    bool RequestQuit = false;
    bool ContentHovered = false;

    ImVec2 ContentPos(0, 0);
    ImVec2 ContentSize(0, 0);

    std::vector<EditorDocument*> Documents;
    std::vector<EditorPanel*> Panels;
    std::vector<EditorDialog*> DialogBoxes;
    EditorDialog* ActiveDialog = nullptr;

    EditorDocument* FocusedDocument = nullptr;

    std::map<DockingNodes, ImGuiID> DockspaceNodes;

    float MenuBarOffset = 0;

    void Setup()
    {
        rlImGuiSetup(true);
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;

        ImGui::GetStyle().WindowMenuButtonPosition = ImGuiDir_Right;

        EditorMenuManager::Setup();
    }

    std::vector<EditorWindow*> GetWindowsOfType(size_t typeId)
    {
        std::vector<EditorWindow*> windows;
        for (auto document : Documents)
        {
            if (document->IsA(typeId))
                windows.push_back(document);
        }

        for (auto panel : Panels)
        {
            if (panel->IsA(typeId))
                windows.push_back(panel);
        }
        return windows;
    }

    size_t GetWindowTypeIndex(EditorWindow* window)
    {
        std::vector<EditorWindow*> windows = GetWindowsOfType(window->GetTypeId());
        if (windows.size() <= 1)
            return 0;

        for (size_t i = 0; i < windows.size(); i++)
        {
            if (windows[i] == window)
                return i;
        }

        return size_t(-1);
    }

    ImGuiID GetLeftDockspaceNode()
    {
        auto* centralNode = ImGui::DockBuilderGetCentralNode(DockspaceId);

        ImGuiID leftNode = ImGui::DockBuilderSplitNode(centralNode->ID, ImGuiDir_Left, 0.25f, nullptr, &DockspaceId);
        DockspaceNodes[DockingNodes::Left] = leftNode;

        return leftNode;
    }

    ImGuiID GetRightDockspaceNode()
    {
        auto* centralNode = ImGui::DockBuilderGetCentralNode(DockspaceId);

        ImGuiID rightNode = ImGui::DockBuilderSplitNode(centralNode->ID, ImGuiDir_Right, 0.25f, nullptr, &DockspaceId);
        DockspaceNodes[DockingNodes::Right] = rightNode;

        return rightNode;
    }

    ImGuiID GetBottomDockspaceNode()
    {
        auto* centralNode = ImGui::DockBuilderGetCentralNode(DockspaceId);

        ImGuiID bottomNode = ImGui::DockBuilderSplitNode(centralNode->ID, ImGuiDir_Down, 0.15f, nullptr, &DockspaceId);
        DockspaceNodes[DockingNodes::Bottom] = bottomNode;

        return bottomNode;
    }

    ImGuiID GetDockspaceId(DockingNodes node)
    {
        if (node == DockingNodes::Central)
            return DockspaceId;

        auto itr = DockspaceNodes.find(node);
        if (itr == DockspaceNodes.end() || ImGui::DockBuilderGetNode(itr->second) == nullptr)
        {
            switch (node)
            {
            case DockingNodes::Left:
                return GetLeftDockspaceNode();
            case DockingNodes::Right:
                return GetRightDockspaceNode();
            case DockingNodes::Bottom:
                return GetBottomDockspaceNode();
            default:
                return ImGuiID(-1);
            }
        }

        return itr->second;
    }

    void ForceDockWindow(const char* name, DockingNodes node)
    {
        ImGui::DockBuilderDockWindow(name, GetDockspaceId(node));
    }

    void Shutdown()
    {
        rlImGuiShutdown();
    }

    void Update()
    {
        for (auto dialog : DialogBoxes)
        {
            ActiveDialog = dialog;
            ActiveDialog->Update();
            ActiveDialog = nullptr;
        }

        for (auto panel : Panels)
        {
            panel->Update(FocusedDocument);
        }

        // draw registered windows
        for (std::vector<EditorDocument*>::iterator docItr = Documents.begin(); docItr != Documents.end();)
        {
            EditorDocument* document = *docItr;
            document->Update();
            if (document->WantClose())
            {
                document->Close();
                docItr = Documents.erase(docItr);
                if (FocusedDocument == document)
                {
                    if (Documents.empty())
                        FocusedDocument = nullptr;
                    else
                        SetActiveDocument(Documents.front());
                }
                delete(document);
            }
            else
            {
                docItr++;
            }
        }
    }

    void AddDocument(EditorDocument* newDocument)
    {
        for (auto existingDoc : Documents)
        {
            if (existingDoc->GetWindowUID() == newDocument->GetWindowUID())
            {
                SetActiveDocument(existingDoc);
                return;
            }
        }

        Documents.push_back(newDocument);
        newDocument->Create();

        SetActiveDocument(newDocument);
    }

    void UpdatePanelMenus()
    {
        auto windowMenu = EditorMenuManager::GetAppMenu().FindMenu(EditorMenuManager::WindowMenu);
        if (windowMenu == nullptr)
            return;

        windowMenu->Children.clear();

        int i = 0;
        for (auto panel : Panels)
        {
            windowMenu->AddMenu<PanelCommand>(i, panel);
        }
        EditorMenuManager::RebuildMenuCache();
    }

    void AddPanel(EditorPanel* panel)
    {
        Panels.push_back(panel);
        panel->Create();
        UpdatePanelMenus();
    }

    void AddDialogBox(EditorDialog* dialogBox)
    {
        if (dialogBox == nullptr)
            return;

        if (ActiveDialog != nullptr)
            ActiveDialog->Child = dialogBox;
        else
            DialogBoxes.push_back(dialogBox);
    }

    EditorDocument* GetActiveDocument()
    {
        return FocusedDocument;
    }
    
    void SetActiveDocument(EditorDocument* document)
    {
        if (FocusedDocument == nullptr || FocusedDocument->GetWindowUID() != document->GetWindowId())
        {
            FocusedDocument = document;
            if (!document->IsFocused())
                document->ForceFocus();
            OnDocumentChanged.Call(document);
        }
    }

    void SetupUI()
    {

    }

    void Quit()
    {
        RequestQuit = true;
    }

    bool WantQuit()
    {
        return RequestQuit || WindowShouldClose();
    }

    Vector2 GetWindowMousePostion()
    {
        Vector2 pos = GetMousePosition();
        pos.y -= MenuBarOffset;
        return pos;
    }

    void ShowDialog(EditorDialog* dialog)
    {
        EditorDialog* lastActive = ActiveDialog;
        ActiveDialog = dialog;

        if (!ActiveDialog->IsCreated)
            ActiveDialog->Create();

        bool isOpen = true;
        bool* openPtr = nullptr;
        if (dialog->GetAllowCloseBox())
            openPtr = &isOpen;

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoSavedSettings;
        if (!dialog->GetResizeable())
            flags |= ImGuiWindowFlags_NoResize;

        ImVec2 max = { float(GetScreenWidth()), float(GetScreenHeight()) };
        ImGui::SetNextWindowSizeConstraints(dialog->GetMinimumSize(), max);

        if (ImGui::BeginPopupModal(dialog->ImguiId.c_str(), openPtr, flags))
        {
            dialog->ShowContent();

            if (dialog->Child != nullptr)
            {
                ShowDialog(dialog->Child);
            }
            else
            {
                if (openPtr != nullptr && !openPtr)
                    dialog->SetDialogResults(EditorDialogResults::Cancel);

                if (dialog->IsActive())
                {
                    ImGui::CloseCurrentPopup();
            
                    if (lastActive != nullptr)
                        lastActive->Child = nullptr;
                    else
                        DialogBoxes.erase(DialogBoxes.begin());
             
                    delete(dialog);
                }
            }
            ImGui::EndPopup();
        }
        ActiveDialog = lastActive;
    }

    void Render()
    {
        rlImGuiBegin();
        ImGuizmo::BeginFrame();

        MenuBarOffset = ImGui::GetTextLineHeightWithSpacing() + ImGui::GetStyle().FramePadding.y;

        ImVec2 screenSize((float)GetScreenWidth(), (float)GetScreenHeight());

        ImGui::SetNextWindowSize(screenSize, ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(0,0), ImGuiCond_Always);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        bool showMainFrame = (ImGui::Begin("MainFrame",
            nullptr,
            ImGuiWindowFlags_MenuBar |
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoNavFocus | 
            ImGuiWindowFlags_NoCollapse | 
            ImGuiWindowFlags_NoBringToFrontOnFocus));

        ImGui::PopStyleVar(1);

        if (showMainFrame)
        {
            screenSize.y -= MenuBarOffset;

            DockspaceId = ImGui::DockSpace(ImGui::GetID("MainWindowDock"), screenSize, ImGuiDockNodeFlags_PassthruCentralNode);

            auto* node = ImGui::DockBuilderGetNode(DockspaceId);
            if (!node->HasCentralNodeChild)
            {
                // the root has never been split, so force all panels to the nodes they belong to
                for (auto panel : Panels)
                {
                    ForceDockWindow(panel->GetWindowId(), panel->GetDefaultDockingNode());
                }
            }

            if (ImGui::IsWindowAppearing())
                SetupUI();

            for (auto panel : Panels)
            {
                panel->Show(FocusedDocument);
            }

            // draw registered windows
            for (auto document : Documents)
            {
                document->Show();

                if (document->IsFocused())
                    SetActiveDocument(document);
            }

            ImGui::ShowDemoWindow();
            EditorMenuManager::Show(FocusedDocument);

            if (!DialogBoxes.empty())
            {
                ShowDialog(DialogBoxes.front());
            }
        }
        ImGui::End();

        rlImGuiEnd();
    }

    bool WantMouse()
    {
        return ImGui::GetIO().WantCaptureMouse;
    }

    float GetMenubarOffset()
    {
        return MenuBarOffset;
    }
}