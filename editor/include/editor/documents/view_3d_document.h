#pragma once

#include "editor/conent_document.h"
#include "raylib.h"
#include "raymath.h"
#include "rlFPCamera.h"
#include "ImGuizmo.h"

class View3dDocument : public ContentDocument
{
public:
    DECLARE_DOCUMENT(View3dDocument, ContentDocument)

    inline rlFPCamera& GetCamera() { return Camera; }

    Ray GetMouseRay();

protected:
    void OnCreate() override;
    void OnShow() override;
    void OnUpdate() override;
    void RenderContent() override;
    virtual void RenderScene();

    void OnResized() override;

    void ShowOverlay() override;

    bool Manipulate(ImGuizmo::OPERATION operation, ImGuizmo::MODE mode, Matrix& matrix, Matrix* deltaMatrix = nullptr, const Vector3* snap = nullptr);

protected:
    rlFPCamera Camera;

    Matrix ObjectMatrix;
    
    Matrix ProjectionMatrix;
    Matrix CameraMatrix;
};