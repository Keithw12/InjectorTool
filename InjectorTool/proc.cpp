#include <vector>
#include <Windows.h>
#include <TlHelp32.h>
#include "proc.hpp"

int ProcNameToPID(const std::string& procName) {
	// TH32CS_SNAPPROCESS ncludes all processes in the system in the snapshot
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	// A snapshot entry
	PROCESSENTRY32 processSnapshot = { 0 };

	processSnapshot.dwSize = sizeof(PROCESSENTRY32);
	// error checking
	if (snapshot == INVALID_HANDLE_VALUE) {
		std::cerr << "Error in converting process name to PID.  CreateToolhelp32Snapshot returned INVALID_HANDLE_VALUE" << std::endl;
		return 0;
	}

	if (Process32First(snapshot, &processSnapshot) == FALSE) {
		std::cerr << "No first entry in snapshot list" << std::endl;
		CloseHandle(snapshot);
		return 0;
	}

	do
	{
		if (!strcmp(processSnapshot.szExeFile, procName.c_str()))
		{
			CloseHandle(snapshot);
			std::cout << "Process name: " << procName << std::endl << "Process ID: " << (processSnapshot.th32ProcessID) << std::endl;
			return processSnapshot.th32ProcessID;
		}
	} while (Process32Next(snapshot, &processSnapshot));

	CloseHandle(snapshot);
	std::cerr << "Error: Process not found" << std::endl;
	return 0;
}