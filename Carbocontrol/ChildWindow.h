#pragma once

#include "NativeWindow.h"
#include "dllmain.h"

class ChildWindow sealed : public NativeWindow
{
private:
	static NativeWindowClass m_windowClass;

public:
	static HWND Create(HWND parent) noexcept
	{
		HWND		hwnd = NULL;
		ChildWindow	*window = new(std::nothrow) ChildWindow();

		if (window)
		{
			const RECT rect = { 0, 0, 800, 600 };
			hwnd = window->CreateHWND(LibraryModule::Module()->GetInstanceHandle(), parent, WS_CHILD | WS_VISIBLE | WS_BORDER, &rect);
		}

		return hwnd;
	}

private:
	ChildWindow() noexcept : NativeWindow(&m_windowClass)
	{
	}

	~ChildWindow() noexcept
	{
	}

protected:
	LRESULT WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept override
	{
		LRESULT lrc = 0;

		switch (message)
		{
		case WM_ERASEBKGND:
			lrc = TRUE;
			break;

		case WM_PAINT:
			OnPaint(hwnd);
			break;

		default:
			lrc = NativeWindow::WindowProc(hwnd, message, wParam, lParam);
		}

		return lrc;
	}

private:
	void OnPaint(HWND hwnd) noexcept
	{
		PAINTSTRUCT ps;
		HDC hdc = ::BeginPaint(hwnd, &ps);

		if (hdc)
		{
			COLORREF	bkOld = ::SetBkColor(hdc, RGB(255, 127, 255));
			RECT		rcClient;

			::GetClientRect(hwnd, &rcClient);
			::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rcClient, NULL, 0, NULL);
			::InflateRect(&rcClient, -4, -4);

			if (!::IsRectEmpty(&rcClient))
			{
				::SetBkColor(hdc, RGB(127, 255, 127));
				::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rcClient, NULL, 0, NULL);
			}

			::SetBkColor(hdc, bkOld);
			::EndPaint(hwnd, &ps);
		}
	}
};

__declspec(selectany) NativeWindowClass ChildWindow::m_windowClass;
