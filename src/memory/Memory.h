#pragma once

// @Credits: Cazz ( https://github.com/cazzwastaken/pro-bhop/blob/master/cheat/memory.h )
class Memory
{
private:
	DWORD pProcessId = 0;
	void* pProcessHandle = nullptr;

public:
	// Constructor that finds the process id
	// and opens a handle
	Memory( const std::string_view processName ) noexcept
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
	~Memory( )
	{
		if (pProcessHandle)
			::CloseHandle( pProcessHandle );
	}

	// Returns the base address of a module by name
	const std::uintptr_t GetModuleAddress( const std::string_view moduleName ) const noexcept
	{
		::MODULEENTRY32 entry = { };
		entry.dwSize = sizeof( ::MODULEENTRY32 );

		const auto snapShot = ::CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, pProcessId );

		std::uintptr_t result = 0;

		while (::Module32Next( snapShot, &entry ))
		{
			if (!moduleName.compare( entry.szModule ))
			{
				result = reinterpret_cast< std::uintptr_t >( entry.modBaseAddr );
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

};