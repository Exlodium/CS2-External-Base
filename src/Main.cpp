#include "Includes.h"

FILE* m_pConsoleStream = nullptr;
std::ofstream m_ofsFile{};

bool ConsoleAttach(const char* szConsoleTitle)
{
    if (!AllocConsole())
        return false;

    AttachConsole(ATTACH_PARENT_PROCESS);

    if (freopen_s(&m_pConsoleStream, X("CONOUT$"), X("w"), stdout) != 0)
        return false;

    if (!SetConsoleTitleA(szConsoleTitle))
        return false;

    return true;
}

bool MainLoop(LPVOID lpParameter)
{
    try
    {
        ConsoleAttach(X("CS2 - Console"));

        // initialize memory
        g_Memory.Initialize(X("cs2.exe"));

        // check if last module is loaded
        while (g_Memory.GetModule(NAVSYSTEM_DLL).m_uBaseAddress == 0U)
        {
			std::cout << X("Looking for navsystem.dll") << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        // setup config system
        Config::Setup(X("default.json"));
        // initialize schema system
        SchemaSystem::Setup();

		g_MapParser.Setup(X("de_mirage"));

        while (1)
        {
            g_Globals.UpdateGlobals();

            std::uintptr_t uLocalPlayerAddress = g_Memory.GetModule(CLIENT_DLL).m_uBaseAddress + 0x1A6E9C0;

            CCSPlayerController* pLocalPlayerController = g_Memory.ReadMemory<CCSPlayerController*>(uLocalPlayerAddress);
            if (!pLocalPlayerController)
				continue;
            
			std::cout << SchemaSystem::m_mapSchemaOffsets[FNV1A::Hash(X("CCSPlayerController->m_hPlayerPawn"))] << std::endl;

            C_CSPlayerPawn* pLocalPlayerPawn = pLocalPlayerController->m_hPlayerPawn().Get();
			if (!pLocalPlayerPawn)
				continue;

            for (int i = 1; i < 65; i++)
            {
				C_BaseEntity* pEntity = C_BaseEntity::GetBaseEntity(i);
				if (!pEntity)
					continue;

				CGameSceneNode* pGameSceneNode = pEntity->m_pGameSceneNode();
				if (!pGameSceneNode)
					continue;

				// it would be better to replacew m_vecAbsOrigin with the aimpoint
                if (!g_MapParser.IsVisible(pLocalPlayerPawn->GetEyePosition(), pGameSceneNode->m_vecAbsOrigin()))
                    continue;

                // do aimbot code
            }

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
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
    if (!MainLoop(hInstance))
    {
        // release handle
        g_Memory.~CMemory();

        // destroy context
        //if (Window::m_bInitialized)
        //    Window::Destroy();
    }

    return EXIT_SUCCESS;
}
