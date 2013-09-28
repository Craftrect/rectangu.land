#pragma once

#ifndef min
#define min(a, b) ((a < b) ? a : b)
#endif

#ifndef max
#define max(a, b) ((a < b) ? b : a)
#endif

#include <objidl.h>
#include <gdiplus.h>
#include <MLang.h>
#include <windowsx.h>
#include <commdlg.h>

struct UnicodeBlock
{
	uint32 minChar;
	uint32 maxChar;
	uint32 id;
	String name;
};

class MainWindow
{
	HWND mWindow;
	ULONG_PTR mGdiToken;
	Gdiplus::Font* mDefFont;
	Gdiplus::SolidBrush* mWhiteBrush;
	std::map<uint32, UnicodeBlock> mBlocks;
	std::vector<UnicodeBlock> mBlockVector;
	std::vector<UnicodeBlock> mActiveBlocks;
	UnicodeBlock mCurrentBlock;
	uint32 mFontSize;
	std::wstring mOutFile;

	static BOOL CALLBACK DialogProc(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam);

	bool onMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void drawCharacters(Gdiplus::Graphics* g);
	void saveToFile();
	void initBlocks();
public:
	MainWindow();

	void runLoop();
};

SHARED_TYPE(MainWindow);

#undef min
#undef max