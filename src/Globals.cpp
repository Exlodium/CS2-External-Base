#include "Includes.h"

bool CInterfaces::Update()
{
	m_GlobalVars = g_Memory.ReadMemory<CGlobalVars>(g_Memory.ReadMemory<std::uintptr_t>(g_Globals.m_Offsets.m_uGlobalVars));
	m_CSGOInput = g_Memory.ReadMemory<CCSGOInput>(g_Memory.ReadMemory<std::uintptr_t>(g_Globals.m_Offsets.m_uCSGOInput));
	m_NetworkGameClient = g_Memory.ReadMemory<CNetWorkGameClient>(g_Memory.ReadMemory<std::uintptr_t>(g_Globals.m_Offsets.m_uNetworkGameClient));
	return true;
}

bool CGlobals::Update()
{
	static std::once_flag flag;
	std::call_once(flag, []()
	{
		g_Globals.m_Offsets.m_uEntityList = g_Memory.PatterScan(CLIENT_DLL, X("48 8B 0D ? ? ? ? 48 89 7C 24 ?? 8B FA C1 EB"), EPatternScanFlags::SCAN_RESOLVE_RIP, 0x3, 0x7);
		g_Globals.m_Offsets.m_uViewMatrix = g_Memory.PatterScan(CLIENT_DLL, X("48 8D 0D ? ? ? ? 48 C1 E0 06"), EPatternScanFlags::SCAN_RESOLVE_RIP, 0x3, 0x7);
		g_Globals.m_Offsets.m_uLocalPlayerController = g_Memory.PatterScan(CLIENT_DLL, X("48 8B 05 ? ? ? ? 41 89 BE"), EPatternScanFlags::SCAN_RESOLVE_RIP, 0x3, 0x7);
		g_Globals.m_Offsets.m_uPlantedC4 = g_Memory.PatterScan(CLIENT_DLL, X("4C 8B 0D ? ? ? ? 8B C8 4A 39 3C 09"), EPatternScanFlags::SCAN_RESOLVE_RIP, 0x3, 0x7);
		g_Globals.m_Offsets.m_uAutoAcceptArray = g_Memory.PatterScan(CLIENT_DLL, X("48 89 05 ? ? ? ? E8 ? ? ? ? 48 85 DB"), EPatternScanFlags::SCAN_RESOLVE_RIP, 0x3, 0x7);
		
		g_Globals.m_Offsets.m_uGlobalVars = g_Memory.PatterScan(CLIENT_DLL, X("48 89 15 ? ? ? ? 48 89 42"), EPatternScanFlags::SCAN_RESOLVE_RIP, 0x3, 0x7);
		g_Globals.m_Offsets.m_uCSGOInput = g_Memory.PatterScan(CLIENT_DLL, X("48 8B 0D ? ? ? ? 4C 8B C6 8B 10 E8"), EPatternScanFlags::SCAN_RESOLVE_RIP, 0x3, 0x7);
		g_Globals.m_Offsets.m_uNetworkGameClient = g_Memory.PatterScan(ENGINE2_DLL, X("48 89 3D ? ? ? ? 48 8D 15"), EPatternScanFlags::SCAN_RESOLVE_RIP, 0x3, 0x7);
		g_Globals.m_Offsets.m_uSensitivity = g_Memory.PatterScan(CLIENT_DLL, X("48 8D 0D ? ? ? ? 66 0F 6E CD"), EPatternScanFlags::SCAN_RESOLVE_RIP, 0x3, 0x7) + 0x8;
	});

	g_Globals.m_uEntityList = g_Memory.ReadMemory<std::uintptr_t>(g_Globals.m_Offsets.m_uEntityList);
	g_Globals.m_LocalPlayer.m_pController = g_Memory.ReadMemory<CCSPlayerController*>(g_Globals.m_Offsets.m_uLocalPlayerController);
	g_Globals.m_LocalPlayer.m_pPlayerPawn = reinterpret_cast<C_CSPlayerPawn*>(g_Globals.m_LocalPlayer.m_pController->m_hPawn().Get());
	g_Globals.m_matViewMatrix = g_Memory.ReadMemory<ViewMatrix_t>(g_Globals.m_Offsets.m_uViewMatrix);

	return true;
}