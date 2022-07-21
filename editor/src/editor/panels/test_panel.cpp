#pragma once

#include "editor/panels/test_panel.h"
#include "editor/documents/view_2d_document.h"
#include "editor/documents/view_3d_document.h"

void TestPanel::OnShow()
{
    if (!IsValid())
    {
        ImGui::TextUnformatted("No Document");
        return;
    }

    float min[3] = { -1000, -1000, -1000 };
    float max[3] = {1000, 1000, 1000 };

    if (IsValid<View2dDocument>())
    {
        View2dDocument* doc2d = GetDocumentAs<View2dDocument>();

        ImGui::TextUnformatted("View 2d");
        ImGui::SliderScalarN("Target", ImGuiDataType_Float, &(doc2d->GetCamera().target.x), 2, min, max, "%f", 1);
    }

    if (IsValid<View3dDocument>())
    {
        View3dDocument* doc3d = GetDocumentAs<View3dDocument>();

        ImGui::TextUnformatted("View 3d");
        ImGui::SliderScalarN("Target", ImGuiDataType_Float, &(doc3d->GetCamera().GetCamera().target.x), 3, min, max, "%f", 1);
    }
}
