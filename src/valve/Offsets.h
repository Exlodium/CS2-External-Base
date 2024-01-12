#pragma once

namespace Offsets {
    namespace Client {
        inline std::ptrdiff_t dwGlobalVars = 0x0;
        inline std::ptrdiff_t dwEntityList = 0x0;
        inline std::ptrdiff_t dwLocalPlayerController = 0x0;
        inline std::ptrdiff_t dwViewMatrix = 0x0;
        inline std::ptrdiff_t dwViewAngles = 0x0;

        inline constexpr std::ptrdiff_t m_hActiveWeapon = 0x60;
        inline constexpr std::ptrdiff_t dwBoneMatrix = 0x80;
    }

    inline bool Setup( ) {
        auto signature = [ & ]( std::string_view sig_name, void* base_module, const char* signature, uint64_t offset = 0, uint64_t base_addr = Modules::m_pClient.m_uAddress ) {
            auto ret = g_Memory.ResolveRelativeAddress( g_Memory.PatternScan( base_module, signature ), 0x3, 0x7 );
            if ( offset )
                ret = g_Memory.Read<uint64_t>( ret ) + offset;

            std::cout << sig_name << ": 0x" << std::hex << ret - base_addr << "\n";
            return ret;
        };

        // Get the client module
        HMODULE hClientDLL = LoadLibraryExA( Modules::m_pClient.m_strPath.c_str( ), 0, DONT_RESOLVE_DLL_REFERENCES );

        Client::dwEntityList = g_Memory.ResolveRelativeAddress( g_Memory.PatternScan( hClientDLL, X( "48 8B 0D ? ? ? ? 48 89 7C 24 ? 8B FA C1 EB" ) ), 0x3, 0x7 ) - Modules::m_pClient.m_uAddress;
        // check if entitylist is invalid
        if ( !Client::dwEntityList ) {
            Logging::PushConsoleColor( FOREGROUND_INTENSE_RED );
            Logging::Print( "Failed to find dwEntityList\n" );
            Logging::PopConsoleColor( );
            return false;
        }
        else {
            Logging::PushConsoleColor( FOREGROUND_INTENSE_GREEN );
            Logging::Print( "dwEntityList: 0x{:X}", Client::dwEntityList );
            Logging::PopConsoleColor( );
        }

        Client::dwLocalPlayerController = g_Memory.ResolveRelativeAddress( g_Memory.PatternScan( hClientDLL, X( "48 8B 05 ? ? ? ? 48 85 C0 74 4F" ) ), 0x3, 0x7 ) - Modules::m_pClient.m_uAddress;
        // check if localplayercontroller is invalid
        if ( !Client::dwLocalPlayerController ) {
            Logging::PushConsoleColor( FOREGROUND_INTENSE_RED );
            Logging::Print( "Failed to find dwLocalPlayerController\n" );
            Logging::PopConsoleColor( );
            return false;
        }
        else {
            Logging::PushConsoleColor( FOREGROUND_INTENSE_GREEN );
            Logging::Print( "dwLocalPlayerController: 0x{:X}", Client::dwLocalPlayerController );
            Logging::PopConsoleColor( );
        }

        Client::dwGlobalVars = g_Memory.ResolveRelativeAddress( g_Memory.PatternScan( hClientDLL, X( "48 89 0D ? ? ? ? 48 89 41" ) ), 0x3, 0x7 ) - Modules::m_pClient.m_uAddress;
        // check if globalvars is invalid
        if ( !Client::dwGlobalVars ) {
            Logging::PushConsoleColor( FOREGROUND_INTENSE_RED );
            Logging::Print( "Failed to find dwGlobalVars\n" );
            Logging::PopConsoleColor( );
            return false;
        }
        else {
            Logging::PushConsoleColor( FOREGROUND_INTENSE_GREEN );
            Logging::Print( "dwGlobalVars: 0x{:X}", Client::dwGlobalVars );
            Logging::PopConsoleColor( );
        }

        Client::dwViewMatrix = g_Memory.ResolveRelativeAddress( g_Memory.PatternScan( hClientDLL, X( "48 8D 0D ? ? ? ? 48 C1 E0 06" ) ), 0x3, 0x7 ) - Modules::m_pClient.m_uAddress;
        // check if viewmatrix is invalid
        if ( !Client::dwViewMatrix ) {
            Logging::PushConsoleColor( FOREGROUND_INTENSE_RED );
            Logging::Print( "Failed to find dwViewMatrix\n" );
            Logging::PopConsoleColor( );
            return false;
        }
        else {
            Logging::PushConsoleColor( FOREGROUND_INTENSE_GREEN );
            Logging::Print( "dwViewMatrix: 0x{:X}", Client::dwViewMatrix );
            Logging::PopConsoleColor( );
        }

        std::ptrdiff_t ret = g_Memory.ResolveRelativeAddress( g_Memory.PatternScan( hClientDLL, X( "48 8B 0D ? ? ? ? E9 ? ? ? ? CC CC CC CC 40 55" ) ), 0x3, 0x7 );
        ret = g_Memory.Read<uint64_t>( ret ) + 24896; // 24896 is our add value
        Client::dwViewAngles = ret - Modules::m_pClient.m_uAddress;
        // check if viewangles is invalid
        if ( !Client::dwViewAngles ) {
            Logging::PushConsoleColor( FOREGROUND_INTENSE_RED );
            Logging::Print( "Failed to find dwViewAngles\n" );
            Logging::PopConsoleColor( );
            return false;
        }
        else {
            Logging::PushConsoleColor( FOREGROUND_INTENSE_GREEN );
            Logging::Print( "dwViewAngles: 0x{:X}", Client::dwViewAngles );
            Logging::PopConsoleColor( );
        }

        // Free the library
        if ( hClientDLL != 0 )
            FreeLibrary( hClientDLL );

        return true;
    }
}