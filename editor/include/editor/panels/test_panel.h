#pragma once

#include "editor/editor_panel.h"

class TestPanel : public EditorPanel
{
public:
    DECLARE_PANEL(TestPanel, EditorPanel)

    TestPanel()
    {
        WindowTitle = "Test";
    }

protected:
    void OnShow() override;
};