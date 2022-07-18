/*
Editor Base System
*/

#include "raylib.h"
#include "editor/conent_render.h"
#include "editor/editor_manager.h"
#include "editor/conent_document.h"
#include "editor/documents/view_2d_document.h"

int main ()
{
	// set up the window
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(1280, 800, "Editor");
	SetTargetFPS(144);

	EditorManager::Setup();

	EditorManager::AddDocument(new View2dDocument());
	EditorManager::AddPanel<EditorPanel>();

	// main loop
	while (!EditorManager::WantQuit())
	{
		EditorManager::Update();

		BeginDrawing();
		ClearBackground(GRAY);
		Vector2 size = MeasureTextEx(GetFontDefault(), "Editor", 60, 60 / 2);
		DrawText("Editor", GetScreenWidth() / 2 - int(size.x) / 2, GetScreenHeight() / 2 - int(size.y) / 2, 60, ColorAlpha(DARKGRAY,0.5f));

		EditorManager::Render();

		EndDrawing();
	}

	EditorManager::Shutdown();
	CloseWindow();
	return 0;
}