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

    virtual void RenderContent() {}
    virtual void ShowOverlay() {}
    virtual void OnResized() {}

    Vector2 GetContentSize() const;
    Vector2 GetLocalMousePosition() const;

protected:
    RenderTexture ContentTexture = { 0 };
    Color ClearColor = SKYBLUE;

    Vector2 LocalMousePosition = { 0 };

    Rectangle ContentRect = { 0, 0, 0, 0 };
};