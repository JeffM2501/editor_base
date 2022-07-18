#include "editor/conent_document.h"
#include "rlImGui.h"

void ContentDocument::Create()
{
    EditorDocument::Create();
}

void ContentDocument::Close()
{
    if (ContentTexture.id != 0)
        UnloadRenderTexture(ContentTexture);
}

void ContentDocument::OnShow()
{
    ImVec2 size = ImGui::GetContentRegionAvail();

    LocalMousePosition = EditorManager::GetWindowMousePostion();
    LocalMousePosition.x -= ImGui::GetWindowPos().x;
    LocalMousePosition.y -= ImGui::GetWindowPos().y;

    if (ContentTexture.id == 0 || (size.x != ContentTexture.texture.width || size.y != ContentTexture.texture.height))
    {
        if (ContentTexture.id != 0)
            UnloadRenderTexture(ContentTexture);

        ContentTexture = LoadRenderTexture(int(size.x), int(size.y));
        ContentRect = { 0,0,size.x, -size.y };
        OnResized();
    }

    BeginTextureMode(ContentTexture);
    ClearBackground(ClearColor);
    RenderContent();
    EndTextureMode();

    rlImGuiImageRect(&ContentTexture.texture, ContentTexture.texture.width, ContentTexture.texture.height, ContentRect);

    ShowOverlay();
}

Vector2 ContentDocument::GetContentSize() const
{
    return Vector2{ float(ContentTexture.texture.width), float(ContentTexture.texture.height) };
}

Vector2 ContentDocument::GetLocalMousePosition() const 
{ 
    return LocalMousePosition; 
}
