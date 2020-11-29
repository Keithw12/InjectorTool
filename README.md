# Max Injector

### Loads a module into the target's address space

## Features
**Current**
 - Standard Injection (LoadLibrary method)
 
**Planned**
 - Injection methods:  LdrLoadDll Stub, LdrpLoadDll Stub, Thread Hijacking and Manual Map

## Usage

Command line
```
  Injector.exe <Process ID | Process Name> <DLL Path>  (Note:  Full path must be given)
```

## Compatibility
Compile as x86 for 32-Bit and x64 for 64-Bit process injection

## Explanation of methods
**Standard**
1. Get handle to target via OpenProcess
1. Allocate memory in target via VirtualAllocEx
1. Write DLL Path to allocated memory via WriteProcessMemory
1. Create thread in target process via CreateRemoteThreadEx that executes LoadLibraryA with DLL path as parameter
