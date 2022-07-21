#pragma once

#include "editor/conent_document.h"
#include "raylib.h"
#include "rlFPCamera.h"

class View3dDocument : public ContentDocument
{
public:
    DECLARE_DOCUMENT(View3dDocument, ContentDocument)

    inline rlFPCamera& GetCamera() { return Camera; }

protected:

    void OnCreate() override;
    void OnUpdate() override;
    void RenderContent() override;
    virtual void RenderScene();

    void OnResized() override;

protected:
    rlFPCamera Camera;
};