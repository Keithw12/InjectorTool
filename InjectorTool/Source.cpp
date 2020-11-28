/*
	Author:  Keith Wilcox
	Purpose:  Used to inject user's code which is executed in the target's address space.
	Date:  11/22/2020
	References:  https://github.com/saeedirha/DLL-Injector/blob/master/DLL_Injector/Source.cpp (Standard injection method)
*/

#include <iostream>
#include <Windows.h>
#include <filesystem>
#include <string>
#include <charconv>
#include <TlHelp32.h>
#include <tchar.h>

using namespace std;

void PrintUsage(char* argv);
int ProcNameToPID(const string& procName);
int InjectDLL(const int& pid, const string& DLLPath, int method);

int main(int argc, char **argv) {
	if (argc != 4) {
		PrintUsage(argv[0]);
		return 0;
	}
	if (!isdigit(argv[3][0]) && argv[3][0] < 2)
	{
		cerr << "Please provide a valid method #" << endl;
		return 0;
	}
	int result = 0;
	int injectMethod = atoi(argv[3]);
	string programID = argv[1];
	char DLLPath[MAX_PATH];
	GetFullPathNameA(argv[2], MAX_PATH, DLLPath, 0);
	// C++ 17 standard function to check if path exists
	if (!std::filesystem::exists(DLLPath)) {
		cerr << "Path to DLL invalid." << endl;
		return 0;
	}
	// if number provided, it's value is stored in result.  If programID isn't a number, then result is unmodified.
	// *p = end of given string if entire pattern matched (the entire input was a recognized number)
	// https://en.cppreference.com/w/cpp/utility/from_chars
	if (auto [p, ec] = from_chars(programID.data(), programID.data() + programID.size(), result);
		*p == '\0' && result != 0)
	{
		cout << "Target Process ID: " << result << endl;
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
	cout << "Usage: " << argv << " <Process ID | Process Name> <DLL Path>" << endl;
}

int ProcNameToPID(const string& procName) {
	// TH32CS_SNAPPROCESS ncludes all processes in the system in the snapshot
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	// A snapshot entry
	PROCESSENTRY32 processSnapshot = { 0 };

	processSnapshot.dwSize = sizeof(PROCESSENTRY32);
	// error checking
	if (snapshot == INVALID_HANDLE_VALUE) {
		cerr << "Error in converting process name to PID.  CreateToolhelp32Snapshot returned INVALID_HANDLE_VALUE" << endl;
		return 0;
	}
	
	if (Process32First(snapshot, &processSnapshot) == FALSE) {
		cerr << "No first entry in snapshot list" << endl;
		CloseHandle(snapshot);
		return 0;
	}

	do
	{
		if (!strcmp(processSnapshot.szExeFile, procName.c_str()))
		{
			CloseHandle(snapshot);
			cout << "Process name: " << procName << endl << "Process ID: " << (processSnapshot.th32ProcessID) << endl;
			return processSnapshot.th32ProcessID;
		}
	} while (Process32Next(snapshot, &processSnapshot));
	
	CloseHandle(snapshot);
	cerr << "Error: Process not found" << endl;
	return 0;
}

int InjectDLL(const int& pid, const string& DLLPath, int method) {
	long dll_size = DLLPath.length() + 1;
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	if (hProc == NULL)
	{
		cerr << "Failed to open target process" << endl;
		return 0;
	}
	cout << "Opening Target Process.." << endl;

	// allocate memory in remote process' address space
	LPVOID myAlloc = VirtualAllocEx(hProc, NULL, dll_size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (myAlloc == NULL)
	{
		cerr << "Failed to allocate memory in Target Process." << endl;
		return 0;
	}

	cout << "Allocating memory in Target Process." << endl;
	int rv = WriteProcessMemory(hProc, myAlloc, DLLPath.c_str(), dll_size, 0);
	if (rv == 0)
	{
		cerr << "Failed to write in Target Process memory." << endl;
		return 0;
	}
	cout << "Creating Remote Thread in Target Process" << endl;

	DWORD dWord;
	HMODULE moduleHandle = LoadLibrary("kernel32");
	if (!moduleHandle)
	{
		cerr << "failed to get module handle of kernel32" << endl;
		return 0;
	}
	FARPROC functionAddress = GetProcAddress(moduleHandle, "LoadLibraryA");
	if (!functionAddress)
	{
		cerr << "Failed to get address of kernel32 LoadLibraryA function." << endl;
		return 0;
	}
	LPTHREAD_START_ROUTINE addrLoadLibrary = (LPTHREAD_START_ROUTINE)functionAddress;
	HANDLE ThreadReturn = CreateRemoteThread(hProc, NULL, 0, addrLoadLibrary, myAlloc, 0, &dWord);
	if (ThreadReturn == NULL)
	{
		cerr << "Fail to create Remote Thread" << endl;
		return 0;
	}

	cout << "DLL Successfully Injected!" << endl;
	return 0;
}