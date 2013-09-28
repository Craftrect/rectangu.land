#pragma once

WEAK_FORWARD(EditorWindow);

#include "ShaderCompiler.h"

class CodeEditor
{
	class Annotation
	{
	public:
		uint32 line;
		String text;
		uint32 style;
	};

	static String gKeywords;

	std::list<Annotation> mAnnotations;
	HWND mWindow;
	EditorWindowWeakPtr mEditor;
	void sendStyle(int style, COLORREF foreground, COLORREF background = RGB(0xFF, 0xFF, 0xFF) , int32 size = -1, const String& face = String());
public:
	CodeEditor(HWND hWindow, EditorWindowWeakPtr editor);

	int send(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) { return SendMessage(mWindow, msg, wParam, lParam); }
	void resize(uint32 w, uint32 h);
	void initDefaultStyle();
	void setAnnotations(const std::list<ShaderCompiler::ShaderError>& errors);

	String getCode();

	static const int StyleErrorAnnotation = 66;
	static const int StyleWarningAnnotation = 67;
};

SHARED_TYPE(CodeEditor);