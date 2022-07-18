#pragma once

#include "editor/conent_document.h"
#include "raylib.h"
#include "rlFPCamera.h"

class View3dDocument : public ContentDocument
{
public:
    DECLARE_DOCUMENT(View3dDocument, ContentDocument)

protected:
     void RenderContent() override;

protected:
    rlFPCamera Camera;
};