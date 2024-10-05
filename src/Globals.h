#pragma once

class IGlobalVars
{
public:
	float m_flRealTime; //0x0000
	int32_t m_nFrameCount; //0x0004
	float m_flFrameTime; //0x0008
	float m_flFrameTime2; //0x000C
	int32_t m_nMaxClients; //0x0010
private:
	char pad0x1c[0x1C];
public:
	float m_flIntervalPerSubTick; //0x0030
	float m_flCurrentTime; //0x0034
	float m_flCurrentTime2; //0x0038
private:
	char pad0x14[0x14];
public:
	int32_t m_nTickCount; //0x0048
};

namespace Interfaces
{
	inline IGlobalVars m_GlobalVariables = { };
}

namespace Modules
{
	inline ModuleInfo_t m_pClient = ModuleInfo_t();
	inline ModuleInfo_t m_pEngine = ModuleInfo_t();
	inline ModuleInfo_t m_pSchemaSystem = ModuleInfo_t();

	inline bool GetModule(ModuleInfo_t& pModuleOut, const std::string_view strModule)
	{
		pModuleOut = g_Memory.GetModuleAddress(strModule);
		if (pModuleOut.m_uAddress == NULL || pModuleOut.m_strPath.empty())
		{
			Logging::PushConsoleColor(FOREGROUND_INTENSE_RED);
			Logging::Print(X("Failed to get module for {}"), strModule);
			Logging::PopConsoleColor();
			return false;
		}

		return true;
	}
}

// class forwarding
class CCSPlayerController;
class C_CSPlayerPawn;
namespace Globals
{
	// original window module
	inline HMODULE m_hDll = nullptr;
	// our own window handle
	inline HWND m_Instance = nullptr;
	// are we unloading?
	inline bool m_bIsUnloading = false;

	// current tick
	inline int	m_nCurrentTick = 0;
	// previous tick
	inline int	m_nPreviousTick = 0;

	// pointer to local player controller
	inline CCSPlayerController* m_pLocalPlayerController = nullptr;
	// pointer to local player pawn ( casted from player controller )
	inline C_CSPlayerPawn* m_pLocalPlayerPawn = nullptr;
	// global entity list access point
	inline std::uintptr_t m_uEntityList = NULL;
	
	bool UpdateGlobals();
}

class CTimer
{
public:
	CTimer( const bool bStart = false )
	{
		if (bStart)
			Reset( );
	}

	/* set time point to now */
	void Reset( )
	{
		timePoint = std::chrono::high_resolution_clock::now( );
	}

	/* returns elapsed time between last time point and now in given duration type (default: milliseconds) */
	template <class C = std::chrono::milliseconds>
	[[nodiscard]] auto Elapsed( ) const
	{
		return std::chrono::duration_cast< C >( std::chrono::high_resolution_clock::now( ) - timePoint ).count( );
	}

private:
	std::chrono::high_resolution_clock::time_point timePoint = { };
};