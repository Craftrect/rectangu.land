#include "StdAfx.h"
#include "EditorWindow.h"


BOOL WINAPI WinMain(HINSTANCE inst, HINSTANCE prev, LPSTR cmdLine, INT nShow) {
	EditorWindowPtr editor = std::make_shared<EditorWindow>();

	editor->initVisual();
	editor->run();

	return 0;
}