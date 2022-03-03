/*
MIT License

Copyright (c) 2021-2022 L. E. Spalt

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#include <windows.h>
#include <windowsx.h>
#include "Overlay.h"
#include "Config.h"

using namespace Microsoft::WRL;

static const int ResizeBorderWidth = 25;

static LRESULT CALLBACK windowProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    Overlay* o = (Overlay*)GetWindowLongPtr( hwnd, GWLP_USERDATA );

    if( !o || !o->isUiEditEnabled() )
        return DefWindowProc( hwnd, msg, wparam, lparam );

    switch( msg )
    {
        // handle moving/resizing
        case WM_NCHITTEST:
        {
            LRESULT hit = DefWindowProc( hwnd, msg, wparam, lparam );
            if( hit == HTCLIENT )
            {
                // check if we hit the corner area of the window to allow resizing despite having no border
                RECT r;
                GetWindowRect( hwnd, &r );
                const int cur_x = GET_X_LPARAM( lparam ) - r.left;
                const int cur_y = GET_Y_LPARAM( lparam ) - r.top;
                const int w = r.right - r.left;
                const int h = r.bottom - r.top;
                const int border = ResizeBorderWidth;
                /*  Disabled these corners because using them doesn't let us update the window contents 
                    fast enough for some reason, leading to a weird window-wobble appearance when resizing.
                if( cur_x < border && cur_y < border )
                    return HTTOPLEFT;
                if( cur_x > w-border && cur_y < border )
                    return HTTOPRIGHT;
                if( cur_x < border && cur_y > h-border )
                    return HTBOTTOMLEFT;
                */
                if( cur_x > w-border && cur_y > h-border )
                    return HTBOTTOMRIGHT;

                // say we hit the caption to allow dragging the window from the client area
                hit = HTCAPTION;
            }
            return hit;
        }
        case WM_MOVING:
        case WM_SIZE:
        {
            if( o )
            {
                RECT r;
                GetWindowRect( hwnd, &r );
                const int x = r.left;
                const int y = r.top;
                const int w = r.right - r.left;
                const int h = r.bottom - r.top;
                o->setWindowPosAndSize( x, y, w, h, false );
                o->saveWindowPosAndSize();
                o->update(); // draw window content while moving/resizing
            }
            break;
        }
    }
    return DefWindowProc( hwnd, msg, wparam, lparam );
}


//
// Overlay
//

Overlay::Overlay( const std::string name )
    : m_name( name )
{}

Overlay::~Overlay()
{
    enable( false );
}

std::string Overlay::getName() const
{
    return m_name;
}

