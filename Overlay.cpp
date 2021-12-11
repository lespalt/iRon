/*
MIT License

Copyright (c) 2021 lespalt

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
#include <dwmapi.h>
#include <GL\gl.h>
#include "Overlay.h"
#include "Config.h"

static LRESULT CALLBACK windowProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    switch( msg )
    {
    case WM_NCHITTEST:
    {
        LRESULT hit = DefWindowProc( hwnd, msg, wparam, lparam );
        if( hit == HTCLIENT )
        {
            // check if we hit the edge area of the window to allow resizing despite having no border
            RECT r;
            GetWindowRect( hwnd, &r );
            const int cur_x = GET_X_LPARAM( lparam ) - r.left;
            const int cur_y = GET_Y_LPARAM( lparam ) - r.top;
            const int w = r.right - r.left;
            const int h = r.bottom - r.top;
            const int border = 40;

            if( cur_x < border && cur_y < border )
                return HTTOPLEFT;
            if( cur_x > w-border && cur_y < border )
                return HTTOPRIGHT;
            if( cur_x < border && cur_y > h-border )
                return HTBOTTOMLEFT;
            if( cur_x > w-border && cur_y > h-border )
                return HTBOTTOMRIGHT;
            if( cur_x < border )
                return HTLEFT;
            if( cur_x > w-border )
                return HTRIGHT;
            if( cur_y < border )
                return HTTOP;
            if( cur_y > h-border )
                return HTBOTTOM;

            // say we hit the caption to allow dragging the window from the client area
            hit = HTCAPTION;
        }
        return hit;
    }
    case WM_MOVING:
    {
        Overlay* o = (Overlay*)GetWindowLongPtr( hwnd, GWLP_USERDATA );
        if( o )
            o->saveWindowPosAndSize();
        break;
    }
    case WM_SIZE:
    {
        Overlay* o = (Overlay*)GetWindowLongPtr( hwnd, GWLP_USERDATA );
        if( o )
        {
            o->resizeGlViewport();
            o->saveWindowPosAndSize();
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
        // Create window.
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

        m_hwnd = CreateWindowEx( WS_EX_TOPMOST, wndclassName, m_name.c_str(), WS_POPUP|WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 500, 400, NULL, NULL, NULL, NULL );
        SetWindowLongPtr( m_hwnd, GWLP_USERDATA, (LONG_PTR)this );

        //
        // Create and configure GL context.
        //

        PIXELFORMATDESCRIPTOR pfd = {};
        pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_SUPPORT_COMPOSITION;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 24;
        pfd.cAlphaBits = 8;
        pfd.cDepthBits = 32;

        m_hdc = GetDC( m_hwnd );
        if( !SetPixelFormat(m_hdc, ChoosePixelFormat(m_hdc, &pfd), &pfd) )
            printf("SetPixelFormat failed\n");

        m_hglrc = wglCreateContext( m_hdc );
        if( !m_hglrc )
            printf("wglCreateContext failed\n");

        wglMakeCurrent( m_hdc, m_hglrc );
        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        glEnable( GL_LINE_SMOOTH );
        glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );

        //
        // The following enables composition of our window into the desktop.
        //

        DWM_BLURBEHIND bb = {};
        bb.dwFlags = DWM_BB_ENABLE;
        bb.fEnable = true;
        DwmEnableBlurBehindWindow(m_hwnd, &bb);                

        //
        // Finalize enable.
        //

        m_enabled = true;
        onEnable();
    }
    else if( !on && m_hwnd ) // disable
    {
        onDisable();
        wglDeleteContext( m_hglrc );
        DestroyWindow( m_hwnd );
        m_hwnd = 0;
        m_hdc = 0;
        m_hglrc = 0;
        m_enabled = false;
    }
}

void Overlay::enableUiEdit( bool on )
{
}

void Overlay::configChanged()
{
    onConfigChanged();
}

void Overlay::update()
{
    onUpdate();
}

void Overlay::setWindowPosAndSize( int x, int y, int w, int h )
{
    SetWindowPos( m_hwnd, HWND_TOPMOST, x, y, w, h, SWP_NOACTIVATE|SWP_SHOWWINDOW );
    resizeGlViewport();
}

void Overlay::saveWindowPosAndSize()
{
    RECT r;
    GetWindowRect( m_hwnd, &r );
    const int w = r.right - r.left;
    const int h = r.bottom - r.top;

    g_cfg.setInt( m_name, "x_pos", r.left );
    g_cfg.setInt( m_name, "y_pos", r.top );
    g_cfg.setInt( m_name, "width", w );
    g_cfg.setInt( m_name, "height", h  );

    g_cfg.save();
}

void Overlay::resizeGlViewport()
{
    if( !m_hglrc )
        return;

    RECT r;
    GetWindowRect( m_hwnd, &r );
    const int w = r.right - r.left;
    const int h = r.bottom - r.top;

    wglMakeCurrent( m_hdc, m_hglrc );
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,w,0,h,1,-1);
}

void Overlay::onEnable() {}
void Overlay::onDisable() {}
void Overlay::onUpdate() {}
void Overlay::onConfigChanged() {}
