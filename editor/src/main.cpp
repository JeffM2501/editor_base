/*
Editor Base System
*/

#include "raylib.h"
#include "editor/conent_render.h"
#include "editor/editor_manager.h"
#include "editor/conent_document.h"
#include "editor/documents/view_2d_document.h"
#include "editor/documents/view_3d_document.h"
#include "editor/panels/test_panel.h"
#include "editor/panels/log_panel.h"

#ifdef _WIN32
#ifdef __cplusplus
extern "C" {
#endif

    __declspec(dllexport) unsigned long NvOptimusEnablement = 1;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#ifdef __cplusplus
}
#endif
#endif

int main ()
{
    EditorManager::AddPanel<TestPanel>();
    EditorManager::AddPanel<LogPanel>();

	// set up the window
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(1280, 800, "Editor");
	SetTargetFPS(144);

	EditorManager::Setup();

	EditorManager::AddDocument(new View3dDocument());
	EditorManager::AddDocument(new View2dDocument());


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