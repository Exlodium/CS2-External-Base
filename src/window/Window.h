#pragma once

namespace Window
{
    inline bool m_bInitialized = false;
    inline int m_iWidth = 1920;
    inline int m_iHeight = 1080;
    inline unsigned int m_uRefreshRate = 60;

    inline ID3D11Device* m_pDevice = nullptr;
    inline ID3D11DeviceContext* m_pContext = nullptr;
    inline IDXGISwapChain* m_pSwapChain = nullptr;
    inline ID3D11RenderTargetView* m_pRenderTargetView = nullptr;

    inline WNDCLASSEXW m_windowClass = { };

    bool Create();
    bool Render();
    void Destroy();
}