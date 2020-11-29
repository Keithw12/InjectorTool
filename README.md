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
How the injection works:

Process handle is obtained through a returned handle from OpenProcess.
Use VirtualAllocEx and subsequently WriteProcessMemory to allocate free memory in the target's address space and then write the path of our DLL into the allocated memory.
Kernel32.dll is a module that is loaded into all executables by default.  Therefore, the module's exported function LoadLibrary can be used to load our DLL module.
Calculate the address of the LoadLibrary function using GetProcAddress.
The address is then passed to CreateRemoteThreadEx with the argument of a pointer to the DLL file string we allocated for earlier in the target's address space.
This will create a remote thread in the target and execute LoadLibrary("OurDLL.exe").
