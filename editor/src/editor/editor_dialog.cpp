#include "editor/editor_dialog.h"

#include "imgui.h"
#include "editor/utils/imgui_layout.h"

void EditorDialog::Create()
{
    IsCreated = true;
    OnCreate();

    if (CreateCallback != nullptr)
        CreateCallback(this);

    static char temp[256] = { 0 };
    sprintf(temp, "%s###%s-dlog-%d", GetName(), GetName(), GetRandomValue(0, 0xffffff));

    ImguiId = temp;

    ImGui::OpenPopup(ImguiId.c_str(), ImGuiPopupFlags_None);
}

void EditorDialog::OnCreate()
{
    SetAccept();
}

void EditorDialog::SetAccept(const char* acceptStr)
{
    AcceptButton = acceptStr;
    DeclineButton.clear();
    CancelButton.clear();
}

void EditorDialog::SetAcceptCancel(const char* acceptStr, const char* cancelStr)
{
    AcceptButton = acceptStr;
    DeclineButton.clear();
    CancelButton = cancelStr;
}

void EditorDialog::SetAcceptDeclineCancel(const char* acceptStr, const char* declineStr, const char* cancelStr)
{
    AcceptButton = acceptStr;
    DeclineButton = declineStr;
    CancelButton = cancelStr;
}

void EditorDialog::ShowContent()
{
    OnShowContent();

    // buttons
    ImGui::BeginDisabled(!IsTopMost());

    std::vector<const char*> labels;
    if (!AcceptButton.empty())
        labels.push_back(AcceptButton.c_str());
    if (!DeclineButton.empty())
        labels.push_back(DeclineButton.c_str());
    if (!CancelButton.empty())
        labels.push_back(CancelButton.c_str());

    auto size = ImGuiLayout::GetButtonsSize(labels);

    ImGuiLayout::BottomJustify(size.y);
    ImGuiLayout::RightJustify(size.x);

    if (!AcceptButton.empty())
    {
        ImGui::BeginDisabled(!EnableAccept());
        if (ImGui::Button(AcceptButton.c_str()))
        {
            SetDialogResults(EditorDialogResults::Accept);
        }
        ImGui::EndDisabled();
    }

    if (!DeclineButton.empty())
    {
        if (!AcceptButton.empty())
            ImGui::SameLine();

        if (ImGui::Button(DeclineButton.c_str()))
        {
            SetDialogResults(EditorDialogResults::Decline);
        }
    }

    if (!CancelButton.empty())
    {
        if (!AcceptButton.empty() || !DeclineButton.empty())
            ImGui::SameLine();

        if (ImGui::Button(CancelButton.c_str()))
        {
            SetDialogResults(EditorDialogResults::Cancel);
        }
    }

    ImGui::EndDisabled();
}

void EditorDialog::Update()
{
    OnUpdate();
}

bool EditorDialog::IsActive()
{
    return DialogResults != EditorDialogResults::None;
}

void EditorDialog::SetDialogResults(EditorDialogResults result)
{
    DialogResults = result;
    OnResults(result);
}

EditorDialogResults EditorDialog::GetDialogResults()
{
    return DialogResults;
}

