
#include <iostream>
#include <Windows.h>
#include "inject.hpp"

int InjectDLL(const int& pid, const std::string& DLLPath, int method) {
	long dll_size = DLLPath.length() + 1;
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	if (hProc == NULL)
	{
		printInjectErr(InjectErr::OPEN_PROC);
		return 0;
	}
	std::cout << "Opening Target Process.." << std::endl;

	// allocate memory in remote process' address space
	LPVOID myAlloc = VirtualAllocEx(hProc, NULL, dll_size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (myAlloc == NULL)
	{
		printInjectErr(InjectErr::VIRTUAL_ALLOC);
		return 0;
	}

	std::cout << "Allocating memory in Target Process." << std::endl;
	int rv = WriteProcessMemory(hProc, myAlloc, DLLPath.c_str(), dll_size, 0);
	if (rv == 0)
	{
		printInjectErr(InjectErr::WRITE_PROC);
		return 0;
	}
	std::cout << "Creating Remote Thread in Target Process" << std::endl;

	DWORD dWord;
	HMODULE moduleHandle = LoadLibrary("kernel32");
	if (!moduleHandle)
	{
		printInjectErr(InjectErr::LOAD_LIB);
		return 0;
	}
	FARPROC functionAddress = GetProcAddress(moduleHandle, "LoadLibraryA");
	if (!functionAddress)
	{
		printInjectErr(InjectErr::GET_PROC_ADDR);
		return 0;
	}
	LPTHREAD_START_ROUTINE addrLoadLibrary = (LPTHREAD_START_ROUTINE)functionAddress;
	HANDLE ThreadReturn = CreateRemoteThread(hProc, NULL, 0, addrLoadLibrary, myAlloc, 0, &dWord);
	if (ThreadReturn == NULL)
	{
		printInjectErr(InjectErr::CREATE_REMOTE_THREAD);
		return 0;
	}

	std::cout << "DLL Successfully Injected!" << std::endl;
	return 0;
}

void printInjectErr(InjectErr err)
{
	switch (err)
	{
	case InjectErr::OPEN_PROC:
		std::cerr << "Failed to get a handle to the Target Process." << std::endl;
		break;
	case InjectErr::VIRTUAL_ALLOC:
		std::cerr << "Failed to allocate memory in Target Process." << std::endl;
		break;
	case InjectErr::WRITE_PROC:
		std::cerr << "Failed to write in Target Process memory." << std::endl;
		break;
	case InjectErr::LOAD_LIB:
		std::cerr << "failed to get module handle." << std::endl;
		break;
	case InjectErr::GET_PROC_ADDR:
		std::cerr << "Failed to get address of function." << std::endl;
		break;
	case InjectErr::CREATE_REMOTE_THREAD:
		std::cerr << "Fail to create Remote Thread" << std::endl;
		break;
	default:
		break;
	}
}
