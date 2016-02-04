// Carbocontrol.h

#pragma once

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Windows::Interop;

namespace Carbocontrol
{
	public ref class NativeControl sealed : public HwndHost
	{
	public:
		HandleRef BuildWindowCore(HandleRef parentHandle) override;
		void DestroyWindowCore(HandleRef handle) override;

	private:
		HWND CreateNativeWindow(HWND hwndParent);
	};
}