void Overlay::enable( bool on )
{
    if( on && !m_hwnd )  // enable
    {
        //
        // Create window
        //

        const char* const wndclassName = "overlay";
        WNDCLASSEX wndclass = {};
        if( !GetClassInfoEx( 0, wndclassName, &wndclass ) )  // only the first overlay we open registers the window class
        {
            wndclass.cbSize = sizeof(WNDCLASSEX);
            wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
            wndclass.lpfnWndProc = windowProc;
            wndclass.lpszClassName = wndclassName;
            wndclass.hbrBackground = CreateSolidBrush(0);
            RegisterClassEx(&wndclass);
        }

        m_hwnd = CreateWindowEx( WS_EX_TOPMOST|WS_EX_TOOLWINDOW|WS_EX_NOREDIRECTIONBITMAP, wndclassName, m_name.c_str(), WS_POPUP|WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 500, 400, NULL, NULL, NULL, NULL );
        SetWindowLongPtr( m_hwnd, GWLP_USERDATA, (LONG_PTR)this );

        RECT r;
        GetWindowRect( m_hwnd, &r );
        const int width = r.right - r.left;
        const int height = r.bottom - r.top;

        //
        // Create the unsettling amount of stuff that's needed to get a window to
        // properly alpha-blend our Direct2D rendering into the desktop.
        // See: https://docs.microsoft.com/en-us/archive/msdn-magazine/2014/june/windows-with-c-high-performance-window-layering-using-the-windows-composition-engine
        //

#ifdef _DEBUG
        const bool isdebug = true;
#else
        const bool isdebug = false;
#endif

        // D3D11 device
        HRCHECK(D3D11CreateDevice( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT, NULL, 0, D3D11_SDK_VERSION, &m_d3dDevice, NULL, NULL ));

        // DXGI device
        ComPtr<IDXGIDevice> dxgiDevice;
        HRCHECK(m_d3dDevice.As(&dxgiDevice));

        // DXGI factory
        ComPtr<IDXGIFactory2> dxgiFactory;
        HRCHECK(CreateDXGIFactory2( isdebug ? DXGI_CREATE_FACTORY_DEBUG : 0, IID_PPV_ARGS(&dxgiFactory) ));

        // DXGI Swap chain
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width            = width;
        swapChainDesc.Height           = height;
        swapChainDesc.Format           = DXGI_FORMAT_B8G8R8A8_UNORM;
        swapChainDesc.BufferUsage      = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.SwapEffect       = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        swapChainDesc.BufferCount      = 2;                              
        swapChainDesc.SampleDesc.Count = 1;                              
        swapChainDesc.AlphaMode        = DXGI_ALPHA_MODE_PREMULTIPLIED;
        HRCHECK(dxgiFactory->CreateSwapChainForComposition( dxgiDevice.Get(), &swapChainDesc, NULL, &m_swapChain ));
        HRCHECK(dxgiFactory->MakeWindowAssociation( m_hwnd, DXGI_MWA_NO_ALT_ENTER ));

        // DXGI surface
        ComPtr<IDXGISurface2> dxgiSurface;
        HRCHECK(m_swapChain->GetBuffer( 0, IID_PPV_ARGS(&dxgiSurface) ));

        // D2D factory
        D2D1_FACTORY_OPTIONS factoryOptions = {};
        factoryOptions.debugLevel = isdebug ? D2D1_DEBUG_LEVEL_INFORMATION : D2D1_DEBUG_LEVEL_NONE;
        HRCHECK(D2D1CreateFactory( D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(m_d2dFactory), &factoryOptions, &m_d2dFactory ));

        // D2D render target
        D2D1_RENDER_TARGET_PROPERTIES targetProperties = {};
        targetProperties.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
        targetProperties.pixelFormat.format = DXGI_FORMAT_UNKNOWN;
        targetProperties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
        HRCHECK(m_d2dFactory->CreateDxgiSurfaceRenderTarget( dxgiSurface.Get(), &targetProperties, &m_renderTarget ));

        // Composition stuff
        HRCHECK(DCompositionCreateDevice( dxgiDevice.Get(), IID_PPV_ARGS(&m_compositionDevice) ));
        HRCHECK(m_compositionDevice->CreateTargetForHwnd( m_hwnd, true, &m_compositionTarget ));
        HRCHECK(m_compositionDevice->CreateVisual( &m_compositionVisual ));
        HRCHECK(m_compositionVisual->SetContent(m_swapChain.Get()));
        HRCHECK(m_compositionTarget->SetRoot(m_compositionVisual.Get()));
        HRCHECK(m_compositionDevice->Commit());

        // DirectWrite factory
        HRCHECK(DWriteCreateFactory( DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(m_dwriteFactory.GetAddressOf()) ));

        // Default brush
        HRCHECK(m_renderTarget->CreateSolidColorBrush( float4(0,0,0,1), &m_brush ));

        //
        // Finalize enable
        //

        m_enabled = true;
        onEnable();
    }
    else if( !on && m_hwnd ) // disable
    {
        onDisable();

        m_dwriteFactory.Reset();
        m_compositionVisual.Reset();
        m_compositionTarget.Reset();
        m_compositionDevice.Reset();
        m_renderTarget.Reset();
        m_d2dFactory.Reset();
        m_swapChain.Reset();
        m_d3dDevice.Reset();

        DestroyWindow( m_hwnd );
        m_hwnd = 0;
        m_enabled = false;
    }
}

bool Overlay::isEnabled() const
{
    return m_enabled;
}

void Overlay::enableUiEdit( bool on )
{
    m_uiEditEnabled = on;
    update();
}

bool Overlay::isUiEditEnabled() const
{
    return m_uiEditEnabled;
}

void Overlay::configChanged()
{
    if( !m_enabled )
        return;

    // Somewhat silly way to ensure the default positions of the overlays aren't all on top of each other.
    const unsigned hash = MurmurHash2(m_name.c_str(),(int)m_name.length(),0x1234);
    const int defaultX = (hash % 100) * 15;
    const int defaultY = (hash % 80) * 10;

    const float2 defaultSize = getDefaultSize();

    // Position/dimensions might have changed
    const int x = g_cfg.getInt(m_name,"window_pos_x", defaultX);
    const int y = g_cfg.getInt(m_name,"window_pos_y", defaultY);
    const int w = g_cfg.getInt(m_name,"window_size_x", (int)defaultSize.x);
    const int h = g_cfg.getInt(m_name,"window_size_y", (int)defaultSize.y);
    setWindowPosAndSize( x, y, w, h );

    onConfigChanged();
}

