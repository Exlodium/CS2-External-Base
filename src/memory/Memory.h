#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <iostream>

#define MEM_PAD(SIZE) \
private: \
	char _MEM_CONCATENATE(pad_0, __COUNTER__)[SIZE]; \
public:

#define _MEM_INTERNAL_CONCATENATE(LEFT, RIGHT) LEFT##RIGHT
#define _MEM_CONCATENATE(LEFT, RIGHT) _MEM_INTERNAL_CONCATENATE(LEFT, RIGHT)

#ifdef __has_builtin
#define CS_HAS_BUILTIN(BUILTIN) __has_builtin(BUILTIN)
#else
#define CS_HAS_BUILTIN(BUILTIN) 0
#endif

#if defined(_CRT_USE_BUILTIN_OFFSETOF) || CS_HAS_BUILTIN(__builtin_offsetof)
#define CS_OFFSETOF(STRUCT, MEMBER) __builtin_offsetof(STRUCT, MEMBER)
#else
#define CS_OFFSETOF(STRUCT, MEMBER) reinterpret_cast<std::size_t>(std::addressof(static_cast<STRUCT*>(nullptr)->MEMBER))
#endif

#define BYTE_InRange(x, a, b)	(x >= a && x <= b) 
#define BYTE_GetBits(x)			(BYTE_InRange((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (BYTE_InRange(x,'0','9') ? x - '0' : 0))
#define BYTE_Get(x)				(BYTE_GetBits(x[0]) << 4 | BYTE_GetBits(x[1]))

struct ModuleInformation_t 
{
	ModuleInformation_t() = default;
	ModuleInformation_t(std::uintptr_t uBaseAddress, std::string strModulePath, std::uintptr_t uModuleSize)
	{
		m_uBaseAddress = uBaseAddress;
		m_strPath = strModulePath;
		m_uModuleSize = uModuleSize;
	};

	std::uintptr_t m_uBaseAddress = 0;
	std::string m_strPath{};

	std::uintptr_t m_uModuleSize = 0U;
};

enum EPatternScanFlags : std::uint16_t 
{
	NO_FLAGS = 0U,
	SCAN_RESOLVE_RIP = (1 << 0),
	SCAN_RESOLVE_ABS = (1 << 1)
};

class CMemory 
{
public:
	CMemory() = default;

	~CMemory()
	{
		if (m_pProcessHandle)
			CloseHandle(m_pProcessHandle);
	}

	bool IsWindowInForeground(const char* szWindowName)
	{
		static HWND hWnd = FindWindowA(nullptr, szWindowName);
		if (!hWnd)
			return false;

		HWND hForegroundWnd = GetForegroundWindow();
		if (!hForegroundWnd)
			return false;

		return (hWnd == hForegroundWnd);
	}

	bool IsGameRunning()
	{
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, m_pProcessID);
		if (hProcess != NULL) {
			CloseHandle(hProcess);
			return true;
		}

