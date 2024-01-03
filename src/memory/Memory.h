#pragma once

struct ModuleInfo_t
{
	ModuleInfo_t() = default;
	ModuleInfo_t(std::uintptr_t uAddress, const char* szPath)
	{
		m_uAddress = uAddress;
		m_szPath = szPath;
	}

	~ModuleInfo_t()
	{
		m_uAddress = NULL;
		m_szPath = NULL;
	}

	std::uintptr_t m_uAddress = NULL;
	const char* m_szPath = NULL;
};

// @Credits: Cazz ( https://github.com/cazzwastaken/pro-bhop/blob/master/cheat/memory.h )
class CMemory
{
private:
	DWORD pProcessId = 0;
	void* pProcessHandle = nullptr;

public:
	CMemory( ) = default;

	// Constructor that finds the process id
	// and opens a handle
	CMemory( const std::string_view processName ) noexcept
	{
		::PROCESSENTRY32 entry = { };
		entry.dwSize = sizeof( ::PROCESSENTRY32 );

		const auto snapShot = ::CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

		while (::Process32Next( snapShot, &entry ))
		{
			if (!processName.compare( entry.szExeFile ))
			{
				pProcessId = entry.th32ProcessID;
				pProcessHandle = ::OpenProcess( PROCESS_ALL_ACCESS, FALSE, pProcessId );
				break;
			}
		}

		// Free handle
		if (snapShot)
			::CloseHandle( snapShot );
	}

	// Destructor that frees the opened handle
	~CMemory( )
	{
		if (pProcessHandle)
			::CloseHandle( pProcessHandle );
	}

	// Returns the base address of a module by name
	const ModuleInfo_t GetModuleAddress( const std::string_view moduleName ) const noexcept
	{
		::MODULEENTRY32 entry = { };
		entry.dwSize = sizeof( ::MODULEENTRY32 );

		const auto snapShot = ::CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, pProcessId );

		ModuleInfo_t result = ModuleInfo_t();
		while (::Module32Next( snapShot, &entry ))
		{
			if (!moduleName.compare( entry.szModule ))
			{
				char pBuffer[MAX_PATH + 1];
				K32GetModuleFileNameExA(pProcessHandle, reinterpret_cast<HMODULE>(entry.modBaseAddr), pBuffer, MAX_PATH);
				result = ModuleInfo_t(reinterpret_cast< std::uintptr_t >( entry.modBaseAddr ), pBuffer);
				break;
			}
		}

		if (snapShot)
			::CloseHandle( snapShot );

		return result;
	}

	// Read process memory
	template <typename T>
	constexpr const T Read( const std::uintptr_t& address ) const noexcept
	{
		T value = { };
		::ReadProcessMemory( pProcessHandle, reinterpret_cast< const void* >( address ), &value, sizeof( T ), NULL );
		return value;
	}

	const bool ReadRaw( uintptr_t address, void* buffer, size_t size )
	{
		SIZE_T bytesRead;
		if (ReadProcessMemory( pProcessHandle, reinterpret_cast< LPCVOID >( address ), buffer, size, &bytesRead ))
		{
			return bytesRead == size;
		}
		return false;
	}

	const std::string ReadString( std::uint64_t dst )
	{
		if (!dst)
			return X( "**invalid**" );

		char buf[ 256 ] = {};
		return ( ReadRaw( dst, &buf, sizeof( buf ) ) ? std::string( buf ) : X( "**invalid**" ) );
	}

	DWORD64 TraceAddress( DWORD64 BaseAddress, std::vector<DWORD> Offsets )
	{
		DWORD64 Address = 0;

		if (Offsets.size( ) == 0)
			return BaseAddress;

		Address = Read<DWORD64>( BaseAddress );

		for (int i = 0; i < Offsets.size( ) - 1; i++) {
			Address = Read<DWORD64>( Address + Offsets[ i ] );
		}
		return Address == 0 ? 0 : Address + Offsets[ Offsets.size( ) - 1 ];
	}

	//@NOTE: This is not safe, you should avoid using it
	// Write process memory
	template <typename T>
	constexpr void Write( const std::uintptr_t& address, const T& value ) const noexcept
	{
		::WriteProcessMemory( pProcessHandle, reinterpret_cast< void* >( address ), &value, sizeof( T ), NULL );
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
				{
					vecBytes.push_back(strtoul(szCurrent, &szCurrent, 16));
				}
			}
			return vecBytes;
		};

		auto dosHeader = (PIMAGE_DOS_HEADER)module;
		auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dosHeader->e_lfanew);

		auto dwSizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
		auto vecPatternBytes = PatternToBytes(szSignature);
		auto uScanBytes = reinterpret_cast<std::uint8_t*>(module);

		auto s = vecPatternBytes.size();
		auto d = vecPatternBytes.data();

		for (auto i = 0ul; i < dwSizeOfImage - s; ++i)
		{
			bool bFound = true;
			for (auto j = 0ul; j < s; ++j) 
			{
				if (uScanBytes[i + j] != d[j] && d[j] != -1)
				{

					bFound = false;
					break;
				}
			}
			if (bFound)
			{
				return reinterpret_cast<std::uintptr_t>(&uScanBytes[i]);
			}
		}
		return 0;
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