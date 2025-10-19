#include "Includes.h"

FILE* m_pConsoleStream = nullptr;
std::ofstream m_ofsFile{};

bool ConsoleAttach(const char* szConsoleTitle)
{
    if (!AllocConsole())
        return false;

    AttachConsole(ATTACH_PARENT_PROCESS);

    FILE* fp;
    freopen_s(&fp, X("CONIN$"), X("r"), stdin);
    freopen_s(&fp, X("CONOUT$"), X("w"), stdout);
    freopen_s(&fp, X("CONOUT$"), X("w"), stderr);

    if (!SetConsoleTitleA(szConsoleTitle))
        return false;

    return true;
}

bool DetachConsole()
{
    FILE* fp;
    freopen_s(&fp, X("NUL"), X("r"), stdin);
    freopen_s(&fp, X("NUL"), X("w"), stdout);
    freopen_s(&fp, X("NUL"), X("w"), stderr);

    // Konsole freigeben
    return FreeConsole() != 0;
}

void SetThreadPriorityWrapper()
{
    // Get the handle to your program's thread
    HANDLE hThread = GetCurrentThread();
    if (hThread)
    {
        int nThreadPriority = GetThreadPriority(hThread);
        if (nThreadPriority == THREAD_PRIORITY_ERROR_RETURN) // error
            throw std::runtime_error(X("failed to get thread priority"));

        if (nThreadPriority == THREAD_PRIORITY_HIGHEST) // already set
        {
            // close the handle
            CloseHandle(hThread);
            return;
        }

        // set the thread priority to high
        SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST);
        // close the handle
        CloseHandle(hThread);
    }
    else // error
        throw std::runtime_error(X("failed to set thread priority"));
}

void EntityThread()
{
    // set thread priority
    SetThreadPriorityWrapper();

    while (!g_Globals.m_bIsUnloading)
    {
        if (!g_Utilities.IsInGame())
        {
            // sleep to conserve system resources
            g_Utilities.Sleep(3000.f);
			continue;
		}

        // lock entity list
        EntityList::m_mtxEntities.lock();

        // update entities
        EntityList::UpdateEntities();

        // unlock entity list
        EntityList::m_mtxEntities.unlock();

        // sleep in-between ticks
        g_Utilities.Sleep(INTERVAL_PER_TICK * 1000.0f);
    }
}

void RenderThread()
{
    // set thread priority
    SetThreadPriorityWrapper();

    while (!g_Globals.m_bIsUnloading)
    {
        // clear data from previous call
        Draw::ClearDrawData();

        if (!g_Memory.IsWindowInForeground(X("Counter-Strike 2")))
        {
            // "safe" way of clearing
            Draw::SwapDrawData();

            g_Utilities.Sleep(1000.0f);
            continue;
        }

        // lock entities
        std::unique_lock lockEntityGuard(EntityList::m_mtxEntities);

        // copy entities into separate container
        std::vector<EntityObject_t> vecEntities;
        vecEntities.assign(EntityList::m_vecEntities.begin(), EntityList::m_vecEntities.end());

        // unlock entities
        lockEntityGuard.unlock();
        lockEntityGuard.release();

        if (Window::m_bInitialized)
        {        
            // run ESP
            for (EntityObject_t& object : vecEntities)
            {
                switch (object.m_eType)
                {

                    case EEntityType::ENTITY_PLAYER:
                    {
                        CCSPlayerController* pController = reinterpret_cast<CCSPlayerController*>(object.m_pEntity);
                        if (!pController || pController->m_bIsLocalPlayerController())
                            continue;

                        C_CSPlayerPawn* pPawn = reinterpret_cast<C_CSPlayerPawn*>(pController->m_hPawn().Get());
                        if (!pPawn || !pPawn->IsAlive())
                            continue;

                        // esp
                        if (CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bEnableVisuals))
                        {
                            // run esp here
                        }

                        break;
                    }

                }
            }
        }

        // swap given data to safe container
        Draw::SwapDrawData();
    }
}

