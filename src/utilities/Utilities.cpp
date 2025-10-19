#include "../Includes.h"

void CUtilities::Sleep(float flMilliseconds)
{
	static tZwSetTimerResolution ZwSetTimerResolution = reinterpret_cast<tZwSetTimerResolution>(g_Memory.GetImport(NTDLL_DLL, X("ZwSetTimerResolution")));
	static tNtDelayExecution NtDelayExecution = reinterpret_cast<tNtDelayExecution>(g_Memory.GetImport(NTDLL_DLL,  X("NtDelayExecution")));

	static std::once_flag flag;
	std::call_once(flag, []()
	{
		ULONG uCurrent;
		ZwSetTimerResolution(static_cast<ULONG>(0.5f * 10000.f), true, &uCurrent);
	});

	if (flMilliseconds < 0.5f)
		flMilliseconds = 0.5f;

	LARGE_INTEGER time{};
	time.QuadPart = -1 * static_cast<LONGLONG>(flMilliseconds * 10000.f);
	NtDelayExecution(false, &time);
}

bool CUtilities::IsVisible(C_CSPlayerPawn* pPlayer, Vector vecStart, Vector vecPosition)
{
	if (!pPlayer || vecPosition.IsZero())
		return false;

	const bool bSpotted = pPlayer->m_entitySpottedState().m_bSpottedByMask[0] & (1 << reinterpret_cast<std::uintptr_t>(g_Globals.m_LocalPlayer.m_pPlayerPawn));
	if (!g_MapParser.m_bSetup)
		return bSpotted;

	return g_MapParser.IsVisible(vecStart, vecPosition);
}

bool CUtilities::IsChangingLevel()
{
	return g_Interfaces.m_NetworkGameClient.m_nSignonState == SignonState_t::SIGNONSTATE_CHANGELEVEL;
}

bool CUtilities::IsInGame()
{
	return g_Interfaces.m_NetworkGameClient.m_nSignonState == SignonState_t::SIGNONSTATE_FULL;
}

bool CUtilities::IsConnected()
{
	return g_Interfaces.m_NetworkGameClient.m_nSignonState >= SignonState_t::SIGNONSTATE_CONNECTED;
}