#pragma once

#include "CodeEditor.h"

class EditorWindow : public std::enable_shared_from_this<EditorWindow>
{
	class ParamRegistration
	{
	public:
		String displayName;
		uint32 globalParam;
		uint32 creg;
		String customName;
		bool isCustomVar;
	};

	std::vector<ParamRegistration> mParamsAvailable;
	std::vector<ParamRegistration> mParamRegistered;

	HWND mWindow;
	CodeEditorPtr mCodeEditor;
	String mOutput;

	static BOOL WINAPI WndProc(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL WINAPI MapVarDialogMsgProc(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void onMessage(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void onMapDialogMessage(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam);
	String getDlgText(int32 dlgItem, HWND hwnd = (HWND)-1);
	void clearOutput();
	void appendOutput(const String& line);
	void writeShader(const String& file, const std::vector<char>& code, uint32 shaderType, uint32 model);
public:
	EditorWindow();

	void initVisual();

	void run();
};

SHARED_TYPE(EditorWindow);