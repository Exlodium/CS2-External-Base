#pragma once

namespace Offsets 
{
    namespace Client 
    {
        inline std::ptrdiff_t dwGlobalVars = 0x0;
        inline std::ptrdiff_t dwEntityList = 0x0;
        inline std::ptrdiff_t dwLocalPlayerController = 0x0;
        inline std::ptrdiff_t dwViewMatrix = 0x0;
		inline std::ptrdiff_t dwViewAngles = 0x0;

        inline constexpr std::ptrdiff_t m_hActiveWeapon = 0x60;
        inline constexpr std::ptrdiff_t dwBoneMatrix = 0x80;
    }

    inline bool Setup( ) 
    {
        auto FindOffsetFromSignature = [ & ]( std::ptrdiff_t& pReturnOffset, std::uintptr_t uAddress, std::uintptr_t uModuleBaseAddress, std::uintptr_t uWantedModuleAddress ) -> bool
        {
            pReturnOffset = uWantedModuleAddress + ( uAddress - uModuleBaseAddress );
            if (!pReturnOffset)
            {
                Logging::PushConsoleColor(FOREGROUND_INTENSE_RED);
                Logging::Print("Failed to find {} in {}", uAddress, uWantedModuleAddress);
                Logging::PopConsoleColor();
                return false;
            }
            
            return true;
        };

        // Get the client module
        HMODULE hClientDLL = LoadLibraryExA( Modules::m_pClient.m_strPath.c_str( ), 0, DONT_RESOLVE_DLL_REFERENCES );
        // OUR client module address
        std::uintptr_t uClientAddress = reinterpret_cast<std::uintptr_t>(hClientDLL);

        // Decided to push in RIP(PatternScan) into the function itself to avoid having to add more args for different instructions and RIP offsets..
        FindOffsetFromSignature(Client::dwEntityList, g_Memory.ResolveRelativeAddress(g_Memory.PatternScan(hClientDLL, X("48 8B 0D ? ? ? ? 8B F3")), 0x3, 0x7), uClientAddress, Modules::m_pClient.m_uAddress);
        FindOffsetFromSignature(Client::dwLocalPlayerController, g_Memory.ResolveRelativeAddress(g_Memory.PatternScan(hClientDLL, X("48 8B 05 ? ? ? ? 48 85 C0 74 53")), 0x3, 0x7), uClientAddress, Modules::m_pClient.m_uAddress);
        FindOffsetFromSignature(Client::dwGlobalVars, g_Memory.ResolveRelativeAddress(g_Memory.PatternScan(hClientDLL, X("48 89 15 ? ? ? ? 48 89 42")), 0x3, 0x7), uClientAddress, Modules::m_pClient.m_uAddress);
        FindOffsetFromSignature(Client::dwViewAngles, g_Memory.ResolveRelativeAddress(g_Memory.PatternScan(hClientDLL, X("48 8B 0D ? ? ? ? 4C 8B C6 8B 10 E8")), 0x3, 0x7, 0x3D0), uClientAddress, Modules::m_pClient.m_uAddress);

		Client::dwViewMatrix = uClientAddress + 0x1A1DDE0;

        // Free the library
        if ( hClientDLL != 0 )
            FreeLibrary( hClientDLL );

        return true;
    }
}