void Overlay::sessionChanged()
{
    onSessionChanged();
}

void Overlay::update()
{
    if( !m_enabled )
        return;

    const float w = (float)m_width;
    const float h = (float)m_height;
    const float cornerRadius = g_cfg.getFloat( m_name, "corner_radius", m_name=="OverlayInputs"?2.0f:6.0f );

    // Clear/draw background
    if( !hasCustomBackground() )
    {
        m_renderTarget->BeginDraw();
        m_renderTarget->Clear( float4(0,0,0,0) );
        D2D1_ROUNDED_RECT rr;
        rr.rect = { 0.5f, 0.5f, w-0.5f, h-0.5f };
        rr.radiusX = cornerRadius;
        rr.radiusY = cornerRadius;
        m_brush->SetColor( g_cfg.getFloat4( m_name, "background_col", float4(0,0,0,0.7f) ) );
        m_renderTarget->FillRoundedRectangle( &rr, m_brush.Get() );
        m_renderTarget->EndDraw();
    }

    // Overlay-specific logic and rendering
    onUpdate();

    if( m_uiEditEnabled )
    {
        // Draw highlight frame and resize corner indicators
        m_renderTarget->BeginDraw();
        D2D1_ROUNDED_RECT rr;
        rr.rect = { 0.5f, 0.5f, w-0.5f, h-0.5f };
        rr.radiusX = cornerRadius;
        rr.radiusY = cornerRadius;
        m_brush->SetColor( float4(1,1,1,0.7f) );
        m_renderTarget->DrawRoundedRectangle( &rr, m_brush.Get(), 2 );
        m_renderTarget->DrawLine( float2(w-0.5f,h-0.5f-ResizeBorderWidth), float2(w-0.5f-ResizeBorderWidth,h-0.5f-ResizeBorderWidth), m_brush.Get(), 2 );
        m_renderTarget->DrawLine( float2(w-0.5f-ResizeBorderWidth,h-0.5f), float2(w-0.5f-ResizeBorderWidth,h-0.5f-ResizeBorderWidth), m_brush.Get(), 2 );
        m_renderTarget->EndDraw();
    }

    HRCHECK(m_swapChain->Present( 1, 0 ));
}

void Overlay::setWindowPosAndSize( int x, int y, int w, int h, bool callSetWindowPos )
{
    w = std::max( w, 30 );
    h = std::max( h, 30 );

    if( callSetWindowPos )
        SetWindowPos( m_hwnd, HWND_TOPMOST, x, y, w, h, SWP_NOACTIVATE|SWP_SHOWWINDOW );
    
    m_xpos = x;
    m_ypos = y;
    m_width = w;
    m_height = h;

    m_renderTarget.Reset();  // need to release all references to swap chain's back buffers before calling ResizeBuffers

    HRCHECK(m_swapChain->ResizeBuffers( 0, w, h, DXGI_FORMAT_UNKNOWN, 0 ));

    // Recreate render target
    ComPtr<IDXGISurface2> dxgiSurface;
    HRCHECK(m_swapChain->GetBuffer( 0, IID_PPV_ARGS(&dxgiSurface) ));
    D2D1_RENDER_TARGET_PROPERTIES targetProperties = {};
    targetProperties.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
    targetProperties.pixelFormat.format = DXGI_FORMAT_UNKNOWN;
    targetProperties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    HRCHECK(m_d2dFactory->CreateDxgiSurfaceRenderTarget( dxgiSurface.Get(), &targetProperties, &m_renderTarget ));
}

void Overlay::saveWindowPosAndSize()
{
    g_cfg.setInt( m_name, "window_pos_x", m_xpos );
    g_cfg.setInt( m_name, "window_pos_y", m_ypos );
    g_cfg.setInt( m_name, "window_size_x", m_width );
    g_cfg.setInt( m_name, "window_size_y", m_height  );

    g_cfg.save();
}

bool Overlay::canEnableWhileNotDriving() const
{
    return false;
}

bool Overlay::canEnableWhileDisconnected() const
{
    return false;
}

void Overlay::onEnable() {}
void Overlay::onDisable() {}
void Overlay::onUpdate() {}
void Overlay::onConfigChanged() {}
void Overlay::onSessionChanged() {}
float2 Overlay::getDefaultSize() { return float2(400,300); }
bool Overlay::hasCustomBackground() { return false; }

