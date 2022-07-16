/*
Editor Base System
*/

#include "raylib.h"
#include "editor/conent_render.h"
#include "editor/editor_manager.h"
#include "editor/conent_document.h"

// void RenderCube()
// {
// 	DrawCube(Vector3{ 0,0.5f,0 }, 1, 1, 1, RED);
// 	DrawPlane(Vector3Zero(), Vector2{ 100,100 }, BROWN);
// 
// 	if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
// 	{
// 		Ray pickRay = ContentRender::GetMouseRayInContent();
// 		RayCollision collision = GetRayCollisionQuad(pickRay, Vector3{ 100,0,100 }, Vector3{ -100,0,100 }, Vector3{ -100,0,-100 }, Vector3{ 100,0,-100 });
// 		if (collision.hit && !EditorManager::WantMouse())
// 		{
// 			DrawSphere(collision.point, 0.25f, RED);
// 		}
// 	}
// 	
// }

// void RenderFPS()
// {
// 	DrawFPS(0, 0);
// }

int main ()
{
	// set up the window
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(1280, 800, "Editor");
	SetTargetFPS(144);

	//ContentRender::Setup();
	EditorManager::Setup();

	EditorManager::AddDocument(new ContentDocument());
	EditorManager::AddPanel<EditorPanel>();
	EditorManager::AddPanel<EditorPanel>();

// 	ContentRender::Add3dRenderCallback(RenderCube);
// 	ContentRender::Add2dRenderCallback(RenderFPS);

	// main loop
	while (!EditorManager::WantQuit())
	{
		EditorManager::Update();
		//ContentRender::Update();

		BeginDrawing();
		ClearBackground(GRAY);
		Vector2 size = MeasureTextEx(GetFontDefault(), "Editor", 60, 60 / 2);
		DrawText("Editor", GetScreenWidth() / 2 - int(size.x) / 2, GetScreenHeight() / 2 - int(size.y) / 2, 60, ColorAlpha(DARKGRAY,0.5f));

		//ContentRender::Render();
		EditorManager::Render();

		EndDrawing();
	}

	//ContentRender::Shutdown();
	EditorManager::Shutdown();
	CloseWindow();
	return 0;
}