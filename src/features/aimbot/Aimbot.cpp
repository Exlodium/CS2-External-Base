#include "../../Precompiled.h"

void CAimBot::UpdateValues()
{
    this->m_flSmoothness = Config::Get<float>(g_Variables.m_flAimbotSmooth);
    this->m_flFOV = Config::Get<float>(g_Variables.m_flAimbotFOV);
}

void CAimBot::Run(CCSPlayerController* pEntity, C_CSPlayerPawn* pPawn)
{
    if (!Input::HandleInput(Config::Get<KeyBind_t>(g_Variables.m_AimBotKey)))
        return;

    this->UpdateValues();

    // do your aimbot here
}