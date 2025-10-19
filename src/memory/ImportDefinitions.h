#pragma once

using tNtDelayExecution = NTSTATUS(WINAPI*)(BOOLEAN, PLARGE_INTEGER);
using tZwSetTimerResolution = NTSTATUS(WINAPI*)(ULONG, BOOLEAN Set, PULONG);
using tNtReadVirtualMemory = NTSTATUS(WINAPI*)(HANDLE, PVOID, PVOID, ULONG, PULONG);
using tNtWriteVirtualMemory = NTSTATUS(WINAPI*)(HANDLE, PVOID, PVOID, ULONG, PULONG);