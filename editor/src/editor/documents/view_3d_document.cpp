#include "editor/documents/view_3d_document.h"

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#include "ImGuizmo.h"

Ray View3dDocument::GetMouseRay()
{
    return Camera.GetMouseRay(GetLocalMousePosition());
}

void View3dDocument::OnCreate()
{
    Camera.Setup(45, Vector3{ 0,0,0 });

    Camera.MouseScale = { -1,-1 };

    Camera.ControlsKeys[rlFPCamera::CameraControls::MOVE_UP] = KEY_E;
    Camera.ControlsKeys[rlFPCamera::CameraControls::MOVE_DOWN] = KEY_Q;

    ClearColor = SKYBLUE;

    ObjectMatrix = MatrixIdentity();
}

void View3dDocument::OnShow()
{
    ImGuizmo::SetDrawlist();

    float windowWidth = (float)ImGui::GetWindowWidth();
    float windowHeight = (float)ImGui::GetWindowHeight();
    ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

    ContentDocument::OnShow();
}

void View3dDocument::ShowOverlay()
{
    ImGuizmo::OPERATION op = ImGuizmo::OPERATION::TRANSLATE;
    if (IsKeyDown(KEY_LEFT_ALT))
        op = ImGuizmo::OPERATION::ROTATE;
    if (IsKeyDown(KEY_LEFT_SHIFT))
        op = ImGuizmo::OPERATION::SCALE;

    Manipulate(op, ImGuizmo::MODE::LOCAL, ObjectTransform);
}

bool View3dDocument::Manipulate(ImGuizmo::OPERATION operation, ImGuizmo::MODE mode, Transform3D& transform, Matrix4x4* deltaMatrix, const Vector3* snap)
{
    Matrix4x4 manipMatrix = Math3D::TransformToMatrix(transform);
    float16 deltaManipMatrix;
    float* deltaManipMatrixPtr = deltaManipMatrix.v;
    if (deltaMatrix != nullptr)
        deltaManipMatrixPtr = deltaMatrix->M16;

    if (ImGuizmo::Manipulate(MatrixToFloat(CameraMatrix), MatrixToFloat(ProjectionMatrix), operation, mode, manipMatrix.M16, deltaManipMatrix.v, (float*)snap))
    {
        transform = Math3D::MatrixToTransform(manipMatrix);
        return true;
    }

    return false;
}

void View3dDocument::OnUpdate()
{
    Camera.UseMouseX = Camera.UseMouseY = Camera.UseKeyboard = IsMouseDownIn(MOUSE_RIGHT_BUTTON);
    Camera.Update();
}

void View3dDocument::RenderContent()
{
    Camera.BeginMode3D();
    CameraMatrix = rlGetMatrixModelview();
    ProjectionMatrix = rlGetMatrixProjection();

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

    Vector3 spherePos = { 10,2,10 };
    float sphereRad = 1;

    auto collide = GetRayCollisionSphere(GetMouseRay(), spherePos, sphereRad);

    DrawSphere(spherePos, sphereRad, collide.hit ? GREEN : BLUE);

    if (collide.hit)
        DrawSphere(collide.point, 0.125f, PURPLE);

    BoundingBox bbox;
    bbox.min = { -0.5f,-0.5f,-0.5f };
    bbox.max = { 0.5f,0.5f,0.5f };

    auto hit = Math3D::GetRayCollisionBoxOriented(GetMouseRay(), bbox, ObjectTransform);

    rlPushMatrix();
    ObjectTransform.Apply();
    DrawCube(Vector3{ 0,0,0 }, 1, 1, 1, ColorAlpha(hit.hit ? RED : YELLOW, 1.0f));
    rlPopMatrix();

    if (hit.hit)
    {
        if (hit.hit)
            DrawSphere(hit.point, 0.075f, PURPLE);
    }
}

void View3dDocument::OnResized()
{
    Camera.ViewResized(ContentTexture);
}
