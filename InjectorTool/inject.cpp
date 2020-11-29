
#include <iostream>
#include <Windows.h>
#include "inject.hpp"

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
	int rv = WriteProcessMemory(hProc, myAlloc, DLLPath.c_str(), dll_size, 0);
	if (rv == 0)
	{
		CloseHandle(hProc);
		return INJECT_ERR::WRITE_PROC;
	}
	
	// Create thread in target process that executes LoadLibraryA
	std::cout << "Creating Remote Thread in target Process.." << std::endl;
	HANDLE ThreadReturn = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, myAlloc, 0, 0);
	if (ThreadReturn == NULL)
	{
		CloseHandle(hProc);
		return INJECT_ERR::CREATE_REMOTE_THREAD;
	}

	CloseHandle(hProc);
	CloseHandle(ThreadReturn);
	return INJECT_ERR::SUCCESS;
}

void printInjectStatus(INJECT_ERR err)
{
	switch (err)
	{
	case INJECT_ERR::OPEN_PROC:
		std::cerr << "Failed to get a handle to the Target Process." << std::endl;
		break;
	case INJECT_ERR::VIRTUAL_ALLOC:
		std::cerr << "Failed to allocate memory in Target Process." << std::endl;
		break;
	case INJECT_ERR::WRITE_PROC:
		std::cerr << "Failed to write in Target Process memory." << std::endl;
		break;
	case INJECT_ERR::LOAD_LIB:
		std::cerr << "failed to get module handle." << std::endl;
		break;
	case INJECT_ERR::GET_PROC_ADDR:
		std::cerr << "Failed to get address of function." << std::endl;
		break;
	case INJECT_ERR::CREATE_REMOTE_THREAD:
		std::cerr << "Fail to create Remote Thread" << std::endl;
		break;
	case INJECT_ERR::SUCCESS:
		std::cout << "DLL Successfully Injected!" << std::endl;
		break;
	default:
		break;
	}
}
