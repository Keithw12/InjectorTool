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

void printErr(InjectErr err)
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
