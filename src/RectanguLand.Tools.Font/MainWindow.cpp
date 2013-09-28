#include "stdafx.h"
#include "MainWindow.h"
#include "resource.h"
#include <unicode/uchar.h>

#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")  

using namespace Gdiplus;

MainWindow::MainWindow() {
	CoInitialize(nullptr);

	mFontSize = 18;
	mCurrentBlock = mBlocks[UBLOCK_BASIC_LATIN];

	GdiplusStartupInput input;

	GdiplusStartup(&mGdiToken, &input, nullptr);

	mDefFont = new Font(L"Arial", 18.0f, FontStyleRegular, UnitPixel);
	mWhiteBrush = new SolidBrush(Color(255, 255, 255));

	mWindow = CreateDialogParam(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDD_FORMVIEW), GetDesktopWindow(), &MainWindow::DialogProc, (LPARAM)this);
	SendMessage(mWindow, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_MAIN_ICON)));
	SendMessage(mWindow, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_MAIN_ICON)));
	Edit_SetText(GetDlgItem(mWindow, IDC_FONT_DESC), L"Arial, 18px, Regular");
}

void MainWindow::runLoop() {
	ShowWindow(mWindow, SW_SHOW);

	MSG msg;
	int32 ret = 0;
	while(GetMessage(&msg, nullptr, 0, 0) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

BOOL CALLBACK MainWindow::DialogProc(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_INITDIALOG) {
		MainWindow* mw = reinterpret_cast<MainWindow*>(lParam);
		SetProp(hWindow, L"PROP_WND_INSTANCE", (HANDLE) mw);
		mw->mWindow = hWindow;
		return mw->onMessage(uMsg, wParam, lParam);
	}

	MainWindow* mw = reinterpret_cast<MainWindow*>(GetProp(hWindow, L"PROP_WND_INSTANCE"));

	return mw->onMessage(uMsg, wParam, lParam);
}

bool MainWindow::onMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch(uMsg) {
	case WM_INITDIALOG:
		initBlocks();
		return TRUE;

	case WM_DRAWITEM:
		{
			DRAWITEMSTRUCT* dws = reinterpret_cast<DRAWITEMSTRUCT*>(lParam);
			if(dws->itemAction == ODA_DRAWENTIRE) {
				Graphics* g = Graphics::FromHDC(dws->hDC);

				g->SetTextRenderingHint(TextRenderingHintAntiAlias);

				g->Clear(Color(255, 0, 0, 0));
				drawCharacters(g);

				g->Flush();
				delete g;
			}
		}
		return TRUE;

	case WM_CLOSE:
		{
			PostQuitMessage(0);
			EndDialog(mWindow, 0);
		}
		return TRUE;

	case WM_COMMAND:
		{
			if(HIWORD(wParam) == 0) {
				if(LOWORD(wParam) == ID_FILE_EXIT) {
					PostQuitMessage(0);
					EndDialog(mWindow, 0);
					return TRUE;
				} else if(LOWORD(wParam) == ID_FILE_SAVEAS) {
					std::vector<wchar_t> filePath(2048);

					OPENFILENAME ofn = { 0 };
					
					ofn.lStructSize = sizeof(OPENFILENAME);
					ofn.hwndOwner = mWindow;
					ofn.hInstance = GetModuleHandle(nullptr);
					ofn.lpstrFilter = L"Font Information File\0*.fti\0\0";
					ofn.lpstrFile = filePath.data();
					ofn.nMaxFile = 2048;

					std::vector<wchar_t> curDir(2048);
					GetCurrentDirectory(2048, curDir.data());
					ofn.lpstrInitialDir = curDir.data();
					ofn.lpstrTitle = L"Select output location";
					ofn.Flags |= OFN_PATHMUSTEXIST;
					ofn.lpstrDefExt = L"fti";

					if(GetSaveFileName(&ofn) == FALSE) {
						return TRUE;
					}

					mOutFile = filePath.data();
					saveToFile();
				}
			}

			if(HIWORD(wParam) == BN_CLICKED) {
				if(LOWORD(wParam) == IDC_CP_ADD) {
					int32 index = ListBox_GetCurSel(GetDlgItem(mWindow, IDC_CP_AVAIL_LIST));
					mCurrentBlock = mBlockVector[index];
					RedrawWindow(GetDlgItem(mWindow, IDC_PREVIEW), nullptr, nullptr, RDW_INVALIDATE);
					ListBox_DeleteString(GetDlgItem(mWindow, IDC_CP_AVAIL_LIST), index);
					auto info = mBlockVector[index];
					mBlockVector.erase(mBlockVector.begin() + index);
					mActiveBlocks.push_back(info);

					ListBox_AddString(GetDlgItem(mWindow, IDC_CP_USED), info.name->c_str());
				}

				if(LOWORD(wParam) == IDC_FONT_SELECT) {
					LOGFONT logFont;
					CHOOSEFONT cf = { 0 };
					cf.lStructSize = sizeof(CHOOSEFONT);
					cf.hwndOwner = mWindow;
					cf.hDC = nullptr;
					cf.lpLogFont = &logFont;
					cf.iPointSize = 120;
					BOOL res = ChooseFont(&cf);
					if(res == FALSE) {
						break;
					}

					HDC hDC = GetDC(nullptr);
					auto height = -MulDiv(logFont.lfHeight, GetDeviceCaps(hDC, LOGPIXELSY), 72);
					

					std::wstringstream fontDesc;
					fontDesc << logFont.lfFaceName << L", " << height << L" px, ";
					switch(logFont.lfWeight) {
					case FW_THIN:
						fontDesc << L"Thin";
						break;
						
					case FW_EXTRALIGHT:
						fontDesc << L"Extra Light";
						break;

					case FW_LIGHT:
						fontDesc << L"Light";
						break;

					case FW_NORMAL:
						fontDesc << L"Regular";
						break;

					case FW_MEDIUM:
						fontDesc << L"Medium";
						break;

					case FW_SEMIBOLD:
						fontDesc << L"Semibold";
						break;

					case FW_BOLD:
						fontDesc << L"Bold";
						break;

					case FW_EXTRABOLD:
						fontDesc << L"Extra bold";
						break;

					case FW_HEAVY:
						fontDesc << L"Heavy";
						break;
					}

					Edit_SetText(GetDlgItem(mWindow, IDC_FONT_DESC), fontDesc.str().c_str());

					delete mDefFont;

					mDefFont = new Font(hDC, &logFont);

					ReleaseDC(nullptr, hDC);

					mFontSize = height;

					RedrawWindow(GetDlgItem(mWindow, IDC_PREVIEW), nullptr, nullptr, RDW_INVALIDATE);
				}
			}
		}
		return TRUE;

	default:
		return FALSE;
	}

	return TRUE;
}

void MainWindow::drawCharacters(Graphics* g) {
	uint32 w = 0;
	uint32 h = 0;

	RECT wndRect;
	GetClientRect(GetDlgItem(mWindow, IDC_PREVIEW), &wndRect);

	uint32 maxWidth = wndRect.right - wndRect.left;

	RectF cRect;
	for(uint32 i = mCurrentBlock.minChar; i < mCurrentBlock.maxChar; ++i) {
		g->MeasureString((wchar_t*)&i, 1, mDefFont, PointF(0, 0), &cRect);
		float width = cRect.Width;
		float height = cRect.Height;

		if(w + width >= maxWidth) {
			h += mFontSize + 3;
			w = 0;
		}

		g->DrawString((wchar_t*)&i, 1, mDefFont, PointF((float)w, (float)h), mWhiteBrush);
		w += (uint32)ceil(width) + 2;
	}
}

