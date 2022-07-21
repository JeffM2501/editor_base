#include "editor/documents/view_2d_document.h"

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

void View2dDocument::OnCreate()
{
    ClearColor = BLACK;
    ViewCamera.zoom = 1;

    auto viewMenu = DocumentMenuBar.AddMenu(20, "View");
    auto reset = viewMenu->AddMenu(10, "Reset Camera");
    reset->ExecuteCallback = [this](EditorCommand*)
    {
        ViewCamera.offset = Vector2Scale(GetContentSize(), 0.5f);
        ViewCamera.target = Vector2Zero();
    };
}

Vector2 View2dDocument::GetWorldMousePos() const
{
    return GetScreenToWorld2D(GetLocalMousePosition(), ViewCamera);
}

void View2dDocument::RenderContent()
{
    BeginMode2D(ViewCamera);
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
    if (ViewCamera.offset.x == 0 && ViewCamera.offset.y == 0)
        ViewCamera.offset = Vector2Scale(GetContentSize(),0.5f);
}

void View2dDocument::OnUpdate()
{
    if (!IsHovered())
        return;

    if (IsMouseDownIn(MOUSE_BUTTON_RIGHT))
    {
        Vector2 delta = Vector2Scale(GetMouseDelta(), -1.0f / ViewCamera.zoom);
        ViewCamera.target = Vector2Add(ViewCamera.target, delta);
    }

    float wheelDelta = GetMouseWheelMove();
    if (wheelDelta != 0)
    {
        ViewCamera.target = GetWorldMousePos();
        ViewCamera.offset = GetLocalMousePosition();

        ViewCamera.zoom += wheelDelta * (ViewCamera.zoom <= 1.01f ? 0.0125f : 0.125f);
        if (ViewCamera.zoom < 0.0125f)
            ViewCamera.zoom = 0.0125f;
    }
}