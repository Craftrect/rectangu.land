#include "StdAfx.h"
#include "EditorWindow.h"
#include "resource.h"
#include "ShaderCompiler.h"
#include <CommCtrl.h>
#include <commdlg.h>
#include <windowsx.h>

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

EditorWindow::EditorWindow() {
	INITCOMMONCONTROLSEX ice;
	ice.dwICC = 0xFFFFFFFF;
	ice.dwSize = sizeof(ice);

	ParamRegistration regs [] = {
		{ "PER_INSTANCE_BUFFER", 0, 0, "" },
		{ "PER_FRAME_BUFFER", 1, 0, "" },
		{ "cbuffer_Matrix", 2, 0, "" },
		{ "tex_Block", 16, 0, "" },
	};

	for (auto& reg : regs) {
		mParamsAvailable.push_back(reg);
	}

	InitCommonControlsEx(&ice);

	LoadLibrary(L"riched20.dll");

	mWindow = CreateDialogParam(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDD_FORMVIEW), GetDesktopWindow(), EditorWindow::WndProc, (LPARAM)this);

	LoadLibrary(L"SciLexer.dll");
}

BOOL WINAPI EditorWindow::WndProc(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	EditorWindow* ew = (EditorWindow*) GetProp(hWindow, L"PROP_WND_INSTANCE");
	if (ew != nullptr) {
		ew->onMessage(hWindow, uMsg, wParam, lParam);
	}

	switch (uMsg) {
	case WM_CLOSE:
		{
			if (ew == nullptr) {
				EndDialog(hWindow, 0);
			}
		}
		break;

	case WM_INITDIALOG:
		{
			SetProp(hWindow, L"PROP_WND_INSTANCE", (HANDLE) lParam);
			EditorWindow* wnd = (EditorWindow*) lParam;
			wnd->onMessage(hWindow, uMsg, wParam, lParam);
			return TRUE;
		}
	}

	return FALSE;
}

void EditorWindow::initVisual() {
	HWND hEditor = CreateWindowEx(0, L"Scintilla", L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_CLIPCHILDREN,
		10, 10, 500, 400, mWindow, nullptr, GetModuleHandle(nullptr), NULL);

	mCodeEditor = std::make_shared<CodeEditor>(hEditor, shared_from_this());

	RECT rc;
	GetClientRect(mWindow, &rc);

	mCodeEditor->resize(rc.right - rc.left - 250, rc.bottom - rc.top);
	mCodeEditor->initDefaultStyle();
}

void EditorWindow::run() {
	ShowWindow(mWindow, SW_SHOW);

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

BOOL WINAPI EditorWindow::MapVarDialogMsgProc(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	EditorWindow* ew = (EditorWindow*) GetProp(hWindow, L"PROP_WND_INSTANCE");
	if (ew != nullptr) {
		ew->onMapDialogMessage(hWindow, uMsg, wParam, lParam);
	}

	switch (uMsg) {
	case WM_INITDIALOG:
		{
			SetProp(hWindow, L"PROP_WND_INSTANCE", (HANDLE) lParam);
			EditorWindow* wnd = (EditorWindow*) lParam;
			wnd->onMapDialogMessage(hWindow, uMsg, wParam, lParam);
			return TRUE;
		}
	}

	return FALSE;
}

void EditorWindow::onMapDialogMessage(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG:
		{
			for (auto& param : mParamsAvailable) {
				ListBox_AddString(GetDlgItem(hWindow, IDC_AVAILABLE), param.displayName->c_str());
			}
		}
		break;

	case WM_COMMAND:
		{
			if (HIWORD(wParam) == BN_CLICKED) {
				switch (LOWORD(wParam)) {
				case IDC_BUTTON_REG:
					{
						String text = getDlgText(IDC_INP_REGISTER, hWindow);
						if (text.length() < 2) {
							break;
						}

						if (text[0] != L'c') {
							break;
						}

						std::wstringstream strm;
						strm << text.substring(1);
						uint32 regNumber = 0;
						if (!(strm >> regNumber)) {
							break;
						}

						bool isCustom = Button_GetCheck(GetDlgItem(hWindow, IDC_CHECK_CUSTOM)) == TRUE;

						if (isCustom == false) {
							int32 selected = ListBox_GetCurSel(GetDlgItem(hWindow, IDC_AVAILABLE));
							if (selected < 0 || selected >= (int32) mParamsAvailable.size()) {
								break;
							}

							ParamRegistration reg;
							reg.displayName = mParamsAvailable[selected].displayName;
							reg.displayName += " = ";
							reg.displayName += text;
							reg.globalParam = mParamsAvailable[selected].globalParam;
							reg.creg = regNumber;
							reg.isCustomVar = isCustom;


							mParamRegistered.push_back(reg);
							mParamsAvailable.erase(mParamsAvailable.begin() + selected);

							ListBox_DeleteString(GetDlgItem(hWindow, IDC_AVAILABLE), selected);
							ListBox_AddString(GetDlgItem(hWindow, IDC_REGISTERED), reg.displayName->c_str());
						} else {
							ParamRegistration reg;
							reg.displayName = getDlgText(IDC_INP_CUSTOM, hWindow);
							reg.displayName += " = ";
							reg.displayName += text;
							reg.creg = regNumber;
							reg.isCustomVar = isCustom;
							reg.customName = getDlgText(IDC_INP_CUSTOM, hWindow);

							mParamRegistered.push_back(reg);
							ListBox_AddString(GetDlgItem(hWindow, IDC_REGISTERED), reg.displayName->c_str());
						}
					}
					break;
				}
			}
		}
		break;

	case WM_CLOSE:
		{
			EndDialog(hWindow, 0);
		}
		break;
	}
}

