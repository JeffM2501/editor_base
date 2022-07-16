#pragma once

#include "raylib.h"
#include "imgui.h"
#include "rlFPCamera.h"
#include <functional>
#include <vector>

class ContentRender
{
public:
    void Setup();
    void Shutdown();

    void SetContentArea(ImVec2 origin, ImVec2 size);

    using RenderCallback = std::function<void(ContentRender& rendrer)>;

    void Add2dRenderCallback(RenderCallback callback);
    void Add3dRenderCallback(RenderCallback callback);

    rlFPCamera& GetCamera();

    virtual void Update();
    virtual void Render();

    Vector2 GetMousePosInContent();
    Ray GetMouseRayInContent();
    Ray GetMouseRayInContent(const Vector2& pos);

protected:
    std::vector<RenderCallback> RenderCallbacks2d;
    std::vector<RenderCallback> RenderCallbacks3d;

    RenderTexture ContentTexture = { 0 };

    Vector2 ContentOrign = { 0 };

    rlFPCamera ViewCamera;
};
