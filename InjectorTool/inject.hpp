#pragma once

enum class INJECT_ERR
{
	OPEN_PROC,
	VIRTUAL_ALLOC,
	WRITE_PROC,
	LOAD_LIB,
	GET_PROC_ADDR,
	CREATE_REMOTE_THREAD,
	SUCCESS
};

void printInjectStatus(INJECT_ERR err);
INJECT_ERR InjectDLL(const int& pid, const std::string& DLLPath, int method);