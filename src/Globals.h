#pragma once

// class forwarding
class CCSPlayerController;
class C_CSPlayerPawn;
class CGlobals
{
public:
	std::uintptr_t m_uEntityList;

	bool UpdateGlobals();
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