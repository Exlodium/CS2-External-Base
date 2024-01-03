#include "../../Precompiled.h"

void CAimBot::UpdateValues( )
{
    this->m_flSmoothness = Variables::Aimbot::m_flAimbotSmooth;
    this->m_flFOV = Variables::Aimbot::m_flAimbotFOV;
}

void CAimBot::Run( CCSPlayerController* pEntity, C_CSPlayerPawn* pPawn )
{
    if (!Input::HandleInput( Variables::Aimbot::m_iAimbotKey ))
        return;

    this->UpdateValues( );

    // do your aimbot here
}