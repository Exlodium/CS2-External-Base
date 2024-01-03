#pragma once

struct ModuleInfo_t
{
	ModuleInfo_t() = default;
	ModuleInfo_t(std::uintptr_t uAddress, std::string strPath)
	{
		m_uAddress = uAddress;
		m_strPath = strPath;
	}

	~ModuleInfo_t()
	{
		m_uAddress = NULL;
		m_strPath = X("");
	}

	std::uintptr_t m_uAddress = NULL;
	std::string m_strPath = X("");
};

// @Credits: Cazz ( https://github.com/cazzwastaken/pro-bhop/blob/master/cheat/memory.h )
class CMemory
{
private:
	DWORD pProcessId = 0;
	void* pProcessHandle = nullptr;
public:
	CMemory() = default;

	// find process handle and process ID
	void Initialize(const std::string_view processName) noexcept
	{
		while (!pProcessHandle)
		{
			::PROCESSENTRY32 entry = { };
			entry.dwSize = sizeof(::PROCESSENTRY32);

			const HANDLE snapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			while (::Process32Next(snapShot, &entry))
			{
				if (!processName.compare(entry.szExeFile))
				{
					pProcessId = entry.th32ProcessID;
					pProcessHandle = hj::HijackExistingHandle(pProcessId);
					// leaving this commented out here incase you have issues with handle hijacking
					//pProcessHandle = ::OpenProcess(PROCESS_VM_READ, FALSE, pProcessId);
					break;
				}
			}

			// Free handle
			if (snapShot)
				::CloseHandle(snapShot);
		}
	}

	// destructor that frees the opened handle
	~CMemory()
	{
		if (pProcessHandle)
			::CloseHandle(pProcessHandle);

		if (hj::HijackedHandle)
			::CloseHandle(hj::HijackedHandle);
	}

	// returns the base address of a module by name
	const ModuleInfo_t GetModuleAddress(const std::string_view moduleName) const noexcept
	{
		::MODULEENTRY32 entry = { };
		entry.dwSize = sizeof(::MODULEENTRY32);
		const auto snapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pProcessId);

		std::uintptr_t uModuleAddress = NULL;
		std::string strModulePath = X("");

		while (::Module32Next(snapShot, &entry))
		{
			if (!moduleName.compare(entry.szModule))
			{
				uModuleAddress = reinterpret_cast<std::uintptr_t>(entry.modBaseAddr);
				strModulePath = std::string(entry.szExePath);
				break;
			}
		}

		if (snapShot)
			::CloseHandle(snapShot);

		return ModuleInfo_t(uModuleAddress, strModulePath);
	}

	// read process memory
	template <typename T>
	constexpr const T Read(const std::uintptr_t& address) const noexcept
	{
		T value = { };
		::ReadProcessMemory(pProcessHandle, reinterpret_cast<const void*>(address), &value, sizeof(T), NULL);
		return value;
	}

	const bool ReadRaw(uintptr_t address, void* buffer, size_t size)
	{
		SIZE_T bytesRead;
		if (ReadProcessMemory(pProcessHandle, reinterpret_cast<LPCVOID>(address), buffer, size, &bytesRead))
		{
			return bytesRead == size;
		}
		return false;
	}

	const std::string ReadString(std::uint64_t dst)
	{
		if (!dst)
			return X("**invalid**");

		char buf[256] = {};
		return (ReadRaw(dst, &buf, sizeof(buf)) ? std::string(buf) : X("**invalid**"));
	}

	DWORD64 TraceAddress(DWORD64 BaseAddress, std::vector<DWORD> Offsets)
	{
		DWORD64 Address = 0;

		if (Offsets.size() == 0)
			return BaseAddress;

		Address = Read<DWORD64>(BaseAddress);
		for (int i = 0; i < Offsets.size() - 1; i++)
			Address = Read<DWORD64>(Address + Offsets[i]);
		
		return Address == 0 ? 0 : Address + Offsets[Offsets.size() - 1];
	}

	//@NOTE: this is not safe, you should avoid using it
	// write process memory
	template <typename T>
	constexpr void Write(const std::uintptr_t& address, const T& value) const noexcept
	{
		::WriteProcessMemory(pProcessHandle, reinterpret_cast<void*>(address), &value, sizeof(T), NULL);
	}

	std::uintptr_t PatternScan(void* module, const char* szSignature)
	{
		static auto PatternToBytes = [](const char* szPattern)
		{
			auto vecBytes = std::vector<int>{};
			auto szStart = const_cast<char*>(szPattern);
			auto szEnd = const_cast<char*>(szPattern) + CRT::StringLength(szPattern);

			for (auto szCurrent = szStart; szCurrent < szEnd; ++szCurrent)
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
		};

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
		
		return NULL;
	}

	template <typename T = std::uintptr_t>
	T* GetAbsoluteAddress(T* pRelativeAddress, int nPreOffset = 0x0, int nPostOffset = 0x0)
	{
		pRelativeAddress += nPreOffset;
		pRelativeAddress += sizeof(std::int32_t) + *reinterpret_cast<std::int32_t*>(pRelativeAddress);
		pRelativeAddress += nPostOffset;
		return pRelativeAddress;
	}

	std::uintptr_t ResolveRelativeAddress(std::uintptr_t nAddressBytes, std::uint32_t nRVAOffset, std::uint32_t nRIPOffset)
	{
		std::uintptr_t nRVA = *reinterpret_cast<PLONG>(nAddressBytes + nRVAOffset);
		std::uintptr_t nRIP = nAddressBytes + nRIPOffset;

		return nRVA + nRIP;
	}
};

inline CMemory g_Memory;