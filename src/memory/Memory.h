#pragma once

using tNtReadVirtualMemory = NTSTATUS(WINAPI*)(HANDLE, PVOID, PVOID, ULONG, PULONG);
using tNtWriteVirtualMemory = NTSTATUS(WINAPI*)(HANDLE, PVOID, PVOID, ULONG, PULONG);
using tNtUserSendInput = NTSTATUS(WINAPI*)(UINT, LPINPUT, int);

struct ModuleInformation_t
{
	ModuleInformation_t() = default;
	ModuleInformation_t(std::uintptr_t uAddress, std::string strPath)
	{
		m_uBaseAddress = uAddress;
		m_strPath = strPath;
	}

	~ModuleInformation_t()
	{
		m_uBaseAddress = 0U;
		m_strPath = X("");
	}

	std::uintptr_t m_uBaseAddress = 0;
	std::string m_strPath = { };
};

enum EPatternScanFlags : uint16_t
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

	void Initialize(const char* szProcessName)
	{
		while (!m_pProcessHandle)
		{
			PROCESSENTRY32 entry = { };
			entry.dwSize = sizeof(PROCESSENTRY32);

			const HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			while (Process32Next(hSnapShot, &entry))
			{
				if (!strcmp(szProcessName, entry.szExeFile))
				{
					m_pProcessID = entry.th32ProcessID;
					//m_pProcessHandle = OpenProcess(PROCESS_VM_READ, FALSE, m_pProcessID);
					m_pProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_pProcessID);
					break;
				}
			}

			if (hSnapShot)
				CloseHandle(hSnapShot);
		}

		const HMODULE hNtDLL = GetModuleHandleA(NTDLL_DLL);
		if (hNtDLL != INVALID_HANDLE_VALUE)
		{
			m_mapImports[FNV1A::HashConst("NtReadVirtualMemory")] = reinterpret_cast<std::uintptr_t>(GetProcAddress(hNtDLL, X("NtReadVirtualMemory")));
			m_mapImports[FNV1A::HashConst("NtWriteVirtualMemory")] = reinterpret_cast<std::uintptr_t>(GetProcAddress(hNtDLL, X("NtWriteVirtualMemory")));
			m_mapImports[FNV1A::HashConst("NtUserSendInput")] = reinterpret_cast<std::uintptr_t>(GetProcAddress(hNtDLL, X("NtUserSendInput")));

			FreeLibrary(hNtDLL);
		}
	}
	
	ModuleInformation_t GetModule(const char* szModuleName)
	{
		FNV1A_t uHash = FNV1A::Hash(szModuleName);
		if (m_mapModules.find(uHash) == m_mapModules.end())
		{
			MODULEENTRY32 entry = { };
			entry.dwSize = sizeof(MODULEENTRY32);
			const HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_pProcessID);

			while (Module32Next(hSnapShot, &entry))
			{
				if (!strcmp(szModuleName, entry.szModule))
				{
					m_mapModules[uHash] = { (reinterpret_cast<std::uintptr_t>(entry.modBaseAddr)), std::string(entry.szExePath) };
					break;
				}
			}

			if (hSnapShot)
				CloseHandle(hSnapShot);
		}

		return m_mapModules[uHash];
	}

	bool ReadMemoryRaw(const std::uintptr_t& uAddress, void* pBuffer, std::size_t uSize)
	{
		static tNtReadVirtualMemory NtReadVirtualMemory = reinterpret_cast<tNtReadVirtualMemory>(m_mapImports[FNV1A::HashConst("NtReadVirtualMemory")]);
		const NTSTATUS ret = NtReadVirtualMemory(m_pProcessHandle, reinterpret_cast<PVOID>(uAddress), pBuffer, static_cast<ULONG>(uSize), 0);
		return ret != 0;
	} 

	template<typename T = std::uintptr_t>
	T ReadMemory(const std::uintptr_t& uAddress)
	{
		T buffer = { };
		ReadMemoryRaw(uAddress, &buffer, sizeof(T));
		return buffer;
	}

	std::string ReadMemoryString(const std::uintptr_t& uAddress)
	{
		char buffer[MAX_PATH] = { };
		ReadMemoryRaw(uAddress, &buffer, sizeof(buffer));
		return std::string(buffer);
	}

	template<typename T = std::uintptr_t>
	void WriteMemory(const std::uintptr_t& uAddress, T value)
	{
		static tNtWriteVirtualMemory NtWriteVirtualMemory = reinterpret_cast<tNtWriteVirtualMemory>(m_mapImports[FNV1A::HashConst("NtWriteVirtualMemory")]);
		NtWriteVirtualMemory(m_pProcessHandle, reinterpret_cast<PVOID>(uAddress), &value, sizeof(T), nullptr);
	}

	std::uintptr_t PatterScan(const char* szModuleName, const char* szSignature, uint16_t uFlags = NO_FLAGS, std::uint32_t uOption1 = 0U, std::uint32_t uOption2 = 0U)
	{
		// get information about module
		ModuleInformation_t moduleInformation = GetModule(szModuleName);
		
		// load module into our program
		HMODULE hLoadedModule = LoadLibraryExA(moduleInformation.m_strPath.c_str(), nullptr, DONT_RESOLVE_DLL_REFERENCES);

		// get address of offset in loaded module
		std::uintptr_t uAddress = PatternScanInternal(reinterpret_cast<void*>(moduleInformation.m_uBaseAddress), szSignature);

		// check if we should resolve rip
		if (uFlags & SCAN_RESOLVE_RIP)
		{
			uAddress = ResolveRelativeAddress(uAddress, uOption1, uOption2);
		}

		// check if we should resolve absoulute address
		if (uFlags & SCAN_RESOLVE_ABS)
		{
			uAddress = GetAbsoluteAddress(uAddress, uOption1, uOption2);
		}

		// subtract the base address of our module
		uAddress -= reinterpret_cast<std::uintptr_t>(hLoadedModule);

		// free the loaded module
		FreeLibrary(hLoadedModule);

		// return offset with base address of the game's module
		return moduleInformation.m_uBaseAddress + uAddress;
	}

