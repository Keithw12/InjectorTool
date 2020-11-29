#pragma once

enum class InjectErr
{
	OPEN_PROC,
	VIRTUAL_ALLOC,
	WRITE_PROC,
	LOAD_LIB,
	GET_PROC_ADDR,
	CREATE_REMOTE_THREAD
};

void printInjectErr(InjectErr err);
int InjectDLL(const int& pid, const std::string& DLLPath, int method);