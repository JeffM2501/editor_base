#include "editor/editor_menu_manager.h"
#include "editor/editor_document.h"
#include "editor/editor_manager.h"
#include "editor/dialogs/message_dialog.h"

class EditorMenuInstance
{
public:
    using Ptr = std::shared_ptr<EditorMenuInstance>;

    EditorMenuItem* Item = nullptr;
    std::map<int, Ptr> Children;

    void Fill(EditorMenuItem* menu)
    {
        Item = menu;

        for (auto child : menu->Children)
        {
            auto existingChild = Children.find(child.first);
            if (existingChild == Children.end())
                Children[child.first] = std::make_shared<EditorMenuInstance>();

            Children[child.first]->Fill(child.second.get());
        }
    }

    void Clear()
    {
        Item = nullptr;
        Children.clear();
    }

    void Show()
    {
        if (!Item)
            return;

        if (Children.empty())
        {
            if (ImGui::MenuItem(Item->Label.c_str(), nullptr, Item->Checked(), Item->Enabled()))
            {
                Item->Execute();
            }
        }
        else
        {
            if (Item->Label.empty() || ImGui::BeginMenu(Item->Label.c_str(), Item->Enabled()))
            {
                for (auto child : Children)
                    child.second->Show();

                if (!Item->Label.empty())
                    ImGui::EndMenu();
            }
        }
    }
};

namespace EditorMenuManager
{
    EditorMenuItem AppMenu;

    EditorMenuInstance MenuCache;

    void UpdateMenuCache(EditorDocument* document)
    {
        MenuCache.Clear();
        MenuCache.Fill(&AppMenu);
        if (document)
            MenuCache.Fill(&document->GetMenuBar());
    }

    void RebuildMenuCache()
    {
        UpdateMenuCache(EditorManager::GetActiveDocument());
    }

    void Setup()
    {
        EditorManager::OnDocumentChanged.Add(UpdateMenuCache);

        auto fileMenu = AppMenu.AddMenu(FileMenu, "File");

        auto quit = fileMenu->AddMenu(std::make_shared<EditorMenuItem>(9999, "Exit", nullptr, [](EditorCommand*)
            {
                EditorManager::Quit();
            })); 


        AppMenu.AddMenu(WindowMenu, "Window");
        
        auto help = AppMenu.AddMenu(HelpMenu, "Help");

        help->AddMenu(9999, "About", nullptr, [](EditorCommand*)
            {
                MessageDialog::Show("About", "Editor Base");
            });
    }

    void Show(EditorDocument* currentDocument)
    {
        if (ImGui::BeginMenuBar())
        {
            for (auto child : MenuCache.Children)
                child.second->Show();
            ImGui::EndMenuBar();
        }
    }

    EditorMenuItem& GetAppMenu()
    {
        return AppMenu;
    }
}

EditorMenuItem::Ptr EditorMenuItem::AddMenu(EditorMenuItem::Ptr item)
{
    Children[item->Order] = item;

    return Children[item->Order];
}

EditorMenuItem::EditorMenuItem(int index, const char* name, const char* icon /*= nullptr*/, std::function<void(EditorCommand*)> callback /*= nullptr*/)
    : Order(index)
{
    if (name != nullptr)
        Label = name;

    if (icon != nullptr)
        Icon = icon;

    if (callback != nullptr)
        ExecuteCallback = callback;
}

void EditorCommand::Execute()
{
    if (ExecuteCallback != nullptr)
        ExecuteCallback(this);

    OnExecute();
}
