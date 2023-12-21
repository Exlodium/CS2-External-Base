#include "Precompiled.h"

bool MainLoop( LPVOID lpParameter )
{
    try
    {
        #ifdef DEBUG_CONSOLE
        // console logging
        if (!Logging::Attach( X( "Base developer-mode" ) ))
            throw std::runtime_error( X( "failed to attach console" ) );

        Logging::Print( X( "console opened" ) );
        #endif

        // check for cs2 window
        while (!FindWindowA( NULL, X( "Counter-Strike 2" ) ))
            std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );

        if (!Window::m_bInitialized)
            Window::Create( );

        // find client.dll address
        do
        {
            Modules::m_pClient = Globals::m_Memory.GetModuleAddress( X( "client.dll" ) );
            if (Modules::m_pClient == NULL)
                std::this_thread::sleep_for( std::chrono::seconds( 100 ) );
        } while (Modules::m_pClient == NULL);

        SetPriorityClass( GetCurrentProcess( ), HIGH_PRIORITY_CLASS );

        while (!Globals::m_bIsUnloading)
        {
            // update globals
            Globals::m_pLocalPlayerController = Globals::m_Memory.Read<CCSPlayerController*>( Modules::m_pClient + Offsets::dwLocalPlayerController );
            Globals::m_pLocalPlayerPawn = Globals::m_pLocalPlayerController->m_hPlayerPawn( );
            Globals::m_uEntityList = Globals::m_Memory.Read<std::uintptr_t>( Modules::m_pClient + Offsets::dwEntityList );
            
            // update global variables
            Interfaces::m_GlobalVariables = Globals::m_Memory.Read<IGlobalVars>( Globals::m_Memory.Read<std::uintptr_t>( Modules::m_pClient + Offsets::dwGlobalVars ) );

            // clear data from previous call
            Draw::ClearDrawData( );

            if (Window::m_bInitialized)
            {
                for (int nIndex = 1; nIndex < Interfaces::m_GlobalVariables.m_nMaxClients; nIndex++)
                {
                    CCSPlayerController* pEntity = reinterpret_cast< CCSPlayerController* >( CBaseEntity::GetBaseEntity( nIndex ) );
                    if (!pEntity || !pEntity->m_bPawnIsAlive( ) || pEntity->m_bIsLocalPlayerController( ))
                        continue;

                    CCSPlayerPawn* pPawn = pEntity->m_hPlayerPawn( );
                    if (!pPawn)
                        continue;

                    // aimbot
                    if (Variables::Aimbot::m_bEnabled)
                        g_AimBot.Run( pEntity, pPawn );

                    // triggerbot
                    if (Variables::TriggerBot::m_bEnabled)
                        g_TriggerBot.Run( );

                    // esp
                    if (Variables::Visuals::m_bEnabled)
                        g_PlayerESP.Run( pEntity, pPawn, nIndex );
                }
            }

            // swap given data to safe container
            Draw::SwapDrawData( );

            if (!Window::Render( ))
                return false;

            std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
        }

    }
    catch (const std::exception& ex)
    {
        // print error message
        Logging::PushConsoleColor( FOREGROUND_INTENSE_RED );
        Logging::Print( X( "[error] {}" ), ex.what( ) );
        Logging::PopConsoleColor( );

        #ifdef _DEBUG
        // show error message window (or replace to your exception handler)
        _RPT0( _CRT_ERROR, ex.what( ) );
        #else
        // unload
        FreeLibraryAndExitThread( static_cast< HMODULE >( lpParameter ), EXIT_FAILURE );
        #endif
    }
}

#ifdef DLL
DWORD WINAPI OnDllAttach( LPVOID lpParameter )
{
    if ( !MainLoop( lpParameter ) )
        return 0UL;
    
    return 1UL;
}

DWORD WINAPI OnDllDetach( LPVOID lpParameter )
{
    while (!GetAsyncKeyState( Variables::m_iUnloadKey ))
        std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );

    Globals::m_bIsUnloading = true;

    // release handle
    Globals::m_Memory.~Memory( );

    // destroy context
    if (Window::m_bInitialized)
        Window::Destroy( );

    #ifdef DEBUG_CONSOLE
    // detach console
    Logging::Detach( );
    #endif

    // free our library memory from process and exit from our thread
    FreeLibraryAndExitThread( ( HMODULE )lpParameter, EXIT_SUCCESS );
}

BOOL WINAPI DllMain( HMODULE hModule, DWORD dwReason, LPVOID lpReserved )
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        // disable DLL_THREAD_ATTACH and DLL_THREAD_DETACH reasons to call
        DisableThreadLibraryCalls( hModule );

        // save our module
        Globals::m_hDll = hModule;

        // create main thread
        if (auto hThread = CreateThread( nullptr, 0U, OnDllAttach, hModule, 0UL, nullptr ); hThread != nullptr)
            CloseHandle( hThread );

        // create detach thread
        if (auto hThread = CreateThread( nullptr, 0U, OnDllDetach, hModule, 0UL, nullptr ); hThread != nullptr)
            CloseHandle( hThread );

        return TRUE;
    }

    return FALSE;
}
#else
int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPreviousInstance, LPSTR pArgs, int iCmdShow )
{
    // save our module
    Globals::m_hDll = hInstance;

    if (!MainLoop( hInstance ))
    {
        // release handle
        Globals::m_Memory.~Memory( );

        // destroy context
        if (Window::m_bInitialized)
            Window::Destroy( );
    }

    return 0;
}
#endif
