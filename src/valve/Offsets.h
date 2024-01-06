#pragma once

namespace Signatures
{
    inline const char* GlobalVars = X("48 89 0D ? ? ? ? 48 89 41"); // rip 3 | 7
    inline const char* ViewMatrix = X("48 8D 0D ? ? ? ? 48 C1 E0 06"); // rip 3 | 7
    inline const char* EntityList = X("48 8B 0D ? ? ? ? 48 89 7C 24 ? 8B FA C1 EB"); // rip 3 | 7
    inline const char* LocalPlayerController = X("48 8B 05 ? ? ? ? 48 85 C0 74 4F"); // rip 3 | 7

    inline const char* WindowWidth = X("8B 05 ? ? ? ? 89 07"); // rip 2 | 6
    inline const char* WindowHeight = X("8B 05 ? ? ? ? 89 03"); // rip 2 | 6
}

namespace Offsets
{
    namespace Client
    {
        inline std::uintptr_t dwGlobalVars = 0x0;
        inline std::uintptr_t dwEntityList = 0x0;
        inline std::uintptr_t dwLocalPlayerController = 0x0;
        inline std::uintptr_t dwViewMatrix = 0x0;

        inline constexpr std::uintptr_t dwBoneMatrix = 0x80;
    }

    namespace Engine2
    {
        inline std::uintptr_t dwWindowHeight = 0x0;
        inline std::uintptr_t dwWindowWidth = 0x0;
    }

    // NOTE: You can change pattern scanning if you want, currently loading the module into our own process and finding the address that way.
    // NOTE: Remember to do result - base address as the module address is not the same as the game's :)
    inline void GetOffsets()
    {
        HMODULE hClientDLL = LoadLibraryExA(Modules::m_pClient.m_strPath.c_str(), 0, DONT_RESOLVE_DLL_REFERENCES);
        HMODULE hEngine2DLL = LoadLibraryExA(Modules::m_pEngine.m_strPath.c_str(), 0, DONT_RESOLVE_DLL_REFERENCES);
        
        std::uintptr_t uClientBaseAddress = reinterpret_cast<std::uintptr_t>(hClientDLL);
        std::uintptr_t uEngine2BaseAddress = reinterpret_cast<std::uintptr_t>(hEngine2DLL);

        // @TODO: Fix me
        // get entity list
        //Client::dwEntityList = Modules::m_pClient.m_uAddress + (g_Memory.ResolveRelativeAddress(g_Memory.PatternScan(hClientDLL, Signatures::EntityList), 0x3, 0x7) - uClientBaseAddress);
        //// get local player controller
        //Client::dwLocalPlayerController = Modules::m_pClient.m_uAddress + (g_Memory.ResolveRelativeAddress(g_Memory.PatternScan(hClientDLL, Signatures::LocalPlayerController), 0x3, 0x7) - uClientBaseAddress);
        //// get view matrix
        //Client::dwViewMatrix = Modules::m_pClient.m_uAddress + (g_Memory.ResolveRelativeAddress(g_Memory.PatternScan(hClientDLL, Signatures::ViewMatrix), 0x3, 0x7) - uClientBaseAddress);
        //// get global variables
        //Client::dwGlobalVars = Modules::m_pClient.m_uAddress + (g_Memory.ResolveRelativeAddress(g_Memory.PatternScan(hClientDLL, Signatures::GlobalVars), 0x3, 0x7) - uClientBaseAddress);

        //// get window width
        //Engine2::dwWindowWidth = Modules::m_pEngine.m_uAddress + (g_Memory.ResolveRelativeAddress(g_Memory.PatternScan(hEngine2DLL, Signatures::WindowWidth), 0x2, 0x6) - uEngine2BaseAddress);
        //// get window height
        //Engine2::dwWindowHeight = Modules::m_pEngine.m_uAddress + (g_Memory.ResolveRelativeAddress(g_Memory.PatternScan(hEngine2DLL, Signatures::WindowHeight), 0x2, 0x6) - uEngine2BaseAddress);

        Client::dwEntityList = Modules::m_pClient.m_uAddress + 0x17C1950;
        // get local player controller
        Client::dwLocalPlayerController = Modules::m_pClient.m_uAddress + 0x1810F48;
        // get view matrix
        Client::dwViewMatrix = Modules::m_pClient.m_uAddress + 0x1820150;
        // get global variables
        Client::dwGlobalVars = Modules::m_pClient.m_uAddress + 0x16BDCB8;

        // get window width
        Engine2::dwWindowWidth = Modules::m_pEngine.m_uAddress + 0x597E08;
        // get window height
        Engine2::dwWindowHeight = Modules::m_pEngine.m_uAddress + 0x597E0C;

        if (hClientDLL)
            FreeLibrary(hClientDLL);

        if (hEngine2DLL)
            FreeLibrary(hEngine2DLL);
    }
}
