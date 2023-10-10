#pragma once

namespace Offsets
{
    inline constexpr std::ptrdiff_t dwGlobalVars = 0x168DD60;

    inline constexpr std::ptrdiff_t dwEntityList = 0x178A808;
    inline constexpr std::ptrdiff_t dwLocalPlayerController = 0x17D9218;
    inline constexpr std::ptrdiff_t dwViewMatrix = 0x18787E0;

    inline constexpr std::ptrdiff_t dwGameSceneNode = 0x310;
    inline constexpr std::ptrdiff_t dwModelState = 0x160;
    inline constexpr std::ptrdiff_t dwBoneMatrix = 0x80;

    inline constexpr std::ptrdiff_t dwSanitizedName = 0x720;
    inline constexpr std::ptrdiff_t dwPlayerPawn = 0x7FC;
    
    inline constexpr std::ptrdiff_t m_bPawnIsAlive = 0x804;
    inline constexpr std::ptrdiff_t m_bIsLocalPlayerController = 0x6A0;
    inline constexpr std::ptrdiff_t m_bPawnHasDefuser = 0x810;
    inline constexpr std::ptrdiff_t m_bPawnHasHelmet = 0x811;
    
    inline constexpr std::ptrdiff_t m_iPawnHealth = 0x808;
    inline constexpr std::ptrdiff_t m_iPawnArmor = 0x80C;
    inline constexpr std::ptrdiff_t m_iTeamNum = 0x3BF;

    inline constexpr std::ptrdiff_t m_flFlashMaxAlpha = 0x1450;
    inline constexpr std::ptrdiff_t m_flFlashDuration = 0x1454;

    inline constexpr std::ptrdiff_t m_vecOrigin = 0x1214;
    inline constexpr std::ptrdiff_t m_vecLastClipCameraPos = 0x1274;

    inline constexpr std::ptrdiff_t m_angEyeAngles = 0x1500;

    inline constexpr std::ptrdiff_t m_entitySpottedState = 0x1620;
}