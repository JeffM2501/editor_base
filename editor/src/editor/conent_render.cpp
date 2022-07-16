#include "editor/conent_render.h"


void ContentRender::Setup()
{
    SetContentArea(ImVec2(0, 0), ImVec2(float(GetScreenWidth()), float(GetScreenHeight())));
}

void ContentRender::Shutdown()
{
    if (ContentTexture.id != 0)
        UnloadRenderTexture(ContentTexture);
}

void ContentRender::SetContentArea(ImVec2 origin, ImVec2 size)
{
    if (ViewCamera.GetFOVX() <= 0)
    {
        ViewCamera.Setup(45, Vector3Zero());
        ViewCamera.HideCursor = false;

        ViewCamera.ControlsKeys[rlFPCamera::MOVE_UP] = KEY_E;
        ViewCamera.ControlsKeys[rlFPCamera::MOVE_DOWN] = KEY_Q;
    }

    bool rebuild = false;
    if (ContentTexture.id == 0)
        rebuild = true;
    else if (size.x != ContentTexture.texture.width || size.y != ContentTexture.texture.height)
        rebuild = true;

    ContentOrign.x = origin.x;
    ContentOrign.y = origin.y;

    if (!rebuild)
        return;

    if (ContentTexture.id != 0)
        UnloadRenderTexture(ContentTexture);

    ContentTexture = LoadRenderTexture(int(size.x), int(size.y));

    ViewCamera.ViewResized(ContentTexture);
}

void ContentRender::Add2dRenderCallback(RenderCallback callback)
{
    RenderCallbacks2d.push_back(callback);
}

void ContentRender::Add3dRenderCallback(RenderCallback callback)
{
    RenderCallbacks3d.push_back(callback);
}

rlFPCamera& ContentRender::GetCamera()
{
    return ViewCamera;
}

void ContentRender::Update()
{
    if (RenderCallbacks3d.size() > 0)
    {
        ViewCamera.UseMouseX = ViewCamera.UseMouseY = IsMouseButtonDown(MOUSE_RIGHT_BUTTON);

        ViewCamera.Update();
    }
}

void ContentRender::Render()
{
    if (ContentTexture.id == 0)
        return;

    BeginTextureMode(ContentTexture);
    ClearBackground(BLACK);
    if (RenderCallbacks3d.size() > 0)
    {
        ViewCamera.BeginMode3D();
        for (auto cb : RenderCallbacks3d)
            cb(*this);
        ViewCamera.EndMode3D();
    }

    for (auto cb : RenderCallbacks2d)
        cb(*this);

    EndTextureMode();

    DrawTextureRec(ContentTexture.texture,
        Rectangle{ 0,0,(float)ContentTexture.texture.width,(float)-ContentTexture.texture.height },
        ContentOrign,
        WHITE);
}

Vector2 ContentRender::GetMousePosInContent()
{
    return Vector2Subtract(GetMousePosition(), ContentOrign);
}

Ray ContentRender::GetMouseRayInContent()
{
    return ViewCamera.GetMouseRay(GetMousePosInContent());
}

Ray ContentRender::GetMouseRayInContent(const Vector2& pos)
{
    return ViewCamera.GetMouseRay(pos);
}