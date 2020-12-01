/*
	Author:  Keith Wilcox
	Purpose:  Used to inject user's code which is executed in the target's address space.
	First Commit Date:  11/22/2020
	References:  https://github.com/saeedirha/DLL-Injector/blob/master/DLL_Injector/Source.cpp (Standard injection method)
*/

#include <iostream>
#include <Windows.h>
#include <filesystem>
#include <charconv>
#include <TlHelp32.h>
#include "proc.hpp"
#include "inject.hpp"

/// <summary>
/// Usage:  Injector.exe [ [Process ID] | [Process Name] ] [DLL Path]  (Note:  Full path must be given) 
/// </summary>
/// <param name="argc">Number of arguments</param>
/// <param name="argv">Arguments passed</param>
/// <returns></returns>
int main(int argc, char **argv) {
	if (argc != 4) {
		std::cout << "Usage: " << argv[0] << " <Process ID | Process Name> <DLL Path>" << std::endl;
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
	if (!std::filesystem::exists(DLLPath))
	{
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
		InjectStatus(InjectDLL(result, DLLPath, injectMethod));
	}
	// user provided the image name
	else
	{
		if (int progPID = ProcNameToPID(programID); progPID)
		{
			InjectStatus(InjectDLL(progPID, DLLPath, injectMethod));
		}
	}
}
