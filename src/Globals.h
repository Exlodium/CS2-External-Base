#pragma once

class CGlobalVars
{
public:
	float m_flRealTime;             // 0x0000
	int32_t m_nFrameCount;          // 0x0004
	float m_flFrameTime;            // 0x0008
	float m_flFrameTime2;           // 0x000C
	int32_t m_nMaxClients;          // 0x0010
	MEM_PAD(0x1C);                  // 0x0014
	float m_flCurrentTime;          // 0x0030
	float m_flFrameTime3;           // 0x0034
	float m_flTickFraction;         // 0x0038
	float m_flTickFraction2;        // 0x003C
	MEM_PAD(0x8);                   // 0x0040
	int32_t m_nTickCount;           // 0x0048
	MEM_PAD(0xC);                   // 0x004C
	MEM_PAD(0x8);
	MEM_PAD(0x118);                 // 0x0060
	uint64_t m_uMapName;			// 0x0178
	uint64_t m_uMapNameShort;		// 0x0180
};

class CCSGOInput
{
public:
	MEM_PAD(0x06B0);				// 0x0000
	QAngle m_angViewAngle;			// 0x06B0
};

class CNetWorkGameClient
{
public:
	MEM_PAD(0x220);
	int m_nSignonState; 			// 0x0220
};

class CInterfaces
{
public:
	CGlobalVars m_GlobalVars{ };
	CCSGOInput m_CSGOInput{ };
	CNetWorkGameClient m_NetworkGameClient{ };
	
	bool Update();
};

inline CInterfaces g_Interfaces;

// class forwarding
class CCSPlayerController;
class C_CSPlayerPawn;
class CGlobals
{
public:
	struct Offsets_t
	{
		std::uintptr_t m_uEntityList = 0U;
		std::uintptr_t m_uViewMatrix = 0U;
		std::uintptr_t m_uLocalPlayerController = 0U;
		std::uintptr_t m_uPlantedC4 = 0U;
		std::uintptr_t m_uAutoAcceptArray = 0U;
		
		std::uintptr_t m_uGlobalVars = 0U;
		std::uintptr_t m_uCSGOInput = 0U;
		std::uintptr_t m_uNetworkGameClient = 0U;
		std::uintptr_t m_uSensitivity = 0U;
	}; Offsets_t m_Offsets;

	struct LocalPlayer_t
	{
		CCSPlayerController* m_pController = nullptr;
		C_CSPlayerPawn* m_pPlayerPawn = nullptr;

	}; LocalPlayer_t m_LocalPlayer;

	// original window module
	HMODULE m_hDll = nullptr;
	// our own window handle
	HWND m_Instance = nullptr;
	// are we unloading?
	bool m_bIsUnloading = false;

	std::uintptr_t m_uEntityList = 0U;
	ViewMatrix_t m_matViewMatrix{};

	std::shared_mutex m_mtxGlobals{ };

	bool Update();
};

inline CGlobals g_Globals;

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
		m_TimePoint = std::chrono::high_resolution_clock::now( );
	}

	/* returns elapsed time between last time point and now in given duration type (default: milliseconds) */
	template <class C = std::chrono::milliseconds>
	[[nodiscard]] auto Elapsed( ) const
	{
		return std::chrono::duration_cast< C >( std::chrono::high_resolution_clock::now( ) - m_TimePoint).count( );
	}

private:
	std::chrono::high_resolution_clock::time_point m_TimePoint = { };
};