#include "editor/conent_document.h"
#include "rlImGui.h"

void ContentDocument::Create()
{

}

void ContentDocument::Close()
{
    if (ContentTexture.id != 0)
        UnloadRenderTexture(ContentTexture);
}

void ContentDocument::OnShow()
{
    ImVec2 size = ImGui::GetContentRegionAvail();

    if (ContentTexture.id == 0 || (size.x != ContentTexture.texture.width || size.y != ContentTexture.texture.height))
    {
        if (ContentTexture.id != 0)
            UnloadRenderTexture(ContentTexture);

        ContentTexture = LoadRenderTexture(int(size.x), int(size.y));
    }

    BeginTextureMode(ContentTexture);
    ClearBackground(SKYBLUE);
    RenderContent();
    EndTextureMode();

    rlImGuiImageSize(&ContentTexture.texture, ContentTexture.texture.width, ContentTexture.texture.height);
}