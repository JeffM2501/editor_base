#include "editor/editor_manager.h"
#include "editor/conent_render.h"
#include "editor/editor_panel.h"
#include "editor/editor_document.h"
#include "editor/editor_window.h"

#include "rlImGui.h"
#include "imgui.h"
#include "imgui_internal.h"

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
    ImGuiID DockspaceId = 0;
    bool RequestQuit = false;
    bool ContentHovered = false;

    ImVec2 ContentPos(0, 0);
    ImVec2 ContentSize(0, 0);

    std::vector<EditorDocument*> Documents;
    std::vector<EditorPanel*> Panels;

    EditorDocument* FocusedDocument = nullptr;

    std::map<DockingNodes, ImGuiID> DockspaceNodes;

    float MenuBarOffset = 0;

    void Setup()
    {
        rlImGuiSetup(true);
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;

        ImGui::GetStyle().WindowMenuButtonPosition = ImGuiDir_Right;
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
                delete(document);
                docItr = Documents.erase(docItr);
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

    void AddPanel(EditorPanel* panel)
    {
        Panels.push_back(panel);
        panel->Create();
    }

    EditorDocument* GetActiveDocument()
    {
        return FocusedDocument;
    }
    
    void SetActiveDocument(EditorDocument* document)
    {
        if (FocusedDocument != document)
        {
            FocusedDocument = document;
            document->ForceFocus();
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

    void ShowMenu()
    {
        bool openAbout = false;
        bool copyScreenshot = false;

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Exit", "Alt+F4"))
                    Quit();

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Window"))
            {
                for (auto& panel : Panels)
                {
                    bool open = panel->IsOpen();
                    if (ImGui::MenuItem(panel->GetWindowTitle(), nullptr, &open))
                    {
                        panel->SetOpen(open);
                    }
                }
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
    }

    Vector2 GetWindowMousePostion()
    {
        Vector2 pos = GetMousePosition();
        pos.y -= MenuBarOffset;
        return pos;
    }

    void Render()
    {
        rlImGuiBegin();
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
                    FocusedDocument = document;

            }

            ImGui::ShowDemoWindow();

            ShowMenu();

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