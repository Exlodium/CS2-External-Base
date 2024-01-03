#include "Precompiled.h"

static CTimer timer;
bool MainLoop(LPVOID lpParameter)
{
    try
    {
#ifdef DEBUG_CONSOLE
        // console logging
        if (!Logging::Attach(X("Base developer-mode")))
            throw std::runtime_error(X("failed to attach console"));

        Logging::Print(X("console opened"));
#endif

        // initialize memory
        g_Memory.Initialize(X("cs2.exe"));

        // check if last module is loaded
        while (g_Memory.GetModuleAddress(NAVSYSTEM_DLL).m_uAddress == NULL)
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // find client.dll address
        if (!Modules::GetModule(Modules::m_pClient, CLIENT_DLL))
            throw std::runtime_error(X("failed to get client.dll module"));
        
        // find engine2.dll address
        if (!Modules::GetModule(Modules::m_pEngine, ENGINE2_DLL))
            throw std::runtime_error(X("failed to get engine2.dll module"));
        
        // find schemasystem.dll address
        if (!Modules::GetModule(Modules::m_pSchemaSystem, SCHEMASYSTEM_DLL))
            throw std::runtime_error(X("failed to get schemasystem.dll module"));

        // setup config
        if (!Config::Setup(X("Default.json")))
            throw std::runtime_error(X("failed to setup config"));

        // dump schemas
        if (!Schema::Setup(X(L"Schema.txt")))
            throw std::runtime_error(X("failed to dump schemas"));

        // update offsets from sigs
        Offsets::GetOffsets();

        // create our window
        if (!Window::m_bInitialized)
            Window::Create();

        // set process priority
        SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
        // set window process priority
        SetPriorityClass(Globals::m_Instance, HIGH_PRIORITY_CLASS);

        Globals::UpdateGlobals();
        
        Logging::PushConsoleColor(FOREGROUND_INTENSE_GREEN);
        Logging::Print(X("External Base by Exlodium initialized, have fun!"));
        Logging::PopConsoleColor();
               
        // run main loop
        while (!Globals::m_bIsUnloading)
        {  
            // update globals every 3 seconds
            if (timer.Elapsed<std::chrono::seconds>() > 3)
            {
                if (!Globals::UpdateGlobals())
                {
                    Logging::PushConsoleColor(FOREGROUND_INTENSE_RED);
                    Logging::Print(X("failed to update globals"));
                    Logging::PopConsoleColor();
                }
                Interfaces::m_GlobalVariables = g_Memory.Read<IGlobalVars>(g_Memory.Read<std::uintptr_t>(Offsets::Client::dwGlobalVars));
                
                // reset timer
                timer.Reset();
            }
            
            // update current tick
            Globals::m_nCurrentTick = Interfaces::m_GlobalVariables.m_nTickCount;
            // run only if tick changed
            if (Globals::m_nCurrentTick != Globals::m_nPreviousTick)
            {
                // update entity list
                EntityList::UpdateEntities();

                // run tick related features
                for (EntityObject_t& object : EntityList::m_vecEntities)
                {
                    CCSPlayerController* pController = reinterpret_cast<CCSPlayerController*>(object.m_pEntity);
                    if (!pController || !pController->m_bPawnIsAlive() || pController->m_bIsLocalPlayerController())
                        continue;

                    C_CSPlayerPawn* pPawn = pController->m_hPlayerPawn();
                    if (!pPawn)
                        continue;

                    // aimbot
                    if (Variables::Aimbot::m_bEnabled)
                        g_AimBot.Run(pController, pPawn);

                    // triggerbot
                    if (Variables::TriggerBot::m_bEnabled)
                        g_TriggerBot.Run();
                }

                // update previous tick
                Globals::m_nPreviousTick = Globals::m_nCurrentTick;
            }

            // clear data from previous call
            Draw::ClearDrawData();

            if (Window::m_bInitialized)
            {
                // run ESP
                for (EntityObject_t& object : EntityList::m_vecEntities)
                {
                    CCSPlayerController* pController = reinterpret_cast<CCSPlayerController*>(object.m_pEntity);
                    if (!pController || !pController->m_bPawnIsAlive() || pController->m_bIsLocalPlayerController())
                        continue;

                    C_CSPlayerPawn* pPawn = pController->m_hPlayerPawn();
                    if (!pPawn)
                        continue;

                    // esp
                    if (Variables::Visuals::m_bEnabled)
                        g_PlayerESP.Run(pController, pPawn, object.m_nIndex);
                }
            }

            // swap given data to safe container
            Draw::SwapDrawData();

            if (!Window::Render())
                return false;

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

    }
    catch (const std::exception& ex)
    {
        // print error message
        Logging::PushConsoleColor(FOREGROUND_INTENSE_RED);
        Logging::Print(X("[error] {}"), ex.what());
        Logging::PopConsoleColor();

#ifdef _DEBUG
        // show error message window (or replace to your exception handler)
        _RPT0(_CRT_ERROR, ex.what());
#else
        // unload
        FreeLibraryAndExitThread(static_cast<HMODULE>(lpParameter), EXIT_FAILURE);
#endif
    }
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInstance, LPSTR pArgs, int iCmdShow)
{
    // save our module
    Globals::m_hDll = hInstance;

    if (!MainLoop(hInstance))
    {
        // release handle
        g_Memory.~CMemory();

        // destroy context
        if (Window::m_bInitialized)
            Window::Destroy();
    }

    return 0;
}
