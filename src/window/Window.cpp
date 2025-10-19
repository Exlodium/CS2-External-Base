#include "../Includes.h"

static BOOL CheckForUIAccess(DWORD* pdwErr, DWORD* pfUIAccess) {
    BOOL result = FALSE;
    HANDLE hToken;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        DWORD dwRetLen;

        if (GetTokenInformation(hToken, TokenUIAccess, pfUIAccess, sizeof(*pfUIAccess), &dwRetLen)) {
            result = TRUE;
        }
        else {
            *pdwErr = GetLastError();
        }
        CloseHandle(hToken);
    }
    else {
        *pdwErr = GetLastError();
    }

    return result;
}

static DWORD DuplicateWinloginToken(DWORD dwSessionId, DWORD dwDesiredAccess, PHANDLE phToken) {
    DWORD dwErr;
    PRIVILEGE_SET ps;

    ps.PrivilegeCount = 1;
    ps.Control = PRIVILEGE_SET_ALL_NECESSARY;

    if (LookupPrivilegeValue(NULL, SE_TCB_NAME, &ps.Privilege[0].Luid)) 
    {
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (INVALID_HANDLE_VALUE != hSnapshot) 
        {
            BOOL bCont, bFound = FALSE;
            PROCESSENTRY32 pe;

            pe.dwSize = sizeof(pe);
            dwErr = ERROR_NOT_FOUND;

            for (bCont = Process32First(hSnapshot, &pe); bCont; bCont = Process32Next(hSnapshot, &pe)) 
            {
                HANDLE hProcess;
                if (strcmp(pe.szExeFile, X("winlogon.exe")) != 0)
                    continue;

                hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pe.th32ProcessID);
                if (hProcess) 
                {
                    HANDLE hToken;
                    DWORD dwRetLen, sid;

                    if (OpenProcessToken(hProcess, TOKEN_QUERY | TOKEN_DUPLICATE, &hToken)) 
                    {
                        BOOL fTcb;
                        if (PrivilegeCheck(hToken, &ps, &fTcb) && fTcb) 
                        {
                            if (GetTokenInformation(hToken, TokenSessionId, &sid, sizeof(sid), &dwRetLen) && sid == dwSessionId) 
                            {
                                bFound = TRUE;
                                if (DuplicateTokenEx(hToken, dwDesiredAccess, NULL, SecurityImpersonation, TokenImpersonation, phToken))
                                {
                                    dwErr = ERROR_SUCCESS;
                                }
                                else {
                                    dwErr = GetLastError();
                                }
                            }
                        }
                        CloseHandle(hToken);
                    }
                    CloseHandle(hProcess);
                }

                if (bFound) break;
            }

            CloseHandle(hSnapshot);
        }
        else {
            dwErr = GetLastError();
        }
    }
    else {
        dwErr = GetLastError();
    }


    return dwErr;
}

static DWORD CreateUIAccessToken(PHANDLE phToken) 
{
    DWORD dwErr;
    HANDLE hTokenSelf;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_DUPLICATE, &hTokenSelf)) 
    {
        DWORD dwSessionId = 0, dwRetLen;

        if (GetTokenInformation(hTokenSelf, TokenSessionId, &dwSessionId, sizeof(dwSessionId), &dwRetLen)) 
        {
            HANDLE hTokenSystem;

            dwErr = DuplicateWinloginToken(dwSessionId, TOKEN_IMPERSONATE, &hTokenSystem);
            if (ERROR_SUCCESS == dwErr) 
            {
                if (SetThreadToken(NULL, hTokenSystem)) {
                    if (DuplicateTokenEx(hTokenSelf, TOKEN_QUERY | TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_ADJUST_DEFAULT, NULL, SecurityAnonymous, TokenPrimary, phToken)) {
                        BOOL bUIAccess = TRUE;

                        if (!SetTokenInformation(*phToken, TokenUIAccess, &bUIAccess, sizeof(bUIAccess))) 
                        {
                            dwErr = GetLastError();
                            CloseHandle(*phToken);
                        }
                    }
                    else 
                    {
                        dwErr = GetLastError();
                    }
                    RevertToSelf();
                }
                else 
                {
                    dwErr = GetLastError();
                }
                CloseHandle(hTokenSystem);
            }
        }
        else 
        {
            dwErr = GetLastError();
        }

        CloseHandle(hTokenSelf);
    }
    else {
        dwErr = GetLastError();
    }

    return dwErr;
}

