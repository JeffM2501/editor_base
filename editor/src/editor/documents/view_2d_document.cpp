#include "editor/documents/view_2d_document.h"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

void View2dDocument::OnCreate()
{
    ClearColor = BLACK;
    Camera.zoom = 1;
}

Vector2 View2dDocument::GetWorldMousePos() const
{
    return GetScreenToWorld2D(GetLocalMousePosition(), Camera);
}

void View2dDocument::RenderContent()
{
    BeginMode2D(Camera);

    rlPushMatrix();
    rlTranslatef(0, 25*50, 0);
    rlRotatef(90, 1, 0, 0);
    DrawGrid(100, 50);
    rlPopMatrix();

    DrawCircle(0, 0, 100, IsHovered() ? RED : BLUE);
    EndMode2D();
}

void View2dDocument::OnResized()
{
   Camera.offset = Vector2Scale(GetContentSize(),0.5f);
}

void View2dDocument::OnUpdate()
{
    if (!IsHovered())
        return;

    float wheelDelta = GetMouseWheelMove();
    if (wheelDelta != 0)
    {
        Camera.target = GetWorldMousePos();
        Camera.offset = GetLocalMousePosition();

        Camera.zoom += wheelDelta * (Camera.zoom <= 1.01f ? 0.0125f : 0.125f);
        if (Camera.zoom < 0.0125f)
            Camera.zoom = 0.0125f;
    }

    if (IsMouseDownIn(MOUSE_BUTTON_RIGHT))
    {
        Vector2 delta = Vector2Scale(GetMouseDelta(), 1.0f / Camera.zoom);
        delta.x *= -1;
        Camera.target = Vector2Add(Camera.target, delta);
    }
}