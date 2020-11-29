/*
	Author:  Keith Wilcox
	Purpose:  Used to inject user's code which is executed in the target's address space.
	Date:  11/22/2020
	References:  https://github.com/saeedirha/DLL-Injector/blob/master/DLL_Injector/Source.cpp (Standard injection method)
*/

#include <Windows.h>
#include <filesystem>
#include <charconv>
#include <TlHelp32.h>
#include "proc.hpp"

void PrintUsage(char* argv);
int InjectDLL(const int& pid, const std::string& DLLPath, int method);

int main(int argc, char **argv) {
	if (argc != 4) {
		PrintUsage(argv[0]);
		return 0;
	}
	if (!isdigit(argv[3][0]) && argv[3][0] < 2)
	{
		std::cerr << "Please provide a valid method #" << std::endl;
		return 0;
	}
	int result = 0;
	int injectMethod = atoi(argv[3]);
	std::string programID = argv[1];
	char DLLPath[MAX_PATH];
	GetFullPathNameA(argv[2], MAX_PATH, DLLPath, 0);
	// C++ 17 standard function to check if path exists
	if (!std::filesystem::exists(DLLPath)) {
		std::cerr << "Path to DLL invalid." << std::endl;
		return 0;
	}
	// if number provided, it's value is stored in result.  If programID isn't a number, then result is unmodified.
	// *p = end of given string if entire pattern matched (the entire input was a recognized number)
	// https://en.cppreference.com/w/cpp/utility/from_chars
	if (auto [p, ec] = std::from_chars(programID.data(), programID.data() + programID.size(), result);
		*p == '\0' && result != 0)
	{
		std::cout << "Target Process ID: " << result << std::endl;
		InjectDLL(result, DLLPath, injectMethod);
	}
	// user provided the image name
	else {
		if (int progPID = ProcNameToPID(programID); progPID)
		{
			InjectDLL(progPID, DLLPath, injectMethod);
		}		
	}
}

void PrintUsage(char *argv) {
	std::cout << "Usage: " << argv << " <Process ID | Process Name> <DLL Path>" << std::endl;
}

int InjectDLL(const int& pid, const std::string& DLLPath, int method) {
	long dll_size = DLLPath.length() + 1;
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	if (hProc == NULL)
	{
		std::cerr << "Failed to open target process" << std::endl;
		return 0;
	}
	std::cout << "Opening Target Process.." << std::endl;
	
	// allocate memory in remote process' address space
	LPVOID myAlloc = VirtualAllocEx(hProc, NULL, dll_size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (myAlloc == NULL)
	{
		std::cerr << "Failed to allocate memory in Target Process." << std::endl;
		return 0;
	}

	std::cout << "Allocating memory in Target Process." << std::endl;
	int rv = WriteProcessMemory(hProc, myAlloc, DLLPath.c_str(), dll_size, 0);
	if (rv == 0)
	{
		std::cerr << "Failed to write in Target Process memory." << std::endl;
		return 0;
	}
	std::cout << "Creating Remote Thread in Target Process" << std::endl;

	DWORD dWord;
	HMODULE moduleHandle = LoadLibrary("kernel32");
	if (!moduleHandle)
	{
		std::cerr << "failed to get module handle of kernel32" << std::endl;
		return 0;
	}
	FARPROC functionAddress = GetProcAddress(moduleHandle, "LoadLibraryA");
	if (!functionAddress)
	{
		std::cerr << "Failed to get address of kernel32 LoadLibraryA function." << std::endl;
		return 0;
	}
	LPTHREAD_START_ROUTINE addrLoadLibrary = (LPTHREAD_START_ROUTINE)functionAddress;
	HANDLE ThreadReturn = CreateRemoteThread(hProc, NULL, 0, addrLoadLibrary, myAlloc, 0, &dWord);
	if (ThreadReturn == NULL)
	{
		std::cerr << "Fail to create Remote Thread" << std::endl;
		return 0;
	}

	std::cout << "DLL Successfully Injected!" << std::endl;
	return 0;
}