		return false;
	}

	void Initialize(const char* szProcessName)
	{
		while (!m_pProcessID)
		{
			PROCESSENTRY32 entry = {};
			entry.dwSize = sizeof(PROCESSENTRY32);

			HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			while (Process32Next(hSnapShot, &entry))
			{
				if (!strcmp(szProcessName, entry.szExeFile))
				{
					m_pProcessID = entry.th32ProcessID;
					m_pProcessHandle = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, m_pProcessID);
					break;
				}
			}
			if (hSnapShot)
				CloseHandle(hSnapShot);
		}
	}

	bool ReadMemoryRaw(const std::uintptr_t& uAddress, void* pBuffer, std::size_t uSize)
	{
		static tNtReadVirtualMemory NtReadVirtualMemory = reinterpret_cast<tNtReadVirtualMemory>(GetImport(NTDLL_DLL, X("NtReadVirtualMemory")));
		const NTSTATUS ret = NtReadVirtualMemory(m_pProcessHandle, reinterpret_cast<PVOID>(uAddress), pBuffer, static_cast<ULONG>(uSize), 0);
		return ret == 0;
	}

	bool ReadMemoryRaw(void* pAddress, void* pBuffer, std::size_t uSize)
	{
		static tNtReadVirtualMemory NtReadVirtualMemory = reinterpret_cast<tNtReadVirtualMemory>(GetImport(NTDLL_DLL, X("NtReadVirtualMemory")));
		const NTSTATUS ret = NtReadVirtualMemory(m_pProcessHandle, pAddress, pBuffer, static_cast<ULONG>(uSize), 0);
		return ret == 0;
	}

	template<typename T = std::uintptr_t>
	T ReadMemory(const std::uintptr_t& uAddress)
	{
		T buffer{};
		ReadMemoryRaw(uAddress, &buffer, sizeof(T));
		return buffer;
	}

	template<typename T = std::uintptr_t>
	T ReadMemory(const std::uintptr_t& uBaseAddress, std::vector<std::uintptr_t> vecOffsets)
	{
		std::uintptr_t uAddress = ReadMemory<std::uintptr_t>(uBaseAddress);
		if (vecOffsets.empty())
			return uAddress;

		for (std::uintptr_t uOffset : vecOffsets)
			uAddress = ReadMemory<std::uintptr_t>(uAddress + uOffset);

		return uAddress;
	}

	std::string ReadMemoryString(const std::uintptr_t& uAddress)
	{
		char buffer[MAX_PATH]{};
		ReadMemoryRaw(uAddress, &buffer, sizeof(buffer));
		return std::string(buffer);
	}

	template<typename T = std::uintptr_t>
	void WriteMemory(const std::uintptr_t& uAddress, T value)
	{
		static tNtWriteVirtualMemory NtWriteVirtualMemory = reinterpret_cast<tNtWriteVirtualMemory>(GetImport(NTDLL_DLL, X("NtWriteVirtualMemory")));
		NtWriteVirtualMemory(m_pProcessHandle, reinterpret_cast<PVOID>(uAddress), &value, sizeof(T), nullptr);
	}

	std::uintptr_t GetImport(const char* szModule, const char* szImport)
	{
		FNV1A_t uHash = FNV1A::Hash(szImport);
		if (m_mapImports.find(uHash) == m_mapImports.end())
		{
			HMODULE hModule = GetModuleHandleA(szModule);
			if (hModule != nullptr)
			{
				m_mapImports[uHash] = reinterpret_cast<std::uintptr_t>(GetProcAddress(hModule, szImport));
			}
		}
		return m_mapImports[uHash];
	}

	ModuleInformation_t GetModule(const char* szModuleName)
	{
		FNV1A_t uHash = FNV1A::Hash(szModuleName);
		if (m_mapModules.find(uHash) == m_mapModules.end())
		{
			MODULEENTRY32 entry = {};
			entry.dwSize = sizeof(MODULEENTRY32);
			const HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_pProcessID);

			while (Module32Next(hSnapShot, &entry))
			{
				if (!strcmp(szModuleName, entry.szModule))
				{
					if (hSnapShot)
						CloseHandle(hSnapShot);

					m_mapModules[uHash] = { reinterpret_cast<std::uintptr_t>(entry.modBaseAddr), std::string(entry.szExePath), static_cast<std::uintptr_t>(entry.modBaseSize) };
 					return m_mapModules[uHash];
				}
			}

			if (hSnapShot)
				CloseHandle(hSnapShot);
		}

		return m_mapModules[uHash];
	}

	std::uintptr_t PatterScan(const char* szModuleName, const char* szSignature, uint16_t uFlags = NO_FLAGS, std::uint32_t uOption1 = 0U, std::uint32_t uOption2 = 0U)
	{
		ModuleInformation_t moduleInformation = GetModule(szModuleName);

		uint8_t* pModuleBytes = new uint8_t[moduleInformation.m_uModuleSize];
		if (!ReadMemoryRaw(moduleInformation.m_uBaseAddress, pModuleBytes, moduleInformation.m_uModuleSize))
		{
		    std::cout << X("Failed to read module bytes") << std::endl;
		    return 0U;
		}

		std::uintptr_t uAddress = FindSignature(reinterpret_cast<std::uintptr_t>(pModuleBytes), moduleInformation.m_uBaseAddress, szSignature);
		if (uAddress == 0U)
		{
			std::cout << X("Failed to find signature in ") << szModuleName << std::endl;
		    return false;
		}

		if (uFlags & SCAN_RESOLVE_RIP)
			uAddress = ResolveRelativeAddress(uAddress, uOption1, uOption2);

		if (uFlags & SCAN_RESOLVE_ABS)
			uAddress = GetAbsoluteAddress(uAddress, uOption1, uOption2);

		uAddress -= reinterpret_cast<std::uintptr_t>(pModuleBytes);
		std::uintptr_t uFinalAddress = moduleInformation.m_uBaseAddress + uAddress;

		delete[] pModuleBytes;
		return uFinalAddress;
	}

