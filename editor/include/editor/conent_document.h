#pragma once

#include "editor/editor_document.h"
#include "raylib.h"

class ContentDocument : public EditorDocument
{
public:
    DECLARE_DOCUMENT(ContentDocument, EditorDocument)

    void Create() override;
    void Close() override;

protected:
    void OnShow() override;

    virtual void RenderContent() {};

protected:
    RenderTexture ContentTexture = { 0 };
};