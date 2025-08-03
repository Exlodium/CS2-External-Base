#include "Precompiled.h"

bool Globals::UpdateGlobals()
{
	Globals::m_pLocalPlayerController = g_Memory.Read<CCSPlayerController*>(Offsets::Client::dwLocalPlayerController);
	Globals::m_pLocalPlayerPawn = Globals::m_pLocalPlayerController->m_hPlayerPawn().Get( );
	Globals::m_uEntityList = g_Memory.Read<std::uintptr_t>(Offsets::Client::dwEntityList);

	if (!Globals::m_pLocalPlayerController || !Globals::m_pLocalPlayerPawn || Globals::m_uEntityList == NULL)
		return false;

	return true;
}