#pragma once

class LibraryModule sealed
{
private:
	static LibraryModule *g_module;
	const HMODULE m_module;

	LibraryModule(HMODULE module) noexcept;
	~LibraryModule() noexcept;

public:
	_Check_return_
	static const LibraryModule *Module() noexcept
	{
		return g_module;
	}

	_Check_return_
	HMODULE GetInstanceHandle() const noexcept
	{
		return m_module;
	}

	_Check_return_
	static BOOL ProcessAttach(HMODULE module) noexcept;
	static void ProcessDetach() noexcept;
};
