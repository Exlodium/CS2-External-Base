#pragma once

namespace Offsets
{
    inline constexpr std::ptrdiff_t dwGlobalVars = 0x16AB2E0;

    inline constexpr std::ptrdiff_t dwEntityList = 0x17AA8F8;
    inline constexpr std::ptrdiff_t dwLocalPlayerController = 0x17F9C18;
    inline constexpr std::ptrdiff_t dwViewMatrix = 0x1899080;

    inline constexpr std::ptrdiff_t dwGameSceneNode = 0x310;
    inline constexpr std::ptrdiff_t dwModelState = 0x160;
    inline constexpr std::ptrdiff_t dwBoneMatrix = 0x80;

    inline constexpr std::ptrdiff_t m_sSanitizedPlayerName = 0x720;
    inline constexpr std::ptrdiff_t m_hPlayerPawn = 0x7BC;
    
    inline constexpr std::ptrdiff_t m_bPawnIsAlive = 0x7C4;
    inline constexpr std::ptrdiff_t m_bIsLocalPlayerController = 0x6A0;
    
    inline constexpr std::ptrdiff_t m_iTeamNum = 0x3BF;

    inline constexpr std::ptrdiff_t m_ArmorValue = 0x1500;
    inline constexpr std::ptrdiff_t m_iHealth = 0x32C;

    inline constexpr std::ptrdiff_t m_flFlashMaxAlpha = 0x144C;
    inline constexpr std::ptrdiff_t m_flFlashDuration = 0x1450;

    inline constexpr std::ptrdiff_t m_vOldOrigin = 0x1224;
    inline constexpr std::ptrdiff_t m_vecLastClipCameraPos = 0x1284;

    inline constexpr std::ptrdiff_t m_angEyeAngles = 0x1508;

    inline constexpr std::ptrdiff_t m_entitySpottedState = 0x1628;

    inline constexpr std::ptrdiff_t m_bHasDefuser = 0x40;
    inline constexpr std::ptrdiff_t m_bHasHelmet = 0x41;
    inline constexpr std::ptrdiff_t m_bHasHeavyArmor = 0x42;
 
    inline constexpr std::ptrdiff_t m_pInGameMoneyServices = 0x6D0;
    inline constexpr std::ptrdiff_t m_pItemServices = 0x10B0;

    inline constexpr std::ptrdiff_t m_bIsScoped = 0x1398;
    inline constexpr std::ptrdiff_t m_bIsDefusing = 0x13A0;

    inline constexpr std::ptrdiff_t m_iAccount = 0x40;
    inline constexpr std::ptrdiff_t m_iStartAccount = 0x44;
    inline constexpr std::ptrdiff_t m_iTotalCashSpent = 0x48;
    inline constexpr std::ptrdiff_t m_iCashSpentThisRound = 0x4C;
    inline constexpr std::ptrdiff_t m_nPreviousAccount = 0x50;
}