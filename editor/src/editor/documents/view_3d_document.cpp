#include "editor/documents/view_3d_document.h"

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#include "ImGuizmo.h"

RayCollision TestRayBBoxIntersection(Ray ray, BoundingBox& bbox, Matrix& transform)
{
    // Intersection method from Real-Time Rendering and Essential Mathematics for Games
    Vector3 translation = { 0,0,0 };
    Vector3 rotation = { 0,0,0 };
    Vector3 scale = { 0,0,0 };

    ImGuizmo::DecomposeMatrixToComponents(MatrixToFloat(transform), &translation.x, &rotation.x, &scale.x);

    Matrix rotMat = MatrixRotateZYX(Vector3Scale(rotation, DEG2RAD));

    Vector3 xAxis = Vector3Transform(Vector3{ 1,0,0 }, rotMat);
    Vector3 yAxis = Vector3Transform(Vector3{ 0,1,0 }, rotMat);
    Vector3 zAxis = Vector3Transform(Vector3{ 0,0,1 }, rotMat);

    float tMin = 0.0f;
    float tMax = 100000.0f;

    RayCollision hit = { 0 };

    Vector3 worldPos = translation;

    Vector3 delta = Vector3Subtract(worldPos, ray.position);

    // Test intersection with the 2 planes perpendicular to the OBB's X axis
    {
        //  Vector3 xAxis = { transform.m0, transform.m4, transform.m8 };
        float e = Vector3DotProduct(xAxis, delta);
        float f = Vector3DotProduct(ray.direction, xAxis);

        if (fabs(f) > 0.001f) // Standard case
        {

            float t1 = (e + bbox.min.x) / f; // Intersection with the "left" plane
            float t2 = (e + bbox.max.x) / f; // Intersection with the "right" plane
            // t1 and t2 now contain distances between ray origin and ray-plane intersections

            // We want t1 to represent the nearest intersection, 
            // so if it's not the case, invert t1 and t2
            if (t1 > t2)
            {
                float w = t1; t1 = t2; t2 = w; // swap t1 and t2
            }

            // tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
            if (t2 < tMax)
                tMax = t2;
            // tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
            if (t1 > tMin)
                tMin = t1;

            // And here's the trick :
            // If "far" is closer than "near", then there is NO intersection.
            // See the images in the tutorials for the visual explanation.
            if (tMax < tMin)
                return hit;

        }
        else // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
        {
            if (-e + bbox.min.x > 0.0f || -e + bbox.max.x < 0.0f)
                return hit;
        }
    }

    // Test intersection with the 2 planes perpendicular to the OBB's Y axis
    // Exactly the same thing than above.
    {
        //   Vector3 yaxis = { transform.m1, transform.m5, transform.m9 };
        float e = Vector3DotProduct(yAxis, delta);
        float f = Vector3DotProduct(ray.direction, yAxis);

        if (fabs(f) > 0.001f)
        {
            float t1 = (e + bbox.min.y) / f;
            float t2 = (e + bbox.max.y) / f;

            if (t1 > t2)
            {
                float w = t1;
                t1 = t2;
                t2 = w;
            }

            if (t2 < tMax)
                tMax = t2;
            if (t1 > tMin)
                tMin = t1;
            if (tMin > tMax)
                return hit;
        }
        else
        {
            if (-e + bbox.min.y > 0.0f || -e + bbox.max.y < 0.0f)
                return hit;
        }
    }

    // Test intersection with the 2 planes perpendicular to the OBB's Z axis
    // Exactly the same thing than above.
    {
        //  Vector3 zaxis = { transform.m2, transform.m6, transform.m10 };
        float e = Vector3DotProduct(zAxis, delta);
        float f = Vector3DotProduct(ray.direction, zAxis);

        if (fabs(f) > 0.001f)
        {

            float t1 = (e + bbox.min.z) / f;
            float t2 = (e + bbox.max.z) / f;

            if (t1 > t2) { float w = t1; t1 = t2; t2 = w; }

            if (t2 < tMax)
                tMax = t2;
            if (t1 > tMin)
                tMin = t1;
            if (tMin > tMax)
                return hit;
        }
        else
        {
            if (-e + bbox.min.z > 0.0f || -e + bbox.max.z < 0.0f)
                return hit;
        }
    }

    hit.hit = true;
    hit.distance = tMin;
    hit.point = Vector3Add(ray.position, Vector3Scale(ray.direction, tMin));

    return hit;
}


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

bool View3dDocument::Manipulate(ImGuizmo::OPERATION operation, ImGuizmo::MODE mode, Transform3D& transform, Matrix* deltaMatrix, const Vector3* snap)
{
    Matrix4x4 manipMatrix = Math3D::TransformToMatrix(transform);
    float16 deltaManipMatrix;
    if (deltaMatrix != nullptr)
        deltaManipMatrix = MatrixToFloatV(*deltaMatrix);

    if (ImGuizmo::Manipulate(MatrixToFloat(CameraMatrix), MatrixToFloat(ProjectionMatrix), operation, mode, manipMatrix.M16, deltaManipMatrix.v, (float*)snap))
    {
        transform = Math3D::MatrixToTransform(manipMatrix);
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
    rlMultMatrixf(Math3D::TransformToMatrix(ObjectTransform).M16);
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
