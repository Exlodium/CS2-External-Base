#include "../Precompiled.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK WindowProcess( HWND window, UINT message, WPARAM wparam, LPARAM lparam )
{
    if (ImGui_ImplWin32_WndProcHandler( window, message, wparam, lparam ))
        return 0L;

    if (message == WM_DESTROY)
    {
        Globals::m_bIsUnloading = true;
        PostQuitMessage( 0 );
        return 0L;
    }

    return DefWindowProc( window, message, wparam, lparam );
}

bool Window::Render( )
{
    // get screen resolution
    m_iWidth = GetSystemMetrics( SM_CXSCREEN );
    m_iHeight = GetSystemMetrics( SM_CYSCREEN );

    if (!m_bInitialized)
        return false;

    static bool bRunning = true;
    if (bRunning)
    {
        MSG message;
        while (PeekMessage( &message, nullptr, 0U, 0U, PM_REMOVE ))
        {
            TranslateMessage( &message );
            DispatchMessage( &message );
        }
        if (message.message == WM_QUIT)
            bRunning = false;

        static bool bToggled = false;
        if (GetAsyncKeyState( Variables::m_iMenuKey ) & 1 && !bToggled)
        {
            Gui::m_bOpen = !Gui::m_bOpen;
            SetForegroundWindow( Globals::m_Instance );
            bToggled = true;
        }
        else if (!GetAsyncKeyState( Variables::m_iMenuKey ) & 1)
        {
            LONG_PTR windowStyle = GetWindowLongPtr( Globals::m_Instance, GWL_EXSTYLE );
            SetWindowLongPtr( Globals::m_Instance, GWL_EXSTYLE, windowStyle | WS_EX_TRANSPARENT );
            bToggled = false;
        }

        if (GetAsyncKeyState( Variables::m_iUnloadKey ) & 1)
        {
            return 0;
        }

        ImGui_ImplDX11_NewFrame( );
        ImGui_ImplWin32_NewFrame( );

        ImGui::NewFrame( );

        if (Gui::m_bOpen)
        {
            LONG_PTR windowStyle = GetWindowLongPtr( Globals::m_Instance, GWL_EXSTYLE );
            windowStyle &= ~WS_EX_TRANSPARENT;
            SetWindowLongPtr( Globals::m_Instance, GWL_EXSTYLE, windowStyle );

            Gui::Render( );
        }

        ImDrawList* pBackgroundDrawList = ImGui::GetBackgroundDrawList( );
        Draw::RenderDrawData( pBackgroundDrawList );

        ImGui::Render( );

        constexpr float flColor[ 4 ] = { 0.f, 0.f, 0.f, 0.f };
        pContext->OMSetRenderTargets( 1U, &pRenderTargetView, NULL );
        pContext->ClearRenderTargetView( pRenderTargetView, flColor );

        ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData( ) );

        pSwapChain->Present( 1U, 0U );
    }

    if (!bRunning)
    {
        Destroy( );
        return false;
    }

    return true;
}

bool Window::Create( )
{
    if (m_bInitialized)
        return true;

    m_iWidth = GetSystemMetrics( SM_CXSCREEN );
    m_iHeight = GetSystemMetrics( SM_CYSCREEN );

    windowClass.cbSize = sizeof( WNDCLASSEX );
    windowClass.style = 0;
    windowClass.lpfnWndProc = WindowProcess;
    windowClass.hInstance = Globals::m_hDll;
    windowClass.lpszClassName = X( L"External Base" );

    RegisterClassExW( &windowClass );

    Globals::m_Instance = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED,
        windowClass.lpszClassName, X( L"" ), WS_POPUP | WS_VISIBLE,
        0, 0, m_iWidth, m_iHeight, nullptr, nullptr, Globals::m_hDll, nullptr );

    if (!SetLayeredWindowAttributes( Globals::m_Instance, RGB( 0, 0, 0 ), BYTE( 255 ), LWA_ALPHA ))
        throw std::runtime_error( X( "failed to set layered window attributes" ) );

    {
        RECT clientArea = { };
        if (!GetClientRect( Globals::m_Instance, &clientArea ))
            throw std::runtime_error( X( "failed to get client rect" ) );

        RECT windowArea = { };
        if (!GetWindowRect( Globals::m_Instance, &windowArea ))
            throw std::runtime_error( X( "failed to get window rect" ) );

        POINT diff = { };
        if (!ClientToScreen( Globals::m_Instance, &diff ))
            throw std::runtime_error( X( "failed to get client to screen" ) );

        const MARGINS margins{
            windowArea.left + ( diff.x - windowArea.left ),
            windowArea.top + ( diff.y - windowArea.top ),
            windowArea.right,
            windowArea.bottom
        };

        if (FAILED( DwmExtendFrameIntoClientArea( Globals::m_Instance, &margins ) ))
            throw std::runtime_error( X( "failed to extend frame into client area" ) );
    }

    // get refreshrate
    HDC hDC = GetDC( Globals::m_Instance );
    m_uRefreshRate = GetDeviceCaps( hDC, VREFRESH );
    ReleaseDC( Globals::m_Instance, hDC );

    DXGI_SWAP_CHAIN_DESC swapChainDesc = { };
    swapChainDesc.BufferDesc.RefreshRate.Numerator = m_uRefreshRate;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1U;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.SampleDesc.Count = 1U;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2U;
    swapChainDesc.OutputWindow = Globals::m_Instance;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    constexpr D3D_FEATURE_LEVEL levels[ 2 ]
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
        &pSwapChain,
        &pDevice,
        &level,
        &pContext ) != S_OK)
        return false;

    ID3D11Texture2D* pBackBuffer = nullptr;
    pSwapChain->GetBuffer( 0U, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
    if (pBackBuffer)
    {
        pDevice->CreateRenderTargetView( pBackBuffer, nullptr, &pRenderTargetView );
        pBackBuffer->Release( );
    }
    else throw std::runtime_error( X( "failed to get back buffer" ) );

    SetWindowLong( Globals::m_Instance, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW );
    ShowWindow( Globals::m_Instance, SW_SHOW );
    UpdateWindow( Globals::m_Instance );

    ImGui::CreateContext( );

    ImGui_ImplWin32_Init( Globals::m_Instance );
    ImGui_ImplDX11_Init( pDevice, pContext );

    Gui::Initialize( );

    m_bInitialized = true;
}

void Window::Destroy( )
{
    ImGui_ImplDX11_Shutdown( );
    ImGui_ImplWin32_Shutdown( );

    ImGui::DestroyContext( );

    if (Window::pSwapChain)
        Window::pSwapChain->Release( );

    if (Window::pContext)
        Window::pContext->Release( );

    if (Window::pDevice)
        Window::pDevice->Release( );

    if (Window::pRenderTargetView)
        Window::pRenderTargetView->Release( );

    if (!DestroyWindow( Globals::m_Instance ))
        throw std::runtime_error( X( "failed to destroy window" ) );

    if (!UnregisterClassW( windowClass.lpszClassName, windowClass.hInstance ))
        throw std::runtime_error( X( "failed to destroy window" ) );
}