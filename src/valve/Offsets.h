#pragma once

namespace Signatures
{
    inline const char* GlobalVars = X("48 89 0D ?? ?? ?? ?? 48 89 41"); // rip 3 | 7
    inline const char* ViewMatrix = X("48 8D 0D ?? ?? ?? ?? 48 C1 E0 06"); // rip 3 | 7
    inline const char* ViewAngles = X("48 8B 0D ?? ?? ?? ?? E9 ?? ?? ?? ?? CC CC CC CC 40 55"); // rip 3 | 7
    inline const char* EntityList = X("48 8B 0D ?? ?? ?? ?? 48 89 7C 24 ?? 8B FA C1 EB"); // rip 3 | 7
    inline const char* LocalPlayerController = X("48 8B 05 ?? ?? ?? ?? 48 85 C0 74 4F"); // rip 3 | 7
    inline const char* ForceJump = X("48 8B 05 ?? ?? ?? ?? 48 8D 1D ?? ?? ?? ?? 48 89 45"); // rip 3 | 7
    inline const char* PlantedC4 = X("48 8B 15 ?? ?? ?? ?? FF C0 48 8D 4C 24 40"); // rip 3 | 7

    inline const char* WindowWidth = X("8B 05 ?? ?? ?? ?? 89 07"); // rip 2 | 6
    inline const char* WindowHeight = X("8B 05 ?? ?? ?? ?? 89 03"); // rip 2 | 6
}

namespace Offsets
{
    namespace Client
    {
        inline std::uintptr_t dwGlobalVars = 0x0;
        inline std::uintptr_t dwEntityList = 0x0;
        inline std::uintptr_t dwLocalPlayerController = 0x0;
        inline std::uintptr_t dwViewMatrix = 0x0;
        inline std::uintptr_t dwViewAngles = 0x0;
        inline std::uintptr_t dwForceJump = 0x0;
        inline std::uintptr_t dwPlatedC4 = 0x0;

        inline constexpr std::uintptr_t dwBoneMatrix = 0x80;
    }

    namespace Engine2
    {
        inline std::uintptr_t dwWindowHeight = 0x0;
        inline std::uintptr_t dwWindowWidth = 0x0;
    }

    inline constexpr std::ptrdiff_t dwGlobalVars = 0x16BDCA8;

    inline constexpr std::ptrdiff_t dwEntityList = 0x17C1960;
    inline constexpr std::ptrdiff_t dwLocalPlayerController = 0x1810F58;
    inline constexpr std::ptrdiff_t dwViewMatrix = 0x1820160;

    inline constexpr std::ptrdiff_t dwGameSceneNode = 0x310;
    inline constexpr std::ptrdiff_t dwModelState = 0x160;
    inline constexpr std::ptrdiff_t dwBoneMatrix = 0x80;

    inline constexpr std::ptrdiff_t m_sSanitizedPlayerName = 0x750;
    inline constexpr std::ptrdiff_t m_hPlayerPawn = 0x7EC;
    
    inline constexpr std::ptrdiff_t m_bPawnIsAlive = 0x7F4;
    inline constexpr std::ptrdiff_t m_bIsLocalPlayerController = 0x6D0;
    
    inline constexpr std::ptrdiff_t m_iTeamNum = 0x3BF;

    inline constexpr std::ptrdiff_t m_ArmorValue = 0x1508;
    inline constexpr std::ptrdiff_t m_iHealth = 0x32C;

    inline constexpr std::ptrdiff_t m_flFlashMaxAlpha = 0x146C;
    inline constexpr std::ptrdiff_t m_flFlashDuration = 0x1470;

    inline constexpr std::ptrdiff_t m_vOldOrigin = 0x1224;
    inline constexpr std::ptrdiff_t m_vecLastClipCameraPos = 0x1294;

    inline constexpr std::ptrdiff_t m_angEyeAngles = 0x1518;

    inline constexpr std::ptrdiff_t m_entitySpottedState = 0x1638;

