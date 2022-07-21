#include "editor/documents/view_3d_document.h"

#include "raylib.h"
#include "rlgl.h"


void View3dDocument::OnCreate()
{
    Camera.Setup(45, Vector3{ 0,0,0 });

    Camera.ControlsKeys[rlFPCamera::CameraControls::MOVE_UP] = KEY_E;
    Camera.ControlsKeys[rlFPCamera::CameraControls::MOVE_DOWN] = KEY_Q;

    ClearColor = SKYBLUE;
}

void View3dDocument::OnUpdate()
{
    Camera.UseMouseX = Camera.UseMouseY = Camera.UseKeyboard = IsMouseDownIn(MOUSE_RIGHT_BUTTON);
    Camera.Update();
}

void View3dDocument::RenderContent()
{
    Camera.BeginMode3D();
    RenderScene();
    Camera.EndMode3D();
}

void View3dDocument::RenderScene()
{
    rlDisableDepthTest();
    DrawPlane(Vector3Zero(), Vector2{ 50,50 }, BROWN);
    DrawGrid(50, 1);
    rlDrawRenderBatchActive();
    rlEnableDepthTest();

    DrawCube(Vector3{ 0,1,0 }, 1, 1, 1, RED);
}

void View3dDocument::OnResized()
{
    Camera.ViewResized(ContentTexture);
}
