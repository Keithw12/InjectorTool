#pragma once

/// <summary>
/// Method used for injection
/// </summary>
enum class INJECT_METHOD
{
	LOAD_LIBRARY,
	MANUAL_MAP
};

/// <summary>
/// Possible injection errors
/// </summary>
enum class INJECT_ERR
{
	OPEN_PROC,				// OpenProcess
	VIRTUAL_ALLOC,			// VirtualAllocEx 
	WRITE_PROC,				// WriteProcessMemory
	CREATE_REMOTE_THREAD,	// CreateRemoteThread
	SUCCESS					// Successful
};

/// <summary>
/// Prints failure code from GetLastError or success msg
/// </summary>
/// <param name="err">the function that failed</param>
void InjectStatus(INJECT_ERR err);

/// <summary>
/// Injects a DLL into the specified target
/// </summary>
/// <param name="pid">Process ID</param>
/// <param name="DLLPath">Full path of DLL</param>
/// <param name="method">Injection method</param>
/// <returns></returns>
INJECT_ERR	InjectDLL(const int& pid, const std::string& DLLPath, int method);