# Max Injector

### Loads a module into the target's address space

## Features
**Current**
 - Standard Injection (LoadLibrary method)
 
**Planned**
 - Injection methods:  LdrLoadDll Stub, LdrpLoadDll Stub, Thread Hijacking, Manual Map, IAT Hooking

## Usage

Command line
```
  Injector.exe <Process ID | Process Name> <DLL Path>  (Note:  Full path must be given)
```

## Compatibility
* Project settings:
  * Platform x86 for 32-Bit injection (requires 32-bit module)
  * Platform x64 for 64-Bit injection (requires 64-bit module)
    *A 64-bit module cannot be loaded or mapped into a 32-bit process' address space, vice-versa.
  * Multi-byte Character Set
  * ISO C++ 17 Language Standard
  * C Language Standard: Default
  * Platform Toolset: VS 2019 (v142)

## Explanation of methods
**Standard (windows loader)**
1. Get handle to target via OpenProcess
1. Allocate memory in target via VirtualAllocEx
1. Write DLL Path to allocated memory via WriteProcessMemory
1. Create thread in target process via CreateRemoteThreadEx that executes LoadLibrary with DLL path as parameter

**Manual Map (custom loader/ does all the steps LoadLibrary does, but manually)** 
1. Load module as raw data into injector process
1. Map sections of module into target process
1. Inject loader shellcode
1. Shellcode will relocate DLL, fix imports, execute TLS callbacks, and then call DllMain