private:
	bool SignatureIsMatch(std::uint8_t* pAddress, std::uint8_t* pPattern, std::uint8_t* pMask)
	{
		std::size_t uIndex = 0U;
		while (pAddress[uIndex] == pPattern[uIndex] || pMask[uIndex] == (uint8_t)('?'))
		{
			if (!pMask[++uIndex])
				return true;
		}

		return false;
	}

	std::uintptr_t FindSignature(uintptr_t uAddress, uintptr_t uSize, const char* szSignature)
	{
		std::size_t uSignatureLength = strlen(szSignature);

		std::uint8_t* pPatternAlloc = new std::uint8_t[uSignatureLength >> 1];
		std::uint8_t* pMaskAlloc = new std::uint8_t[uSignatureLength >> 1];
		std::uint8_t* pPattern = pPatternAlloc;
		std::uint8_t* pMask = pMaskAlloc;

		// Run-Time IDA Sig to Sig & Mask
		std::size_t uPatternLength = 0;
		while (*szSignature)
		{
			if (*szSignature == ' ') szSignature++;
			if (!*szSignature) break;

			if (*(std::uint8_t*)(szSignature) == (std::uint8_t)('\?'))
			{
				*pPattern++ = 0;
				*pMask++ = '?';
				szSignature += ((*(std::uint16_t*)(szSignature) == (std::uint16_t)('\?\?')) ? 2 : 1);
			}
			else
			{
				*pPattern++ = BYTE_Get(szSignature);
				*pMask++ = 'x';
				szSignature += 2;
			}

			++uPatternLength;
		}

		// Find Address
		*pMask = 0;

		std::uintptr_t uReturnValue = 0;
		for (std::uintptr_t i = 0; uSize > i; ++i)
		{
			if (SignatureIsMatch(reinterpret_cast<std::uint8_t*>(uAddress + i), pPatternAlloc, pMaskAlloc))
			{
				uReturnValue = (uAddress + i);
				break;
			}
		}

		delete[] pPatternAlloc;
		delete[] pMaskAlloc;
		return uReturnValue;
	}

	std::uintptr_t GetAbsoluteAddress(std::uintptr_t uRelativeAddr, std::uint32_t uPreOffset = 0U, std::uint32_t uPostOffset = 0U)
	{
		uRelativeAddr += uPreOffset;
		uRelativeAddr += sizeof(std::int32_t) + *reinterpret_cast<std::int32_t*>(uRelativeAddr);
		uRelativeAddr += uPostOffset;
		return uRelativeAddr;
	}

	std::uintptr_t ResolveRelativeAddress(std::uintptr_t uAddressBytes, std::uint32_t uRVAOffset, std::uint32_t uRIPOffset, std::uint32_t uAdditionalOffset = 0U)
	{
		const std::uintptr_t uRVA = *reinterpret_cast<PLONG>(uAddressBytes + uRVAOffset);
		const std::uintptr_t uRIP = uAddressBytes + uRIPOffset;

		if (uAdditionalOffset)
			return ReadMemory<std::uintptr_t>(uRVA + uRIP) + uAdditionalOffset;

		return uRVA + uRIP;
	}

private:
	HANDLE m_pProcessHandle = nullptr;
	DWORD m_pProcessID = 0;
	
	std::unordered_map<FNV1A_t, std::uintptr_t> m_mapImports;
	std::unordered_map<FNV1A_t, ModuleInformation_t> m_mapModules;
};

inline CMemory g_Memory;
