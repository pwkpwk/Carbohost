// This is the main DLL file.

#include "stdafx.h"
#include "NativeControl.h"
#include "NativeWindow.h"
#include "ChildWindow.h"
#include "dllmain.h"

using namespace Carbocontrol;

namespace
{
	class ScrollerWindow : public NativeWindow
	{
	private:
		static NativeWindowClass m_windowClass;
		HWND m_child;

	public:
		ScrollerWindow() noexcept : NativeWindow(&m_windowClass), m_child(NULL)
		{
		}

		~ScrollerWindow() noexcept
		{
		}

		HWND Create(HWND parent) noexcept
		{
			RECT rect;
			::GetClientRect(parent, &rect);
			return __super::CreateHWND(LibraryModule::Module()->GetInstanceHandle(), parent, WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE, &rect);
		}

	protected:
		LRESULT WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept override
		{
			LRESULT lrc = 0;

			switch (message)
			{
			case WM_CREATE:
				lrc = OnCreate(hwnd, reinterpret_cast<LPCREATESTRUCT>(lParam));
				break;

			case WM_ERASEBKGND:
				lrc = TRUE;
				break;

			case WM_PAINT:
				OnPaint(hwnd);
				break;

			case WM_SIZE:
				OnSize(hwnd, wParam, LOWORD(lParam), HIWORD(lParam));
				break;

			case WM_VSCROLL:
				OnVScroll(hwnd, HIWORD(wParam), LOWORD(wParam));
				break;

			case WM_HSCROLL:
				OnHScroll(hwnd, HIWORD(wParam), LOWORD(wParam));
				break;

			default:
				lrc = NativeWindow::WindowProc(hwnd, message, wParam, lParam);
			}

			return lrc;
		}

	private:
		_Check_return_
		LRESULT OnCreate(_In_ HWND hwnd, _In_ LPCREATESTRUCT lpcs) noexcept
		{
			_ASSERTE(NULL == m_child);
			m_child = ChildWindow::Create(hwnd);
			return 0;
		}

		void OnPaint(HWND hwnd) noexcept
		{
			PAINTSTRUCT ps;
			HDC hdc = ::BeginPaint(hwnd, &ps);

			if (hdc)
			{
				COLORREF bkOld = ::SetBkColor(hdc, RGB(255, 64, 64));

				::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &ps.rcPaint, NULL, 0, NULL);
				::SetBkColor(hdc, bkOld);
				::EndPaint(hwnd, &ps);
			}
		}

		void OnSize(HWND hwnd, WPARAM action, WORD cx, WORD cy) noexcept
		{
			RECT	childRect;
			SIZE	childSize;
			POINT	origin = { 0, 0 };
			DWORD	oldStyle, newStyle;

			newStyle = oldStyle = ::GetWindowLong(hwnd, GWL_STYLE);
			::GetWindowRect(m_child, &childRect);
			::ClientToScreen(hwnd, &origin);
			::OffsetRect(&childRect, -origin.x, -origin.y);

			childSize.cx = childRect.right - childRect.left;
			childSize.cy = childRect.bottom - childRect.top;

			if (childSize.cx <= cx)
			{
				//
				// Center-align the child window that is smaller than the client area.
				//
				origin.x = (cx - childSize.cx) / 2;
				newStyle &= ~WS_HSCROLL;
			}
			else
			{
				//
				// If there is a gap on the right between the child and the client area, close the gap by moving the child.
				//
				if (childRect.right < cx)
				{
					origin.x = cx - childSize.cx;
				}
				else
				{
					origin.x = childRect.left > 0 ? 0 : childRect.left;
				}
				newStyle |= WS_HSCROLL;
			}

			if (childSize.cy <= cy)
			{
				//
				// Center-align the child window that is smaller than the client area.
				//
				origin.y = (cy - childSize.cy) / 2;
				newStyle &= ~WS_VSCROLL;
			}
			else
			{
				//
				// If there is a gap on the bottom between the child and the client area, close the gap by moving the child.
				//
				if (childRect.bottom < cy)
				{
					origin.y = cy - childSize.cy;
				}
				else
				{
					origin.y = childRect.top > 0 ? 0 : childRect.top;
				}
				newStyle |= WS_VSCROLL;
			}

			::SetWindowPos(m_child, NULL, origin.x, origin.y, childSize.cx, childSize.cy, SWP_NOZORDER|SWP_NOSIZE);

			if (newStyle != oldStyle)
			{
				RECT cr;
				::SetWindowLong(hwnd, GWL_STYLE, newStyle);
				//
				// The style could have been changed because resizing could have added scrollbars.
				//
				newStyle = ::GetWindowLong(hwnd, GWL_STYLE);
				::GetClientRect(hwnd, &cr);
				cx = static_cast<WORD>(cr.right);
				cy = static_cast<WORD>(cr.bottom);
			}

			SCROLLINFO si = { 0 };

			si.cbSize = sizeof(si);
			si.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;

			if (0 != (WS_HSCROLL & newStyle))
			{
				si.nPage = cx;
				si.nMin = 0;
				si.nMax = childSize.cx - 1;
				si.nPos = -childRect.left;
				::SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
			}

			if (0 != (WS_VSCROLL & newStyle))
			{
				si.nPage = cy;
				si.nMin = 0;
				si.nMax = childSize.cy - 1;
				si.nPos = -childRect.top;
				::SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
			}
		}