void MainWindow::saveToFile() {
	std::ofstream outFile(mOutFile, std::ios::binary);
	std::stringstream out;
	uint32 signature = 'FNTI';
	out.write((const char*)&signature, sizeof(uint32));
	
	struct FNTIHeader
	{
		uint32 numPages;
		uint32 ofsPages;
	} header;

	header.numPages = mActiveBlocks.size();
	header.ofsPages = sizeof(FNTIHeader) + sizeof(uint32);
	out.write((const char*)&header, sizeof(FNTIHeader));

	struct FNTIBlockDesc
	{
		uint32 id;
		uint32 numChars;
		uint32 ofsChars;
		uint32 bmpWidth;
		uint32 bmpHeight;
		uint32 ofsBmp;
		wchar_t minChar;
		wchar_t maxChar;
		wchar_t pageName[64];
	};

	std::vector<FNTIBlockDesc> descriptions(mActiveBlocks.size());

	uint32 counter = 0;
	for(auto& block : mActiveBlocks) {
		FNTIBlockDesc desc;
		memset(&desc, 0, sizeof(desc));
		desc.id = block.id;
		desc.bmpWidth = 256;
		desc.bmpHeight = 256;
		desc.minChar = (wchar_t)block.minChar;
		desc.maxChar = (wchar_t)block.maxChar;
		wcsncpy_s(desc.pageName, block.name->c_str(), 64);
		desc.pageName[63] = L'\0';

		out.write((const char*)&desc, sizeof(desc));
		descriptions[counter++] = desc;
	}

	std::vector<Bitmap*> bitmaps(mActiveBlocks.size());

	Bitmap* tmp = new Bitmap(mFontSize * 2, mFontSize);
	Graphics* gchar = Graphics::FromImage(tmp);
	gchar->SetTextRenderingHint(TextRenderingHintAntiAlias);

	for(uint32 i = 0; i < mActiveBlocks.size(); ++i) {
		auto& desc = descriptions[i];
		desc.ofsChars = (uint32)out.tellp();

		Bitmap* bmp = new Bitmap(256, 256);
		bitmaps[i] = bmp;

		Graphics* g = Graphics::FromImage(bmp);
		g->Clear(Color::Transparent);
		g->SetTextRenderingHint(TextRenderingHintAntiAlias);

		uint32 curW = 0, curH = 0;
	
		for(uint32 j = mActiveBlocks[i].minChar; j < mActiveBlocks[i].maxChar; ++j) {
			char c = (char)j;
			wchar_t wc = (wchar_t)j;

			RectF rcChar;
			g->MeasureString(&wc, 1, mDefFont, PointF(0, 0), &rcChar);
			float width = rcChar.Width;
			if(curW + width > bmp->GetWidth()) {
				curW = 0;
				curH += mFontSize + 3;
			}

			g->DrawString(&wc, 1, mDefFont, PointF((float)curW, (float)curH), mWhiteBrush);

			gchar->Clear(Color::Black);
			gchar->DrawString(&wc, 1, mDefFont, PointF(0, 0), mWhiteBrush);

			Color pxl, pxr;
			uint32 ofsl = 0, ofsr = 0;
			bool lfound = false, rfound = false;

			for(uint32 l = 0; l < mFontSize * 2; ++l) {
				for(uint32 h = 0; h < mFontSize; ++h) {
					uint32 r = (2 * mFontSize - 1) - l;

					if(lfound == false) {
						tmp->GetPixel(l, h, &pxl);
						if(pxl.GetRed() > 5) {
							lfound = true;
							ofsl = l;
						}
					}

					if(rfound == false) {
						tmp->GetPixel(r, h, &pxr);
						if(pxr.GetRed() > 5) {
							rfound = true;
							ofsr = r;
						}
					}

					if(lfound && rfound) {
						break;
					}
				}

				if(lfound && rfound) {
					break;
				}
			}

			if(lfound == false || rfound == false || (ofsl >= ofsr)) {
				continue;
			}

			uint16 chrWidth = ofsr - ofsl + 1;

			float txs = (curW + ofsl) / (float)bmp->GetWidth();
			float txe = (curW + width - ofsr) / (float)bmp->GetWidth();
			float tys = curH / (float)bmp->GetHeight();
			float tye = (curH + mFontSize) / (float)bmp->GetHeight();

			curW += (uint32)ceil(width) + 2;

			++desc.numChars;
			out.write((const char*)&wc, sizeof(wchar_t));
			out.write((const char*)&chrWidth, sizeof(uint16));
			out.write((const char*)&txs, sizeof(float));
			out.write((const char*)&txe, sizeof(float));
			out.write((const char*)&tys, sizeof(float));
			out.write((const char*)&tye, sizeof(float));
		}

		delete g;
	}

	delete gchar;
	delete tmp;

	for(uint32 i = 0; i < mActiveBlocks.size(); ++i) {
		auto& desc = descriptions[i];
		desc.ofsBmp = (uint32)out.tellp();
		Bitmap* bmp = bitmaps[i];
		BitmapData data;
		bmp->LockBits(&Rect(0, 0, bmp->GetWidth(), bmp->GetHeight()), 0, PixelFormat32bppARGB, &data);
		out.write((const char*)data.Scan0, bmp->GetWidth() * bmp->GetHeight());
		bmp->UnlockBits(&data);

		delete bmp;
	}

	out.seekp(header.ofsPages, std::ios::beg);
	out.write((const char*)descriptions.data(), descriptions.size() * sizeof(FNTIBlockDesc));

	out.seekp(0, std::ios::end);

	uint32 end = (uint32)out.tellp();
	out.seekg(0, std::ios::beg);

	std::vector<char> content(end);
	out.read(content.data(), end);

	std::vector<char> compressed(end);
	Utils::ZDeflater defl;
	defl.begin();

	uint32 outPos = 0;
	defl.update(content, compressed, outPos);
	compressed.resize(outPos);

	defl.end();

	outFile.write((const char*)&end, sizeof(uint32));
	outFile.write(compressed.data(), compressed.size());
	outFile.close();
}

