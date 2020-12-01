#pragma once
#include <iostream>

/// <summary>
/// Takes a running process' name and returns a PID
/// </summary>
/// <param name="procName">Process name</param>
/// <returns>Process ID</returns>
int ProcNameToPID(const std::string& procName);