		void OnVScroll(HWND hwnd, int position, WORD action) noexcept
		{
			RECT	rectChild;
			SIZE	size;
			POINT	origin = { 0 };

			::GetClientRect(hwnd, &rectChild);
			size.cx = rectChild.right;
			size.cy = rectChild.bottom;
			::GetWindowRect(m_child, &rectChild);
			::ClientToScreen(hwnd, &origin);
			::OffsetRect(&rectChild, -origin.x, -origin.y);

			switch(action)
			{
			case SB_LINEUP:
				position = -rectChild.top - 1;
				if (position >= 0)
				{
					::SetWindowPos(m_child, NULL, rectChild.left, -position, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
					::SetScrollPos(hwnd, SB_VERT, position, FALSE);
				}
				break;

			case SB_LINEDOWN:
				position = -rectChild.top + 1;
				if (position <= (rectChild.bottom - rectChild.top) - size.cy)
				{
					::SetWindowPos(m_child, NULL, rectChild.left, -position, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
					::SetScrollPos(hwnd, SB_VERT, position, FALSE);
				}
				break;

			case SB_THUMBTRACK:
				::SetWindowPos(m_child, NULL, rectChild.left, -position, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
				::SetScrollPos(hwnd, SB_VERT, position, FALSE);
				break;

			case SB_THUMBPOSITION:
				::SetWindowPos(m_child, NULL, rectChild.left, -position, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
				::SetScrollPos(hwnd, SB_VERT, position, TRUE);
				break;
			}
		}

		void OnHScroll(HWND hwnd, int position, WORD action) noexcept
		{
			RECT	rectChild;
			SIZE	size;
			POINT	origin = { 0 };

			::GetClientRect(hwnd, &rectChild);
			size.cx = rectChild.right;
			size.cy = rectChild.bottom;
			::GetWindowRect(m_child, &rectChild);
			::ClientToScreen(hwnd, &origin);
			::OffsetRect(&rectChild, -origin.x, -origin.y);

			switch (action)
			{
			case SB_LINEUP:
				position = -rectChild.left - 1;
				if (position >= 0)
				{
					::SetWindowPos(m_child, NULL, -position, rectChild.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
					::SetScrollPos(hwnd, SB_HORZ, position, FALSE);
				}
				break;

			case SB_LINEDOWN:
				position = -rectChild.left + 1;
				if (position <= (rectChild.right - rectChild.left) - size.cx)
				{
					::SetWindowPos(m_child, NULL, -position, rectChild.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
					::SetScrollPos(hwnd, SB_HORZ, position, FALSE);
				}
				break;

			case SB_THUMBTRACK:
				::SetWindowPos(m_child, NULL, -position, rectChild.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
				::SetScrollPos(hwnd, SB_HORZ, position, FALSE);
				break;

			case SB_THUMBPOSITION:
				::SetWindowPos(m_child, NULL, -position, rectChild.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
				::SetScrollPos(hwnd, SB_HORZ, position, TRUE);
				break;
			}
		}
	};

	NativeWindowClass ScrollerWindow::m_windowClass;
}

HandleRef NativeControl::BuildWindowCore(HandleRef parentHandle)
{
	HWND hwndParent = (HWND)(parentHandle.Handle.ToPointer());
	HWND hwndChild = CreateNativeWindow(hwndParent);

	return HandleRef(this, IntPtr(hwndChild));
}

HWND NativeControl::CreateNativeWindow(HWND hwndParent)
{
	HWND			hwnd = NULL;
	ScrollerWindow	*scroller = new(std::nothrow) ScrollerWindow();

	if (scroller)
	{
		hwnd = scroller->Create(hwndParent);
	}

	return hwnd;
}

void NativeControl::DestroyWindowCore(HandleRef handle)
{
	HWND hwnd = reinterpret_cast<HWND>(handle.Handle.ToPointer());

	if (::IsWindow(hwnd))
	{
		::DestroyWindow(hwnd);
	}
}