void EditorWindow::onMessage(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CLOSE:
		{
			if (hWindow == mWindow) {
				PostQuitMessage(0);
			} else {
				EndDialog(hWindow, 0);
			}
		}
		break;

	case WM_SIZE:
		{
			RECT rc;
			GetClientRect(mWindow, &rc);
			mCodeEditor->resize(rc.right - rc.left - 250, rc.bottom - rc.top);
			SetWindowPos(GetDlgItem(mWindow, IDC_OUTPUT), nullptr, 0, rc.bottom - 150, rc.right - rc.left, 150, SWP_NOZORDER);
			break;
		}

	case WM_COMMAND:
		{
			if (HIWORD(wParam) == BN_CLICKED) {
				uint32 id = LOWORD(wParam);
				switch (id) {
				case IDC_BROWSE_BUTTON:
					{
						std::vector<wchar_t> filePath(2048);

						OPENFILENAME ofn = { 0 };
						ofn.lStructSize = sizeof(OPENFILENAME);
						ofn.hwndOwner = mWindow;
						ofn.hInstance = GetModuleHandle(nullptr);
						ofn.lpstrFilter = L"Binary Shader File\0*.bsx\0\0";
						ofn.lpstrFile = filePath.data();
						ofn.nMaxFile = 2048;

						std::vector<wchar_t> curDir(2048);
						GetCurrentDirectory(2048, curDir.data());
						ofn.lpstrInitialDir = curDir.data();
						ofn.lpstrTitle = L"Select output location";
						ofn.Flags |= OFN_PATHMUSTEXIST;
						ofn.lpstrDefExt = L"xbs";

						GetSaveFileName(&ofn);

						SendDlgItemMessage(hWindow, IDC_BUILD_PATH, WM_SETTEXT, 0, (LPARAM)filePath.data());
					}
					break;

				case IDC_MAP_VARS:
					{
						DialogBoxParam(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDD_VAR_DIALOG), mWindow, &EditorWindow::MapVarDialogMsgProc, (LPARAM)this);
					}
					break;

				case IDC_BUILD_BUTTON:
					{
						clearOutput();
						ShowWindow(GetDlgItem(mWindow, IDC_BUILD_STATUS), SW_HIDE);
						String code = mCodeEditor->getCode();
						code += "\n";
						String entryPoint = getDlgText(IDC_ENTRY_POINT);
						String output = getDlgText(IDC_BUILD_PATH);
						String model = "5_0";
						String shader = "ps_";
						uint32 shaderType = 0;
						uint32 shaderModel = 5;

						if (Button_GetCheck(GetDlgItem(mWindow, IDC_MODEL_3))) {
							model = "3_0";
							shaderModel = 3;
						} else if (Button_GetCheck(GetDlgItem(mWindow, IDC_MODEL_4))) {
							model = "4_0";
							shaderModel = 4;
						}

						if (Button_GetCheck(GetDlgItem(mWindow, IDC_TAR_VERTEX))) {
							shader = "vs_";
							shaderType = 1;
						} else if (Button_GetCheck(GetDlgItem(mWindow, IDC_TAR_GEOM))) {
							shader = "gs_";
							shaderType = 2;
						} else if (Button_GetCheck(GetDlgItem(mWindow, IDC_TAR_DOMAIN))) {
							shader = "ds_";
							shaderType = 3;
						} else if (Button_GetCheck(GetDlgItem(mWindow, IDC_TAR_HULL))) {
							shader = "hs_";
							shaderType = 4;
						}

						String target = shader + model;

						ShaderCompiler comp;
						comp.compile(code, entryPoint, target);
						mCodeEditor->setAnnotations(comp.getErrors());

						SetWindowText(GetDlgItem(mWindow, IDC_BUILD_STATUS), comp.getErrors().size() > 0 ? L"Build failed" : L"Build succeeded");
						ShowWindow(GetDlgItem(mWindow, IDC_BUILD_STATUS), SW_SHOW);

						for (auto& err : comp.getErrors()) {
							appendOutput(err.message);
						}

						if (comp.succeeded() && output.empty() == false) {
							writeShader(output, comp.getCode(), shaderType, shaderModel);
						}
					}
					break;
				}
			}
			break;
		}

	case WM_INITDIALOG:
		{
			SendDlgItemMessage(hWindow, IDC_TAR_PIXEL, BM_SETCHECK, BST_CHECKED, 0);
			SendDlgItemMessage(hWindow, IDC_MODEL_5, BM_SETCHECK, BST_CHECKED, 0);
			break;
		}
	}
}

