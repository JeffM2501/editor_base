#pragma once

#include <string>
#include <map>
#include <functional>
#include <memory>

class EditorDocument;

class EditorCommand
{
public: 
    void Execute();
    inline bool Enabled() { return OnEnabled(); }
    inline bool Checked() { return OnChecked(); }

    std::function<void(EditorCommand*)> ExecuteCallback;

    // TODO shortcuts
protected:
    inline virtual void OnExecute() {}
    inline virtual bool OnEnabled() { return true; }
    inline virtual bool OnChecked() { return false; }
};

class EditorMenuItem : public EditorCommand
{
public:
    int Order = 0;
    std::string Label;
    std::string Icon;

    using Ptr = std::shared_ptr<EditorMenuItem>;
    using Map = std::map<int, Ptr>;
    using MapItr = std::map<int, Ptr>::iterator;

    Map Children;

    Ptr AddMenu(Ptr item);

    inline Ptr AddMenu(int order, const char* name, const char* icon = nullptr, std::function<void(EditorCommand*)> callback = nullptr)
    {
        return AddMenu(std::make_shared<EditorMenuItem>(order, name, icon, callback));
    }

    template<class T>
    inline Ptr AddMenu(int order, const char* name)
    {
        return AddMenu(std::make_shared<T>(order, name));
    }

    template<class T, typename ...Args>
    inline Ptr AddMenu(Args&&... args)
    {
        return AddMenu(std::make_shared<T>(std::forward<Args>(args)...));
    }

    inline Ptr FindMenu(int id)
    {
        auto child = Children.find(id);
        if (child == Children.end())
            return nullptr;

        return child->second;
    }

    EditorMenuItem() {}
    EditorMenuItem(int index, const char* name, const char* icon = nullptr, std::function<void(EditorCommand*)> callback = nullptr);
};

namespace EditorMenuManager
{
    void Setup();
    void Show(EditorDocument* currentDocument);

    EditorMenuItem& GetAppMenu();

    void RebuildMenuCache();

    // main menu constants
    static constexpr int FileMenu = 0;
    static constexpr int EditMenu = 10;
    static constexpr int WindowMenu = 100;
    static constexpr int HelpMenu = 1000;
}