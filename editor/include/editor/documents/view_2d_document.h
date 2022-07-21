#pragma once

#include "editor/conent_document.h"
#include "raylib.h"

class View2dDocument : public ContentDocument
{
public:
    DECLARE_DOCUMENT(View2dDocument, ContentDocument)

    inline Camera2D& GetCamera() { return ViewCamera; }
    Vector2 GetWorldMousePos() const;

protected:
    void RenderContent() override;
    virtual void RenderScene();

    void OnCreate() override;
    void OnUpdate() override;

    void OnResized() override;

protected:
    Camera2D ViewCamera = { 0 };
};