    inline constexpr std::ptrdiff_t m_bHasDefuser = 0x40;
    inline constexpr std::ptrdiff_t m_bHasHelmet = 0x41;
    inline constexpr std::ptrdiff_t m_bHasHeavyArmor = 0x42;
 
    inline constexpr std::ptrdiff_t m_pInGameMoneyServices = 0x700;
    inline constexpr std::ptrdiff_t m_pItemServices = 0x10B0;

    inline constexpr std::ptrdiff_t m_bIsScoped = 0x13A8;
    inline constexpr std::ptrdiff_t m_bIsDefusing = 0x13A9;

    inline constexpr std::ptrdiff_t m_iAccount = 0x40;
    inline constexpr std::ptrdiff_t m_iStartAccount = 0x44;
    inline constexpr std::ptrdiff_t m_iTotalCashSpent = 0x48;
    inline constexpr std::ptrdiff_t m_iCashSpentThisRound = 0x4C;
    inline constexpr std::ptrdiff_t m_nPreviousAccount = 0x50;

    inline bool GetOffsets()
    {
        HMODULE hClientDLL = LoadLibraryExA(Modules::m_pClient.m_szPath, 0, DONT_RESOLVE_DLL_REFERENCES);
        HMODULE hEngine2DLL = LoadLibraryExA(Modules::m_pEngine.m_szPath, 0, DONT_RESOLVE_DLL_REFERENCES);
        
        Client::dwEntityList = g_Memory.ResolveRelativeAddress(g_Memory.PatternScan(hClientDLL, Signatures::EntityList), 0x3, 0x7);
        if (Client::dwEntityList == 0)
            return false;

        Client::dwLocalPlayerController = g_Memory.ResolveRelativeAddress(g_Memory.PatternScan(hClientDLL, Signatures::LocalPlayerController), 0x3, 0x7);
        if (Client::dwLocalPlayerController == 0)
            return false;

        Client::dwViewMatrix = g_Memory.ResolveRelativeAddress(g_Memory.PatternScan(hClientDLL, Signatures::ViewMatrix), 0x3, 0x7);
        if (Client::dwViewMatrix == 0)
            return false;

        std::ptrdiff_t tempValue = g_Memory.ResolveRelativeAddress(g_Memory.PatternScan(hClientDLL, Signatures::ViewAngles), 0x3, 0x7);
        tempValue = g_Memory.Read<std::ptrdiff_t>(tempValue);
        Client::dwViewAngles = tempValue + 0x6140;
        if (Client::dwViewAngles == 0)
            return false;

       Client::dwGlobalVars = g_Memory.ResolveRelativeAddress(g_Memory.PatternScan(hClientDLL, Signatures::GlobalVars), 0x3, 0x7);
        if (Client::dwGlobalVars == 0)
            return false;

        Client::dwForceJump = g_Memory.ResolveRelativeAddress(g_Memory.PatternScan(hClientDLL, Signatures::ForceJump), 0x3, 0x7); + 0x30;
        if (Client::dwForceJump == 0)
            return false;

        Client::dwPlatedC4 = g_Memory.ResolveRelativeAddress(g_Memory.PatternScan(hClientDLL, Signatures::PlantedC4), 0x3, 0x7);
        if (Client::dwPlatedC4 == 0)
            return false;

        Engine2::dwWindowWidth = g_Memory.ResolveRelativeAddress(g_Memory.PatternScan(hEngine2DLL, Signatures::WindowWidth), 0x2, 0x6);
        if (Engine2::dwWindowWidth == 0)
            return false;

        Engine2::dwWindowHeight = g_Memory.ResolveRelativeAddress(g_Memory.PatternScan(hEngine2DLL, Signatures::WindowHeight), 0x2, 0x6);
        if (Engine2::dwWindowHeight == 0)
            return false;
 
        FreeLibrary(hClientDLL);
        FreeLibrary(hEngine2DLL);
    }
}
