#pragma once
// @Credits: Cazz ( https://www.youtube.com/watch?v=aY8MNCNN-cY )
namespace Window
{
    inline bool m_bInitialized = false;
    inline int m_iWidth = 1920;
    inline int m_iHeight = 1080;
    inline unsigned int m_uRefreshRate = 60;

    inline ID3D11Device* pDevice = nullptr;
    inline ID3D11DeviceContext* pContext = nullptr;
    inline IDXGISwapChain* pSwapChain = nullptr;
    inline ID3D11RenderTargetView* pRenderTargetView = nullptr;

    inline WNDCLASSEXW windowClass = { };

    bool Create( );
    bool Render( );
    void Destroy( );
}