#include "Precompiled.h"

bool Globals::UpdateGlobals()
{
	Globals::m_pLocalPlayerController = g_Memory.Read<CCSPlayerController*>(Offsets::Client::dwLocalPlayerController);
	Globals::m_pLocalPlayerPawn = Globals::m_pLocalPlayerController->m_hPlayerPawn();
	Globals::m_uEntityList = g_Memory.Read<std::uintptr_t>(Offsets::Client::dwEntityList);

	Globals::m_nCSWindowWidth = g_Memory.Read<int>(Offsets::Engine2::dwWindowWidth);
	Globals::m_nCSWindowHeight = g_Memory.Read<int>(Offsets::Engine2::dwWindowHeight);

	if (!Globals::m_pLocalPlayerController || !Globals::m_pLocalPlayerPawn || Globals::m_uEntityList == NULL || Globals::m_nCSWindowWidth == NULL || Globals::m_nCSWindowHeight == NULL)
		return false;

	return true;
}