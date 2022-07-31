#include "editor/documents/view_3d_document.h"

#include "raylib.h"
#include "rlgl.h"

#include "ImGuizmo.h"


// Get float array of matrix data
void FloatToMatrix(float16 floats, Matrix& mat)
{
    mat.m0 = floats.v[0];
    mat.m1 = floats.v[1];
    mat.m2 = floats.v[2];
    mat.m3 = floats.v[3];
    mat.m4 = floats.v[4];
    mat.m5 = floats.v[5];
    mat.m6 = floats.v[6];
    mat.m7 = floats.v[7];
    mat.m8 = floats.v[8];
    mat.m9 = floats.v[9];
    mat.m10 = floats.v[10];
    mat.m11 = floats.v[11];
    mat.m12 = floats.v[12];
    mat.m13 = floats.v[13];
    mat.m14 = floats.v[14];
    mat.m15 = floats.v[15];
}


void View3dDocument::OnCreate()
{
    Camera.Setup(45, Vector3{ 0,0,0 });

    Camera.ControlsKeys[rlFPCamera::CameraControls::MOVE_UP] = KEY_E;
    Camera.ControlsKeys[rlFPCamera::CameraControls::MOVE_DOWN] = KEY_Q;

    ClearColor = SKYBLUE;

    ObjectMatrix = /*MatrixTranspose*/(MatrixIdentity());
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

    Manipulate(op, ImGuizmo::MODE::LOCAL, ObjectMatrix);
}

bool View3dDocument::Manipulate(ImGuizmo::OPERATION operation, ImGuizmo::MODE mode, Matrix& matrix, Matrix* deltaMatrix /*= nullptr*/, const Vector3* snap /*= nullptr*/)
{
    float16 manipMatrix = MatrixToFloatV(matrix);
    float16 deltaManipMatrix;
    if (deltaMatrix != nullptr)
        deltaManipMatrix = MatrixToFloatV(*deltaMatrix);

    if (ImGuizmo::Manipulate(MatrixToFloat(CameraMatrix), MatrixToFloat(ProjectionMatrix), operation, mode, manipMatrix.v, deltaManipMatrix.v, (float*)snap))
    {
        FloatToMatrix(manipMatrix, ObjectMatrix);
        if (deltaMatrix != nullptr)
            FloatToMatrix(deltaManipMatrix, *deltaMatrix);

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
    CameraMatrix = /*MatrixTranspose*/(rlGetMatrixModelview());
    ProjectionMatrix = /*MatrixTranspose*/(rlGetMatrixProjection());

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

    rlPushMatrix();
    rlMultMatrixf(MatrixToFloat(ObjectMatrix));
    DrawCube(Vector3{ 0,0,0 }, 1, 1, 1, ColorAlpha(RED,1.0f));
    rlPopMatrix();
}

void View3dDocument::OnResized()
{
    Camera.ViewResized(ContentTexture);
}
