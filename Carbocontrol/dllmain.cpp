#include "Stdafx.h"
#include "dllmain.h"

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID) noexcept
{
	BOOL rc = TRUE;

	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		rc = LibraryModule::ProcessAttach(module);
		break;

	case DLL_PROCESS_DETACH:
		LibraryModule::ProcessDetach();
		break;
	}

	return rc;
}

LibraryModule *LibraryModule::g_module = nullptr;

LibraryModule::LibraryModule(HMODULE module) noexcept
:	m_module(module)
{
}

LibraryModule::~LibraryModule() noexcept
{
}

_Check_return_
BOOL LibraryModule::ProcessAttach(HMODULE module) noexcept
{
	_ASSERTE(nullptr == g_module);
	g_module = new(std::nothrow) LibraryModule(module);
	return nullptr != g_module;
}

void LibraryModule::ProcessDetach() noexcept
{
	delete g_module;
	g_module = nullptr;
}