String EditorWindow::getDlgText(int32 dlgItem, HWND hWindow) {
	int32 len = SendMessage(GetDlgItem((hWindow == (HWND)-1) ? mWindow : hWindow, dlgItem), WM_GETTEXTLENGTH, 0, 0);
	if (len == 0) {
		return String();
	}

	std::vector<wchar_t> strData(len + 2);
	SendMessage(GetDlgItem((hWindow == (HWND) - 1) ? mWindow : hWindow, dlgItem), WM_GETTEXT, len + 1, (LPARAM) strData.data());
	return strData.data();
}

void EditorWindow::clearOutput() {
	mOutput = "";

	SendMessage(GetDlgItem(mWindow, IDC_OUTPUT), WM_SETTEXT, 0, (LPARAM) mOutput->c_str());
}

void EditorWindow::appendOutput(const String& message) {
	mOutput += message;
	mOutput += "\n";

	SendMessage(GetDlgItem(mWindow, IDC_OUTPUT), WM_SETTEXT, 0, (LPARAM) mOutput->c_str());
}

void EditorWindow::writeShader(const String& file, const std::vector<char>& code, uint32 shader, uint32 model) {
	std::ofstream out(file.toMultibyte().c_str(), std::ios::out | std::ios::binary);
	struct XBSHeader
	{
		uint32 signature;
		uint32 version;
		uint32 shaderType;
		uint32 shaderModel;
		uint32 numPermutations;
		uint32 ofsPermutationTable;
		uint32 nConstTable;
		uint32 ofsConstTable;
	} header;

	struct XBSConstTableEntry
	{
		uint32 flags;
		uint32 ofsCustomName;
		uint32 lenCustomName;
		uint32 globalId;
		uint32 regNumber;
		uint32 regType;
	};

	header.signature = 'XBSH';
	header.version = 0;
	header.numPermutations = 1;
	header.shaderModel = model;
	header.shaderType = shader;
	header.nConstTable = mParamRegistered.size();
	header.ofsConstTable = sizeof(XBSHeader);
	header.ofsPermutationTable = sizeof(XBSHeader) + header.nConstTable * sizeof(XBSConstTableEntry);

	out.write((const char*) &header, sizeof(header));
	uint32 paramTable = 'XBSC';
	out.write((const char*) &paramTable, sizeof(uint32));

	String customVarStr = "";

	for (auto& reg : mParamRegistered) {
		XBSConstTableEntry e;
		e.flags = reg.isCustomVar ? 1 : 0;
		e.globalId = reg.isCustomVar ? -1 : reg.globalParam;
		e.regNumber = reg.creg;
		e.regType = 'c';
		e.lenCustomName = reg.isCustomVar ? reg.customName.length() : 0;
		e.ofsCustomName = reg.isCustomVar ? customVarStr.length() : 0;

		customVarStr += reg.customName;
		out.write((const char*) &e, sizeof(XBSConstTableEntry));
	}

	out.write(customVarStr.toMultibyte().c_str(), customVarStr.length());

	auto curPos = out.tellp();
	header.ofsPermutationTable = (uint32)curPos;
	out.seekp(0);
	out.write((const char*) &header, sizeof(header));
	out.seekp(curPos);

	struct XBSPermutation
	{
		uint32 signature;
		uint32 nMacros;
		uint32 ofsMacros;
		uint32 sizeCode;
		uint32 ofsCode;
	};

	XBSPermutation mainPerm;
	mainPerm.signature = 'XBSP';
	mainPerm.nMacros = 0;
	mainPerm.ofsMacros = 0;
	mainPerm.sizeCode = code.size();
	mainPerm.ofsCode = (uint32)curPos + header.numPermutations * sizeof(XBSPermutation);

	out.write((const char*) &mainPerm, sizeof(mainPerm));
	out.write(code.data(), code.size());

	out.close();
}