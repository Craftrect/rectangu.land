#include "stdafx.h"
#include "MainWindow.h"
#define U_STATIC_IMPLEMENTATION
#include <unicode/gregocal.h>
#include <unicode/datefmt.h>
#include <unicode/uchar.h>

BOOL WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT) {
	MainWindowPtr mwp = std::make_shared<MainWindow>();
	mwp->runLoop();

	return TRUE;
}