#pragma once

namespace Offsets
{
    inline constexpr std::ptrdiff_t dwGlobalVars = 0x1690EC8;

    inline constexpr std::ptrdiff_t dwEntityList = 0x178DC58;
    inline constexpr std::ptrdiff_t dwLocalPlayerController = 0x17DC4E8;
    inline constexpr std::ptrdiff_t dwViewMatrix = 0x187BAB0;

    inline constexpr std::ptrdiff_t dwGameSceneNode = 0x310;
    inline constexpr std::ptrdiff_t dwModelState = 0x160;
    inline constexpr std::ptrdiff_t dwBoneMatrix = 0x80;

    inline constexpr std::ptrdiff_t m_sSanitizedPlayerName = 0x720;
    inline constexpr std::ptrdiff_t m_hPlayerPawn = 0x7BC;
    
    inline constexpr std::ptrdiff_t m_bPawnIsAlive = 0x7C4;
    inline constexpr std::ptrdiff_t m_bIsLocalPlayerController = 0x6A0;
    
    inline constexpr std::ptrdiff_t m_iTeamNum = 0x3BF;

    inline constexpr std::ptrdiff_t m_ArmorValue = 0x14F8;
    inline constexpr std::ptrdiff_t m_iHealth = 0x32C;

    inline constexpr std::ptrdiff_t m_flFlashMaxAlpha = 0x1450;
    inline constexpr std::ptrdiff_t m_flFlashDuration = 0x1454;

    inline constexpr std::ptrdiff_t m_vecOrigin = 0x1214;
    inline constexpr std::ptrdiff_t m_vecLastClipCameraPos = 0x1274;

    inline constexpr std::ptrdiff_t m_angEyeAngles = 0x1500;

    inline constexpr std::ptrdiff_t m_entitySpottedState = 0x1620;
}