void MapParserThread()
{
    // set thread priority
    SetThreadPriorityWrapper();

    while (!g_Globals.m_bIsUnloading)
    {
        if (!g_Utilities.IsInGame())
        {
            // sleep to conserve system resources
            g_Utilities.Sleep(3000.f);
            continue;
        }

        std::string strMapName = g_Memory.ReadMemoryString(g_Interfaces.m_GlobalVars.m_uMapNameShort);
        if (strMapName.empty())
        {
            g_Utilities.Sleep(3000.f);
            continue;
        }

        g_MapParser.VerifyMapNameHash(strMapName);
        
        // sleep to conserve system resources
        g_Utilities.Sleep(3000.f);
    }
}

void TickThread()
{
    // set thread priority
    SetThreadPriorityWrapper();

    while (!g_Globals.m_bIsUnloading)
    {
        if (!g_Utilities.IsInGame())
        {
            // sleep to conserve system resources
            g_Utilities.Sleep(3000.f);
            continue;
        }

        if (!g_Memory.IsWindowInForeground(X("Counter-Strike 2")) || Gui::m_bOpen)
        {
            g_Utilities.Sleep(1000.0f);
            continue;
        }
        
        if (CONFIG_GET(bool, g_Variables.m_AimBot.m_bEnableAimbot))
        {
            // run aimbot here
        }

        if (CONFIG_GET(bool, g_Variables.m_TriggerBot.m_bEnableTriggerbot))
        {
			// run triggerbot here
        }

        // sleep to conserve system resources
        g_Utilities.Sleep(INTERVAL_PER_TICK * 1000.0f);
    }
}

void UpdateThread()
{
    // set thread priority
    SetThreadPriorityWrapper();

    while (!g_Globals.m_bIsUnloading)
    {
        // update globals
        g_Globals.Update();
        // update interfaces
        g_Interfaces.Update();

        // sleep to conserve system resources
        g_Utilities.Sleep(1);
    }
}

__forceinline void CreateThreads()
{
    std::thread(&EntityThread).detach();
    std::thread(&RenderThread).detach();
    std::thread(&MapParserThread).detach();
    std::thread(&TickThread).detach();
    std::thread(&UpdateThread).detach();
}

bool MainLoop(LPVOID lpParameter)
{
    system(X("cls"));

    #ifndef _DEBUG
    DetachConsole();
    #endif // !_DEBUG

    try
    {
        // initialize memory
        g_Memory.Initialize(X("cs2.exe"));

        // check if last module is loaded
        while (g_Memory.GetModule(NAVSYSTEM_DLL).m_uBaseAddress == 0U)
        {
			std::cout << X("Looking for navsystem.dll") << std::endl;
            g_Utilities.Sleep(500.0f);
        }

        // setup config system
        Config::Setup(X("default.json"));
        // initialize schema system
        SchemaSystem::Setup();

        // create our window
        if (!Window::m_bInitialized)
            Window::Create();

        // set window process priority
        SetPriorityClass(g_Globals.m_Instance, HIGH_PRIORITY_CLASS);
        // set program priority
        SetPriorityClass(g_Globals.m_hDll, HIGH_PRIORITY_CLASS);
		// set program priority
        SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

        // create all our threads
        CreateThreads();

        while (!g_Globals.m_bIsUnloading)
        {
            if (!Window::Render())
               return false;
        }
    }
    catch (const std::exception& ex)
    {
        #ifdef _DEBUG
        _RPT0(_CRT_ERROR, ex.what());
        #else
        // unload
        FreeLibraryAndExitThread(static_cast<HMODULE>(lpParameter), EXIT_FAILURE);
        #endif
    }

    return EXIT_SUCCESS;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInstance, LPSTR pArgs, int iCmdShow)
{
    ConsoleAttach(X("External Base"));
   
    g_Globals.m_hDll = hInstance;

    if (!MainLoop(hInstance))
    {
        // release handle
        g_Memory.~CMemory();

        // destroy context
        if (Window::m_bInitialized)
            Window::Destroy();
    }

    return EXIT_SUCCESS;
}