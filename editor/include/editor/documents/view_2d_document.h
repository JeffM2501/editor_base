#pragma once

#include "editor/conent_document.h"
#include "raylib.h"

class View2dDocument : public ContentDocument
{
public:
    DECLARE_DOCUMENT(View2dDocument, ContentDocument)

protected:
    void RenderContent() override;

    void OnCreate() override;
    void OnUpdate() override;

    void OnResized() override;

    Vector2 GetWorldMousePos() const;

    inline Camera2D& GetCamera() { return Camera; }

protected:
    Camera2D Camera = { 0 };
};