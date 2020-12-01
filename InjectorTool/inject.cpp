
#include <iostream>
#include <Windows.h>
#include "inject.hpp"

/// <summary>
/// Prints failure code from GetLastError or success msg
/// </summary>
/// <param name="err">the function that failed</param>
void InjectStatus(INJECT_ERR err)
{
	DWORD Err = GetLastError();
	switch (err)
	{
	case INJECT_ERR::OPEN_PROC:
		std::cerr << "OpenProcess failed: 0x" << std::hex << Err << std::endl;
		break;
	case INJECT_ERR::VIRTUAL_ALLOC:
		std::cerr << "VirtualAllocEx failed: 0x" << std::hex << Err << std::endl;
		break;
	case INJECT_ERR::WRITE_PROC:
		std::cerr << "WriteProcessMemory failed: 0x" << std::hex << Err << std::endl;
		break;
	case INJECT_ERR::CREATE_REMOTE_THREAD:
		std::cerr << "CreateRemoteThread failed: 0x" << std::hex << Err << std::endl;
		break;
	case INJECT_ERR::SUCCESS:
		std::cout << "DLL Successfully Injected!" << std::endl;
		break;
	default:
		break;
	}
}

/// <summary>
/// Injects a DLL into the specified target
/// </summary>
/// <param name="pid">Process ID</param>
/// <param name="DLLPath">Full path of DLL</param>
/// <param name="method">Injection method</param>
/// <returns></returns>
INJECT_ERR InjectDLL(const int& pid, const std::string& DLLPath, int method) {
	long dll_size = DLLPath.length() + 1;
	
	// get a handle to the target process
	std::cout << "Opening Target Process.." << std::endl;
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	if (hProc == NULL || hProc == INVALID_HANDLE_VALUE)
	{
		return INJECT_ERR::OPEN_PROC;
	}

	// allocate memory in remote process' address space
	std::cout << "Allocating memory in Target Process.." << std::endl;
	LPVOID myAlloc = VirtualAllocEx(hProc, NULL, dll_size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (myAlloc == NULL)
	{
		CloseHandle(hProc);
		return INJECT_ERR::VIRTUAL_ALLOC;
	}

	// write the DLL path into the allocated memory in our target
	std::cout << "Writing DLL path into allocated memory.." << std::endl;
	if (!WriteProcessMemory(hProc, myAlloc, DLLPath.c_str(), dll_size, 0))
	{
		VirtualFreeEx(hProc, myAlloc, dll_size, MEM_FREE);
		CloseHandle(hProc);
		return INJECT_ERR::WRITE_PROC;
	}

	// Create thread in target process that executes LoadLibraryA
	std::cout << "Creating Remote Thread in target Process.." << std::endl;
	HANDLE ThreadReturn = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, myAlloc, 0, 0);
	if (ThreadReturn == NULL)
	{
		VirtualFreeEx(hProc, myAlloc, dll_size, MEM_FREE);
		CloseHandle(hProc);
		return INJECT_ERR::CREATE_REMOTE_THREAD;
	}

	CloseHandle(hProc);
	CloseHandle(ThreadReturn);
	return INJECT_ERR::SUCCESS;
}
