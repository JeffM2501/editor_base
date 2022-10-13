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

#include "editor/utils/platform.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

#include <iostream>
#include <stdio.h>

using namespace rapidjson;

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

struct EditorSettings
{
public:
	Vector2 WindowPos = { -1, -1 };
	Vector2 WindowSize = { 1280,800 };

	std::string PrefsFile()
	{
		std::string dir = PlatformUtils::GetUserPrefsDir("Editor_Base");

		if (!DirectoryExists(dir.c_str()))
			PlatformUtils::CreateDirecory(dir.c_str());
		dir += "editor_prefs.json";

		return dir;
	}

	void Load()
	{
		std::string file = PrefsFile();

		if (!FileExists(file.c_str()))
			return;

		char* text = LoadFileText(file.c_str());
		Document doc;
		doc.Parse(text);
		
		UnloadFileText(text);
	}
	
	void Update()
	{
		WindowPos = GetWindowPosition();
		WindowSize.x = GetScreenWidth();
		WindowSize.y = GetScreenHeight();
	}

	void Save()
	{
		Update();

		std::string file = PrefsFile();
		if (FileExists(file.c_str()))
			remove(file.c_str());

		Document doc;
		doc.SetObject();

		Value editorSettings;
		editorSettings.SetObject();

		Value windowPos;
		windowPos.SetObject();
        Value px;
        Value py;

		px.SetInt(WindowPos.x);
		py.SetInt(WindowPos.y);

		windowPos.AddMember("X", px, doc.GetAllocator());
		windowPos.AddMember("Y", py, doc.GetAllocator());

        editorSettings.AddMember("WindowPos", windowPos, doc.GetAllocator());

        Value windowSize;
		windowSize.SetObject();

        Value sx;
        Value sy;

        sx.SetInt(WindowSize.x);
        sy.SetInt(WindowSize.y);

		windowSize.AddMember("X", sx, doc.GetAllocator());
		windowSize.AddMember("Y", sy, doc.GetAllocator());

        editorSettings.AddMember("WindowSize", windowSize, doc.GetAllocator());

		doc.AddMember("EditorSettings", editorSettings, doc.GetAllocator());

        StringBuffer buffer;
        PrettyWriter<StringBuffer> writer(buffer);
		doc.Accept(writer);

		SaveFileText(file.c_str(), const_cast<char*>(buffer.GetString()));
	}
};

int main ()
{
    EditorManager::AddPanel<TestPanel>();
    EditorManager::AddPanel<LogPanel>();

	EditorSettings settings;
	settings.Load();

	// set up the window
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(settings.WindowSize.x, settings.WindowSize.y, "Editor");
	SetTargetFPS(144);

	if (settings.WindowPos.x != -1 && settings.WindowPos.y != -1)
		SetWindowPosition(settings.WindowPos.x, settings.WindowPos.y);

	EditorManager::Setup();

	EditorManager::AddDocument(new View3dDocument());
	EditorManager::AddDocument(new View2dDocument());

	PlatformUtils::GetUserPrefsDir("Editor");

	// main loop
	while (!EditorManager::WantQuit())
	{
		if (IsWindowResized())
			settings.Save();

		EditorManager::Update();

		BeginDrawing();
		ClearBackground(GRAY);
		Vector2 size = MeasureTextEx(GetFontDefault(), "Editor", 60, 60 / 2);
		DrawText("Editor", GetScreenWidth() / 2 - int(size.x) / 2, GetScreenHeight() / 2 - int(size.y) / 2, 60, ColorAlpha(DARKGRAY,0.5f));

		EditorManager::Render();

		EndDrawing();
	}

	settings.Save();
	EditorManager::Shutdown();
	CloseWindow();
	return 0;
}