DWORD PrepareForUIAccess() {
    DWORD dwErr;
    HANDLE hTokenUIAccess;
    DWORD fUIAccess = 0;

    if (CheckForUIAccess(&dwErr, &fUIAccess)) {
        if (fUIAccess) {
            dwErr = ERROR_SUCCESS;
        }
        else {
            dwErr = CreateUIAccessToken(&hTokenUIAccess);
            if (ERROR_SUCCESS == dwErr) {
                STARTUPINFO si{};
                PROCESS_INFORMATION pi{};
                GetStartupInfo(&si);

                std::string newCmd = std::string(GetCommandLine()) + " --uiaccess";

                if (CreateProcessAsUser(hTokenUIAccess, NULL, (LPSTR)newCmd.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                    CloseHandle(pi.hProcess);
                    CloseHandle(pi.hThread);
                    ExitProcess(0);
                }
                else {
                    dwErr = GetLastError();
                }

                CloseHandle(hTokenUIAccess);
            }
        }
    }

    return dwErr;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProcess(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    if (ImGui_ImplWin32_WndProcHandler(window, message, wparam, lparam))
        return 0L;

    if (message == WM_DESTROY)
    {
        g_Globals.m_bIsUnloading = true;
        PostQuitMessage(0);
        return 0L;
    }

    return DefWindowProc(window, message, wparam, lparam);
}

bool Window::Render()
{
    // get screen resolution
    m_iWidth = GetSystemMetrics(SM_CXSCREEN);
    m_iHeight = GetSystemMetrics(SM_CYSCREEN);

    if (!m_bInitialized)
        return false;

    static bool bRunning = true;
    if (bRunning)
    {
        MSG message;
        while (PeekMessageA(&message, nullptr, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessageA(&message);
        }

        if (message.message == WM_QUIT)
            bRunning = false;

        if (GetAsyncKeyState(CONFIG_GET(int, g_Variables.m_Gui.m_iUnloadKey)) & 1)
            return false;

        if (GetAsyncKeyState(CONFIG_GET(int, g_Variables.m_Gui.m_iMenuKey)) & 1)
            Gui::m_bOpen = !Gui::m_bOpen;

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();

        ImGui::NewFrame();

        static bool bLastWindowAffinityState = CONFIG_GET(bool, g_Variables.m_Gui.m_bExcludeFromDesktopCapture);
        if (CONFIG_GET(bool, g_Variables.m_Gui.m_bExcludeFromDesktopCapture) != bLastWindowAffinityState)
        {
            SetWindowDisplayAffinity(g_Globals.m_Instance, CONFIG_GET(bool, g_Variables.m_Gui.m_bExcludeFromDesktopCapture) ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);
            bLastWindowAffinityState = CONFIG_GET(bool, g_Variables.m_Gui.m_bExcludeFromDesktopCapture);
        }

        if (Gui::m_bOpen)
        {
            LONG_PTR windowStyle = GetWindowLongPtrA(g_Globals.m_Instance, GWL_EXSTYLE);
            windowStyle &= ~WS_EX_TRANSPARENT;
            SetWindowLongPtrA(g_Globals.m_Instance, GWL_EXSTYLE, windowStyle);

            Gui::Render();
        }
        else
        {
            LONG_PTR windowStyle = GetWindowLongPtrA(g_Globals.m_Instance, GWL_EXSTYLE);
            windowStyle |= WS_EX_TRANSPARENT;
            SetWindowLongPtrA(g_Globals.m_Instance, GWL_EXSTYLE, windowStyle);
        }

        ImDrawList* pBackgroundDrawList = ImGui::GetBackgroundDrawList();
        Draw::RenderDrawData(pBackgroundDrawList);

        ImGui::Render();

        constexpr float flColor[4] = { 0.f, 0.f, 0.f, 0.f };
        m_pContext->OMSetRenderTargets(1U, &m_pRenderTargetView, NULL);
        m_pContext->ClearRenderTargetView(m_pRenderTargetView, flColor);
        
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // overlay with vsync example
        // m_pSwapChain->Present(1U, 0U);
        m_pSwapChain->Present(0U, DXGI_PRESENT_DO_NOT_WAIT);   
    }

    if (!bRunning)
    {
        Destroy();
        return false;
    }

    return true;
}


enum ZBID
{
    ZBID_DEFAULT = 0,
    ZBID_DESKTOP = 1,
    ZBID_UIACCESS = 2,
    ZBID_IMMERSIVE_IHM = 3,
    ZBID_IMMERSIVE_NOTIFICATION = 4,
    ZBID_IMMERSIVE_APPCHROME = 5,
    ZBID_IMMERSIVE_MOGO = 6,
    ZBID_IMMERSIVE_EDGY = 7,
    ZBID_IMMERSIVE_INACTIVEMOBODY = 8,
    ZBID_IMMERSIVE_INACTIVEDOCK = 9,
    ZBID_IMMERSIVE_ACTIVEMOBODY = 10,
    ZBID_IMMERSIVE_ACTIVEDOCK = 11,
    ZBID_IMMERSIVE_BACKGROUND = 12,
    ZBID_IMMERSIVE_SEARCH = 13,
    ZBID_GENUINE_WINDOWS = 14,
    ZBID_IMMERSIVE_RESTRICTED = 15,
    ZBID_SYSTEM_TOOLS = 16,

    //Windows 10+
    ZBID_LOCK = 17,
    ZBID_ABOVELOCK_UX = 18
};

typedef HWND(WINAPI* CreateWindowInBand)(_In_ DWORD dwExStyle, _In_opt_ ATOM atom, _In_opt_ LPCWSTR lpWindowName, _In_ DWORD dwStyle, _In_ int X, _In_ int Y, _In_ int nWidth, _In_ int nHeight, _In_opt_ HWND hWndParent, _In_opt_ HMENU hMenu, _In_opt_ HINSTANCE hInstance, _In_opt_ LPVOID lpParam, DWORD band);
bool Window::Create()
{
    if (m_bInitialized)
        return true;

    HMODULE hUser32 = GetModuleHandleA(X("user32.dll"));
    if (hUser32 == NULL)
        return false;

    CreateWindowInBand pCreateWindowInBand = reinterpret_cast<CreateWindowInBand>(GetProcAddress(hUser32, X("CreateWindowInBand")));

    m_iWidth = GetSystemMetrics(SM_CXSCREEN);
    m_iHeight = GetSystemMetrics(SM_CYSCREEN);

    m_windowClass.cbSize = sizeof(WNDCLASSEX);
    m_windowClass.style = 0;
    m_windowClass.lpfnWndProc = WindowProcess;
    m_windowClass.hInstance = g_Globals.m_hDll;
    m_windowClass.lpszClassName = X(L"External Base");

    ATOM atom = RegisterClassExW(&m_windowClass);

    auto dwErr = PrepareForUIAccess();
    if (ERROR_SUCCESS != dwErr && pCreateWindowInBand)
        g_Globals.m_Instance = pCreateWindowInBand(
            WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED, 
            atom, X(L""), WS_POPUP | WS_VISIBLE, 0, 0, 
            m_iWidth, m_iHeight, nullptr, nullptr, 
            g_Globals.m_hDll, nullptr, ZBID_UIACCESS);
    else
        g_Globals.m_Instance = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED,
            m_windowClass.lpszClassName, X(L""), WS_POPUP | WS_VISIBLE,
            0, 0, m_iWidth, m_iHeight, nullptr, nullptr, g_Globals.m_hDll, nullptr);
    
    SetLayeredWindowAttributes(g_Globals.m_Instance, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

    {
        RECT clientArea = { };
        if (!GetClientRect(g_Globals.m_Instance, &clientArea))
            throw std::runtime_error(X("failed to get client rect"));

        RECT windowArea = { };
        if (!GetWindowRect(g_Globals.m_Instance, &windowArea))
            throw std::runtime_error(X("failed to get window rect"));

        POINT diff = { };
        if (!ClientToScreen(g_Globals.m_Instance, &diff))
            throw std::runtime_error(X("failed to get client to screen"));

        const MARGINS margins
        {
            windowArea.left + (diff.x - windowArea.left),
            windowArea.top + (diff.y - windowArea.top),
            windowArea.right,
            windowArea.bottom
        };

        if (FAILED(DwmExtendFrameIntoClientArea(g_Globals.m_Instance, &margins)))
            throw std::runtime_error(X("failed to extend frame into client area"));
    }

    // get refreshrate
    HDC hDC = GetDC(g_Globals.m_Instance);
    m_uRefreshRate = GetDeviceCaps(hDC, VREFRESH);
    ReleaseDC(g_Globals.m_Instance, hDC);

    DXGI_SWAP_CHAIN_DESC swapChainDesc = { };
    swapChainDesc.BufferDesc.RefreshRate.Numerator = m_uRefreshRate;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1U;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.SampleDesc.Count = 1U;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2U;
    swapChainDesc.BufferCount = 1U;
    swapChainDesc.OutputWindow = g_Globals.m_Instance;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    constexpr D3D_FEATURE_LEVEL levels[2]
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_0
    };

    D3D_FEATURE_LEVEL level = { };

    if (D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0U,
        levels,
        2U,
        D3D11_SDK_VERSION,
        &swapChainDesc,
        &m_pSwapChain,
        &m_pDevice,
        &level,
        &m_pContext) != S_OK)
        return false;

    ID3D11Texture2D* pBackBuffer = nullptr;
    m_pSwapChain->GetBuffer(0U, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    if (pBackBuffer)
    {
        m_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView);
        pBackBuffer->Release();
    }
    else throw std::runtime_error(X("failed to get back buffer"));

    SetWindowLongA(g_Globals.m_Instance, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
    ShowWindow(g_Globals.m_Instance, SW_SHOW);
    UpdateWindow(g_Globals.m_Instance);

    ImGui::CreateContext();

    ImGui_ImplWin32_Init(g_Globals.m_Instance);
    ImGui_ImplDX11_Init(m_pDevice, m_pContext);

    Gui::Initialize();

    m_bInitialized = true;
}

void Window::Destroy()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();

    if (Window::m_pSwapChain)
        Window::m_pSwapChain->Release();

    if (Window::m_pContext)
        Window::m_pContext->Release();

    if (Window::m_pDevice)
        Window::m_pDevice->Release();

    if (Window::m_pRenderTargetView)
        Window::m_pRenderTargetView->Release();

    if (!DestroyWindow(g_Globals.m_Instance))
        throw std::runtime_error(X("failed to destroy window"));

    if (!UnregisterClassW(m_windowClass.lpszClassName, m_windowClass.hInstance))
        throw std::runtime_error(X("failed to destroy window"));
}