private:
	std::vector<int> PatternToBytes(const char* szPattern)
	{
		std::vector<int> vecBytes = {};
		char* szStart = const_cast<char*>(szPattern);
		char* szEnd = const_cast<char*>(szPattern) + strlen(szPattern);

		for (char* szCurrent = szStart; szCurrent < szEnd; ++szCurrent)
		{
			if (*szCurrent == '?')
			{
				++szCurrent;
				if (*szCurrent == '?')
					++szCurrent;
				vecBytes.push_back(-1);
			}
			else
				vecBytes.push_back(strtoul(szCurrent, &szCurrent, 16));
		}
		return vecBytes;
	}

	std::uintptr_t PatternScanInternal(void* module, const char* szSignature)
	{
		PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(module);
		PIMAGE_NT_HEADERS ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<std::uint8_t*>(module) + dosHeader->e_lfanew);

		DWORD dwSizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
		std::vector<int> vecPatternBytes = PatternToBytes(szSignature);
		std::uint8_t* uScanBytes = reinterpret_cast<std::uint8_t*>(module);

		size_t uSize = vecPatternBytes.size();
		int* pData = vecPatternBytes.data();

		for (unsigned long i = 0ul; i < dwSizeOfImage - uSize; ++i)
		{
			bool bFound = true;
			for (unsigned long j = 0ul; j < uSize; ++j)
			{
				if (uScanBytes[i + j] != pData[j] && pData[j] != -1)
				{
					bFound = false;
					break;
				}
			}

			if (bFound)
				return reinterpret_cast<std::uintptr_t>(&uScanBytes[i]);
		}

		return 0U;
	}

	std::uintptr_t GetAbsoluteAddress(std::uintptr_t pRelativeAddress, std::uint32_t nPreOffset = 0x0, std::uint32_t nPostOffset = 0x0)
	{
		pRelativeAddress += nPreOffset;
		pRelativeAddress += sizeof(std::int32_t) + *reinterpret_cast<std::int32_t*>(pRelativeAddress);
		pRelativeAddress += nPostOffset;
		return pRelativeAddress;
	}

	std::uintptr_t ResolveRelativeAddress(std::uintptr_t nAddressBytes, std::uint32_t nRVAOffset, std::uint32_t nRIPOffset, std::uint32_t nOffset = 0) 
	{
		const std::uintptr_t nRVA = *reinterpret_cast<PLONG>(nAddressBytes + nRVAOffset);
		const std::uintptr_t nRIP = nAddressBytes + nRIPOffset;

		if (nOffset)
			return ReadMemory< std::uintptr_t >(nRVA + nRIP) + nOffset;

		return nRVA + nRIP;
	}

	std::unordered_map<FNV1A_t, std::uintptr_t> m_mapImports;
	std::unordered_map<FNV1A_t, ModuleInformation_t> m_mapModules;
	
	DWORD m_pProcessID = 0;
	void* m_pProcessHandle = nullptr;
};

inline CMemory g_Memory;