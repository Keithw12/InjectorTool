#pragma once

#include <iostream>

enum class InjectErr
{
	OPEN_PROC,
	VIRTUAL_ALLOC,
	WRITE_PROC,
	LOAD_LIB,
	GET_PROC_ADDR,
	CREATE_REMOTE_THREAD
};

int ProcNameToPID(const std::string& procName);
void printErr(InjectErr err);