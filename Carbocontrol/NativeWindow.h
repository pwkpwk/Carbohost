#pragma once

class NativeWindowClass sealed
{
private:
	ATOM		m_classAtom;
	HINSTANCE	m_instance;
	int			m_regCount;

public:
	NativeWindowClass() noexcept
	:	m_classAtom(0),
		m_instance(NULL),
		m_regCount(0)
	{
	}

	~NativeWindowClass() noexcept {}

	_Check_return_
	LPCTSTR Register(_In_ HINSTANCE instance, _In_ WNDPROC wndProc) noexcept
	{
		WNDCLASSEX	classex = { 0 };
		TCHAR		className[64];

		StringCchPrintf(className, _ARRAYSIZE(className), TEXT("Window-%p"), this);
		classex.cbSize = sizeof(classex);
		classex.hInstance = instance;
		classex.hCursor = ::LoadCursor(NULL, IDC_HAND);
		classex.lpszClassName = className;
		classex.lpfnWndProc = wndProc;

		m_classAtom = ::RegisterClassEx(&classex);
		if (0 != m_classAtom)
		{
			m_instance = instance;
			++m_regCount;
		}

		return reinterpret_cast<LPCTSTR>(m_classAtom);
	}

	void Unregister() noexcept
	{
		_ASSERTE(m_regCount > 0);

		if (0 == --m_regCount)
		{
			_ASSERTE(0 != m_classAtom);
			_ASSERTE(NULL != m_instance);

			::UnregisterClass(reinterpret_cast<LPCTSTR>(m_classAtom), m_instance);
			m_classAtom = 0;
			m_instance = NULL;
		}
	}
};

class NativeWindow abstract
{
private:
	//
	// Pointer to a static window class object shared by all instances of the particular window class.
	// Each window class defines its own window class object.
	//
	NativeWindowClass * const m_windowClass;

protected:
	NativeWindow(_In_ NativeWindowClass *windowClass) noexcept
	:	m_windowClass(windowClass)
	{
	}

	virtual ~NativeWindow() noexcept
	{
	}

	_Check_return_
	HWND CreateHWND(_In_ HINSTANCE instance, _In_ HWND parent, DWORD style, _In_ LPCRECT rect) noexcept
	{
		_ASSERTE(m_windowClass);
		_ASSERTE(NULL != instance);
		_ASSERTE(::IsWindow(parent));

		HWND	hwnd = NULL;
		LPCTSTR	registeredClassName = m_windowClass->Register(instance, WindowProcStub);

		if (registeredClassName)
		{
			hwnd = ::CreateWindowEx(0, registeredClassName, NULL, style,
				rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top,
				parent, NULL, instance, this);
		}
		else
		{
			delete this;
		}

		return hwnd;
	}

	virtual LRESULT WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept
	{
		return ::DefWindowProc(hwnd, message, wParam, lParam);
	}

	virtual void OnFinalMessage(HWND hwnd) noexcept
	{
	}

private:
	static LRESULT CALLBACK WindowProcStub(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept
	{
		LRESULT lrc = 0;

		switch (message)
		{
		case WM_NCCREATE:
			lrc = OnNonClientCreate(hwnd, reinterpret_cast<LPCREATESTRUCT>(lParam));
			break;

		case WM_NCDESTROY:
			OnNonClientDestroy(hwnd, reinterpret_cast<NativeWindow*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA)));
			break;

		default:
			lrc = reinterpret_cast<NativeWindow*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA))->WindowProc(hwnd, message, wParam, lParam);
			break;
		}

		return lrc;
	}

	_Check_return_
	static LRESULT OnNonClientCreate(HWND hwnd, LPCREATESTRUCT lpcs) noexcept
	{
		::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(lpcs->lpCreateParams));
		return TRUE;
	}

	static void OnNonClientDestroy(HWND hwnd, NativeWindow *window) noexcept
	{
		window->OnFinalMessage(hwnd);
		window->m_windowClass->Unregister();
		delete window;
	}
};