void MainWindow::initBlocks() {
	UnicodeBlock block;

	block.minChar = 0x0000;
	block.maxChar = 0x007F;
	block.name = L"Basic Latin";
	block.id = UBLOCK_BASIC_LATIN;
	mBlocks[UBLOCK_BASIC_LATIN] = block;

	block.minChar = 0x0080;
	block.maxChar = 0x00FF;
	block.name = L"Latin-1 Supplement";
	block.id = UBLOCK_LATIN_1_SUPPLEMENT;
	mBlocks[UBLOCK_LATIN_1_SUPPLEMENT] = block;

	block.minChar = 0x0100;
	block.maxChar = 0x017F;
	block.name = L"Latin Extended-A";
	block.id = UBLOCK_LATIN_EXTENDED_A;
	mBlocks[UBLOCK_LATIN_EXTENDED_A] = block;

	block.minChar = 0x0180;
	block.maxChar = 0x024F;
	block.name = L"Latin Extended-B";
	block.id = UBLOCK_LATIN_EXTENDED_B;
	mBlocks[UBLOCK_LATIN_EXTENDED_B] = block;

	block.minChar = 0x0250;
	block.maxChar = 0x02AF;
	block.name = L"IPA Extensions";
	block.id = UBLOCK_IPA_EXTENSIONS;
	mBlocks[UBLOCK_IPA_EXTENSIONS] = block;

	block.minChar = 0x02B0;
	block.maxChar = 0x02FF;
	block.name = L"Spacing Modifier Letters";
	block.id = UBLOCK_SPACING_MODIFIER_LETTERS;
	mBlocks[UBLOCK_SPACING_MODIFIER_LETTERS] = block;

	block.minChar = 0x0300;
	block.maxChar = 0x036F;
	block.name = L"Combining Diacritical Marks";
	block.id = UBLOCK_COMBINING_DIACRITICAL_MARKS;
	mBlocks[UBLOCK_COMBINING_DIACRITICAL_MARKS] = block;

	block.minChar = 0x0370;
	block.maxChar = 0x03FF;
	block.name = L"Greek and Coptic";
	block.id = UBLOCK_GREEK;
	mBlocks[UBLOCK_GREEK] = block;

	block.minChar = 0x0400;
	block.maxChar = 0x04FF;
	block.name = L"Cyrillic";
	block.id = UBLOCK_CYRILLIC;
	mBlocks[UBLOCK_CYRILLIC] = block;

	block.minChar = 0x0500;
	block.maxChar = 0x052F;
	block.name = L"Cyrillic Supplement";
	block.id = UBLOCK_CYRILLIC_SUPPLEMENT;
	mBlocks[UBLOCK_CYRILLIC_SUPPLEMENT] = block;

	block.minChar = 0x0530;
	block.maxChar = 0x058F;
	block.name = L"Armenian";
	block.id = UBLOCK_ARMENIAN;
	mBlocks[UBLOCK_ARMENIAN] = block;

	block.minChar = 0x0590;
	block.maxChar = 0x05FF;
	block.name = L"Hebrew";
	block.id = UBLOCK_HEBREW;
	mBlocks[UBLOCK_HEBREW] = block;

	block.minChar = 0x0600;
	block.maxChar = 0x06FF;
	block.name = L"Arabic";
	block.id = UBLOCK_ARABIC;
	mBlocks[UBLOCK_ARABIC] = block;

	block.minChar = 0x0700;
	block.maxChar = 0x074F;
	block.name = L"Syriac";
	block.id = UBLOCK_SYRIAC;
	mBlocks[UBLOCK_SYRIAC] = block;

	block.minChar = 0x0750;
	block.maxChar = 0x077F;
	block.name = L"Arabic Supplement";
	block.id = UBLOCK_ARABIC_SUPPLEMENT;
	mBlocks[UBLOCK_ARABIC_SUPPLEMENT] = block;

	block.minChar = 0x0780;
	block.maxChar = 0x07BF;
	block.name = L"Thaana";
	block.id = UBLOCK_THAANA;
	mBlocks[UBLOCK_THAANA] = block;

	block.minChar = 0x07C0;
	block.maxChar = 0x07FF;
	block.name = L"NKo";
	block.id = UBLOCK_NKO;
	mBlocks[UBLOCK_NKO] = block;

	block.minChar = 0x0800;
	block.maxChar = 0x083F;
	block.name = L"Samaritan";
	block.id = UBLOCK_SAMARITAN;
	mBlocks[UBLOCK_SAMARITAN] = block;

	block.minChar = 0x0840;
	block.maxChar = 0x085F;
	block.name = L"Mandaic";
	block.id = UBLOCK_MANDAIC;
	mBlocks[UBLOCK_MANDAIC] = block;

	block.minChar = 0x08A0;
	block.maxChar = 0x08FF;
	block.name = L"Arabic Extended-A";
	block.id = UBLOCK_ARABIC_EXTENDED_A;
	mBlocks[UBLOCK_ARABIC_EXTENDED_A] = block;

	block.minChar = 0x0900;
	block.maxChar = 0x097F;
	block.name = L"Devanagari";
	block.id = UBLOCK_DEVANAGARI;
	mBlocks[UBLOCK_DEVANAGARI] = block;

	block.minChar = 0x0980;
	block.maxChar = 0x09FF;
	block.name = L"Bengali";
	block.id = UBLOCK_BENGALI;
	mBlocks[UBLOCK_BENGALI] = block;

	block.minChar = 0x0A00;
	block.maxChar = 0x0A7F;
	block.name = L"Gurmukhi";
	block.id = UBLOCK_GURMUKHI;
	mBlocks[UBLOCK_GURMUKHI] = block;

	block.minChar = 0x0A80;
	block.maxChar = 0x0AFF;
	block.name = L"Gujarati";
	block.id = UBLOCK_GUJARATI;
	mBlocks[UBLOCK_GUJARATI] = block;

	block.minChar = 0x0B00;
	block.maxChar = 0x0B7F;
	block.name = L"Oriya";
	block.id = UBLOCK_ORIYA;
	mBlocks[UBLOCK_ORIYA] = block;

	block.minChar = 0x0B80;
	block.maxChar = 0x0BFF;
	block.name = L"Tamil";
	block.id = UBLOCK_TAMIL;
	mBlocks[UBLOCK_TAMIL] = block;

	block.minChar = 0x0C00;
	block.maxChar = 0x0C7F;
	block.name = L"Telugu";
	block.id = UBLOCK_TELUGU;
	mBlocks[UBLOCK_TELUGU] = block;

	block.minChar = 0x0C80;
	block.maxChar = 0x0CFF;
	block.name = L"Kannada";
	block.id = UBLOCK_KANNADA;
	mBlocks[UBLOCK_KANNADA] = block;

	block.minChar = 0x0D00;
	block.maxChar = 0x0D7F;
	block.name = L"Malayalam";
	block.id = UBLOCK_MALAYALAM;
	mBlocks[UBLOCK_MALAYALAM] = block;

	block.minChar = 0x0D80;
	block.maxChar = 0x0DFF;
	block.name = L"Sinhala";
	block.id = UBLOCK_SINHALA;
	mBlocks[UBLOCK_SINHALA] = block;

	block.minChar = 0x0E00;
	block.maxChar = 0x0E7F;
	block.name = L"Thai";
	block.id = UBLOCK_THAI;
	mBlocks[UBLOCK_THAI] = block;

	block.minChar = 0x0E80;
	block.maxChar = 0x0EFF;
	block.name = L"Lao";
	block.id = UBLOCK_LAO;
	mBlocks[UBLOCK_LAO] = block;

	block.minChar = 0x0F00;
	block.maxChar = 0x0FFF;
	block.name = L"Tibetan";
	block.id = UBLOCK_TIBETAN;
	mBlocks[UBLOCK_TIBETAN] = block;

	block.minChar = 0x1000;
	block.maxChar = 0x109F;
	block.name = L"Myanmar";
	block.id = UBLOCK_MYANMAR;
	mBlocks[UBLOCK_MYANMAR] = block;

	block.minChar = 0x10A0;
	block.maxChar = 0x10FF;
	block.name = L"Georgian";
	block.id = UBLOCK_GEORGIAN;
	mBlocks[UBLOCK_GEORGIAN] = block;

	block.minChar = 0x1100;
	block.maxChar = 0x11FF;
	block.name = L"Hangul Jamo";
	block.id = UBLOCK_HANGUL_JAMO;
	mBlocks[UBLOCK_HANGUL_JAMO] = block;

	block.minChar = 0x1200;
	block.maxChar = 0x137F;
	block.name = L"Ethiopic";
	block.id = UBLOCK_ETHIOPIC;
	mBlocks[UBLOCK_ETHIOPIC] = block;

	block.minChar = 0x1380;
	block.maxChar = 0x139F;
	block.name = L"Ethiopic Supplement";
	block.id = UBLOCK_ETHIOPIC_SUPPLEMENT;
	mBlocks[UBLOCK_ETHIOPIC_SUPPLEMENT] = block;

	block.minChar = 0x13A0;
	block.maxChar = 0x13FF;
	block.name = L"Cherokee";
	block.id = UBLOCK_CHEROKEE;
	mBlocks[UBLOCK_CHEROKEE] = block;

	block.minChar = 0x1400;
	block.maxChar = 0x167F;
	block.name = L"Unified Canadian Aboriginal Syllabics";
	block.id = UBLOCK_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS;
	mBlocks[UBLOCK_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS] = block;

	block.minChar = 0x1680;
	block.maxChar = 0x169F;
	block.name = L"Ogham";
	block.id = UBLOCK_OGHAM;
	mBlocks[UBLOCK_OGHAM] = block;

	block.minChar = 0x16A0;
	block.maxChar = 0x16FF;
	block.name = L"Runic";
	block.id = UBLOCK_RUNIC;
	mBlocks[UBLOCK_RUNIC] = block;

	block.minChar = 0x1700;
	block.maxChar = 0x171F;
	block.name = L"Tagalog";
	block.id = UBLOCK_TAGALOG;
	mBlocks[UBLOCK_TAGALOG] = block;

	block.minChar = 0x1720;
	block.maxChar = 0x173F;
	block.name = L"Hanunoo";
	block.id = UBLOCK_HANUNOO;
	mBlocks[UBLOCK_HANUNOO] = block;

	block.minChar = 0x1740;
	block.maxChar = 0x175F;
	block.name = L"Buhid";
	block.id = UBLOCK_BUHID;
	mBlocks[UBLOCK_BUHID] = block;

	block.minChar = 0x1760;
	block.maxChar = 0x177F;
	block.name = L"Tagbanwa";
	block.id = UBLOCK_TAGBANWA;
	mBlocks[UBLOCK_TAGBANWA] = block;

	block.minChar = 0x1780;
	block.maxChar = 0x17FF;
	block.name = L"Khmer";
	block.id = UBLOCK_KHMER;
	mBlocks[UBLOCK_KHMER] = block;

	block.minChar = 0x1800;
	block.maxChar = 0x18AF;
	block.name = L"Mongolian";
	block.id = UBLOCK_MONGOLIAN;
	mBlocks[UBLOCK_MONGOLIAN] = block;

	block.minChar = 0x18B0;
	block.maxChar = 0x18FF;
	block.name = L"Unified Canadian Aboriginal Syllabics Extended";
	block.id = UBLOCK_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_EXTENDED;
	mBlocks[UBLOCK_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_EXTENDED] = block;

	block.minChar = 0x1900;
	block.maxChar = 0x194F;
	block.name = L"Limbu";
	block.id = UBLOCK_LIMBU;
	mBlocks[UBLOCK_LIMBU] = block;

	block.minChar = 0x1950;
	block.maxChar = 0x197F;
	block.name = L"Tai Le";
	block.id = UBLOCK_TAI_LE;
	mBlocks[UBLOCK_TAI_LE] = block;

	block.minChar = 0x1980;
	block.maxChar = 0x19DF;
	block.name = L"New Tai Lue";
	block.id = UBLOCK_NEW_TAI_LUE;
	mBlocks[UBLOCK_NEW_TAI_LUE] = block;

	block.minChar = 0x19E0;
	block.maxChar = 0x19FF;
	block.name = L"Khmer Symbols";
	block.id = UBLOCK_KHMER_SYMBOLS;
	mBlocks[UBLOCK_KHMER_SYMBOLS] = block;

	block.minChar = 0x1A00;
	block.maxChar = 0x1A1F;
	block.name = L"Buginese";
	block.id = UBLOCK_BUGINESE;
	mBlocks[UBLOCK_BUGINESE] = block;

	block.minChar = 0x1A20;
	block.maxChar = 0x1AAF;
	block.name = L"Tai Tham";
	block.id = UBLOCK_TAI_THAM;
	mBlocks[UBLOCK_TAI_THAM] = block;

	block.minChar = 0x1B00;
	block.maxChar = 0x1B7F;
	block.name = L"Balinese";
	block.id = UBLOCK_BALINESE;
	mBlocks[UBLOCK_BALINESE] = block;

	block.minChar = 0x1B80;
	block.maxChar = 0x1BBF;
	block.name = L"Sundanese";
	block.id = UBLOCK_SUNDANESE;
	mBlocks[UBLOCK_SUNDANESE] = block;

	block.minChar = 0x1BC0;
	block.maxChar = 0x1BFF;
	block.name = L"Batak";
	block.id = UBLOCK_BATAK;
	mBlocks[UBLOCK_BATAK] = block;

	block.minChar = 0x1C00;
	block.maxChar = 0x1C4F;
	block.name = L"Lepcha";
	block.id = UBLOCK_LEPCHA;
	mBlocks[UBLOCK_LEPCHA] = block;

	block.minChar = 0x1C50;
	block.maxChar = 0x1C7F;
	block.name = L"Ol Chiki";
	block.id = UBLOCK_OL_CHIKI;
	mBlocks[UBLOCK_OL_CHIKI] = block;

	block.minChar = 0x1CC0;
	block.maxChar = 0x1CCF;
	block.name = L"Sundanese Supplement";
	block.id = UBLOCK_SUNDANESE_SUPPLEMENT;
	mBlocks[UBLOCK_SUNDANESE_SUPPLEMENT] = block;

	block.minChar = 0x1CD0;
	block.maxChar = 0x1CFF;
	block.name = L"Vedic Extensions";
	block.id = UBLOCK_VEDIC_EXTENSIONS;
	mBlocks[UBLOCK_VEDIC_EXTENSIONS] = block;

	block.minChar = 0x1D00;
	block.maxChar = 0x1D7F;
	block.name = L"Phonetic Extensions";
	block.id = UBLOCK_PHONETIC_EXTENSIONS;
	mBlocks[UBLOCK_PHONETIC_EXTENSIONS] = block;

	block.minChar = 0x1D80;
	block.maxChar = 0x1DBF;
	block.name = L"Phonetic Extensions Supplement";
	block.id = UBLOCK_PHONETIC_EXTENSIONS_SUPPLEMENT;
	mBlocks[UBLOCK_PHONETIC_EXTENSIONS_SUPPLEMENT] = block;

	block.minChar = 0x1DC0;
	block.maxChar = 0x1DFF;
	block.name = L"Combining Diacritical Marks Supplement";
	block.id = UBLOCK_COMBINING_DIACRITICAL_MARKS_SUPPLEMENT;
	mBlocks[UBLOCK_COMBINING_DIACRITICAL_MARKS_SUPPLEMENT] = block;

	block.minChar = 0x1E00;
	block.maxChar = 0x1EFF;
	block.name = L"Latin Extended Additional";
	block.id = UBLOCK_LATIN_EXTENDED_ADDITIONAL;
	mBlocks[UBLOCK_LATIN_EXTENDED_ADDITIONAL] = block;

	block.minChar = 0x1F00;
	block.maxChar = 0x1FFF;
	block.name = L"Greek Extended";
	block.id = UBLOCK_GREEK_EXTENDED;
	mBlocks[UBLOCK_GREEK_EXTENDED] = block;

	block.minChar = 0x2000;
	block.maxChar = 0x206F;
	block.name = L"General Punctuation";
	block.id = UBLOCK_GENERAL_PUNCTUATION;
	mBlocks[UBLOCK_GENERAL_PUNCTUATION] = block;

	block.minChar = 0x2070;
	block.maxChar = 0x209F;
	block.name = L"Superscripts and Subscripts";
	block.id = UBLOCK_SUPERSCRIPTS_AND_SUBSCRIPTS;
	mBlocks[UBLOCK_SUPERSCRIPTS_AND_SUBSCRIPTS] = block;

	block.minChar = 0x20A0;
	block.maxChar = 0x20CF;
	block.name = L"Currency Symbols";
	block.id = UBLOCK_CURRENCY_SYMBOLS;
	mBlocks[UBLOCK_CURRENCY_SYMBOLS] = block;

	block.minChar = 0x20D0;
	block.maxChar = 0x20FF;
	block.name = L"Combining Diacritical Marks for Symbols";
	block.id = UBLOCK_COMBINING_MARKS_FOR_SYMBOLS;
	mBlocks[UBLOCK_COMBINING_MARKS_FOR_SYMBOLS] = block;

	block.minChar = 0x2100;
	block.maxChar = 0x214F;
	block.name = L"Letterlike Symbols";
	block.id = UBLOCK_LETTERLIKE_SYMBOLS;
	mBlocks[UBLOCK_LETTERLIKE_SYMBOLS] = block;

	block.minChar = 0x2150;
	block.maxChar = 0x218F;
	block.name = L"Number Forms";
	block.id = UBLOCK_NUMBER_FORMS;
	mBlocks[UBLOCK_NUMBER_FORMS] = block;

	block.minChar = 0x2190;
	block.maxChar = 0x21FF;
	block.name = L"Arrows";
	block.id = UBLOCK_ARROWS;
	mBlocks[UBLOCK_ARROWS] = block;

	block.minChar = 0x2200;
	block.maxChar = 0x22FF;
	block.name = L"Mathematical Operators";
	block.id = UBLOCK_MATHEMATICAL_OPERATORS;
	mBlocks[UBLOCK_MATHEMATICAL_OPERATORS] = block;

	block.minChar = 0x2300;
	block.maxChar = 0x23FF;
	block.name = L"Miscellaneous Technical";
	block.id = UBLOCK_MISCELLANEOUS_TECHNICAL;
	mBlocks[UBLOCK_MISCELLANEOUS_TECHNICAL] = block;

	block.minChar = 0x2400;
	block.maxChar = 0x243F;
	block.name = L"Control Pictures";
	block.id = UBLOCK_CONTROL_PICTURES;
	mBlocks[UBLOCK_CONTROL_PICTURES] = block;

	block.minChar = 0x2440;
	block.maxChar = 0x245F;
	block.name = L"Optical Character Recognition";
	block.id = UBLOCK_OPTICAL_CHARACTER_RECOGNITION;
	mBlocks[UBLOCK_OPTICAL_CHARACTER_RECOGNITION] = block;

	block.minChar = 0x2460;
	block.maxChar = 0x24FF;
	block.name = L"Enclosed Alphanumerics";
	block.id = UBLOCK_ENCLOSED_ALPHANUMERICS;
	mBlocks[UBLOCK_ENCLOSED_ALPHANUMERICS] = block;

	block.minChar = 0x2500;
	block.maxChar = 0x257F;
	block.name = L"Box Drawing";
	block.id = UBLOCK_BOX_DRAWING;
	mBlocks[UBLOCK_BOX_DRAWING] = block;

	block.minChar = 0x2580;
	block.maxChar = 0x259F;
	block.name = L"Block Elements";
	block.id = UBLOCK_BLOCK_ELEMENTS;
	mBlocks[UBLOCK_BLOCK_ELEMENTS] = block;

	block.minChar = 0x25A0;
	block.maxChar = 0x25FF;
	block.name = L"Geometric Shapes";
	block.id = UBLOCK_GEOMETRIC_SHAPES;
	mBlocks[UBLOCK_GEOMETRIC_SHAPES] = block;

	block.minChar = 0x2600;
	block.maxChar = 0x26FF;
	block.name = L"Miscellaneous Symbols";
	block.id = UBLOCK_MISCELLANEOUS_SYMBOLS;
	mBlocks[UBLOCK_MISCELLANEOUS_SYMBOLS] = block;

	block.minChar = 0x2700;
	block.maxChar = 0x27BF;
	block.name = L"Dingbats";
	block.id = UBLOCK_DINGBATS;
	mBlocks[UBLOCK_DINGBATS] = block;

	block.minChar = 0x27C0;
	block.maxChar = 0x27EF;
	block.name = L"Miscellaneous Mathematical Symbols-A";
	block.id = UBLOCK_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A;
	mBlocks[UBLOCK_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A] = block;

	block.minChar = 0x27F0;
	block.maxChar = 0x27FF;
	block.name = L"Supplemental Arrows-A";
	block.id = UBLOCK_SUPPLEMENTAL_ARROWS_A;
	mBlocks[UBLOCK_SUPPLEMENTAL_ARROWS_A] = block;

	block.minChar = 0x2800;
	block.maxChar = 0x28FF;
	block.name = L"Braille Patterns";
	block.id = UBLOCK_BRAILLE_PATTERNS;
	mBlocks[UBLOCK_BRAILLE_PATTERNS] = block;

	block.minChar = 0x2900;
	block.maxChar = 0x297F;
	block.name = L"Supplemental Arrows-B";
	block.id = UBLOCK_SUPPLEMENTAL_ARROWS_B;
	mBlocks[UBLOCK_SUPPLEMENTAL_ARROWS_B] = block;

	block.minChar = 0x2980;
	block.maxChar = 0x29FF;
	block.name = L"Miscellaneous Mathematical Symbols-B";
	block.id = UBLOCK_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B;
	mBlocks[UBLOCK_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B] = block;

	block.minChar = 0x2A00;
	block.maxChar = 0x2AFF;
	block.name = L"Supplemental Mathematical Operators";
	block.id = UBLOCK_SUPPLEMENTAL_MATHEMATICAL_OPERATORS;
	mBlocks[UBLOCK_SUPPLEMENTAL_MATHEMATICAL_OPERATORS] = block;

	block.minChar = 0x2B00;
	block.maxChar = 0x2BFF;
	block.name = L"Miscellaneous Symbols and Arrows";
	block.id = UBLOCK_MISCELLANEOUS_SYMBOLS_AND_ARROWS;
	mBlocks[UBLOCK_MISCELLANEOUS_SYMBOLS_AND_ARROWS] = block;

	block.minChar = 0x2C00;
	block.maxChar = 0x2C5F;
	block.name = L"Glagolitic";
	block.id = UBLOCK_GLAGOLITIC;
	mBlocks[UBLOCK_GLAGOLITIC] = block;

	block.minChar = 0x2C60;
	block.maxChar = 0x2C7F;
	block.name = L"Latin Extended-C";
	block.id = UBLOCK_LATIN_EXTENDED_C;
	mBlocks[UBLOCK_LATIN_EXTENDED_C] = block;

	block.minChar = 0x2C80;
	block.maxChar = 0x2CFF;
	block.name = L"Coptic";
	block.id = UBLOCK_COPTIC;
	mBlocks[UBLOCK_COPTIC] = block;

	block.minChar = 0x2D00;
	block.maxChar = 0x2D2F;
	block.name = L"Georgian Supplement";
	block.id = UBLOCK_GEORGIAN_SUPPLEMENT;
	mBlocks[UBLOCK_GEORGIAN_SUPPLEMENT] = block;

	block.minChar = 0x2D30;
	block.maxChar = 0x2D7F;
	block.name = L"Tifinagh";
	block.id = UBLOCK_TIFINAGH;
	mBlocks[UBLOCK_TIFINAGH] = block;

	block.minChar = 0x2D80;
	block.maxChar = 0x2DDF;
	block.name = L"Ethiopic Extended";
	block.id = UBLOCK_ETHIOPIC_EXTENDED;
	mBlocks[UBLOCK_ETHIOPIC_EXTENDED] = block;

	block.minChar = 0x2DE0;
	block.maxChar = 0x2DFF;
	block.name = L"Cyrillic Extended-A";
	block.id = UBLOCK_CYRILLIC_EXTENDED_A;
	mBlocks[UBLOCK_CYRILLIC_EXTENDED_A] = block;

	block.minChar = 0x2E00;
	block.maxChar = 0x2E7F;
	block.name = L"Supplemental Punctuation";
	block.id = UBLOCK_SUPPLEMENTAL_PUNCTUATION;
	mBlocks[UBLOCK_SUPPLEMENTAL_PUNCTUATION] = block;

	block.minChar = 0x2E80;
	block.maxChar = 0x2EFF;
	block.name = L"CJK Radicals Supplement";
	block.id = UBLOCK_CJK_RADICALS_SUPPLEMENT;
	mBlocks[UBLOCK_CJK_RADICALS_SUPPLEMENT] = block;

	block.minChar = 0x2F00;
	block.maxChar = 0x2FDF;
	block.name = L"Kangxi Radicals";
	block.id = UBLOCK_KANGXI_RADICALS;
	mBlocks[UBLOCK_KANGXI_RADICALS] = block;

	block.minChar = 0x2FF0;
	block.maxChar = 0x2FFF;
	block.name = L"Ideographic Description Characters";
	block.id = UBLOCK_IDEOGRAPHIC_DESCRIPTION_CHARACTERS;
	mBlocks[UBLOCK_IDEOGRAPHIC_DESCRIPTION_CHARACTERS] = block;

	block.minChar = 0x3000;
	block.maxChar = 0x303F;
	block.name = L"CJK Symbols and Punctuation";
	block.id = UBLOCK_CJK_SYMBOLS_AND_PUNCTUATION;
	mBlocks[UBLOCK_CJK_SYMBOLS_AND_PUNCTUATION] = block;

	block.minChar = 0x3040;
	block.maxChar = 0x309F;
	block.name = L"Hiragana";
	block.id = UBLOCK_HIRAGANA;
	mBlocks[UBLOCK_HIRAGANA] = block;

	block.minChar = 0x30A0;
	block.maxChar = 0x30FF;
	block.name = L"Katakana";
	block.id = UBLOCK_KATAKANA;
	mBlocks[UBLOCK_KATAKANA] = block;

	block.minChar = 0x3100;
	block.maxChar = 0x312F;
	block.name = L"Bopomofo";
	block.id = UBLOCK_BOPOMOFO;
	mBlocks[UBLOCK_BOPOMOFO] = block;

	block.minChar = 0x3130;
	block.maxChar = 0x318F;
	block.name = L"Hangul Compatibility Jamo";
	block.id = UBLOCK_HANGUL_COMPATIBILITY_JAMO;
	mBlocks[UBLOCK_HANGUL_COMPATIBILITY_JAMO] = block;

	block.minChar = 0x3190;
	block.maxChar = 0x319F;
	block.name = L"Kanbun";
	block.id = UBLOCK_KANBUN;
	mBlocks[UBLOCK_KANBUN] = block;

	block.minChar = 0x31A0;
	block.maxChar = 0x31BF;
	block.name = L"Bopomofo Extended";
	block.id = UBLOCK_BOPOMOFO_EXTENDED;
	mBlocks[UBLOCK_BOPOMOFO_EXTENDED] = block;

	block.minChar = 0x31C0;
	block.maxChar = 0x31EF;
	block.name = L"CJK Strokes";
	block.id = UBLOCK_CJK_STROKES;
	mBlocks[UBLOCK_CJK_STROKES] = block;

	block.minChar = 0x31F0;
	block.maxChar = 0x31FF;
	block.name = L"Katakana Phonetic Extensions";
	block.id = UBLOCK_KATAKANA_PHONETIC_EXTENSIONS;
	mBlocks[UBLOCK_KATAKANA_PHONETIC_EXTENSIONS] = block;

	block.minChar = 0x3200;
	block.maxChar = 0x32FF;
	block.name = L"Enclosed CJK Letters and Months";
	block.id = UBLOCK_ENCLOSED_CJK_LETTERS_AND_MONTHS;
	mBlocks[UBLOCK_ENCLOSED_CJK_LETTERS_AND_MONTHS] = block;

	block.minChar = 0x3300;
	block.maxChar = 0x33FF;
	block.name = L"CJK Compatibility";
	block.id = UBLOCK_CJK_COMPATIBILITY;
	mBlocks[UBLOCK_CJK_COMPATIBILITY] = block;

	block.minChar = 0x3400;
	block.maxChar = 0x4DBF;
	block.name = L"CJK Unified Ideographs Extension A";
	block.id = UBLOCK_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A;
	mBlocks[UBLOCK_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A] = block;

	block.minChar = 0x4DC0;
	block.maxChar = 0x4DFF;
	block.name = L"Yijing Hexagram Symbols";
	block.id = UBLOCK_YIJING_HEXAGRAM_SYMBOLS;
	mBlocks[UBLOCK_YIJING_HEXAGRAM_SYMBOLS] = block;

	block.minChar = 0x4E00;
	block.maxChar = 0x9FFF;
	block.name = L"CJK Unified Ideographs";
	block.id = UBLOCK_CJK_UNIFIED_IDEOGRAPHS;
	mBlocks[UBLOCK_CJK_UNIFIED_IDEOGRAPHS] = block;

	block.minChar = 0xA000;
	block.maxChar = 0xA48F;
	block.name = L"Yi Syllables";
	block.id = UBLOCK_YI_SYLLABLES;
	mBlocks[UBLOCK_YI_SYLLABLES] = block;

	block.minChar = 0xA490;
	block.maxChar = 0xA4CF;
	block.name = L"Yi Radicals";
	block.id = UBLOCK_YI_RADICALS;
	mBlocks[UBLOCK_YI_RADICALS] = block;

	block.minChar = 0xA4D0;
	block.maxChar = 0xA4FF;
	block.name = L"Lisu";
	block.id = UBLOCK_LISU;
	mBlocks[UBLOCK_LISU] = block;

	block.minChar = 0xA500;
	block.maxChar = 0xA63F;
	block.name = L"Vai";
	block.id = UBLOCK_VAI;
	mBlocks[UBLOCK_VAI] = block;

	block.minChar = 0xA640;
	block.maxChar = 0xA69F;
	block.name = L"Cyrillic Extended-B";
	block.id = UBLOCK_CYRILLIC_EXTENDED_B;
	mBlocks[UBLOCK_CYRILLIC_EXTENDED_B] = block;

	block.minChar = 0xA6A0;
	block.maxChar = 0xA6FF;
	block.name = L"Bamum";
	block.id = UBLOCK_BAMUM;
	mBlocks[UBLOCK_BAMUM] = block;

	block.minChar = 0xA700;
	block.maxChar = 0xA71F;
	block.name = L"Modifier Tone Letters";
	block.id = UBLOCK_MODIFIER_TONE_LETTERS;
	mBlocks[UBLOCK_MODIFIER_TONE_LETTERS] = block;

	block.minChar = 0xA720;
	block.maxChar = 0xA7FF;
	block.name = L"Latin Extended-D";
	block.id = UBLOCK_LATIN_EXTENDED_D;
	mBlocks[UBLOCK_LATIN_EXTENDED_D] = block;

	block.minChar = 0xA800;
	block.maxChar = 0xA82F;
	block.name = L"Syloti Nagri";
	block.id = UBLOCK_SYLOTI_NAGRI;
	mBlocks[UBLOCK_SYLOTI_NAGRI] = block;

	block.minChar = 0xA830;
	block.maxChar = 0xA83F;
	block.name = L"Common Indic Number Forms";
	block.id = UBLOCK_COMMON_INDIC_NUMBER_FORMS;
	mBlocks[UBLOCK_COMMON_INDIC_NUMBER_FORMS] = block;

	block.minChar = 0xA840;
	block.maxChar = 0xA87F;
	block.name = L"Phags-pa";
	block.id = UBLOCK_PHAGS_PA;
	mBlocks[UBLOCK_PHAGS_PA] = block;

	block.minChar = 0xA880;
	block.maxChar = 0xA8DF;
	block.name = L"Saurashtra";
	block.id = UBLOCK_SAURASHTRA;
	mBlocks[UBLOCK_SAURASHTRA] = block;

	block.minChar = 0xA8E0;
	block.maxChar = 0xA8FF;
	block.name = L"Devanagari Extended";
	block.id = UBLOCK_DEVANAGARI_EXTENDED;
	mBlocks[UBLOCK_DEVANAGARI_EXTENDED] = block;

	block.minChar = 0xA900;
	block.maxChar = 0xA92F;
	block.name = L"Kayah Li";
	block.id = UBLOCK_KAYAH_LI;
	mBlocks[UBLOCK_KAYAH_LI] = block;

	block.minChar = 0xA930;
	block.maxChar = 0xA95F;
	block.name = L"Rejang";
	block.id = UBLOCK_REJANG;
	mBlocks[UBLOCK_REJANG] = block;

	block.minChar = 0xA960;
	block.maxChar = 0xA97F;
	block.name = L"Hangul Jamo Extended-A";
	block.id = UBLOCK_HANGUL_JAMO_EXTENDED_A;
	mBlocks[UBLOCK_HANGUL_JAMO_EXTENDED_A] = block;

	block.minChar = 0xA980;
	block.maxChar = 0xA9DF;
	block.name = L"Javanese";
	block.id = UBLOCK_JAVANESE;
	mBlocks[UBLOCK_JAVANESE] = block;

	block.minChar = 0xAA00;
	block.maxChar = 0xAA5F;
	block.name = L"Cham";
	block.id = UBLOCK_CHAM;
	mBlocks[UBLOCK_CHAM] = block;

	block.minChar = 0xAA60;
	block.maxChar = 0xAA7F;
	block.name = L"Myanmar Extended-A";
	block.id = UBLOCK_MYANMAR_EXTENDED_A;
	mBlocks[UBLOCK_MYANMAR_EXTENDED_A] = block;

	block.minChar = 0xAA80;
	block.maxChar = 0xAADF;
	block.name = L"Tai Viet";
	block.id = UBLOCK_TAI_VIET;
	mBlocks[UBLOCK_TAI_VIET] = block;

	block.minChar = 0xAAE0;
	block.maxChar = 0xAAFF;
	block.name = L"Meetei Mayek Extensions";
	block.id = UBLOCK_MEETEI_MAYEK_EXTENSIONS;
	mBlocks[UBLOCK_MEETEI_MAYEK_EXTENSIONS] = block;

	block.minChar = 0xAB00;
	block.maxChar = 0xAB2F;
	block.name = L"Ethiopic Extended-A";
	block.id = UBLOCK_ETHIOPIC_EXTENDED_A;
	mBlocks[UBLOCK_ETHIOPIC_EXTENDED_A] = block;

	block.minChar = 0xABC0;
	block.maxChar = 0xABFF;
	block.name = L"Meetei Mayek";
	block.id = UBLOCK_MEETEI_MAYEK;
	mBlocks[UBLOCK_MEETEI_MAYEK] = block;

	block.minChar = 0xAC00;
	block.maxChar = 0xD7AF;
	block.name = L"Hangul Syllables";
	block.id = UBLOCK_HANGUL_SYLLABLES;
	mBlocks[UBLOCK_HANGUL_SYLLABLES] = block;

	block.minChar = 0xD7B0;
	block.maxChar = 0xD7FF;
	block.name = L"Hangul Jamo Extended-B";
	block.id = UBLOCK_HANGUL_JAMO_EXTENDED_B;
	mBlocks[UBLOCK_HANGUL_JAMO_EXTENDED_B] = block;

	block.minChar = 0xD800;
	block.maxChar = 0xDB7F;
	block.name = L"High Surrogates";
	block.id = UBLOCK_HIGH_SURROGATES;
	mBlocks[UBLOCK_HIGH_SURROGATES] = block;

	block.minChar = 0xDB80;
	block.maxChar = 0xDBFF;
	block.name = L"High Private Use Surrogates";
	block.id = UBLOCK_HIGH_PRIVATE_USE_SURROGATES;
	mBlocks[UBLOCK_HIGH_PRIVATE_USE_SURROGATES] = block;

	block.minChar = 0xDC00;
	block.maxChar = 0xDFFF;
	block.name = L"Low Surrogates";
	block.id = UBLOCK_LOW_SURROGATES;
	mBlocks[UBLOCK_LOW_SURROGATES] = block;

	block.minChar = 0xE000;
	block.maxChar = 0xF8FF;
	block.name = L"Private Use Area";
	block.id = UBLOCK_PRIVATE_USE_AREA;
	mBlocks[UBLOCK_PRIVATE_USE_AREA] = block;

	block.minChar = 0xF900;
	block.maxChar = 0xFAFF;
	block.name = L"CJK Compatibility Ideographs";
	block.id = UBLOCK_CJK_COMPATIBILITY_IDEOGRAPHS;
	mBlocks[UBLOCK_CJK_COMPATIBILITY_IDEOGRAPHS] = block;

	block.minChar = 0xFB00;
	block.maxChar = 0xFB4F;
	block.name = L"Alphabetic Presentation Forms";
	block.id = UBLOCK_ALPHABETIC_PRESENTATION_FORMS;
	mBlocks[UBLOCK_ALPHABETIC_PRESENTATION_FORMS] = block;

	block.minChar = 0xFB50;
	block.maxChar = 0xFDFF;
	block.name = L"Arabic Presentation Forms-A";
	block.id = UBLOCK_ARABIC_PRESENTATION_FORMS_A;
	mBlocks[UBLOCK_ARABIC_PRESENTATION_FORMS_A] = block;

	block.minChar = 0xFE00;
	block.maxChar = 0xFE0F;
	block.name = L"Variation Selectors";
	block.id = UBLOCK_VARIATION_SELECTORS;
	mBlocks[UBLOCK_VARIATION_SELECTORS] = block;

	block.minChar = 0xFE10;
	block.maxChar = 0xFE1F;
	block.name = L"Vertical Forms";
	block.id = UBLOCK_VERTICAL_FORMS;
	mBlocks[UBLOCK_VERTICAL_FORMS] = block;

	block.minChar = 0xFE20;
	block.maxChar = 0xFE2F;
	block.name = L"Combining Half Marks";
	block.id = UBLOCK_COMBINING_HALF_MARKS;
	mBlocks[UBLOCK_COMBINING_HALF_MARKS] = block;

	block.minChar = 0xFE30;
	block.maxChar = 0xFE4F;
	block.name = L"CJK Compatibility Forms";
	block.id = UBLOCK_CJK_COMPATIBILITY_FORMS;
	mBlocks[UBLOCK_CJK_COMPATIBILITY_FORMS] = block;

	block.minChar = 0xFE50;
	block.maxChar = 0xFE6F;
	block.name = L"Small Form Variants";
	block.id = UBLOCK_SMALL_FORM_VARIANTS;
	mBlocks[UBLOCK_SMALL_FORM_VARIANTS] = block;

	block.minChar = 0xFE70;
	block.maxChar = 0xFEFF;
	block.name = L"Arabic Presentation Forms-B";
	block.id = UBLOCK_ARABIC_PRESENTATION_FORMS_B;
	mBlocks[UBLOCK_ARABIC_PRESENTATION_FORMS_B] = block;

	block.minChar = 0xFF00;
	block.maxChar = 0xFFEF;
	block.name = L"Halfwidth and Fullwidth Forms";
	block.id = UBLOCK_HALFWIDTH_AND_FULLWIDTH_FORMS;
	mBlocks[UBLOCK_HALFWIDTH_AND_FULLWIDTH_FORMS] = block;

	block.minChar = 0xFFF0;
	block.maxChar = 0xFFFF;
	block.name = L"Specials";
	block.id = UBLOCK_SPECIALS;
	mBlocks[UBLOCK_SPECIALS] = block;

	block.minChar = 0x10000;
	block.maxChar = 0x1007F;
	block.name = L"Linear B Syllabary";
	block.id = UBLOCK_LINEAR_B_SYLLABARY;
	mBlocks[UBLOCK_LINEAR_B_SYLLABARY] = block;

	block.minChar = 0x10080;
	block.maxChar = 0x100FF;
	block.name = L"Linear B Ideograms";
	block.id = UBLOCK_LINEAR_B_IDEOGRAMS;
	mBlocks[UBLOCK_LINEAR_B_IDEOGRAMS] = block;

	block.minChar = 0x10100;
	block.maxChar = 0x1013F;
	block.name = L"Aegean Numbers";
	block.id = UBLOCK_AEGEAN_NUMBERS;
	mBlocks[UBLOCK_AEGEAN_NUMBERS] = block;

	block.minChar = 0x10140;
	block.maxChar = 0x1018F;
	block.name = L"Ancient Greek Numbers";
	block.id = UBLOCK_ANCIENT_GREEK_NUMBERS;
	mBlocks[UBLOCK_ANCIENT_GREEK_NUMBERS] = block;

	block.minChar = 0x10190;
	block.maxChar = 0x101CF;
	block.name = L"Ancient Symbols";
	block.id = UBLOCK_ANCIENT_SYMBOLS;
	mBlocks[UBLOCK_ANCIENT_SYMBOLS] = block;

	block.minChar = 0x101D0;
	block.maxChar = 0x101FF;
	block.name = L"Phaistos Disc";
	block.id = UBLOCK_PHAISTOS_DISC;
	mBlocks[UBLOCK_PHAISTOS_DISC] = block;

	block.minChar = 0x10280;
	block.maxChar = 0x1029F;
	block.name = L"Lycian";
	block.id = UBLOCK_LYCIAN;
	mBlocks[UBLOCK_LYCIAN] = block;

	block.minChar = 0x102A0;
	block.maxChar = 0x102DF;
	block.name = L"Carian";
	block.id = UBLOCK_CARIAN;
	mBlocks[UBLOCK_CARIAN] = block;

	block.minChar = 0x10300;
	block.maxChar = 0x1032F;
	block.name = L"Old Italic";
	block.id = UBLOCK_OLD_ITALIC;
	mBlocks[UBLOCK_OLD_ITALIC] = block;

	block.minChar = 0x10330;
	block.maxChar = 0x1034F;
	block.name = L"Gothic";
	block.id = UBLOCK_GOTHIC;
	mBlocks[UBLOCK_GOTHIC] = block;

	block.minChar = 0x10380;
	block.maxChar = 0x1039F;
	block.name = L"Ugaritic";
	block.id = UBLOCK_UGARITIC;
	mBlocks[UBLOCK_UGARITIC] = block;

	block.minChar = 0x103A0;
	block.maxChar = 0x103DF;
	block.name = L"Old Persian";
	block.id = UBLOCK_OLD_PERSIAN;
	mBlocks[UBLOCK_OLD_PERSIAN] = block;

	block.minChar = 0x10400;
	block.maxChar = 0x1044F;
	block.name = L"Deseret";
	block.id = UBLOCK_DESERET;
	mBlocks[UBLOCK_DESERET] = block;

	block.minChar = 0x10450;
	block.maxChar = 0x1047F;
	block.name = L"Shavian";
	block.id = UBLOCK_SHAVIAN;
	mBlocks[UBLOCK_SHAVIAN] = block;

	block.minChar = 0x10480;
	block.maxChar = 0x104AF;
	block.name = L"Osmanya";
	block.id = UBLOCK_OSMANYA;
	mBlocks[UBLOCK_OSMANYA] = block;

	block.minChar = 0x10800;
	block.maxChar = 0x1083F;
	block.name = L"Cypriot Syllabary";
	block.id = UBLOCK_CYPRIOT_SYLLABARY;
	mBlocks[UBLOCK_CYPRIOT_SYLLABARY] = block;

	block.minChar = 0x10840;
	block.maxChar = 0x1085F;
	block.name = L"Imperial Aramaic";
	block.id = UBLOCK_IMPERIAL_ARAMAIC;
	mBlocks[UBLOCK_IMPERIAL_ARAMAIC] = block;

	block.minChar = 0x10900;
	block.maxChar = 0x1091F;
	block.name = L"Phoenician";
	block.id = UBLOCK_PHOENICIAN;
	mBlocks[UBLOCK_PHOENICIAN] = block;

	block.minChar = 0x10920;
	block.maxChar = 0x1093F;
	block.name = L"Lydian";
	block.id = UBLOCK_LYDIAN;
	mBlocks[UBLOCK_LYDIAN] = block;

	block.minChar = 0x10980;
	block.maxChar = 0x1099F;
	block.name = L"Meroitic Hieroglyphs";
	block.id = UBLOCK_MEROITIC_HIEROGLYPHS;
	mBlocks[UBLOCK_MEROITIC_HIEROGLYPHS] = block;

	block.minChar = 0x109A0;
	block.maxChar = 0x109FF;
	block.name = L"Meroitic Cursive";
	block.id = UBLOCK_MEROITIC_CURSIVE;
	mBlocks[UBLOCK_MEROITIC_CURSIVE] = block;

	block.minChar = 0x10A00;
	block.maxChar = 0x10A5F;
	block.name = L"Kharoshthi";
	block.id = UBLOCK_KHAROSHTHI;
	mBlocks[UBLOCK_KHAROSHTHI] = block;

	block.minChar = 0x10A60;
	block.maxChar = 0x10A7F;
	block.name = L"Old South Arabian";
	block.id = UBLOCK_OLD_SOUTH_ARABIAN;
	mBlocks[UBLOCK_OLD_SOUTH_ARABIAN] = block;

	block.minChar = 0x10B00;
	block.maxChar = 0x10B3F;
	block.name = L"Avestan";
	block.id = UBLOCK_AVESTAN;
	mBlocks[UBLOCK_AVESTAN] = block;

	block.minChar = 0x10B40;
	block.maxChar = 0x10B5F;
	block.name = L"Inscriptional Parthian";
	block.id = UBLOCK_INSCRIPTIONAL_PARTHIAN;
	mBlocks[UBLOCK_INSCRIPTIONAL_PARTHIAN] = block;

	block.minChar = 0x10B60;
	block.maxChar = 0x10B7F;
	block.name = L"Inscriptional Pahlavi";
	block.id = UBLOCK_INSCRIPTIONAL_PAHLAVI;
	mBlocks[UBLOCK_INSCRIPTIONAL_PAHLAVI] = block;

	block.minChar = 0x10C00;
	block.maxChar = 0x10C4F;
	block.name = L"Old Turkic";
	block.id = UBLOCK_OLD_TURKIC;
	mBlocks[UBLOCK_OLD_TURKIC] = block;

	block.minChar = 0x10E60;
	block.maxChar = 0x10E7F;
	block.name = L"Rumi Numeral Symbols";
	block.id = UBLOCK_RUMI_NUMERAL_SYMBOLS;
	mBlocks[UBLOCK_RUMI_NUMERAL_SYMBOLS] = block;

	block.minChar = 0x11000;
	block.maxChar = 0x1107F;
	block.name = L"Brahmi";
	block.id = UBLOCK_BRAHMI;
	mBlocks[UBLOCK_BRAHMI] = block;

	block.minChar = 0x11080;
	block.maxChar = 0x110CF;
	block.name = L"Kaithi";
	block.id = UBLOCK_KAITHI;
	mBlocks[UBLOCK_KAITHI] = block;

	block.minChar = 0x110D0;
	block.maxChar = 0x110FF;
	block.name = L"Sora Sompeng";
	block.id = UBLOCK_SORA_SOMPENG;
	mBlocks[UBLOCK_SORA_SOMPENG] = block;

	block.minChar = 0x11100;
	block.maxChar = 0x1114F;
	block.name = L"Chakma";
	block.id = UBLOCK_CHAKMA;
	mBlocks[UBLOCK_CHAKMA] = block;

	block.minChar = 0x11180;
	block.maxChar = 0x111DF;
	block.name = L"Sharada";
	block.id = UBLOCK_SHARADA;
	mBlocks[UBLOCK_SHARADA] = block;

	block.minChar = 0x11680;
	block.maxChar = 0x116CF;
	block.name = L"Takri";
	block.id = UBLOCK_TAKRI;
	mBlocks[UBLOCK_TAKRI] = block;

	block.minChar = 0x12000;
	block.maxChar = 0x123FF;
	block.name = L"Cuneiform";
	block.id = UBLOCK_CUNEIFORM;
	mBlocks[UBLOCK_CUNEIFORM] = block;

	block.minChar = 0x12400;
	block.maxChar = 0x1247F;
	block.name = L"Cuneiform Numbers and Punctuation";
	block.id = UBLOCK_CUNEIFORM_NUMBERS_AND_PUNCTUATION;
	mBlocks[UBLOCK_CUNEIFORM_NUMBERS_AND_PUNCTUATION] = block;

	block.minChar = 0x13000;
	block.maxChar = 0x1342F;
	block.name = L"Egyptian Hieroglyphs";
	block.id = UBLOCK_EGYPTIAN_HIEROGLYPHS;
	mBlocks[UBLOCK_EGYPTIAN_HIEROGLYPHS] = block;

	block.minChar = 0x16800;
	block.maxChar = 0x16A3F;
	block.name = L"Bamum Supplement";
	block.id = UBLOCK_BAMUM_SUPPLEMENT;
	mBlocks[UBLOCK_BAMUM_SUPPLEMENT] = block;

	block.minChar = 0x16F00;
	block.maxChar = 0x16F9F;
	block.name = L"Miao";
	block.id = UBLOCK_MIAO;
	mBlocks[UBLOCK_MIAO] = block;

	block.minChar = 0x1B000;
	block.maxChar = 0x1B0FF;
	block.name = L"Kana Supplement";
	block.id = UBLOCK_KANA_SUPPLEMENT;
	mBlocks[UBLOCK_KANA_SUPPLEMENT] = block;

	block.minChar = 0x1D000;
	block.maxChar = 0x1D0FF;
	block.name = L"Byzantine Musical Symbols";
	block.id = UBLOCK_BYZANTINE_MUSICAL_SYMBOLS;
	mBlocks[UBLOCK_BYZANTINE_MUSICAL_SYMBOLS] = block;

	block.minChar = 0x1D100;
	block.maxChar = 0x1D1FF;
	block.name = L"Musical Symbols";
	block.id = UBLOCK_MUSICAL_SYMBOLS;
	mBlocks[UBLOCK_MUSICAL_SYMBOLS] = block;

	block.minChar = 0x1D200;
	block.maxChar = 0x1D24F;
	block.name = L"Ancient Greek Musical Notation";
	block.id = UBLOCK_ANCIENT_GREEK_MUSICAL_NOTATION;
	mBlocks[UBLOCK_ANCIENT_GREEK_MUSICAL_NOTATION] = block;

	block.minChar = 0x1D300;
	block.maxChar = 0x1D35F;
	block.name = L"Tai Xuan Jing Symbols";
	block.id = UBLOCK_TAI_XUAN_JING_SYMBOLS;
	mBlocks[UBLOCK_TAI_XUAN_JING_SYMBOLS] = block;

	block.minChar = 0x1D360;
	block.maxChar = 0x1D37F;
	block.name = L"Counting Rod Numerals";
	block.id = UBLOCK_COUNTING_ROD_NUMERALS;
	mBlocks[UBLOCK_COUNTING_ROD_NUMERALS] = block;

	block.minChar = 0x1D400;
	block.maxChar = 0x1D7FF;
	block.name = L"Mathematical Alphanumeric Symbols";
	block.id = UBLOCK_MATHEMATICAL_ALPHANUMERIC_SYMBOLS;
	mBlocks[UBLOCK_MATHEMATICAL_ALPHANUMERIC_SYMBOLS] = block;

	block.minChar = 0x1EE00;
	block.maxChar = 0x1EEFF;
	block.name = L"Arabic Mathematical Alphabetic Symbols";
	block.id = UBLOCK_ARABIC_MATHEMATICAL_ALPHABETIC_SYMBOLS;
	mBlocks[UBLOCK_ARABIC_MATHEMATICAL_ALPHABETIC_SYMBOLS] = block;

	block.minChar = 0x1F000;
	block.maxChar = 0x1F02F;
	block.name = L"Mahjong Tiles";
	block.id = UBLOCK_MAHJONG_TILES;
	mBlocks[UBLOCK_MAHJONG_TILES] = block;

	block.minChar = 0x1F030;
	block.maxChar = 0x1F09F;
	block.name = L"Domino Tiles";
	block.id = UBLOCK_DOMINO_TILES;
	mBlocks[UBLOCK_DOMINO_TILES] = block;

	block.minChar = 0x1F0A0;
	block.maxChar = 0x1F0FF;
	block.name = L"Playing Cards";
	block.id = UBLOCK_PLAYING_CARDS;
	mBlocks[UBLOCK_PLAYING_CARDS] = block;

	block.minChar = 0x1F100;
	block.maxChar = 0x1F1FF;
	block.name = L"Enclosed Alphanumeric Supplement";
	block.id = UBLOCK_ENCLOSED_ALPHANUMERIC_SUPPLEMENT;
	mBlocks[UBLOCK_ENCLOSED_ALPHANUMERIC_SUPPLEMENT] = block;

	block.minChar = 0x1F200;
	block.maxChar = 0x1F2FF;
	block.name = L"Enclosed Ideographic Supplement";
	block.id = UBLOCK_ENCLOSED_IDEOGRAPHIC_SUPPLEMENT;
	mBlocks[UBLOCK_ENCLOSED_IDEOGRAPHIC_SUPPLEMENT] = block;

	block.minChar = 0x1F300;
	block.maxChar = 0x1F5FF;
	block.name = L"Miscellaneous Symbols And Pictographs";
	block.id = UBLOCK_MISCELLANEOUS_SYMBOLS_AND_PICTOGRAPHS;
	mBlocks[UBLOCK_MISCELLANEOUS_SYMBOLS_AND_PICTOGRAPHS] = block;

	block.minChar = 0x1F600;
	block.maxChar = 0x1F64F;
	block.name = L"Emoticons";
	block.id = UBLOCK_EMOTICONS;
	mBlocks[UBLOCK_EMOTICONS] = block;

	block.minChar = 0x1F680;
	block.maxChar = 0x1F6FF;
	block.name = L"Transport And Map Symbols";
	block.id = UBLOCK_TRANSPORT_AND_MAP_SYMBOLS;
	mBlocks[UBLOCK_TRANSPORT_AND_MAP_SYMBOLS] = block;

	block.minChar = 0x1F700;
	block.maxChar = 0x1F77F;
	block.name = L"Alchemical Symbols";
	block.id = UBLOCK_ALCHEMICAL_SYMBOLS;
	mBlocks[UBLOCK_ALCHEMICAL_SYMBOLS] = block;

	block.minChar = 0x20000;
	block.maxChar = 0x2A6DF;
	block.name = L"CJK Unified Ideographs Extension B";
	block.id = UBLOCK_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B;
	mBlocks[UBLOCK_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B] = block;

	block.minChar = 0x2A700;
	block.maxChar = 0x2B73F;
	block.name = L"CJK Unified Ideographs Extension C";
	block.id = UBLOCK_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_C;
	mBlocks[UBLOCK_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_C] = block;

	block.minChar = 0x2B740;
	block.maxChar = 0x2B81F;
	block.name = L"CJK Unified Ideographs Extension D";
	block.id = UBLOCK_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_D;
	mBlocks[UBLOCK_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_D] = block;

	block.minChar = 0x2F800;
	block.maxChar = 0x2FA1F;
	block.name = L"CJK Compatibility Ideographs Supplement";
	block.id = UBLOCK_CJK_COMPATIBILITY_IDEOGRAPHS_SUPPLEMENT;
	mBlocks[UBLOCK_CJK_COMPATIBILITY_IDEOGRAPHS_SUPPLEMENT] = block;

	block.minChar = 0xE0000;
	block.maxChar = 0xE007F;
	block.name = L"Tags";
	block.id = UBLOCK_TAGS;
	mBlocks[UBLOCK_TAGS] = block;

	block.minChar = 0xE0100;
	block.maxChar = 0xE01EF;
	block.name = L"Variation Selectors Supplement";
	block.id = UBLOCK_VARIATION_SELECTORS_SUPPLEMENT;
	mBlocks[UBLOCK_VARIATION_SELECTORS_SUPPLEMENT] = block;

	block.minChar = 0xF0000;
	block.maxChar = 0xFFFFF;
	block.name = L"Supplementary Private Use Area-A";
	block.id = UBLOCK_SUPPLEMENTARY_PRIVATE_USE_AREA_A;
	mBlocks[UBLOCK_SUPPLEMENTARY_PRIVATE_USE_AREA_A] = block;

	block.minChar = 0x100000;
	block.maxChar = 0x10FFFF;
	block.name = L"Supplementary Private Use Area-B";
	block.id = UBLOCK_SUPPLEMENTARY_PRIVATE_USE_AREA_B;
	mBlocks[UBLOCK_SUPPLEMENTARY_PRIVATE_USE_AREA_B] = block;
	mBlockVector.reserve(mBlocks.size());
	for(auto& pair : mBlocks) {
		ListBox_AddString(GetDlgItem(mWindow, IDC_CP_AVAIL_LIST), pair.second.name->c_str());
		mBlockVector.push_back(pair.second);
	}
}
