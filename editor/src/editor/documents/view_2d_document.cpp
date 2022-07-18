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
    RenderScene();
    EndMode2D();
}


void DrawGrid2D(int slices, float spacing)
{
    int halfSlices = slices / 2;

    //rlCheckRenderBatchLimit((slices + 2)*4);

    rlBegin(RL_LINES);
    for (int i = -halfSlices; i <= halfSlices; i++)
    {
        if (i == 0)
        {
            rlColor3f(0.5f, 0.5f, 0.5f);
            rlColor3f(0.5f, 0.5f, 0.5f);
            rlColor3f(0.5f, 0.5f, 0.5f);
            rlColor3f(0.5f, 0.5f, 0.5f);
        }
        else
        {
            rlColor3f(0.75f, 0.75f, 0.75f);
            rlColor3f(0.75f, 0.75f, 0.75f);
            rlColor3f(0.75f, 0.75f, 0.75f);
            rlColor3f(0.75f, 0.75f, 0.75f);
        }

        rlVertex2f((float)i * spacing, (float)-halfSlices * spacing);
        rlVertex2f((float)i * spacing, (float)halfSlices * spacing);

        rlVertex2f((float)-halfSlices * spacing, (float)i * spacing);
        rlVertex2f((float)halfSlices * spacing, (float)i * spacing);
    }
    rlEnd();
}
void View2dDocument::RenderScene()
{
    DrawGrid2D(50, 50);

    DrawCircle(0, 0, 100, IsHovered() ? RED : BLUE);
}

void View2dDocument::OnResized()
{
    if (Camera.offset.x == 0 && Camera.offset.y == 0)
        Camera.offset = Vector2Scale(GetContentSize(),0.5f);
}

void View2dDocument::OnUpdate()
{
    if (!IsHovered())
        return;

    if (IsMouseDownIn(MOUSE_BUTTON_RIGHT))
    {
        Vector2 delta = Vector2Scale(GetMouseDelta(), -1.0f / Camera.zoom);
        Camera.target = Vector2Add(Camera.target, delta);
    }

    float wheelDelta = GetMouseWheelMove();
    if (wheelDelta != 0)
    {
        Camera.target = GetWorldMousePos();
        Camera.offset = GetLocalMousePosition();

        Camera.zoom += wheelDelta * (Camera.zoom <= 1.01f ? 0.0125f : 0.125f);
        if (Camera.zoom < 0.0125f)
            Camera.zoom = 0.0125f;
    }
}